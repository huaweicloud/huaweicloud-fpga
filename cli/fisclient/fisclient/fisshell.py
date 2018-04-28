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

"""
Command-line interface to the fis API.
"""
from __future__ import print_function

import argparse
import os
import re
from argparse import Namespace

from keystoneauth1 import loading
from keystoneauth1.identity import v3 as v3_auth
from oslo_utils import encodeutils

import fisclient
from fisclient.common import utils, log, exceptions

OS_ENTRY_CMDSHELL = 'CMDSHELL'
OS_ENTRY_WRAPSHELL = 'WRAPSHELL'


class FisShell(object):

    def __init__(self):
        self.auth_url = utils.env('OS_AUTH_URL')
        self.fis_url = utils.env('OS_FIS_URL')
        self.project_name = utils.env('OS_PROJECT_NAME')
        self.domain_name = utils.env('OS_DOMAIN_NAME')
        self.user_name = utils.env('OS_USER_NAME')
        self.password = utils.env('OS_PASSWORD')

        self.api_version = '1'
        self.subcommands = {}
        self.base_parser = self.get_base_parser()
        self.sub_parser = self.get_subcommand_parser(self.api_version)
        self.client = self.get_versioned_client(self.api_version)
        self.log_client = log.Log('/var/log/fiscli')

    def get_base_parser(self):
        parser = argparse.ArgumentParser(
            prog='fis',
            description=__doc__.strip(),
            epilog='See "fis help COMMAND" for help on a specific command.',
            add_help=False,
        )

        return parser

    def _find_actions(self, subparsers, actions_module):
        attr_list = [a for a in dir(actions_module) if a.startswith('do_')]

        for attr in attr_list:
            callback = getattr(actions_module, attr)
            entry_shell = getattr(callback, 'OS_ENTRY_SHELL', None)
            if entry_shell and entry_shell != os.getenv('OS_ENTRY_SHELL'):
                continue

            desc = callback.__doc__ or ''
            help = desc.strip().split('\n')[0]
            arguments = getattr(callback, 'arguments', [])

            command = attr[3:].replace('_', '-')
            subparser = subparsers.add_parser(command,
                                              help=help,
                                              description=desc,
                                              add_help=False,
                                              )
            self.subcommands[command] = subparser
            for (args, kwargs) in arguments:
                subparser.add_argument(*args, **kwargs)
            subparser.set_defaults(func=callback)

    @utils.entry(OS_ENTRY_WRAPSHELL)
    @utils.arg('command', metavar='<subcommand>', nargs='?',
               help='Display help for <subcommand>')
    def do_help(self, args, parser):
        """Display help about fis or one of its subcommands"""
        command = getattr(args, 'command', '')

        if command:
            if args.command in self.subcommands:
                self.subcommands[args.command].print_help()
            else:
                raise exceptions.CommandError(
                    "'%s' is not a valid subcommand" % args.command)
        else:
            parser.print_help()

    def get_subcommand_parser(self, version):
        parser = self.get_base_parser()

        subparsers = parser.add_subparsers(dest='subcmd',
                                           metavar='<subcommand>')
        submodule = utils.import_versioned_module(version, 'shell')

        self._find_actions(subparsers, submodule)
        self._find_actions(subparsers, self)

        return parser

    def _get_keystone_auth_plugin(self, args):
        v3_auth_url = args.os_auth_url
        user_id = args.os_user_id
        username = args.os_username
        password = args.os_password
        user_domain_name = args.os_user_domain_name
        user_domain_id = args.os_user_domain_id
        project_id = args.os_project_id or args.os_tenant_id
        project_name = args.os_project_name or args.os_tenant_name
        project_domain_id = args.os_project_domain_id
        project_domain_name = args.os_project_domain_name

        auth = v3_auth.Password(
            v3_auth_url,
            user_id=user_id,
            username=username,
            password=password,
            user_domain_id=user_domain_id,
            user_domain_name=user_domain_name,
            project_id=project_id,
            project_name=project_name,
            project_domain_id=project_domain_id,
            project_domain_name=project_domain_name)

        return auth

    def _set_artificial_attr(self, args):
        # session_argparse_arguments
        args.insecure = False
        args.os_cacert = None
        args.os_cert = None
        args.os_key = None
        args.timeout = 600
        # auth_argparse_arguments
        args.os_auth_type = 'password'
        args.os_auth_url = self.auth_url
        args.os_default_domain_id = None
        args.os_default_domain_name = None
        args.os_domain_id = None
        args.os_domain_name = None
        args.os_password = self.password
        args.os_project_domain_id = None
        args.os_project_domain_name = None
        args.os_project_id = None
        args.os_project_name = self.project_name
        args.os_tenant_id = None
        args.os_tenant_name = None
        args.os_trust_id = None
        args.os_user_domain_id = None
        args.os_user_domain_name = self.domain_name
        args.os_user_id = None
        args.os_username = self.user_name or self.domain_name

    def get_versioned_client(self, api_version):
        args = Namespace()
        self._set_artificial_attr(args)
        ks_session = loading.load_session_from_argparse_arguments(args)
        ks_session.auth = self._get_keystone_auth_plugin(args)

        return fisclient.Client(api_version, self.fis_url, ks_session)

    def get_token(self):
        try:
            # get project_id by getting token
            project_id = self.client.http_client.session.get_project_id()
            os.environ['OS_TENANT_ID'] = project_id
            self.log_client.log('Success', 'Created. (HTTP 201)')
        except Exception as e:
            msg = encodeutils.exception_to_unicode(e)
            self.log_client.log('Error', re.sub('(\n)+|(\r\n)+', ', ', msg))
            utils.exit('Error: %s' % msg)

    def check_config_and_password(self):
        self.get_token()
        try:
            self.client.fis.fpga_image_relation_list()
        except Exception as e:
            exit('Error: %s' % encodeutils.exception_to_unicode(e))

    def main(self, argv):
        if not argv:
            self.do_help(None, parser=self.sub_parser)
            return

        args = self.sub_parser.parse_args(argv)
        if args.func == self.do_help:
            self.do_help(args, parser=self.sub_parser)
            return
        try:
            args.func(self.client, self.log_client, args)
        except Exception as e:
            utils.exit('Error: %s' % encodeutils.exception_to_unicode(e))
