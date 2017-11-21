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

import os
from fisclient.v1 import base


class Fis(base.Resource):
    def __repr__(self):
        return '<%s>' % self._info

    def update(self, **fields):
        self.manager.update(self, **fields)

    def delete(self, **kwargs):
        return self.manager.delete(self)

    def data(self, **kwargs):
        return self.manager.data(self, **kwargs)


class FisManager(base.ManagerWithFind):
    resource_class = Fis

    def __init__(self, client):
        self.client = client
        # no certificate verification
        self.client.session.verify = False

    def fpga_image_register(self, **kwargs):
        tenant_id = os.getenv('OS_TENANT_ID')
        location = kwargs.get('location', None)
        name = kwargs.get('name', None)
        metadata = kwargs.get('metadata', None)
        description = kwargs.get('description', None)
        data = {
            'fpga_image': {
                'location': location,
                'name': name,
                'metadata': metadata,
                'description': description
            }
        }
        url = '/v1/%s/cloudservers/fpga_image' % tenant_id
        resp, body = self.client.post(url, json=data)
        return resp, body

    def fpga_image_delete(self, **kwargs):
        tenant_id = os.getenv('OS_TENANT_ID')
        fpga_image_id = kwargs.get('fpga_image_id', None)
        url = '/v1/%s/cloudservers/fpga_image/%s' % (tenant_id, fpga_image_id)
        resp, body = self.client.delete(url)
        return resp, body

    def fpga_image_list(self, **kwargs):
        tenant_id = os.getenv('OS_TENANT_ID')
        query_string = '&'.join('%s=%s' % (k, v) for k, v in kwargs.items())
        url = '/v1/%s/cloudservers/fpga_image/detail?%s' %\
              (tenant_id, query_string)
        resp, body = self.client.get(url)
        return resp, body

    def fpga_image_relation_create(self, **kwargs):
        tenant_id = os.getenv('OS_TENANT_ID')
        fpga_image_id = kwargs.get('fpga_image_id', None)
        image_id = kwargs.get('image_id', None)
        data = {
            'image': {
                    'id': image_id
                }
        }
        url = '/v1/%s/cloudservers/fpga_image/%s/association' %\
              (tenant_id, fpga_image_id)
        resp, body = self.client.post(url, json=data)
        return resp, body

    def fpga_image_relation_delete(self, **kwargs):
        tenant_id = os.getenv('OS_TENANT_ID')
        fpga_image_id = kwargs.get('fpga_image_id', None)
        image_id = kwargs.get('image_id', None)
        data = {
            'image': {
                    'id': image_id
                }
        }
        url = '/v1/%s/cloudservers/fpga_image/%s/association' %\
              (tenant_id, fpga_image_id)
        resp, body = self.client.delete(url, json=data)
        return resp, body

    def fpga_image_relation_list(self, **kwargs):
        tenant_id = os.getenv('OS_TENANT_ID')
        query_string = '&'.join('%s=%s' % (k, v) for k, v in kwargs.items())
        url = '/v1/%s/cloudservers/fpga_image/associations?%s' %\
              (tenant_id, query_string)
        resp, body = self.client.get(url)
        return resp, body
