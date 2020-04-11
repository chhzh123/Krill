# This code is part of the project "Krill"
# Copyright (c) 2020 Hongzheng Chen

import os
import sys
import regular
import layout_trans

if len(sys.argv) <= 2: # py, infile
    get_props_class = regular.get_props_class
    get_main_class = regular.get_main_class
else:
    get_props_class = layout_trans.get_props_class
    get_main_class = layout_trans.get_main_class

path = sys.argv[1]
dir_name, infile_name = os.path.split(path)
pb_name = infile_name.split(".")[0]
outfile_name = os.path.join(dir_name,"{}.pb.h".format(pb_name))
pb_name = pb_name.replace("-","")

head = """// Generated by the property buffer compiler. DO NOT EDIT!
// source: {0}

#ifndef {1}_PROPERTY_BUFFER_H
#define {1}_PROPERTY_BUFFER_H

#include <cctype>
#include <cstdlib>
#include <vector>
#include "parallel.h"

namespace {2} {{

typedef int intE;
typedef unsigned int uintE;

class Property;

""".format(infile_name,pb_name.upper(),pb_name)

tail = """
}} // namespace {0}

#endif // {1}_PROPERTY_BUFFER_H
""".format(pb_name,pb_name.upper())

job = "main"
props = {}
# parse
with open(path,"r") as infile:
    for line in infile:
        line = line.strip()[:-1].split()
        if len(line) == 0:
            continue
        elif line[0] == "property":
            job = line[1]
        else:
            initial_val = None
            if "=" in line:
                expr = line[3:]
                if len(expr) == 1:
                    initial_val = expr[-1]
                else:
                    initial_val = (expr[1],' '.join(expr[3:])[:-1])
            type_name, prop_name = line[0], line[1]
            if props.get(job,None) == None:
                props[job] = []
            props[job].append((prop_name,type_name,initial_val))

# codegen
with open(outfile_name,"w") as outfile:
    outfile.write(head)
    outfile.write(get_props_class(props,pb_name))
    outfile.write(get_main_class(props))
    outfile.write(tail)