# Copyright 2012 OpenStack Foundation
# Copyright 2017 Huawei Technologies Co., Ltd.
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

import json
import re
from collections import OrderedDict

from oslo_utils import encodeutils

from fisclient.common import utils, exceptions
from fisclient.fisshell import OS_ENTRY_CMDSHELL, OS_ENTRY_WRAPSHELL


def _do_resp(resp):
    print('Success:', resp.status_code, resp.reason)


@utils.entry(OS_ENTRY_CMDSHELL)
@utils.arg('--location', metavar='<LOCATION>', required=True,
           help='The location in the obs bucket of fpga image')
@utils.arg('--name', metavar='<NAME>', required=True,
           help='The name of fpga image')
@utils.arg('--metadata', metavar='<METADATA>', required=True,
           help='The metadata of fpga image')
@utils.arg('--description', metavar='<DESCRIPATION>',
           help='The description of fpga image')
def do_fpga_image_register(fc, lc, args):
    """register the fpga image"""
    kwargs = OrderedDict()
    kwargs['location'] = args.location
    kwargs['name'] = args.name
    kwargs['metadata'] = args.metadata
    if args.description is not None:
        kwargs['description'] = args.description
    utils.check_param(**kwargs)

    # string to dict
    kwargs['metadata'] = json.loads(args.metadata,
                                    object_pairs_hook=OrderedDict)

    try:
        resp, body = fc.fis.fpga_image_register(**kwargs)
        if (getattr(resp, 'status_code', None) != 200
                or not isinstance(body, dict)):
            raise exceptions.FisException("Invalid response from server")
        fi = body.get('fpga_image', {})
        lc.log(json.dumps(kwargs),
               'Success', '%s %s' % (resp.status_code, resp.reason),
               'fpga_image_id %s' % fi.get('id', ''))
    except Exception as e:
        msg = encodeutils.exception_to_unicode(e)
        lc.log(json.dumps(kwargs),
               'Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
        raise

    _do_resp(resp)
    print('id: ' + fi.get('id', ''))
    print('status: ' + fi.get('status', ''))


@utils.entry(OS_ENTRY_WRAPSHELL)
@utils.arg('--fpga-image-id', metavar='<ID>', required=True,
           help='The id of fpga image')
@utils.arg('--force', dest='force', action='store_true',
           help='force delete')
def do_fpga_image_delete(fc, lc, args):
    """delete the fpga image"""
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

    try:
        resp, _ = fc.fis.fpga_image_delete(**kwargs)
        if getattr(resp, 'status_code', None) != 204:
            raise exceptions.FisException("Invalid response from server")
        lc.log(json.dumps(kwargs),
               'Success', '%s %s' % (resp.status_code, resp.reason))
    except Exception as e:
        msg = encodeutils.exception_to_unicode(e)
        lc.log(json.dumps(kwargs),
               'Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
        raise

    _do_resp(resp)


@utils.entry(OS_ENTRY_WRAPSHELL)
@utils.arg('--page', metavar='<PAGE>',
           help='The page number of list')
@utils.arg('--size', metavar='<SIZE>',
           help='The page size of list')
def do_fpga_image_list(fc, lc, args):
    """list the fpga image of tenant"""
    kwargs = OrderedDict()
    if args.page is not None and args.size is not None:
        kwargs['page'] = args.page
        kwargs['size'] = args.size
    elif args.page is not None and args.size is None\
            or args.page is None and args.size is not None:
        utils.print_err('Error: argument --page and --size '
                        'must exist or not exist at the same time')
        return
    utils.check_param(**kwargs)

    try:
        resp, body = fc.fis.fpga_image_list(**kwargs)
        if (getattr(resp, 'status_code', None) != 200
                or not isinstance(body, dict)):
            raise exceptions.FisException("Invalid response from server")
        fi_list = body.get('fpgaimages', [])
        lc.log(json.dumps(kwargs),
               'Success', '%s %s' % (resp.status_code, resp.reason))
    except Exception as e:
        msg = encodeutils.exception_to_unicode(e)
        lc.log(json.dumps(kwargs),
               'Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
        raise

    _do_resp(resp)
    columns = ['id', 'name', 'status', 'protected', 'size', 'createdAt',
               'description', 'metadata', 'message']
    utils.print_list(fi_list, columns)


@utils.entry(OS_ENTRY_WRAPSHELL)
@utils.arg('--fpga-image-id', metavar='<ID>', required=True,
           help='The id of fpga image')
@utils.arg('--image-id', metavar='<ID>', required=True,
           help='The id of image')
def do_fpga_image_relation_create(fc, lc, args):
    """create the relation of fpga image and image"""
    kwargs = OrderedDict()
    kwargs['fpga_image_id'] = args.fpga_image_id
    kwargs['image_id'] = args.image_id
    utils.check_param(**kwargs)

    try:
        resp, _ = fc.fis.fpga_image_relation_create(**kwargs)
        if getattr(resp, 'status_code', None) != 204:
            raise exceptions.FisException("Invalid response from server")
        lc.log(json.dumps(kwargs),
               'Success', '%s %s' % (resp.status_code, resp.reason))
    except Exception as e:
        msg = encodeutils.exception_to_unicode(e)
        lc.log(json.dumps(kwargs),
               'Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
        raise

    _do_resp(resp)


@utils.entry(OS_ENTRY_WRAPSHELL)
@utils.arg('--fpga-image-id', metavar='<ID>', required=True,
           help='The id of fpga image')
@utils.arg('--image-id', metavar='<ID>', required=True,
           help='The id of image')
def do_fpga_image_relation_delete(fc, lc, args):
    """delete the relation of fpga image and image"""
    kwargs = OrderedDict()
    kwargs['fpga_image_id'] = args.fpga_image_id
    kwargs['image_id'] = args.image_id
    utils.check_param(**kwargs)

    try:
        resp, _ = fc.fis.fpga_image_relation_delete(**kwargs)
        if getattr(resp, 'status_code', None) != 204:
            raise exceptions.FisException("Invalid response from server")
        lc.log(json.dumps(kwargs),
               'Success', '%s %s' % (resp.status_code, resp.reason))
    except Exception as e:
        msg = encodeutils.exception_to_unicode(e)
        lc.log(json.dumps(kwargs),
               'Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
        raise

    _do_resp(resp)


@utils.entry(OS_ENTRY_WRAPSHELL)
@utils.arg('--fpga-image-id', metavar='<ID>',
           help='The id of fpga image')
@utils.arg('--image-id', metavar='<ID>',
           help='The id of image')
@utils.arg('--page', metavar='<PAGE>',
           help='The page number of list')
@utils.arg('--size', metavar='<SIZE>',
           help='The page size of list')
def do_fpga_image_relation_list(fc, lc, args):
    """list the relation of tenant"""
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

    try:
        resp, body = fc.fis.fpga_image_relation_list(**kwargs)
        if (getattr(resp, 'status_code', None) != 200
                or not isinstance(body, dict)):
            raise exceptions.FisException("Invalid response from server")
        lc.log(json.dumps(kwargs),
               'Success', '%s %s' % (resp.status_code, resp.reason))
    except Exception as e:
        msg = encodeutils.exception_to_unicode(e)
        lc.log(json.dumps(kwargs),
               'Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
        raise

    _do_resp(resp)
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
