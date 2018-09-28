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

import re
import xml.etree.ElementTree as ET

from encode import convert_to_unicode


def stripNameSpace(xml):
    ns_regex = b'^(<?[^>]+?>\s*)(<\w+) xmlns=[\'"](http://[^\'"]+)[\'"](.*)'
    r = re.compile(ns_regex, re.MULTILINE)
    if r.match(xml):
        xmlns = r.match(xml).groups()[2]
        xml = r.sub('\\1\\2\\4', xml)
    else:
        xmlns = None
    return xml, xmlns


def getTreeFromXml(xml):
    xml, xmlns = stripNameSpace(xml)
    tree = ET.fromstring(xml)
    if xmlns:
        tree.attrib['xmlns'] = xmlns
    return tree


def getDictFromTree(tree):
    ret_dict = {}
    for child in tree.getchildren():
        if child.getchildren():
            content = getDictFromTree(child)
        elif child.text is not None:
            content = convert_to_unicode(child.text)
        else:
            content = None
        child_tag = convert_to_unicode(child.tag)
        if child_tag in ret_dict:
            if not type(ret_dict[child_tag]) == list:
                ret_dict[child_tag] = [ret_dict[child_tag]]
            ret_dict[child_tag].append(content or "")
        else:
            ret_dict[child_tag] = content or ""
    return ret_dict


def getDictFromXml(xml):
    tree = getTreeFromXml(xml)
    return getDictFromTree(tree)
