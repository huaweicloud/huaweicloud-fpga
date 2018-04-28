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

import os
from getpass import getpass
from urlparse import urlparse

from oslo_utils import encodeutils

import utils

CONFIG_FILE = '~/.fiscfg'
CONFIG_TIPS = 'Try running "fisconfig" command'
CONF_VAR_REQUIRED = ('OS_AUTH_URL', 'OS_FIS_URL', 'OS_PROJECT_NAME',
                     'OS_DOMAIN_NAME')
CONF_VAR_OPTIONAL = ('OS_USER_NAME',)
SUPPORT_SCHEME = ('http', 'https')
DEFAULT_SCHEME = 'https'


def construct_url():
    """construct the auth_url and fis_url"""
    for url_env in ('OS_AUTH_URL', 'OS_FIS_URL'):
        url = os.getenv(url_env)
        os.environ[url_env + '_OLD'] = url  # save the old url

        version = '/v3' if url_env == 'OS_AUTH_URL' else ''
        if '://' not in url:
            url = '%s://%s' % (DEFAULT_SCHEME, url)
        o = urlparse(url)
        os.environ[url_env] = '%s://%s%s' % (
            o.scheme if o.scheme in SUPPORT_SCHEME else DEFAULT_SCHEME,
            o.netloc,
            version)


def read_config_and_password(password=None):
    """read the user configurations and password"""
    # open and read the config file
    try:
        config_file = open(os.path.expanduser(CONFIG_FILE), 'r')
    except IOError as e:
        utils.exit('Read config file failed: %s\n%s' % (
                   encodeutils.exception_to_unicode(e),
                   CONFIG_TIPS))
    settings_required = {}
    settings_optional = {}
    for line in config_file:
        line = line.rstrip()
        if line and not line.startswith('#'):
            li = [s.strip() for s in line.split('=')]
            if len(li) != 2 or not li[1]:
                continue
            if li[0] in CONF_VAR_REQUIRED:
                settings_required[li[0]] = li[1]
            elif li[0] in CONF_VAR_OPTIONAL:
                settings_optional[li[0]] = li[1]
    config_file.close()

    if len(settings_required) == len(CONF_VAR_REQUIRED):
        for k, v in settings_required.items():
            os.environ[k] = v
        for k, v in settings_optional.items():
            os.environ[k] = v
    else:
        missing = set(CONF_VAR_REQUIRED) - set(settings_required.keys())
        utils.exit('Missing required config variable: %s\n%s' % (
                   ', '.join(missing),
                   CONFIG_TIPS))

    # construct auth_url and fis_url
    construct_url()

    # read password
    if not password:
        username = os.getenv('OS_USER_NAME') or os.getenv('OS_DOMAIN_NAME')
        password = getpass('Please input the password of "%s": ' % username)
    os.environ['OS_PASSWORD'] = password


def read_default_config():
    """read the default configurations"""
    default = {}
    try:
        config_file = open(os.path.expanduser(CONFIG_FILE), 'r')
        for line in config_file:
            line = line.rstrip()
            if line and not line.startswith('#'):
                li = [s.strip() for s in line.split('=')]
                if len(li) == 2 and li[1]:
                    default[li[0]] = li[1]
        config_file.close()
    except:
        pass

    return (default.get('OS_AUTH_URL', ''),
            default.get('OS_FIS_URL', ''),
            default.get('OS_PROJECT_NAME', ''),
            default.get('OS_DOMAIN_NAME', ''),
            default.get('OS_USER_NAME', '')
            )


def save_config(auth_url, fis_url, project_name, domain_name, user_name):
    with open(os.path.expanduser(CONFIG_FILE), 'w') as config_file:
        config_file.write('%s = %s\n' % ('OS_AUTH_URL', auth_url))
        config_file.write('%s = %s\n' % ('OS_FIS_URL', fis_url))
        config_file.write('%s = %s\n' % ('OS_PROJECT_NAME', project_name))
        config_file.write('%s = %s\n' % ('OS_DOMAIN_NAME', domain_name))
        config_file.write('%s = %s\n' % ('OS_USER_NAME', user_name))
