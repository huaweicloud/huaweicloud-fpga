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

from setuptools import setup, find_packages

long_description = 'fisclient is a command-line client for \
FIS (FPGA Image Service) that brings the command set for \
FPGA image management together in a single shell'''

requires = [
    'requests==2.13.0',
    'certifi==2017.7.27.1',
    'prettytable==0.7.2',
    'setuptools==19.6.2',
]

setup(
    name='fisclient',
    version='2.2.0',
    description='FIS API Client',
    long_description=long_description,
    license='Apache License, Version 2.0',
    packages=find_packages(exclude=['data']),
    zip_safe=True,
    install_requires=requires,
    entry_points={
        'console_scripts': [
            'fis=fisclient.fis:main',
            'fischeck=fisclient.fischeck:main',
        ]
    }
)
