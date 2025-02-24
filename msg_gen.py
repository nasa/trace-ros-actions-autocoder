#!/usr/bin/env python3

# Copyright 2016-2025 California Institute of Technology
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from mako.template import Template
from mako.lookup import TemplateLookup

from glob import glob

import genmsg
import genmsg.msg_loader
import genmsg.command_line

import os
import re

from optparse import OptionParser

from pathlib import Path


if __name__ == "__main__":

    parser = OptionParser()

    parser.add_option("-a", "--autocoded-include-dir-name-", dest="header_dest" )
    parser.add_option("-n", "--package-name", dest="package_name")
    parser.add_option("-d", "--devel-prefix", dest="devel_prefix")
    parser.add_option("-s", "--source-prefix", dest="source_prefix")
    parser.add_option("-p", "--prefix", dest="prefix") # for built in ros message libraries
    parser.add_option("-t", "--template-directory", dest="template_directory")

    (options, args) = parser.parse_args()

    # TODO (asabel): should probably let people specify what ros message libraries 
    # they want to be able to use from the command line 
    # Should also change cmakelists so you can specify a ros distro?
    search_path = {
        "actionlib_msgs": {options.prefix + '/share/actionlib_msgs/msg'},
        "geometry_msgs": {options.prefix + '/share/geometry_msgs/msg/'},
        "sensor_msgs": {options.prefix + '/share/sensor_msgs/msg/'},
        "std_msgs": {options.prefix + '/share/std_msgs/msg/'}
    }

    # Passed in via cmakelists - 
    # -n "${ELAP_MSGS_PKGS}"
    # -d "${ELAP_MSGS_PKGS_DEVEL_PREFIXES}"
    # -s "${ELAP_MSGS_PKGS_SOURCE_PREFIXES}"
    header_dest = options.header_dest
    package_names = options.package_name.split(";")
    devel_prefixes = options.devel_prefix.split(";")
    source_prefixes = options.source_prefix.split(";")

    files = {}

    for i in range(len(package_names)):

        package_name = package_names[i]
        devel_prefix = devel_prefixes[i]
        source_prefix = source_prefixes[i]

        files[package_name] = glob(devel_prefix + '/share/' + package_name + '/msg/*Action.msg')

        search_path[package_name] = {
            source_prefix + '/action',
            source_prefix + '/msg',
            source_prefix + '/srv',
            devel_prefix + '/share/' + package_name + '/msg'
        }

    # print(search_path)

    msg_specs = {}

    mako_lookup = TemplateLookup(directories=[os.path.abspath(options.template_directory)])

    at_hdr_tmpl = mako_lookup.get_template('action_translator.h.template')
    at_impl_tmpl = mako_lookup.get_template('action_translator.cc.template')
    at_list_tmpl = mako_lookup.get_template('ros_action_clients.h.template')

    # Create a directory for the new .h and .cc files to be created from the template
    include_dir_path = "include/%s"%(options.header_dest)
    Path(include_dir_path).mkdir(parents=True, exist_ok=True)
    Path("src").mkdir(parents=True, exist_ok=True)

    # print(files)

    for package_name in files:
        for input_file in files[package_name]:
        
            msg_context = genmsg.msg_loader.MsgContext.create_default()
            full_type_name = genmsg.gentools.compute_full_type_name(package_name, os.path.basename(input_file))
            spec = genmsg.msg_loader.load_msg_from_file(msg_context, input_file, full_type_name)


            genmsg.msg_loader.load_depends(msg_context, spec, search_path)

            msg_specs[full_type_name] = spec

            file_name = '%s_translator'%(re.sub(r'(?<!^)(?=[A-Z])', '_', spec.short_name).lower())

            hdr = at_hdr_tmpl.render(spec=spec, search_path=search_path, msg_context=msg_context, header_dest=header_dest)
            with open('include/' + header_dest + '/' + file_name + '.h', "w+") as text_file:
                text_file.write(hdr)

            impl = at_impl_tmpl.render(spec=spec, search_path=search_path, msg_context=msg_context, header_dest=header_dest)
            with open('src/' + file_name + '.cc', "w+") as text_file:
                text_file.write(impl)

    with open('include/' + header_dest + '/ros_action_clients.h', "w+") as text_file:
        text_file.write(at_list_tmpl.render(msg_specs=msg_specs, header_dest=header_dest))

    at_factory_tmpl = mako_lookup.get_template('action_client_factory.cc.template')

    with open('src/action_client_factory.cc', "w+") as text_file:
        text_file.write(at_factory_tmpl.render(msg_specs=msg_specs, header_dest=header_dest))
