// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef HOMO1_PROPERTY_BUFFER_H
#define HOMO1_PROPERTY_BUFFER_H

#include <cctype>
#include <cstdlib>

namespace Homo1 {

typedef unsigned int uintE;

class Parents
{
public:
    Parents(size_t n){
        data = (uintE*) malloc(sizeof(uintE) * n);
    }
    inline uintE operator[](int i) const { return data[i]; }
    inline uintE &operator[](int i) { return data[i]; }
    uintE* data;
};

class Property
{
public:
    Property(size_t _n):
        n(_n){}
    inline Parents* add_parents(){
        Parents* parents = new Parents(n);
        return parents;
    }
    size_t n;
};

} // namespace Homo1

#endif // HOMO1_PROPERTY_BUFFER_H