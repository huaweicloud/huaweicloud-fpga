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


from encode import convert_to_unicode


class FisException(Exception):
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return convert_to_unicode(self.message)


class HttpException(FisException):
    def __init__(self, code, reason, message=None):
        self.code = code
        self.reason = reason
        self.message = message

    def __str__(self):
        if self.message:
            msg = u'%s %s\n%s' % (self.code,
                                  self.reason,
                                  self.message)
        else:
            msg = u'%s %s' % (self.code, self.reason)
        return msg


class ParameterException(FisException):
    def __init__(self, key, value=None):
        self.key = key
        self.value = value

    def __str__(self):
        if self.value:
            msg = u'parameter "%s" value (%s) is malformed' % (self.key,
                                                               self.value)
        else:
            msg = u'parameter "%s" is empty' % (self.key)
        return msg
