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

import os

import encode
import exception
import utils


CONFIG_FILE = '~/.fiscfg'
CONFIG_TIPS = 'Consider running \033[31mfis configure\033[0m command to (re)create one.'
CONFIG_VAR = ('OS_ACCESS_KEY', 'OS_SECRET_KEY', 'OS_REGION_ID',
              'OS_BUCKET_NAME', 'OS_DOMAIN_ID', 'OS_PROJECT_ID',
              'OS_OBS_ENDPOINT', 'OS_IAM_ENDPOINT', 'OS_FIS_ENDPOINT',
              'OS_CONFIG_HASH')

endpoints = {
    'cn-north-1': {
        'obs': 'obs.cn-north-1.myhwclouds.com',
        'iam': 'iam.cn-north-1.myhuaweicloud.com',
        'fis': 'ecs.cn-north-1.myhuaweicloud.com',
        'dns': ['100.125.1.250', '100.125.21.250']
    },
    'cn-south-1': {
        'obs': 'obs.cn-south-1.myhwclouds.com',
        'iam': 'iam.cn-south-1.myhuaweicloud.com',
        'fis': 'ecs.cn-south-1.myhuaweicloud.com',
        'dns': ['100.125.1.250', '100.125.136.29']
    },
    'cn-east-2': {
        'obs': 'obs.cn-east-2.myhwclouds.com',
        'iam': 'iam.cn-east-2.myhuaweicloud.com',
        'fis': 'ecs.cn-east-2.myhuaweicloud.com',
        'dns': ['100.125.17.29', '100.125.135.29']
    },
}

DNS_TIPS = '\n\033[31mNote: Intranet DNS will bring a more stable and fast network environment, but we found that it may NOT be configured correctly. See section "Configuring an Intranet DNS" of the FACS User Guide for more details.\033[0m'


def check_intranet_dns(region_id):
    try:
        dns = endpoints.get(region_id, {}).get('dns')
        if not dns:
            return
        with open('/etc/resolv.conf') as resolv:
            for line in resolv:
                record = line.split()
                if len(record) < 2:
                    continue
                if record[0] == 'nameserver':
                    if record[1] not in dns:
                        print(DNS_TIPS)
                    return
            print(DNS_TIPS)
    except Exception:
        print(DNS_TIPS)


def get_endpoint(region_id, service):
    if region_id in endpoints:
        return endpoints.get(region_id).get(service)
    else:
        return endpoints.get('cn-north-1').get(service)


def _read_config_and_update(config_file, update_dict):
    for line in config_file:
        line = line.rstrip()
        if line and not line.startswith('#'):
            words = [word.strip() for word in line.split('=')]
            if len(words) == 2 and words[1] and words[0] in CONFIG_VAR:
                update_dict[words[0]] = words[1]


def read_config_and_verify():
    """read the current configurations"""
    try:
        with open(os.path.expanduser(CONFIG_FILE), 'r') as config_file:
            _read_config_and_update(config_file, os.environ)

            config_hash = utils.compute_md5(os.getenv('OS_ACCESS_KEY'),
                                            os.getenv('OS_SECRET_KEY'),
                                            os.getenv('OS_REGION_ID'),
                                            os.getenv('OS_BUCKET_NAME'),
                                            os.getenv('OS_DOMAIN_ID'),
                                            os.getenv('OS_PROJECT_ID'),
                                            os.getenv('OS_OBS_ENDPOINT'),
                                            os.getenv('OS_IAM_ENDPOINT'),
                                            os.getenv('OS_FIS_ENDPOINT'))
            if config_hash != os.getenv('OS_CONFIG_HASH'):
                raise exception.FisException('Config file is damaged')
    except Exception as e:
        utils.exit('Read config file failed: %s\n%s' % (
                   encode.exception_to_unicode(e),
                   CONFIG_TIPS))


def read_current_config():
    """read the default configurations"""
    default = {}
    try:
        with open(os.path.expanduser(CONFIG_FILE), 'r') as config_file:
            _read_config_and_update(config_file, default)
    except Exception:
        pass
    return default


def save_config(access_key, secret_key, region_id,
                bucket_name, domain_id, project_id,
                obs_endpoint, iam_endpoint, fis_endpoint,
                compute_hash=True):
    config_hash = utils.compute_md5(access_key, secret_key, region_id,
                                    bucket_name, domain_id, project_id,
                                    obs_endpoint, iam_endpoint, fis_endpoint)
    with open(os.path.expanduser(CONFIG_FILE), 'w') as config_file:
        config_file.write('%s = %s\n' % ('OS_ACCESS_KEY', access_key))
        config_file.write('%s = %s\n' % ('OS_SECRET_KEY', secret_key))
        config_file.write('%s = %s\n' % ('OS_REGION_ID', region_id))
        config_file.write('%s = %s\n' % ('OS_BUCKET_NAME', bucket_name))
        config_file.write('%s = %s\n' % ('OS_DOMAIN_ID', domain_id))
        config_file.write('%s = %s\n' % ('OS_PROJECT_ID', project_id))
        config_file.write('%s = %s\n' % ('OS_OBS_ENDPOINT', obs_endpoint))
        config_file.write('%s = %s\n' % ('OS_IAM_ENDPOINT', iam_endpoint))
        config_file.write('%s = %s\n' % ('OS_FIS_ENDPOINT', fis_endpoint))
        if compute_hash:
            config_file.write('%s = %s\n' % ('OS_CONFIG_HASH', config_hash))
