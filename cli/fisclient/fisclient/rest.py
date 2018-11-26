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

import base64
import collections
import datetime
import hashlib
import hmac
import json
import os
import sys
import time

import requests

from auth import sign_request_v4
from parse_xml import getDictFromXml
from exception import HttpException
import config
import encode
import utils


# disable insecure warning
requests.packages.urllib3.disable_warnings()

timeout = 60
cert_verify = False


# error message
def get_xml_error(text, func=None, args=None):
    try:
        error = getDictFromXml(text)
        if func is None:
            func = sys._getframe().f_back.f_code.co_name
        return '%s, Code=%s, RequestId=%s, Function=%s, Arguments=%s' % (
            error.get('Message'),
            error.get('Code'),
            error.get('RequestId'),
            func,
            args)
    except Exception:
        return text


def get_json_error(text, func=None):
    try:
        error = json.loads(text).get('error')
        if func is None:
            func = sys._getframe().f_back.f_code.co_name
        return '%s, Code=%s, Function=%s' % (
            error.get('message'),
            error.get('code'),
            func)
    except Exception:
        return text


def get_text_error(text, func=None):
    if func is None:
        func = sys._getframe().f_back.f_code.co_name
    return '%s, Function=%s' % (text, func)


# response handler
def resp_with_json_error(resp):
    if resp.status_code >= 300:
        func = sys._getframe().f_back.f_code.co_name
        raise HttpException(resp.status_code, resp.reason, get_json_error(resp.text, func))
    try:
        resp_body = resp.json(object_pairs_hook=collections.OrderedDict)
    except Exception:
        resp_body = resp.text
    return resp.status_code, resp.reason, resp_body


def resp_without_body(resp):
    if resp.status_code >= 300:
        func = sys._getframe().f_back.f_code.co_name
        raise HttpException(resp.status_code, resp.reason, get_json_error(resp.text, func))
    return resp.status_code, resp.reason


def resp_with_text_error(resp):
    if resp.status_code >= 300:
        func = sys._getframe().f_back.f_code.co_name
        raise HttpException(resp.status_code, resp.reason, get_text_error(resp.text, func))
    try:
        return resp.json()
    except Exception:
        return dict()


# obs
def _get_bucket_info(ak, sk, resource, host):
    date = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime())
    string_to_sign = 'GET\n\n\n%s\n%s' % (date, resource)
    signature = base64.b64encode(hmac.new(sk, string_to_sign, hashlib.sha1).digest())

    resp = requests.get(
            'https://%s%s' % (host, resource),
            headers={'Host': host,
                     'Date': date,
                     'Authorization': 'AWS %s:%s' % (ak, signature)},
            timeout=timeout,
            verify=cert_verify)
    if resp.status_code >= 300:
        func = sys._getframe().f_back.f_code.co_name
        raise HttpException(resp.status_code, resp.reason, get_xml_error(resp.text, func, args=resource))
    try:
        return getDictFromXml(resp.text)
    except Exception:
        return dict()


def get_bucket_list(ak, sk, host):
    resource = '/'
    return _get_bucket_info(ak, sk, resource, host)


def get_bucket_location(ak, sk, bucket_name, host):
    resource = '/%s?location' % bucket_name
    return _get_bucket_info(ak, sk, resource, host)


def get_bucket_acl(ak, sk, bucket_name, host):
    resource = '/%s?acl' % bucket_name
    return _get_bucket_info(ak, sk, resource, host)


def make_bucket(ak, sk, bucket_name, location, host):
    resource = '/' + bucket_name
    date = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime())
    content_type = 'application/xml'
    string_to_sign = 'PUT\n\n%s\n%s\n%s' % (content_type, date, resource)
    signature = base64.b64encode(hmac.new(sk, string_to_sign, hashlib.sha1).digest())
    body = '<CreateBucketConfiguration><LocationConstraint>%s</LocationConstraint></CreateBucketConfiguration>' % location

    resp = requests.put(
            'https://%s%s' % (host, resource),
            headers={'Host': host,
                     'Date': date,
                     'Authorization': 'AWS %s:%s' % (ak, signature),
                     'Content-Type': content_type},
            data=body,
            timeout=timeout,
            verify=cert_verify)
    if resp.status_code >= 300:
        raise HttpException(resp.status_code, resp.reason, get_xml_error(resp.text, args=(bucket_name, location)))
    return (resp.status_code, resp.reason)


@utils.retry_decorator
def put_dcp_file(ak, sk, file_name, bucket_name, object_key, host):
    expand_file_name = os.path.expanduser(file_name)
    with open(expand_file_name, 'rb') as f:
        m = hashlib.md5()
        while True:
            b = f.read(10*1024*1024)
            if not b:
                break
            m.update(b)
        f.seek(0)

        content_md5 = base64.b64encode(m.digest())
        content_length = os.stat(expand_file_name).st_size
        resource = '/%s/%s' % (bucket_name, object_key)
        date = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime())
        string_to_sign = 'PUT\n%s\n\n%s\n%s' % (content_md5, date, resource)
        signature = base64.b64encode(hmac.new(sk, string_to_sign, hashlib.sha1).digest())

        t1 = datetime.datetime.now()
        resp = requests.put(
                'https://%s%s' % (host, resource),
                headers={'Host': host,
                         'Date': date,
                         'Authorization': 'AWS %s:%s' % (ak, signature),
                         'Content-MD5': content_md5,
                         'Content-Length': str(content_length)},
                data=f,
                timeout=timeout,
                verify=cert_verify)
        t2 = datetime.datetime.now()
        time_diff = (t2 - t1).total_seconds()
        if resp.status_code >= 300:
            raise HttpException(resp.status_code, resp.reason, get_xml_error(resp.text, args=(bucket_name, object_key)))
        return (resp.status_code, resp.reason, content_length, time_diff)


def get_log_file(ak, sk, file_name, bucket_name, object_key, host):
    resource = '/%s/%s' % (bucket_name, object_key)
    date = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime())
    string_to_sign = 'GET\n\n\n%s\n%s' % (date, resource)
    signature = base64.b64encode(hmac.new(sk, string_to_sign, hashlib.sha1).digest())

    t1 = datetime.datetime.now()
    resp = requests.get(
            'https://%s%s' % (host, resource),
            headers={'Host': host,
                     'Date': date,
                     'Authorization': 'AWS %s:%s' % (ak, signature)},
            timeout=timeout,
            verify=cert_verify)
    t2 = datetime.datetime.now()
    time_diff = (t2 - t1).total_seconds()
    if resp.status_code >= 300:
        msg = get_xml_error(resp.text, args=(bucket_name, object_key))
        if 'NoSuchKey' in msg:
            msg += '\n\033[31mTips: The log file may have NOT been generated, or have been Deleted or Moved.\033[0m'
        raise HttpException(resp.status_code, resp.reason, msg)

    with open(file_name, 'wb') as f:
        f.write(resp.content)

    return (resp.status_code, resp.reason, len(resp.content), time_diff)


# iam
def get_project(ak, sk, region, host):
    headers = sign_request_v4(ak, sk, 'GET', host, '/v3/projects',
                              region, 'iam', params={'name': region})
    headers['Content-Type'] = 'application/json;charset=utf8'
    resp = requests.get(
            'https://%s/v3/projects?name=%s' % (host, region),
            headers=headers,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_text_error(resp)


# ecs
def get_os_interface(ak, sk, project_id, region, host, instance_id):
    uri = '/v2/%s/servers/%s/os-interface' % (project_id, instance_id)
    headers = sign_request_v4(ak, sk, 'GET', host, uri, region, 'ecs')
    resp = requests.get(
            'https://%s%s' % (host, uri),
            headers=headers,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_text_error(resp)


# vpc
def get_subnet(ak, sk, project_id, region, host, net_id):
    uri = '/v1/%s/subnets/%s' % (project_id, net_id)
    headers = sign_request_v4(ak, sk, 'GET', host, uri, region, 'vpc')
    resp = requests.get(
            'https://%s%s' % (host, uri),
            headers=headers,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_text_error(resp)


def put_subnet(ak, sk, project_id, region, host, vpc_id, net_id, body):
    uri = '/v1/%s/vpcs/%s/subnets/%s' % (project_id, vpc_id, net_id)
    headers = sign_request_v4(ak, sk, 'PUT', host, uri, region, 'vpc', body=body)
    headers['Content-Type'] = 'application/json;charset=utf8'
    resp = requests.put(
            'https://%s%s' % (host, uri),
            headers=headers,
            data=body,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_text_error(resp)


# fis
def fpga_image_create(ak, sk, project_id, region, host, fpga_image):
    uri = '/v2/%s/cloudservers/fpga_image' % project_id
    body = json.dumps({'fpga_image': fpga_image})
    headers = sign_request_v4(ak, sk, 'POST', host, uri, region, 'ecs', body=body)
    headers['Content-Type'] = 'application/json;charset=utf8'
    resp = requests.post(
            'https://%s%s' % (host, uri),
            headers=headers,
            data=body,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_json_error(resp)


def fpga_image_delete(ak, sk, project_id, region, host, fpga_image_id):
    uri = '/v1/%s/cloudservers/fpga_image/%s' % (project_id, fpga_image_id)
    headers = sign_request_v4(ak, sk, 'DELETE', host, uri, region, 'ecs')
    resp = requests.delete(
            'https://%s%s' % (host, uri),
            headers=headers,
            timeout=timeout,
            verify=cert_verify)
    return resp_without_body(resp)


def fpga_image_list(ak, sk, project_id, region, host, params=None):
    uri = '/v1/%s/cloudservers/fpga_image/detail' % project_id
    if params is not None:
        params = {k: str(v) for k, v in params.items()}
        query_str = '&'.join('%s=%s' % (k, v) for k, v in params.items())
    else:
        query_str = ''
    headers = sign_request_v4(ak, sk, 'GET', host, uri, region, 'ecs', params=params)
    resp = requests.get(
            'https://%s%s?%s' % (host, uri, query_str),
            headers=headers,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_json_error(resp)


def fpga_image_relation_create(ak, sk, project_id, region, host, fpga_image_id, image_id):
    uri = '/v1/%s/cloudservers/fpga_image/%s/association' % (project_id, fpga_image_id)
    body = json.dumps({'image': {'id': image_id}})
    headers = sign_request_v4(ak, sk, 'POST', host, uri, region, 'ecs', body=body)
    headers['Content-Type'] = 'application/json;charset=utf8'
    resp = requests.post(
            'https://%s%s' % (host, uri),
            headers=headers,
            data=body,
            timeout=timeout,
            verify=cert_verify)
    return resp_without_body(resp)


def fpga_image_relation_delete(ak, sk, project_id, region, host, fpga_image_id, image_id):
    uri = '/v1/%s/cloudservers/fpga_image/%s/association' % (project_id, fpga_image_id)
    body = json.dumps({'image': {'id': image_id}})
    headers = sign_request_v4(ak, sk, 'DELETE', host, uri, region, 'ecs', body=body)
    headers['Content-Type'] = 'application/json;charset=utf8'
    resp = requests.delete(
            'https://%s%s' % (host, uri),
            headers=headers,
            data=body,
            timeout=timeout,
            verify=cert_verify)
    return resp_without_body(resp)


def fpga_image_relation_list(ak, sk, project_id, region, host, params=None):
    uri = '/v1/%s/cloudservers/fpga_image/associations' % project_id
    if params is not None:
        params = {k: str(v) for k, v in params.items()}
        query_str = '&'.join('%s=%s' % (k, v) for k, v in params.items())
    else:
        query_str = ''
    headers = sign_request_v4(ak, sk, 'GET', host, uri, region, 'ecs', params=params)
    resp = requests.get(
            'https://%s%s?%s' % (host, uri, query_str),
            headers=headers,
            timeout=timeout,
            verify=cert_verify)
    return resp_with_json_error(resp)


# metadata
def get_region_id_from_metadata():
    try:
        resp = requests.get('http://169.254.169.254/latest/meta-data/placement/availability-zone', timeout=10)
        az = resp.text.strip()
        if az in config.az_region_map:
            return config.az_region_map.get(az)
    except Exception as e:
        utils.print_err('Get AZ from ECS metadata failed: %s' % encode.exception_to_unicode(e))
    try:
        resp = requests.get('http://169.254.169.254/openstack/latest/meta_data.json', timeout=10)
        az = resp.json().get('availability_zone')
        if az in config.az_region_map:
            return config.az_region_map.get(az)
    except Exception as e:
        utils.print_err('Get AZ from ECS metadata failed: %s' % encode.exception_to_unicode(e))
    utils.print_err('Could not get region_id from ECS metadata.')


def get_instance_id_from_metadata():
    try:
        resp = requests.get('http://169.254.169.254/openstack/latest/meta_data.json', timeout=10)
        return resp.json().get('uuid')
    except Exception as e:
        utils.print_err('Get instance_id from ECS metadata failed: %s' % encode.exception_to_unicode(e))
