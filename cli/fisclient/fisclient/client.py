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

from fisclient.common import utils


def Client(version=None, endpoint=None, session=None, *args, **kwargs):
    """Client for the Cloud Service Fis API.

    Generic client for the  Cloud Service Fis API. See version classes
    for specific details.
    """

    kwargs.setdefault('endpoint_override', endpoint)
    module = utils.import_versioned_module(int(version), 'client')
    client_class = getattr(module, 'Client')
    return client_class(*args, session=session, **kwargs)
