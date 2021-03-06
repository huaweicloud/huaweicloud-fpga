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

import json
import os

import encode
import exception
import rest
import utils


CONFIG_FILE = os.path.expanduser('~/.fiscfg')
CONFIG_TIPS = 'Consider running \033[31mfis configure\033[0m command to (re)create one.'
CONFIG_VAR = ('OS_ACCESS_KEY', 'OS_SECRET_KEY', 'OS_BUCKET_NAME',
              'OS_REGION_ID', 'OS_DOMAIN_ID', 'OS_PROJECT_ID',
              'OS_OBS_ENDPOINT', 'OS_IAM_ENDPOINT', 'OS_VPC_ENDPOINT',
              'OS_FIS_ENDPOINT', 'OS_CONFIG_HASH')
DNS_CONFIG_FILE = '/etc/resolv.conf'

az_region_map = {
    'cn-north-1a': 'cn-north-1',
    'cn-north-1b': 'cn-north-1',
    'cn-east-2a': 'cn-east-2',
    'cn-east-2b': 'cn-east-2',
    'cn-east-2c': 'cn-east-2',
    'cn-south-1a': 'cn-south-1',
    'cn-south-2b': 'cn-south-1',
    'cn-south-1c': 'cn-south-1',
}

endpoints = {
    'cn-north-1': {
        'obs': 'obs.cn-north-1.myhuaweicloud.com',
        'iam': 'iam.myhuaweicloud.com',
        'vpc': 'vpc.cn-north-1.myhuaweicloud.com',
        'fis': 'ecs.cn-north-1.myhuaweicloud.com',
        'dns': ['100.125.1.250', '100.125.21.250']
    },
    'cn-south-1': {
        'obs': 'obs.cn-south-1.myhuaweicloud.com',
        'iam': 'iam.myhuaweicloud.com',
        'vpc': 'vpc.cn-south-1.myhuaweicloud.com',
        'fis': 'ecs.cn-south-1.myhuaweicloud.com',
        'dns': ['100.125.1.250', '100.125.136.29']
    },
    'cn-east-2': {
        'obs': 'obs.cn-east-2.myhuaweicloud.com',
        'iam': 'iam.myhuaweicloud.com',
        'vpc': 'vpc.cn-east-2.myhuaweicloud.com',
        'fis': 'ecs.cn-east-2.myhuaweicloud.com',
        'dns': ['100.125.17.29', '100.125.135.29']
    },
}


def configure_intranet_dns_ecs(region):
    try:
        dns = endpoints.get(region, {}).get('dns')
        if dns is None:
            return

        configure_dns = True
        if os.path.exists(DNS_CONFIG_FILE):
            with open(DNS_CONFIG_FILE) as resolv:
                record = []
                for line in resolv:
                    record = line.split()
                    if len(record) < 2:
                        continue
                    if record[0] == 'nameserver':
                        break
                if len(record) >= 2 and record[0] == 'nameserver' and record[1] in dns:
                    configure_dns = False

        if configure_dns:
            with open('/etc/resolv.conf', 'w') as resolv:
                resolv.write('; generated by fisclient\nsearch openstacklocal novalocal\n')
                resolv.write('nameserver %s\n' % dns[0])
                resolv.write('nameserver %s\n' % dns[1])
    except Exception as e:
        utils.print_err('Configure private DNS of ECS failed: %s' % encode.exception_to_unicode(e))


def configure_intranet_dns_vpc(ak, sk, project_id, region, ecs_host, vpc_host):
    try:
        dns = endpoints.get(region, {}).get('dns')
        instance_id = rest.get_instance_id_from_metadata()
        if dns is None or instance_id is None:
            return
        nics = rest.get_os_interface(ak, sk, project_id, region, ecs_host, instance_id)
        for nic in nics.get('interfaceAttachments', []):
            net_id = nic.get('net_id')
            subnet = rest.get_subnet(ak, sk, project_id, region, vpc_host, net_id).get('subnet', {})
            if subnet.get('primary_dns') in dns:
                continue
            vpc_id = subnet.get('vpc_id')
            dns_body = {'subnet': {'name': subnet.get('name'), 'primary_dns': dns[0], 'secondary_dns': dns[1]}}
            rest.put_subnet(ak, sk, project_id, region, vpc_host, vpc_id, net_id, json.dumps(dns_body))
    except Exception as e:
        msg = encode.exception_to_unicode(e)
        if getattr(e, 'code', None) == 404:
            msg += ', \033[31mTips=Maybe you are not in your own ECS\033[0m'
        utils.print_err('Check private DNS of VPC failed: %s' % msg)


def get_endpoint(region_id, service):
    return endpoints.get(region_id).get(service)


def _read_config_and_update(config_file, update_dict):
    for line in config_file:
        line = line.rstrip()
        if line and not line.startswith('#'):
            words = [word.strip() for word in line.split('=')]
            if len(words) == 2 and words[0] in CONFIG_VAR and words[1]:
                update_dict[words[0]] = words[1]


def read_config_and_verify():
    """read the current configurations"""
    try:
        with open(CONFIG_FILE, 'r') as config_file:
            _read_config_and_update(config_file, os.environ)

            config_hash = utils.compute_md5(os.getenv('OS_ACCESS_KEY'),
                                            os.getenv('OS_SECRET_KEY'),
                                            os.getenv('OS_BUCKET_NAME'),
                                            os.getenv('OS_REGION_ID'),
                                            os.getenv('OS_DOMAIN_ID'),
                                            os.getenv('OS_PROJECT_ID'),
                                            os.getenv('OS_OBS_ENDPOINT'),
                                            os.getenv('OS_IAM_ENDPOINT'),
                                            os.getenv('OS_VPC_ENDPOINT'),
                                            os.getenv('OS_FIS_ENDPOINT'))
            if config_hash != os.getenv('OS_CONFIG_HASH'):
                raise exception.FisException('%s is corrupted' % CONFIG_FILE)
    except Exception as e:
        utils.exit('Read configuration file failed: %s\n%s' % (
                   encode.exception_to_unicode(e),
                   CONFIG_TIPS))


def read_current_config():
    """read the default configurations"""
    default = {}
    try:
        with open(CONFIG_FILE, 'r') as config_file:
            _read_config_and_update(config_file, default)
    except Exception:
        pass
    return default


def save_config(access_key, secret_key, bucket_name,
                region_id, domain_id, project_id,
                obs_endpoint, iam_endpoint, vpc_endpoint,
                fis_endpoint, compute_hash=True):
    config_hash = utils.compute_md5(access_key, secret_key, bucket_name,
                                    region_id, domain_id, project_id,
                                    obs_endpoint, iam_endpoint, vpc_endpoint,
                                    fis_endpoint)
    with open(CONFIG_FILE, 'w') as config_file:
        config_file.write('%s = %s\n' % ('OS_ACCESS_KEY', access_key))
        config_file.write('%s = %s\n' % ('OS_SECRET_KEY', secret_key))
        config_file.write('%s = %s\n' % ('OS_BUCKET_NAME', bucket_name))
        config_file.write('%s = %s\n' % ('OS_REGION_ID', region_id))
        config_file.write('%s = %s\n' % ('OS_DOMAIN_ID', domain_id))
        config_file.write('%s = %s\n' % ('OS_PROJECT_ID', project_id))
        config_file.write('%s = %s\n' % ('OS_OBS_ENDPOINT', obs_endpoint))
        config_file.write('%s = %s\n' % ('OS_IAM_ENDPOINT', iam_endpoint))
        config_file.write('%s = %s\n' % ('OS_VPC_ENDPOINT', vpc_endpoint))
        config_file.write('%s = %s\n' % ('OS_FIS_ENDPOINT', fis_endpoint))
        if compute_hash:
            config_file.write('%s = %s\n' % ('OS_CONFIG_HASH', config_hash))
