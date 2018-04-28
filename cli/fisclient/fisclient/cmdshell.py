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
import os
import sys

from oslo_utils import encodeutils

from fisclient.common import config
from fisclient.fisshell import FisShell, OS_ENTRY_CMDSHELL


def get_parser():
    parser = argparse.ArgumentParser(
        prog='fis',
        description='Command-line interface to the fis API.',
        add_help=False)
    parser.add_argument('--password',
                        dest='password',
                        action='store')
    return parser


def main():
    os.environ['OS_ENTRY_SHELL'] = OS_ENTRY_CMDSHELL

    # parse input option
    argv = [encodeutils.safe_decode(a) for a in sys.argv[1:]]
    args, left_argv = get_parser().parse_known_args(argv)

    # read config and password
    try:
        config.read_config_and_password(args.password)
    except (KeyboardInterrupt, EOFError):
        exit()

    # check password by getting token
    fis_shell = FisShell()
    fis_shell.get_token()

    # parse and run subcommand
    safe_argv = [encodeutils.safe_decode(a) for a in left_argv]
    fis_shell.main(safe_argv)


if __name__ == '__main__':
    main()
