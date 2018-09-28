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


def exception_to_unicode(exc):
    return unicode(exc)


def convert_to_unicode(string, errors="replace"):
    """Convert UTF-8 'string' to Unicode"""
    if isinstance(string, unicode):
        return string
    try:
        return unicode(string, "UTF-8", errors)
    except UnicodeDecodeError:
        raise UnicodeDecodeError("Conversion to unicode failed: %r" % string)


def convert_to_string(string, errors="replace"):
    """Convert Unicode to UTF-8 'string'"""
    if not isinstance(string, unicode):
        return string
    try:
        return string.encode("UTF-8", errors)
    except UnicodeEncodeError:
        raise UnicodeEncodeError("Conversion from unicode failed: %r" % string)
