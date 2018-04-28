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
import sys
import time


class Log(object):

    def __init__(self, log_dir):
        try:
            self.log_dir = log_dir
            self.log_file = None
            self.log_date = None
            self.enable = True

            if not os.path.exists(log_dir):
                os.mkdir(log_dir, 0700)
            elif not os.path.isdir(log_dir):
                self.enable = False
        except Exception:
            self.enable = False

    def _get_log_file(self):
        log_date = time.strftime('%Y_%m_%d')
        if self.log_date != log_date:
            self.log_date = log_date
            log_filename = '%s/%s.log' % (self.log_dir, self.log_date)
            if self.log_file:
                self.log_file.close()
            self.log_file = open(log_filename, 'a', 1)
        return self.log_file

    def log(self, *args):
        if not self.enable:
            return
        try:
            self._get_log_file()
            user = ' '.join(
                [i for i in [os.getenv('OS_DOMAIN_NAME'),
                             os.getenv('OS_USER_NAME'),
                             os.getenv('OS_PROJECT_NAME')]
                 if i is not None])
            print(time.strftime('[%Y-%m-%d-%H-%M-%S]'),
                  'user [%s]' % user,
                  sys._getframe().f_back.f_code.co_name,
                  ', '.join(args),
                  file=self.log_file)
            self.log_file.flush()
        except Exception:
            pass
