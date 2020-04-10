# This code is part of the project "Krill"
# Copyright (c) 2029 Hongzheng Chen

import sys

path = sys.argv[1]
prefix = '/'.join(path.split("/")[:-1])
infile_name = sys.argv[1].split("/")[-1]
pb_name = infile_name.split(".")[0]

outfile_name = "{}.pb.h".format(pb_name)

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
props = []
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
            props.append((job,prop_name,type_name,initial_val))

def get_prop_class(job_prop):
    job, prop_name, type_name, initial_val = job_prop
    class_name = "{}_{}".format(job,prop_name)
    if type_name == "uint":
        type_name = "uintE"
    elif type_name == "int":
        type_name = "intE"
    else:
        pass # other C/C++ inherent types are supported
    res = "class {} {{\npublic:\n".format(class_name)
    # constructor
    res += "  {}(size_t n) {{\n".format(class_name)
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
    res += "  {}* data;\n".format(type_name)
    res += "};\n\n"
    return res

def get_main_class():
    res  = "class Property {\npublic:\n  size_t n;\n" \
           "  Property(size_t _n): n(_n) {}\n"
    for prop in props:
        class_name = "{}_{}".format(prop[0],prop[1])
        prop_name = prop[1]
        res += "  inline {0}* add_{1}() {{\n".format(class_name,prop_name)
        res += "    {0}* {1} = new {0}(n);\n".format(class_name,prop_name)
        res += "    return {};\n".format(prop_name)
        res += "  }\n"
    res += "};\n\n"
    return res

with open(prefix + "/" + outfile_name,"w") as outfile:
    outfile.write(head)
    for prop in props:
        outfile.write(get_prop_class(prop))
    outfile.write(get_main_class())
    outfile.write(tail)