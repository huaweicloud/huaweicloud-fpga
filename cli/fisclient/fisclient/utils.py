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

import hashlib
import json
import os
import re
import sys

import prettytable

import encode
import exception
import rest


def arg(*args, **kwargs):
    def _decorator(func):
        # Because of the semantics of decorator composition, if we just append
        # to the options list positional, options will appear to be backwards.
        func.__dict__.setdefault('arguments', []).insert(0, (args, kwargs))
        return func
    return _decorator


def print_list(objs, fields):
    pt = prettytable.PrettyTable(fields, caching=False)
    pt.align = 'l'

    for o in objs:
        row = []
        for field in fields:
            v = o.get(field, '')
            if isinstance(v, unicode):
                data = v
            elif isinstance(v, dict):
                data = json.dumps(v, ensure_ascii=False)
            else:
                data = str(v) if v is not None else ''
            row.append(data)
        pt.add_row(row)

    print(pt.get_string())


def exit(msg='', exit_code=1):
    if msg:
        print_err(msg)
    sys.exit(exit_code)


def print_err(msg):
    print(encode.convert_to_unicode(msg), file=sys.stderr)


def compute_md5(*args):
    m = hashlib.md5()
    for arg in args:
        m.update(str(arg))
    return m.hexdigest()


def retry_decorator(f):
    def g(*args, **kwargs):
        failed = True
        retry_cnt = 0
        while failed:
            failed = False
            try:
                return f(*args, **kwargs)
            except Exception as e:
                failed = True
                retry_cnt = retry_cnt + 1
                if retry_cnt <= 3:
                    print('%s failed, retry #%d:\n%s' % (f.__name__, retry_cnt, e))
                else:
                    print('%s failed, give up retry' % (f.__name__))
                    raise e
    return g


def _check_bucket_acl_location(bucket_name, ak, sk, host, region_id, domain_id):
    location = rest.get_bucket_location(ak, sk, bucket_name, host)
    location = location.get('LocationConstraint')
    if location != region_id:
        msg = 'Bucket "%s": location "%s" is not "%s"' % (bucket_name, location, region_id)
        raise exception.FisException(msg)

    acl = rest.get_bucket_acl(ak, sk, bucket_name, host)
    grant = acl.get('AccessControlList', {}).get('Grant', {})
    if not isinstance(grant, list):
        grant = [grant]
    permission = []
    for grantee in grant:
        if isinstance(grantee, dict) and grantee.get('Grantee', {}).get('ID') == domain_id:
            permission.append(grantee.get('Permission'))
    read_write = 'FULL_CONTROL' in permission or ('READ' in permission and 'WRITE' in permission)
    if not read_write:
        msg = 'Bucket "%s": domain "%s" does not have the READ and/or WRITE permission(s)' % (bucket_name, domain_id)
        raise exception.FisException(msg)


def is_bucket_valid(bucket_name, ak, sk, host, region_id, domain_id):
    try:
        _check_bucket_acl_location(bucket_name, ak, sk, host, region_id, domain_id)
    except Exception as e:
        print(encode.exception_to_unicode(e))
        return False
    return True


def check_bucket_name(bucket_name):
    if not re.match(u'^(?![.-])[a-z0-9.-]{3,63}(?<![.-])$', bucket_name):
        return False
    if re.match(u'.*(?:\.\.|\.-|-\.).*', bucket_name):
        return False
    if re.match(u'^(?:\d|1?\d{2}|2[0-4]\d|25[0-5])'
                u'(?:\.(\d|1?\d{2}|2[0-4]\d|25[0-5])){3}$',
                bucket_name):
        return False
    return True


def check_dcp_file(dcp_file):
    expand_file_name = os.path.expanduser(dcp_file)
    if not expand_file_name.endswith('.tar'):
        msg = '"%s" should endswith .tar suffix' % dcp_file
        raise exception.FisException(msg)
    if not os.path.isfile(expand_file_name):
        msg = '"%s" is not an existing regular file' % dcp_file
        raise exception.FisException(msg)


def _check_dcp_obs_path(dcp_path):
    if not re.match(u'[a-zA-Z0-9_./\-]{4,128}$', dcp_path):
        return False
    if dcp_path.startswith('/') or not dcp_path.endswith('.tar'):
        return False
    for d in dcp_path.split('/')[:-1]:
        if d == '' or d.startswith('.') or d.endswith('.'):
            return False
    return True


def _check_log_obs_directory(log_directory):
    if not re.match(u'[a-zA-Z0-9_./\-]{0,64}$', log_directory):
        return False
    if log_directory.startswith('/') or log_directory.endswith('/'):
        return False
    if log_directory != '':
        for d in log_directory.split('/'):
            if d == '' or d.startswith('.') or d.endswith('.'):
                return False
    return True


_check_dict = {
    'dcp_obs_path': _check_dcp_obs_path,
    'log_obs_directory': _check_log_obs_directory,
    'name': lambda value: re.match(u'[a-zA-Z0-9-_]{1,64}$', value),
    'description': lambda value: re.match(
        u'[\u4e00-\u9fa5\u3002\uff0ca-zA-Z0-9-_., ]{0,255}$',
        value),
    'fpga_image_id': lambda value: re.match(u'[0-9a-f]{32}$', value),
    'image_id': lambda value: re.match(
        u'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$',
        value),
    'page': lambda value: re.match(u'\d+$', value) and 1 <= int(value) < 65535,
    'size': lambda value: re.match(u'\d+$', value) and 1 <= int(value) <= 100,
}


def check_param(**kwargs):
    for key, value in kwargs.items():
        check_cond = _check_dict.get(key, lambda _: True)
        if not check_cond(value):
            raise exception.ParameterException(key, value)
