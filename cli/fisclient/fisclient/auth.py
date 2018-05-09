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

import datetime
import hmac
from hashlib import sha256
from urllib import quote

from encode import convert_to_unicode, convert_to_string


def _uri_encode(param, quote_backslashes=True, unicode_output=False):
    if quote_backslashes:
        safe_chars = "~"
    else:
        safe_chars = "~/"
    param = convert_to_string(param)
    param = quote(param, safe=safe_chars)
    if unicode_output:
        param = convert_to_unicode(param)
    else:
        param = convert_to_string(param)
    return param


def _format_param_str(params, always_have_equal=False):
    if not params:
        return ""

    param_str = ""
    equal_str = always_have_equal and u'=' or ''
    for key in sorted(params.keys()):
        value = params[key]
        if value in (None, ""):
            param_str += "&%s%s" % (_uri_encode(key, unicode_output=True), equal_str)
        else:
            param_str += "&%s=%s" % (_uri_encode(key, unicode_output=True), _uri_encode(value, unicode_output=True))
    return "?" + param_str[1:]


def _sign(key, msg):
    return hmac.new(key, convert_to_string(msg), sha256).digest()


def _getSignatureKey(key, dateStamp, regionName, serviceName):
    kDate = _sign(convert_to_string('SDK' + key), dateStamp)
    kRegion = _sign(kDate, regionName)
    kService = _sign(kRegion, serviceName)
    kSigning = _sign(kService, 'sdk_request')
    return kSigning


def sign_request_v4(access_key, secret_key, method, host,
                    canonical_uri, region='cn-north-1',
                    service='ecs', params=None, body=b''):
    if not canonical_uri.endswith('/'):
        canonical_uri += '/'

    t = datetime.datetime.utcnow()
    sdkdate = t.strftime('%Y%m%dT%H%M%SZ')
    datestamp = t.strftime('%Y%m%d')

    signing_key = _getSignatureKey(secret_key, datestamp, region, service)

    canonical_uri = _uri_encode(canonical_uri, quote_backslashes=False, unicode_output=True)
    canonical_querystring = _format_param_str(params, always_have_equal=True).lstrip('?')

    payload_hash = convert_to_unicode(sha256(convert_to_string(body)).hexdigest())

    canonical_headers = {'host': host,
                         'x-sdk-date': sdkdate
                         }
    signed_headers = 'host;x-sdk-date'

    canonical_headers_str = ''
    for k, v in sorted(canonical_headers.items()):
        canonical_headers_str += k + ":" + v + "\n"

    canonical_headers = canonical_headers_str
    signed_headers = ';'.join(sorted(signed_headers.split(';')))

    canonical_request = method + '\n' + canonical_uri + '\n' + canonical_querystring + '\n' + canonical_headers + '\n' + signed_headers + '\n' + payload_hash

    algorithm = 'SDK-HMAC-SHA256'
    credential_scope = datestamp + '/' + region + '/' + service + '/' + 'sdk_request'
    string_to_sign = algorithm + '\n' + sdkdate + '\n' + credential_scope + '\n' + convert_to_unicode(sha256(convert_to_string(canonical_request)).hexdigest())

    signature = convert_to_unicode(hmac.new(signing_key, convert_to_string(string_to_sign), sha256).hexdigest())
    authorization_header = algorithm + ' ' + 'Credential=' + access_key + '/' + credential_scope + ', ' + 'SignedHeaders=' + signed_headers + ', ' + 'Signature=' + signature
    new_headers = {'X-Sdk-Date': sdkdate,
                   'Authorization': authorization_header,
                   'Host': host}
    return new_headers
