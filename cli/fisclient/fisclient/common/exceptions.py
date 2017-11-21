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

import re
import sys

import six


class FisException(Exception):
    """An error occurred."""
    def __init__(self, message=None):
        self.message = message

    def __str__(self):
        return self.message or self.__class__.__doc__


class CommandError(FisException):
    """Invalid usage of CLI."""


class ParameterErrorException(FisException):
    """The base exception class for all exceptions this library raises."""
    def __init__(self, key, value=None, message=None):
        super(ParameterErrorException, self).__init__(message)
        self.key = key
        self.value = value

    def __str__(self):
        if self.value:
            return 'parameter %s (%s) is %s' % (self.key,
                                                self.value,
                                                self.message)
        else:
            return 'parameter %s is empty' % (self.key)


class NoUniqueMatch(Exception):
    """Multiple entities found instead of one."""
    pass


class HTTPException(Exception):
    """Base exception for all HTTP-derived exceptions."""
    code = 'N/A'
    reasons = {300: 'Multiple Choices', 301: 'Moved Permanently',
               400: 'Bad Request', 401: 'Unauthorized',
               403: 'Forbidden', 404: 'Not Found', 405: 'Method Not Allowed',
               408: 'Request Timeout', 409: 'Conflict',
               500: 'Internal Server Error', 501: 'Not Implemented',
               502: 'Bad Gateway', 503: 'Service Unavailable',
               504: 'Gateway Timeout'}

    def __init__(self, details=None):
        self.reason = self.reasons.get(self.code, "N/A")
        self.details = details or self.__class__.__name__

    def __str__(self):
        return "%s %s\n%s" % (self.code,
                              self.reason,
                              self.details)


class HTTPMultipleChoices(HTTPException):
    code = 300


class HTTPMovedPermanently(HTTPException):
    code = 301


class HTTPBadRequest(HTTPException):
    code = 400


class HTTPUnauthorized(HTTPException):
    code = 401


class HTTPForbidden(HTTPException):
    code = 403


class HTTPNotFound(HTTPException):
    code = 404


class HTTPMethodNotAllowed(HTTPException):
    code = 405


class HTTPRequestTimeout(HTTPException):
    code = 408


class HTTPConflict(HTTPException):
    code = 409


class HTTPInternalServerError(HTTPException):
    code = 500


class HTTPNotImplemented(HTTPException):
    code = 501


class HTTPBadGateway(HTTPException):
    code = 502


class HTTPServiceUnavailable(HTTPException):
    code = 503


class HTTPGatewayTimeout(HTTPException):
    code = 504


# NOTE(bcwaldon): Build a mapping of HTTP codes to corresponding exception
# classes
_code_map = {}
for obj_name in dir(sys.modules[__name__]):
    if obj_name.startswith('HTTP'):
        obj = getattr(sys.modules[__name__], obj_name)
        _code_map[obj.code] = obj


def from_response(response, body=None):
    """Return an instance of an HTTPException based on httplib response."""
    cls = _code_map.get(response.status_code, HTTPException)
    try:
        if body and 'json' in response.headers['content-type']:
            # Retrieve the "message" attribute and join all of them together
            messages = [obj.get('message') for obj in response.json().values()]
            details = '\n'.join(i for i in messages if i is not None)
            return cls(details=details)
        elif body and 'html' in response.headers['content-type']:
            # Split the lines, strip whitespace and inline HTML
            details = [re.sub(r'<.+?>', '', i.strip())
                       for i in response.text.splitlines()]
            details = [i for i in details if i]
            # Remove duplicates from the list.
            details_seen = set()
            details_temp = []
            for i in details:
                if i not in details_seen:
                    details_temp.append(i)
                    details_seen.add(i)
            # Return joined string
            details = '\n'.join(details_temp)
            return cls(details=details)
        elif body:
            if six.PY3:
                body = body.decode('utf-8')
            details = body.replace('\n\n', '\n')
            return cls(details=details)
        return cls()
    except:
        return cls(details=body.strip())
