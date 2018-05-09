#!/bin/bash
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

pkgs="prettytable-0.7.2.tar.gz requests-2.13.0.tar.gz"

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
