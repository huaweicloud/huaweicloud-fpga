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

from getpass import getpass
import os
import sys

from fisclient.common import config
from fisclient.fisshell import FisShell

# enable command line editing using GNU readline on Linux
if sys.platform.startswith('linux'):
    import readline     # noqa: F401


def get_domain_user_name(domain_user_name):
    names = domain_user_name.split(',')
    if len(names) == 1:
        return names[0].strip(), ''
    elif len(names) >= 2:
        return names[0].strip(), names[1].strip()
    else:
        return '', ''


def main():
    try:
        # get configurations
        print('Enter new values or accept defaults in brackets with Enter\n')
        auth_url, fis_url, project_name, domain_name, user_name = \
            config.read_default_config()
        if user_name:
            domain_user_name = '%s,%s' % (domain_name, user_name)
        else:
            domain_user_name = domain_name

        os.environ['OS_AUTH_URL'] = raw_input(
            'IAM Endpoint [%s]: ' % auth_url).strip() or auth_url
        if not os.environ['OS_AUTH_URL']:
            exit('Error, empty input!')

        os.environ['OS_FIS_URL'] = raw_input(
            'FIS Endpoint [%s]: ' % fis_url).strip() or fis_url
        if not os.environ['OS_FIS_URL']:
            exit('Error, empty input!')

        os.environ['OS_PROJECT_NAME'] = raw_input(
            'Project Name [%s]: ' % project_name).strip() or project_name
        if not os.environ['OS_PROJECT_NAME']:
            exit('Error, empty input!')

        domain_user_name_tmp = (raw_input(
            'Account User Name [%s]: ' % domain_user_name).strip()
            or domain_user_name)
        if not domain_user_name_tmp:
            exit('Error, empty input!')
        os.environ['OS_DOMAIN_NAME'], os.environ['OS_USER_NAME'] = \
            get_domain_user_name(domain_user_name_tmp)

        config.construct_url()

        # get password
        username = os.getenv('OS_USER_NAME') or os.getenv('OS_DOMAIN_NAME')
        password = getpass('\nPlease input the password of "%s": ' % username)
        os.environ['OS_PASSWORD'] = password
    except (KeyboardInterrupt, EOFError):
        exit()

    # check config and password
    print('Test supplied configurations and password')
    FisShell().check_config_and_password()

    # save config
    print('Save the supplied configurations')
    config.save_config(
        os.environ['OS_AUTH_URL_OLD'],
        os.environ['OS_FIS_URL_OLD'],
        os.environ['OS_PROJECT_NAME'],
        os.environ['OS_DOMAIN_NAME'],
        os.environ['OS_USER_NAME'])


if __name__ == '__main__':
    main()
