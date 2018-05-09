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

import argparse
import os
import sys

import config
import encode
import rest
import utils


def get_parser():
    parser = argparse.ArgumentParser(
        prog='fischeck',
        add_help=False)
    parser.add_argument('--name',
                        action='store')
    parser.add_argument('--metadata',
                        action='store')
    parser.add_argument('--description',
                        action='store')
    parser.add_argument('--file-name',
                        action='store')
    parser.add_argument('--fpga-image-id',
                        action='store')
    parser.add_argument('--image-id',
                        action='store')
    parser.add_argument('--page',
                        action='store')
    parser.add_argument('--size',
                        action='store')
    return parser


def main():
    # parse input option
    argv = [encode.convert_to_unicode(a) for a in sys.argv[1:]]
    args = get_parser().parse_args(argv)

    # read and check args
    kwargs = {}
    if args.name is not None:
        kwargs['name'] = args.name
    if args.metadata is not None:
        kwargs['metadata'] = args.metadata
    if args.description is not None:
        kwargs['description'] = args.description
    if args.file_name is not None:
        kwargs['file_name'] = args.file_name
    if args.fpga_image_id is not None:
        kwargs['fpga_image_id'] = args.fpga_image_id
    if args.image_id is not None:
        kwargs['image_id'] = args.image_id
    if args.page is not None:
        kwargs['page'] = args.page
    if args.size is not None:
        kwargs['size'] = args.size
    try:
        utils.check_param(**kwargs)
    except Exception as e:
        utils.exit('Error: %s' % encode.exception_to_unicode(e))

    # read and check config file
    config.read_config_and_verify()
    access_key = os.getenv('OS_ACCESS_KEY')
    secret_key = os.getenv('OS_SECRET_KEY')
    region_id = os.getenv('OS_REGION_ID')
    bucket_name = os.getenv('OS_BUCKET_NAME')
    domain_id = os.getenv('OS_DOMAIN_ID')
    project_id = os.getenv('OS_PROJECT_ID')
    obs_endpoint = os.getenv('OS_OBS_ENDPOINT')
    fis_endpoint = os.getenv('OS_FIS_ENDPOINT')

    try:
        # check bucket
        utils._check_bucket_acl_location(bucket_name, access_key, secret_key,
                                         obs_endpoint, region_id, domain_id)
        # check fis
        rest.fpga_image_relation_list(access_key, secret_key, project_id,
                                      region_id, fis_endpoint)
    except Exception as e:
        utils.exit('Error: %s' % encode.exception_to_unicode(e))

    if kwargs:
        print('fis argument(s) and config file are OK')
    else:
        print('fis config file is OK')

    # check intranet dns
    config.check_intranet_dns(region_id)


if __name__ == '__main__':
    main()
