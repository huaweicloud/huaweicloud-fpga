# Copyright 2012 OpenStack Foundation
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

import json
import os
import re
import sys

import prettytable
import six
from oslo_utils import encodeutils
from oslo_utils import importutils

from fisclient.common import exceptions


def arg(*args, **kwargs):
    def _decorator(func):
        # Because of the semantics of decorator composition if we just append
        # to the options list positional options will appear to be backwards.
        func.__dict__.setdefault('arguments', []).insert(0, (args, kwargs))
        return func
    return _decorator


def entry(shell):
    def _decorator(func):
        func.__dict__['OS_ENTRY_SHELL'] = shell
        return func
    return _decorator


def print_list(objs, fields, formatters=None, field_settings=None):
    formatters = formatters or {}
    field_settings = field_settings or {}
    pt = prettytable.PrettyTable([f for f in fields], caching=False)
    pt.align = 'l'

    for o in objs:
        row = []
        for field in fields:
            if field in field_settings:
                for setting, value in six.iteritems(field_settings[field]):
                    setting_dict = getattr(pt, setting)
                    setting_dict[field] = value

            if field in formatters:
                row.append(formatters[field](o))
            else:
                v = o.get(field, '')
                if isinstance(v, unicode):
                    data = v
                elif isinstance(v, dict):
                    data = json.dumps(v)
                else:
                    data = str(v) if v is not None else ''
                row.append(data)
        pt.add_row(row)

    print(pt.get_string())


def env(*vars, **kwargs):
    """Search for the first defined of possibly many env vars.

    Returns the first environment variable defined in vars, or
    returns the default defined in kwargs.
    """
    for v in vars:
        value = os.environ.get(v, None)
        if value:
            return value
    return kwargs.get('default', '')


def import_versioned_module(version, submodule=None):
    module = 'fisclient.v%s' % version
    if submodule:
        module = '.'.join((module, submodule))
    return importutils.import_module(module)


def exit(msg='', exit_code=1):
    if msg:
        print_err(msg)
    sys.exit(exit_code)


def print_err(msg):
    print(encodeutils.safe_decode(msg), file=sys.stderr)


def _check_location(location):
    i = location.find(':')
    if i == -1:
        return False
    bucket_name = location[:i]
    file_name = location[i+1:]

    # check bucket name
    if not re.match(u'^(?![.-])[a-z0-9.-]{3,63}(?<![.-])$', bucket_name):
        return False
    if re.match(u'.*(?:\.\.|\.-|-\.).*', bucket_name):
        return False
    if re.match(u'^(?:\d|1?\d{2}|2[0-4]\d|25[0-5])'
                u'(?:\.(\d|1?\d{2}|2[0-4]\d|25[0-5])){3}$',
                bucket_name):
        return False

    # check file name
    if not file_name.endswith((u'.bin', u'.xclbin')):
        return False
    if not re.match(u'[a-zA-Z0-9_./\-]{4,64}$', file_name):
        return False

    return True


def _check_metadata(metadata):
    try:
        obj = json.loads(metadata)
    except ValueError:
        return False
    if not isinstance(obj, dict):
        return False
    if len(json.dumps(obj)) > 1024:
        return False
    return True


def _check_page(page):
    if not re.match(u'\d+', page):
        return False
    try:
        if 1 <= int(page) < 65535:
            return True
        else:
            return False
    except ValueError:
        return False


def _check_size(size):
    if not re.match(u'\d+', size):
        return False
    try:
        if 1 <= int(size) <= 100:
            return True
        else:
            return False
    except ValueError:
        return False


_check_dict = {
    'location': _check_location,
    'name': lambda value: re.match(u'[a-zA-Z0-9-_]{1,64}$', value),
    'metadata': lambda value: _check_metadata(value),
    'description': lambda value: re.match(
        u'[\u4e00-\u9fa5\u3002\uff0ca-zA-Z0-9-_., ]{0,255}$',
        value),
    'fpga_image_id': lambda value: re.match(u'[0-9a-f]{32}$', value),
    'image_id': lambda value: re.match(
        u'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$',
        value),
    'page': _check_page,
    'size': _check_size,
}


def check_param(**kwargs):
    for key, value in kwargs.items():
        check_cond = _check_dict.get(key, lambda _: True)
        if not check_cond(value):
            raise exceptions.ParameterErrorException(key, value, 'malformed')
