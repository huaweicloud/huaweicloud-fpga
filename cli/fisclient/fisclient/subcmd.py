# Copyright 2018 Huawei Technologies Co., Ltd.
# All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License"); you may
#    not use this file except in compliance with the License. You may obtain
#    a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#    License for the specific language governing permissions and limitations
#    under the License.

from __future__ import print_function

from collections import OrderedDict
import os
import re
import sys

from requests.exceptions import RequestException

import config
import encode
from exception import FisException
import rest
import utils


# enable command line editing using GNU readline on Linux
if sys.platform.startswith('linux'):
    import readline     # noqa: F401


BUCKET_NAMING_RULES = """\033[31mBucket Naming rules\033[0m:
- Contains 3 to 63 characters
- Can contain only lowercase letters, digits, hyphens (-), and periods (.)
- Cannot start or end with a hyphen (-) or period (.)
- Cannot contain two consecutive periods (.)
- Cannot contain periods (.) and hyphens (-) adjacent to each other
- Cannot be an IP address
"""


def _check_and_create_bucket(bucket_name, bucket_excluded, access_key, secret_key, region_id, obs_endpoint):
    if bucket_name in bucket_excluded:
        utils.print_err('Error: Bucket "%s" is not in region "%s" or you do not '
                        'have the READ and/or WRITE permissions of this bucket' % (bucket_name, region_id))
    elif not bucket_name:
        utils.print_err('Error: empty input')
    elif utils.check_bucket_name(bucket_name):
        try:
            rest.make_bucket(access_key, secret_key, bucket_name, region_id, obs_endpoint)
            print('\033[31mBucket "%s" created\033[0m' % bucket_name)
            return True
        except FisException as e:
            utils.print_err(encode.exception_to_unicode(e))
    else:
        utils.print_err('Error: "%s" is not a valid bucket name\n%s' % (bucket_name, BUCKET_NAMING_RULES))
    return False


def _get_config():
    ak = os.getenv('OS_ACCESS_KEY')
    sk = os.getenv('OS_SECRET_KEY')
    project_id = os.getenv('OS_PROJECT_ID')
    region_id = os.getenv('OS_REGION_ID')
    host = os.getenv('OS_FIS_ENDPOINT')
    return ak, sk, project_id, region_id, host


def _invalid_resp(status_code, reason, body=None):
    invalid_msg = 'Invalid Response From Server'
    if body:
        return '%s\n%s %s, %s' % (invalid_msg, status_code, reason, body)
    else:
        return '%s\n%s %s' % (invalid_msg, status_code, reason)


def _do_resp(status_code, reason):
    print('Success:', status_code, reason)


@utils.arg('command', metavar='<subcommand>', nargs='?',
           help='Display help for <subcommand>')
def do_help(args):
    """Display help about fis or one of its subcommands"""
    command = args.command
    subcommands = args.subcommands
    parser = args.parser

    if command:
        if command in subcommands:
            subcommands[command].print_help()
        else:
            raise FisException("'%s' is not a valid subcommand" % command)
    else:
        parser.print_help()


@utils.arg('--dump', dest='dump', action='store_true',
           help='Dump current configuration')
def do_configure(args):
    """Invoke interactive (re)configuration tool"""
    cur_conf = config.read_current_config()
    if args.dump:
        for key in ('OS_ACCESS_KEY', 'OS_SECRET_KEY', 'OS_BUCKET_NAME', 'OS_REGION_ID'):
            print("%s = %s" % (key, cur_conf.get(key, '')))
        return

    access_key_old = cur_conf.get('OS_ACCESS_KEY', '')
    secret_key_old = cur_conf.get('OS_SECRET_KEY', '')
    bucket_name_old = cur_conf.get('OS_BUCKET_NAME', '')
    region_id_old = cur_conf.get('OS_REGION_ID', '')
    configure_region_id = False

    try:
        print('Enter new values or accept defaults in brackets with Enter')

        # get region_id from ECS metadata
        print('\nGetting region_id from ECS metadata.')
        region_id = rest.get_region_id_from_metadata()
        if region_id:
            print('You are in region "%s".' % region_id)
        else:
            # configure region_id interactively when get it from ECS metadata failed
            configure_region_id = True
            print('\n\033[31mNote: If an incorrect Region ID is used, the FPGA image creation and querying may succeed, but the FPGA loading will fail.\033[0m')
            print('Choose the Region where you are located.')
            regions = config.endpoints.keys()
            print('Available Regions:')
            for i, region in enumerate(regions, 1):
                print('  (%d) %s' % (i, region))
            while True:
                region_id = raw_input('Region ID [%s]: ' % region_id_old).strip() or region_id_old
                if re.match(u'\d+$', region_id) and 1 <= int(region_id) <= len(regions):
                    region_id = regions[int(region_id)-1]
                    break
                elif region_id in regions:
                    break
                elif not region_id:
                    utils.print_err('Error: empty input')
                else:
                    utils.print_err('Error: "%s" is not a valid region' % region_id)

        obs_endpoint = config.get_endpoint(region_id, 'obs')
        iam_endpoint = config.get_endpoint(region_id, 'iam')
        vpc_endpoint = config.get_endpoint(region_id, 'vpc')
        ecs_endpoint = config.get_endpoint(region_id, 'fis')
        fis_endpoint = config.get_endpoint(region_id, 'fis')

        # configure intranet dns of ecs
        config.configure_intranet_dns_ecs(region_id)

        # loop until access_key, secret_key are OK
        while True:
            try:
                print('\nAccess key and Secret key are your identifiers for FIS and OBS.')
                while True:
                    access_key = raw_input('Access Key [%s]: ' % access_key_old).strip() or access_key_old
                    if access_key:
                        break
                    else:
                        utils.print_err('Error: empty input')

                while True:
                    secret_key = raw_input('Secret Key [%s]: ' % secret_key_old).strip() or secret_key_old
                    if secret_key:
                        break
                    else:
                        utils.print_err('Error: empty input')

                bucket_list = rest.get_bucket_list(access_key, secret_key, obs_endpoint)
                project = rest.get_project(access_key, secret_key, region_id, iam_endpoint).get('projects', [])
                if len(project) >= 1:
                    domain_id = project[0].get('domain_id')
                    project_id = project[0].get('id')
                else:
                    raise FisException('You do NOT have project in "%s", \033[31mplease '
                                       'choose another region and try again\033[0m' % region_id)

                # break when access_key, secret_key are OK
                break
            except (FisException, RequestException) as e:
                msg = encode.exception_to_unicode(e)
                if 'InvalidAccessKeyId' in msg:
                    msg += ', \033[31mTips=Maybe your Access Key is invalid\033[0m'
                elif 'SignatureDoesNotMatch' in msg:
                    msg += ', \033[31mTips=Maybe your Secret Key is invalid\033[0m'
                utils.print_err('Error: %s' % msg)
                access_key_old = access_key
                secret_key_old = secret_key

        # loop until bucket_name is OK
        print('\nGetting all your available buckets in "%s".' % region_id)
        buckets = bucket_list.get('Buckets', {})
        bucket_list = buckets.get('Bucket', []) if isinstance(buckets, dict) else []
        if not isinstance(bucket_list, list):
            bucket_list = [bucket_list]
        all_bucket = [bucket.get('Name') for bucket in bucket_list if isinstance(bucket, dict)]
        available_bucket = [bucket for bucket in all_bucket
                            if utils.is_bucket_valid(bucket, access_key, secret_key, obs_endpoint, region_id, domain_id)]
        if available_bucket:
            print('\nChoose or Create a Bucket for storing the DCP and LOG files.')
            print('Available Bucket(s):')
            for i, bucket in enumerate(available_bucket, 1):
                print('  (%d) %s' % (i, bucket))
            while True:
                bucket_name = raw_input('Bucket Name [%s]: ' % bucket_name_old).strip() or bucket_name_old
                if re.match(u'\d+$', bucket_name) and 1 <= int(bucket_name) <= len(available_bucket):
                    bucket_name = available_bucket[int(bucket_name)-1]
                    break
                if bucket_name.startswith('!'):
                    bucket_name = bucket_name[1:]
                if (bucket_name in available_bucket or
                        _check_and_create_bucket(bucket_name, all_bucket, access_key, secret_key, region_id, obs_endpoint)):
                    break
        else:
            print('\nCreate a Bucket for storing the DCP and LOG files.')
            while True:
                bucket_name = raw_input('Bucket Name [%s]: ' % bucket_name_old).strip() or bucket_name_old
                if _check_and_create_bucket(bucket_name, all_bucket, access_key, secret_key, region_id, obs_endpoint):
                    break

        # configure intranet dns of vpc
        print('\nChecking private DNS of VPC.')
        config.configure_intranet_dns_vpc(access_key, secret_key, project_id, region_id, ecs_endpoint, vpc_endpoint)

        # save new settings
        if not configure_region_id:
            print('\nNew settings:\n  Access key: %s\n  Secret Key: %s\n  Bucket Name: %s' %
                  (access_key, secret_key, bucket_name))
        else:
            print('\nNew settings:\n  Region ID: %s\n  Access key: %s\n  Secret Key: %s\n  Bucket Name: %s' %
                  (region_id, access_key, secret_key, bucket_name))
        save_option = raw_input('Save settings? [Y/n]: ').strip() or 'Y'
        if 'yes'.startswith(save_option.lower()):
            config.save_config(access_key, secret_key, bucket_name,
                               region_id, domain_id, project_id,
                               obs_endpoint, iam_endpoint, vpc_endpoint, fis_endpoint)
            print('Configuration saved to "%s".' % os.path.expanduser(config.CONFIG_FILE))
        else:
            print('Changes were NOT saved.')
    except (KeyboardInterrupt, EOFError):
        exit()


@utils.arg('--dcp-file', metavar='<FilePath>', required=True,
           help='The DCP file path in the local file system')
@utils.arg('--dcp-obs-path', metavar='<OBSFilePath>', required=True,
           help='The DCP file path in the OBS bucket')
@utils.arg('--log-obs-directory', metavar='<OBSDirPath>',
           help='The LOG file directory in the OBS bucket')
@utils.arg('--name', metavar='<String>', required=True,
           help='The name of FPGA image')
@utils.arg('--description', metavar='<String>',
           help='The description of FPGA image')
def do_fpga_image_create(args):
    """Create an FPGA image"""
    access_key = os.getenv('OS_ACCESS_KEY')
    secret_key = os.getenv('OS_SECRET_KEY')
    obs_endpoint = os.getenv('OS_OBS_ENDPOINT')
    bucket_name = os.getenv('OS_BUCKET_NAME')

    utils.check_dcp_file(args.dcp_file)

    kwargs = OrderedDict()
    kwargs['dcp_obs_path'] = args.dcp_obs_path
    if args.log_obs_directory is not None:
        kwargs['log_obs_directory'] = args.log_obs_directory
    kwargs['name'] = args.name
    if args.description is not None:
        kwargs['description'] = args.description
    utils.check_param(**kwargs)

    print('Uploading DCP file to OBS')
    _, _, filesize, time_diff = rest.put_dcp_file(access_key, secret_key, args.dcp_file,
                                                  bucket_name, args.dcp_obs_path, obs_endpoint)
    print('Upload %s bytes using %s second(s)' % (filesize, time_diff))

    print('Creating FPGA image to FIS')
    fpga_image = {
        'dcp_location': '%s:%s' % (bucket_name, args.dcp_obs_path),
        'log_directory': args.log_obs_directory,
        'name': args.name,
        'description': args.description
    }
    status_code, reason, body = rest.fpga_image_create(*_get_config(), fpga_image=fpga_image)
    if status_code != 200 or not isinstance(body, dict):
        raise FisException(_invalid_resp(status_code, reason, body))
    fi = body.get('fpga_image', {})

    _do_resp(status_code, reason)
    print('id: %s\nstatus: %s' % (fi.get('id'), fi.get('status')))


@utils.arg('--fpga-image-id', metavar='<UUID>', required=True,
           help='The ID of FPGA image')
@utils.arg('--force', dest='force', action='store_true',
           help='Delete without user confirmation')
def do_fpga_image_delete(args):
    """Delete an FPGA image"""
    kwargs = OrderedDict()
    kwargs['fpga_image_id'] = args.fpga_image_id
    utils.check_param(**kwargs)

    if not args.force:
        ans = raw_input('Deleted fpga-image cannot be restored! '
                        'Are you absolutely sure? (yes/no): ').strip()
        while ans != 'yes' and ans != 'no':
            ans = raw_input('please input yes or no: ').strip()
        if ans == 'no':
            print('cancel fpga-image-delete')
            return

    status_code, reason = rest.fpga_image_delete(*_get_config(), **kwargs)
    if status_code != 204:
        raise FisException(_invalid_resp(status_code, reason))

    _do_resp(status_code, reason)


@utils.arg('--fpga-image-id', metavar='<UUID>',
           help='The ID of FPGA image')
@utils.arg('--page', metavar='<Int>',
           help='The page number for pagination query')
@utils.arg('--size', metavar='<Int>',
           help='The page size for pagination query')
def do_fpga_image_list(args):
    """Query FPGA images of a tenant"""
    kwargs = OrderedDict()
    if args.fpga_image_id is not None:
        kwargs['fpga_image_id'] = args.fpga_image_id
    if args.page is not None and args.size is not None:
        kwargs['page'] = args.page
        kwargs['size'] = args.size
    elif args.page is not None and args.size is None\
            or args.page is None and args.size is not None:
        utils.print_err('Error: argument --page and --size '
                        'must exist or not exist at the same time')
        return
    utils.check_param(**kwargs)

    status_code, reason, body = rest.fpga_image_list(*_get_config(), params=kwargs)
    if status_code != 200 or not isinstance(body, dict):
        raise FisException(_invalid_resp(status_code, reason, body))
    fi_list = body.get('fpgaimages', [])

    _do_resp(status_code, reason)
    columns = ['id', 'name', 'status', 'protected', 'size', 'createdAt',
               'description', 'metadata', 'message']
    utils.print_list(fi_list, columns)


@utils.arg('--fpga-image-id', metavar='<UUID>', required=True,
           help='The ID of FPGA image')
@utils.arg('--image-id', metavar='<UUID>', required=True,
           help='The ID of image')
def do_fpga_image_relation_create(args):
    """Create the relation of an FPGA image and an ECS image"""
    kwargs = OrderedDict()
    kwargs['fpga_image_id'] = args.fpga_image_id
    kwargs['image_id'] = args.image_id
    utils.check_param(**kwargs)

    status_code, reason = rest.fpga_image_relation_create(*_get_config(), **kwargs)
    if status_code != 204:
        raise FisException(_invalid_resp(status_code, reason))

    _do_resp(status_code, reason)


@utils.arg('--fpga-image-id', metavar='<UUID>', required=True,
           help='The ID of FPGA image')
@utils.arg('--image-id', metavar='<UUID>', required=True,
           help='The ID of image')
def do_fpga_image_relation_delete(args):
    """Delete the relation of an FPGA image and an ECS image"""
    kwargs = OrderedDict()
    kwargs['fpga_image_id'] = args.fpga_image_id
    kwargs['image_id'] = args.image_id
    utils.check_param(**kwargs)

    status_code, reason = rest.fpga_image_relation_delete(*_get_config(), **kwargs)
    if status_code != 204:
        raise FisException(_invalid_resp(status_code, reason))

    _do_resp(status_code, reason)


@utils.arg('--fpga-image-id', metavar='<UUID>',
           help='The ID of FPGA image')
@utils.arg('--image-id', metavar='<UUID>',
           help='The ID of image')
@utils.arg('--page', metavar='<Int>',
           help='The page number for pagination query')
@utils.arg('--size', metavar='<Int>',
           help='The page size for pagination query')
def do_fpga_image_relation_list(args):
    """Query FPGA image relations visible to a tenant"""
    kwargs = OrderedDict()
    if args.image_id is not None:
        kwargs['image_id'] = args.image_id
    if args.fpga_image_id is not None:
        kwargs['fpga_image_id'] = args.fpga_image_id
    if args.page is not None and args.size is not None:
        kwargs['page'] = args.page
        kwargs['size'] = args.size
    elif args.page is not None and args.size is None\
            or args.page is None and args.size is not None:
        utils.print_err('Error: argument --page and --size '
                        'must exist or not exist at the same time')
        return
    utils.check_param(**kwargs)

    status_code, reason, body = rest.fpga_image_relation_list(*_get_config(), params=kwargs)
    if status_code != 200 or not isinstance(body, dict):
        raise FisException(_invalid_resp(status_code, reason, body))

    _do_resp(status_code, reason)
    relation_list = []
    for relations in body.get('associations', []):
        image_id = relations.get('image_id', None)
        for fpga_image in relations.get('fpgaimages', []):
            relation = {}
            relation['image_id'] = image_id
            relation.update(fpga_image)
            relation['fpga_image_id'] = relation.get('id', None)
            relation_list.append(relation)
    columns = ['image_id', 'fpga_image_id', 'name', 'status', 'protected',
               'size', 'createdAt', 'description', 'metadata', 'message']
    utils.print_list(relation_list, columns)

    if args.image_id is None and args.fpga_image_id is None:
        print('Tips: The FPGA image relations can only be obtained if at least one of the \033[31m--fpga-image-id\033[0m and \033[31m--image-id\033[0m arguments is specified, otherwise only an empty list is returned.')


@utils.arg('--fpga-image-id', metavar='<UUID>', required=True,
           help='The ID of FPGA image')
def do_get_log_file(args):
    """Get the log file of an FPGA image"""
    access_key = os.getenv('OS_ACCESS_KEY')
    secret_key = os.getenv('OS_SECRET_KEY')
    obs_endpoint = os.getenv('OS_OBS_ENDPOINT')

    kwargs = OrderedDict()
    kwargs['fpga_image_id'] = args.fpga_image_id
    utils.check_param(**kwargs)

    status_code, reason, body = rest.fpga_image_list(*_get_config(), params=kwargs)
    if status_code != 200 or not isinstance(body, dict):
        raise FisException(_invalid_resp(status_code, reason, body))
    fi_list = body.get('fpgaimages', [])

    if not fi_list:
        raise FisException('FPGA Image [%s] does not exist' % args.fpga_image_id)
    fpga_image = fi_list[0]
    if fpga_image.get('status') not in ('active', 'error'):
        raise FisException('FPGA Image [%s] status [%s] is not "active" or "error"' % (args.fpga_image_id, fpga_image.get('status')))
    log_directory = fpga_image.get('log_directory')
    if log_directory is None:
        raise FisException('FPGA Image [%s] has no log file' % args.fpga_image_id)

    print('Log directory is "%s"' % log_directory)
    l = log_directory.split(':')
    if len(l) != 2 or l[0] == '':
        raise FisException('Log directory [%s] is invalid' % log_directory)
    file_name = args.fpga_image_id + '_log.tar'
    if os.path.lexists(file_name):
        raise FisException('The file [%s] already exists in the current directory' % file_name)
    bucket_name = l[0]
    if l[1]:
        object_key = '%s/%s' % (l[1], file_name)
    else:
        object_key = file_name

    print('Downloading Log file from OBS')
    status_code, reason, filesize, time_diff = rest.get_log_file(access_key, secret_key, file_name,
                                                                 bucket_name, object_key, obs_endpoint)
    _do_resp(status_code, reason)
    print('Download %s bytes using %s second(s)' % (filesize, time_diff))
