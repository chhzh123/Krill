# This code is part of the project "Krill"
# Copyright (c) 2029 Hongzheng Chen

import os
import sys

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
#include "parallel.h"

namespace {2} {{

typedef int intE;
typedef unsigned int uintE;

""".format(infile_name,pb_name.upper(),pb_name)

tail = """
}} // namespace {0}

#endif // {1}_PROPERTY_BUFFER_H
""".format(pb_name,pb_name.upper())

job = "main"
props = {}
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

def get_prop_class(job_prop):
    prop_name, type_name, initial_val = job_prop
    class_name = "{}".format(prop_name)
    if type_name == "uint":
        type_name = "uintE"
    elif type_name == "int":
        type_name = "intE"
    else:
        pass # other C/C++ inherent types are supported
    res = "class {} {{\npublic:\n".format(class_name)
    # constructor
    res += "  {}(size_t _n): n(_n) {{\n".format(class_name)
    res += "    data = ({0}*) malloc(sizeof({0}) * n);\n".format(type_name)
    if initial_val != None:
        if type(initial_val) == type("str"):
            res += "    parallel_for (int i = 0; i < n; ++i) {\n"
            res += "      data[i] = {};\n".format(initial_val if eval(initial_val) != -1 else "UINT_MAX")
        else: # lambda expression
            res += "    auto lambda = [](int i) -> {} {{ return ".format(type_name)
            res += initial_val[1].replace(initial_val[0],"i")
            res += "; };\n"
            res += "    parallel_for (int i = 0; i < n; ++i) {\n"
            res += "      data[i] = lambda(i);\n"
        res += "    }\n"
    res += "  }\n"
    # destructor
    res += "  ~{}() {{\n".format(class_name)
    res += "    free(data);\n" \
           "  }\n"
    # accessors
    res += "  inline {} operator[] (int i) const {{ return data[i]; }}\n".format(type_name)
    res += "  inline {}& operator[] (int i) {{ return data[i]; }}\n".format(type_name)
    res += "  inline {} get (int i) const {{ return data[i]; }}\n".format(type_name)
    res += "  inline {}& get (int i) {{ return data[i]; }}\n".format(type_name)
    res += "  inline {}* get_addr (int i) {{ return &(data[i]); }}\n".format(type_name)
    res += "  inline {}* get_data () {{ return data; }}\n".format(type_name)
    res += "  inline void set (int i, {} val) {{ data[i] = val; }}\n".format(type_name)
    res += "  inline void set_all ({} val) {{ parallel_for (int i = 0; i < n; ++i) data[i] = val; }}\n".format(type_name)
    # data
    res += "private:\n"
    res += "  size_t n;\n"
    res += "  {}* data;\n".format(type_name)
    res += "};\n\n"
    return res

def get_main_class():
    res  = "class Property {\npublic:\n  size_t n;\n" \
           "  Property(size_t _n): n(_n) {}\n"
    for job in props:
        for prop in props[job]:
            class_name = "{}_Prop::{}".format(job,prop[0])
            prop_name = prop[0]
            res += "  inline {0}* add_{1}() {{\n".format(class_name,prop_name)
            res += "    {0}* {1} = new {0}(n);\n".format(class_name,prop_name)
            res += "    return {};\n".format(prop_name)
            res += "  }\n"
    res += "};\n\n"
    return res

with open(outfile_name,"w") as outfile:
    outfile.write(head)
    for job in props:
        job_namespace = job + "_Prop"
        outfile.write("namespace {} {{\n\n".format(job_namespace))
        for prop in props[job]:
            outfile.write(get_prop_class(prop))
        outfile.write("}} // namespace {}\n\n".format(job_namespace))
    outfile.write(get_main_class())
    outfile.write(tail)