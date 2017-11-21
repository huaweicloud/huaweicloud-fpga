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
import threading
import time

from getpass import getpass
from oslo_utils import encodeutils
from urlparse import urlparse

from fisclient.common import utils
from fisclient.fisshell import FisShell, OS_ENTRY_WRAPSHELL


# enable command line editing using GNU readline on Linux
if sys.platform.startswith('linux'):
    import readline     # noqa: F401


CONF_VARIABLE = ('OS_AUTH_URL', 'OS_FIS_URL', 'OS_USER_ID', 'OS_TENANT_ID')
CLI_PROMPT = '[fisclient] > '
FIS_CONFIG_FILE = '/etc/cfg.file'
TIME_LIMIT = 3600
timeout = False


class Timer(threading.Thread):
    """countdown timer"""

    def __init__(self, interval=60):
        super(Timer, self).__init__()
        self.interval = interval
        self.count = interval
        self.lock = threading.Lock()
        self.force_timeup = False

    def reset(self):
        self.lock.acquire()
        self.count = self.interval
        self.lock.release()

    def countdown(self):
        self.lock.acquire()
        self.count -= 1
        if self.count == 0 or self.force_timeup:
            print('\ntime out, please press Enter to exit')
            global timeout
            timeout = True
        self.lock.release()

    def timeup(self):
        self.force_timeup = True

    def run(self):
        while True:
            if timeout:
                return
            time.sleep(1)
            self.countdown()


def read_config_file():
    # open and read the config file
    try:
        config_file = open(FIS_CONFIG_FILE, 'r')
    except IOError as e:
        utils.exit('read config file failed: %s' %
                   encodeutils.exception_to_unicode(e))
    setting = {}
    for n, line in enumerate(config_file, 1):
        line = line.rstrip()
        if line and not line.startswith('#'):
            li = [s.strip() for s in line.split('=')]
            if len(li) == 2 and li[0] in CONF_VARIABLE and li[1]:
                setting[li[0]] = li[1]
    config_file.close()

    if len(setting) == len(CONF_VARIABLE):
        for k, v in setting.items():
            os.environ[k] = v
    else:
        missing = set(CONF_VARIABLE) - set(setting.keys())
        utils.exit('missing config variable: %s' % ', '.join(missing))

    # set api version, for future use
    os.environ['OS_FIS_API_VERSION'] = '1'

    # check url and set no_proxy
    no_proxy = []
    for url_env in ('OS_AUTH_URL', 'OS_FIS_URL'):
        o = urlparse(os.getenv(url_env))
        if o.scheme and o.netloc:
            version = '/v3' if url_env == 'OS_AUTH_URL' else ''
            os.environ[url_env] = '%s://%s%s' % (o.scheme, o.netloc, version)
        else:
            utils.exit('invalid %s: scheme or netloc is empty' % url_env)
        no_proxy.append(o.netloc)
    os.environ['no_proxy'] = ','.join(no_proxy)


def read_password():
    # read password
    os_password = getpass("please input the password:\n")
    os.environ["OS_PASSWORD"] = os_password


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
        read_config_file()
        read_password()
    except (KeyboardInterrupt, EOFError):
        exit()

    # check password by getting token
    fis_shell = FisShell()
    fis_shell.get_token()

    # start timer
    timer = Timer(TIME_LIMIT)
    timer.daemon = True
    timer.start()

    while True:
        # read user's cmd
        try:
            cmd = raw_input(CLI_PROMPT)
        except (KeyboardInterrupt, EOFError):
            exit()
        if timeout:
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
