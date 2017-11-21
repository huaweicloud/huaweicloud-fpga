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

from collections import OrderedDict

import requests
import six
from keystoneauth1 import adapter
from oslo_utils import encodeutils
from requests.packages.urllib3.exceptions import InsecureRequestWarning

from fisclient.common import exceptions

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)


class SessionClient(adapter.Adapter):

    def __init__(self, session, **kwargs):
        kwargs.setdefault('user_agent', 'fisclient')
        kwargs.setdefault('service_type', 'fis')
        super(SessionClient, self).__init__(session, **kwargs)

    def request(self, url, method, **kwargs):
        headers = self._encode_headers(kwargs.pop('headers', {}))
        kwargs['raise_exc'] = False

        resp = super(SessionClient, self).request(url,
                                                  method,
                                                  headers=headers,
                                                  **kwargs)
        return self._handle_response(resp)

    def _handle_response(self, resp):
        if not resp.ok:
            raise exceptions.from_response(resp, resp.content)
        elif resp.status_code == requests.codes.MULTIPLE_CHOICES:
            raise exceptions.from_response(resp)

        content_type = resp.headers.get('Content-Type')
        if content_type and content_type.startswith('application/json'):
            try:
                body = resp.json(object_pairs_hook=OrderedDict)
            except:
                msg = 'Response is not a valid json type\n%s' % resp.content
                raise exceptions.FisException(msg)
        else:
            body = None

        return resp, body

    @staticmethod
    def _close_after_stream(response, chunk_size):
        """Iterate over the content and ensure the response is closed after."""
        # Yield each chunk in the response body
        for chunk in response.iter_content(chunk_size=chunk_size):
            yield chunk
        # Once we're done streaming the body, ensure everything is closed.
        # This will return the connection to the HTTPConnectionPool in urllib3
        # and ideally reduce the number of HTTPConnectionPool full warnings.
        response.close()

    @staticmethod
    def _encode_headers(headers):
        return {encodeutils.safe_encode(h): encodeutils.safe_encode(v)
                for h, v in six.iteritems(headers) if v is not None}


def get_http_client(session=None, **kwargs):
    if session:
        return SessionClient(session, **kwargs)
    else:
        raise AttributeError('Constructing a client must contain a session')
