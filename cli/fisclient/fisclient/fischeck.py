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

import argparse
import sys

from oslo_utils import encodeutils

from fisclient.common import config, utils
from fisclient.fisshell import FisShell


def get_parser():
    parser = argparse.ArgumentParser(
        prog='fischeck',
        add_help=False)
    parser.add_argument('--password',
                        dest='password',
                        action='store')
    parser.add_argument('--args-only',
                        dest='args_only',
                        action='store_true')
    parser.add_argument('--location',
                        dest='location',
                        action='store')
    parser.add_argument('--name',
                        dest='name',
                        action='store')
    parser.add_argument('--metadata',
                        dest='metadata',
                        action='store')
    parser.add_argument('--description',
                        dest='description',
                        action='store')
    parser.add_argument('--fpga-image-id',
                        dest='fpga_image_id',
                        action='store')
    parser.add_argument('--image-id',
                        dest='image_id',
                        action='store')
    parser.add_argument('--page',
                        dest='page',
                        action='store')
    parser.add_argument('--size',
                        dest='size',
                        action='store')
    return parser


def main():
    # parse input option
    argv = [encodeutils.safe_decode(a) for a in sys.argv[1:]]
    args = get_parser().parse_args(argv)

    # read and check args
    kwargs = {}
    if args.location is not None:
        kwargs['location'] = args.location
    if args.name is not None:
        kwargs['name'] = args.name
    if args.metadata is not None:
        kwargs['metadata'] = args.metadata
    if args.description is not None:
        kwargs['description'] = args.description
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
        utils.exit('Error: %s' % encodeutils.exception_to_unicode(e), 2)

    if args.args_only:
        print('fischeck arguments are OK')
        return

    # read config and password
    try:
        config.read_config_and_password(args.password)
    except (KeyboardInterrupt, EOFError):
        exit()

    # check config and password
    FisShell().check_config_and_password()

    # OK
    if not kwargs:
        print('fischeck password and config file are OK')
    else:
        print('fischeck arguments, password and config file are OK')


if __name__ == '__main__':
    main()
