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

import os
import shlex
import sys

from oslo_utils import encodeutils

from fisclient.common import config, utils
from fisclient.fisshell import FisShell, OS_ENTRY_WRAPSHELL


# enable command line editing using GNU readline on Linux
if sys.platform.startswith('linux'):
    import readline     # noqa: F401


def run_cmd(shell, argv):
    try:
        safe_argv = [encodeutils.safe_decode(a) for a in argv]
        shell.main(safe_argv)
    except SystemExit:
        return
    except Exception as e:
        utils.print_err(encodeutils.exception_to_unicode(e))


def main():
    os.environ['OS_ENTRY_SHELL'] = OS_ENTRY_WRAPSHELL

    # read config and password
    try:
        config.read_config_and_password()
    except (KeyboardInterrupt, EOFError):
        exit()

    # check password by getting token
    fis_shell = FisShell()
    fis_shell.get_token()

    # start timer
    timer = utils.Timer()
    timer.daemon = True
    timer.start()

    while True:
        # read user's cmd
        try:
            cmd = raw_input('[fisclient] > ')
        except (KeyboardInterrupt, EOFError):
            exit()
        if timer.timeout:
            exit()

        # reset timer
        timer.reset()

        # parse and execute cmd
        try:
            argv = shlex.split(cmd)
        except Exception as e:
            utils.print_err('input error: %s' %
                            encodeutils.exception_to_unicode(e))
            continue

        if not argv or argv[0] == '':
            continue
        elif argv[0] == 'fis':
            run_cmd(fis_shell, argv[1:])
        elif 'quit'.startswith(argv[0]) and len(argv) == 1:
            exit()
        else:
            fis_shell.sub_parser.print_help()
            utils.print_err('\nerror: unknown command \'%s\'\n' % cmd)


if __name__ == '__main__':
    main()
