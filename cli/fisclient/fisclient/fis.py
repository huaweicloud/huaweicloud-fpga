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

import argparse
import importlib
import sys

import config
import encode
import utils


subcommands = {}


def get_parser():
    parser = argparse.ArgumentParser(
        prog='fis',
        description='Command-line interface to the fis API.',
        epilog='See "fis help COMMAND" for help on a specific command.',
        add_help=False)

    subparsers = parser.add_subparsers(dest='subcmd',
                                       metavar='<subcommand>')

    subcmd = importlib.import_module('fisclient.subcmd')
    attr_list = [a for a in dir(subcmd) if a.startswith('do_')]

    for attr in attr_list:
        callback = getattr(subcmd, attr)

        desc = callback.__doc__ or ''
        help = desc.strip().split('\n')[0]
        arguments = getattr(callback, 'arguments', [])

        command = attr[3:].replace('_', '-')
        subparser = subparsers.add_parser(command,
                                          help=help,
                                          description=desc,
                                          add_help=False)
        for (args, kwargs) in arguments:
            subparser.add_argument(*args, **kwargs)
        subparser.set_defaults(func=callback)

        subcommands[command] = subparser

    return parser


def main():
    parser = get_parser()
    if len(sys.argv) <= 1:
        parser.print_help()
        return

    argv = [encode.convert_to_unicode(a) for a in sys.argv[1:]]
    args = parser.parse_args(argv)
    if args.subcmd.startswith('fpga-image'):
        config.read_config_and_verify()
    elif args.subcmd == 'help':
        args.subcommands = subcommands
        args.parser = parser
    try:
        args.func(args)
    except Exception as e:
        utils.exit('Error: %s' % encode.exception_to_unicode(e))


if __name__ == '__main__':
    main()
