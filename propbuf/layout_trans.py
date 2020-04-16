# This code is part of the project "Krill"
# Copyright (c) 2020 Hongzheng Chen

def get_prop_class(job_prop,pb_name):
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
    res += "  inline void add (int i, {} val) {{ data[i * sj_num] += val; }}\n".format(type_name)
    res += "  friend class {}::PropertyManager;\n".format(pb_name) # friend class
    # data
    res += "private:\n"
    res += "  size_t n;\n"
    res += "  {}* data;\n".format(type_name)
    res += "};\n\n"
    return res

def get_props_class(props,pb_name):
    res = ""
    for job in props:
        job_namespace = job + "_Prop"
        res += "namespace {} {{\n\n".format(job_namespace)
        for prop in props[job]:
            res += get_prop_class(prop,pb_name)
        res += "}} // namespace {}\n\n".format(job_namespace)
    return res

def get_main_class(props):
    res  = "class PropertyManager {\npublic:\n  size_t n;\n" \
           "  PropertyManager(size_t _n): n(_n) {}\n"
    for job in props:
        for prop in props[job]:
            class_name = "{}_Prop::{}".format(job,prop[0])
            prop_name = prop[0]
            array_name = "arr_{}_{}".format(job,prop[0])
            res += "  inline {0}* add_{1}() {{\n".format(class_name,prop_name)
            res += "    {0}* {1} = new {0}(n);\n".format(class_name,prop_name)
            res += "    {}.push_back({});\n".format(array_name,prop_name)
            res += "    return {};\n".format(prop_name)
            res += "  }\n"
    res += "  inline void initialize() {\n"
    for job in props:
        for prop in props[job]:
            class_name = "{}_Prop::{}".format(job,prop[0])
            prop_name, type_name, initial_val = prop
            array_name = "arr_{}_{}".format(job,prop[0])
            res += "    //  {}\n".format(class_name) # comment
            res += "    {0}* {1}_all = ({0}*) malloc(sizeof({0}) * n * {1}.size());\n".format(type_name,array_name)
            res += "    int {}_idx = 0;\n".format(array_name)
            res += "    for (auto ptr : {}) {{\n".format(array_name)
            res += "      ptr->data = &({0}_all[{0}_idx]);\n".format(array_name)
            if initial_val != None:
                if type(initial_val) == type("str"):
                    res += "      parallel_for (int i = 0; i < n; ++i) {\n"
                    res += "        ptr->data[i] = {};\n".format(initial_val if eval(initial_val) != -1 else "UINT_MAX")
                else: # lambda expression
                    res += "      auto lambda = [](int i) -> {} {{ return ".format(type_name)
                    res += initial_val[1].replace(initial_val[0],"i")
                    res += "; };\n"
                    res += "      parallel_for (int i = 0; i < n; ++i) {\n"
                    res += "        ptr->data[i] = lambda(i);\n"
                res += "      }\n"
            res += "      {}_idx += n;\n".format(array_name)
            res += "    }\n"
    res += "  }\n"
    for job in props:
        for prop in props[job]:
            class_name = "{}_Prop::{}".format(job,prop[0])
            prop_name = prop[0]
            array_name = "arr_{}_{}".format(job,prop[0])
            res += "  std::vector<{}*> {};\n".format(class_name,array_name)
    res += "};\n\n"
    return res