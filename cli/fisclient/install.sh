#!/bin/bash
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

pkgs="pbr-1.8.1.tar.gz six-1.9.0.tar.gz prettytable-0.7.2.tar.gz pytz-2017.2.zip Babel-1.3.tar.gz pyparsing-2.2.0.tar.gz oslo.i18n-3.4.0.tar.gz funcsigs-0.4.tar.gz iso8601-0.1.11.tar.gz monotonic-1.1.tar.gz wrapt-1.10.6.tar.gz debtcollector-1.10.0.tar.gz netaddr-0.7.18.tar.gz oslo.utils-3.18.0.tar.gz jsonpointer-1.10.tar.gz jsonpatch-1.14.tar.gz functools32-3.2.3-2.tar.gz vcversioner-2.16.0.0.tar.gz jsonschema-2.5.1.tar.gz warlock-1.2.0.tar.gz requests-2.13.0.tar.gz positional-1.1.1.tar.gz stevedore-1.19.1.tar.gz keystoneauth1-2.18.0.tar.gz"

for pkg in $pkgs
do
    if [ ! -e data/$pkg ]; then
        echo -e "~~~~~~~~~~~~~~~~ package $pkg is missing ~~~~~~~~~~~~~~~~"
        exit -1
    fi
done

for pkg in $pkgs
do
    echo -e "\n~~~~~~~~~~~~~~~~ install $pkg ~~~~~~~~~~~~~~~~"
    python -m easy_install data/$pkg
    if [ $? -ne 0 ]; then
        echo -e "\n~~~~~~~~~~~~~~~~ install $pkg failed ~~~~~~~~~~~~~~~~"
        exit -1
    fi
done

echo -e "\n~~~~~~~~~~~~~~~~ install fisclient ~~~~~~~~~~~~~~~~"
python setup.py install

echo -e "\n~~~~~~~~~~~~~~~~ install success ~~~~~~~~~~~~~~~~"
