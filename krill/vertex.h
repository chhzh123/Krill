// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen
// Original work Copyright (c) 2013 Julian Shun

#ifndef VERTEX_H
#define VERTEX_H

#include "vertexSubset.h"
using namespace std;

#define INVALID_WEIGHT -1

struct asymmetricVertex
{
    asymmetricVertex(uintT id, uintT od):
       inDegree(id), outDegree(od){};
    // avoid virtual functions to reduce memory overheads
    // virtual void del() = 0;
    inline void setInDegree(uintT _d) { inDegree = _d; }
    inline void setOutDegree(uintT _d) { outDegree = _d; }
    inline uintT getInDegree() { return inDegree; }
    inline uintT getOutDegree() { return outDegree; }
    uintT outDegree;
    uintT inDegree;
};

struct asymmetricUnweightedVertex : public asymmetricVertex
{
    asymmetricUnweightedVertex(uintE* iN, uintE* oN, uintT id, uintT od):
       inNeighbors(iN), outNeighbors(oN), asymmetricVertex(id,od){}
    uintE *getInNeighbors() { return inNeighbors; }
    uintE *getOutNeighbors() { return outNeighbors; }
    uintE getInNeighbor(uintT j) { return inNeighbors[j]; }
    uintE getOutNeighbor(uintT j) { return outNeighbors[j]; }
    intE getInWeight(uintT j) { return INVALID_WEIGHT; } // placeholder!
    intE getOutWeight(uintT j) { return INVALID_WEIGHT; } // placeholder!
    void setInNeighbors(uintE *_i) { inNeighbors = _i; }
    void setOutNeighbors(uintE *_i) { outNeighbors = _i; }
    void flipEdges()
    {
       swap(inNeighbors, outNeighbors);
       swap(inDegree, outDegree);
    }
    void del()
    {
       free(inNeighbors);
       free(outNeighbors);
    }
    uintE *inNeighbors, *outNeighbors;
};

struct asymmetricWeightedVertex : public asymmetricVertex
{
    asymmetricWeightedVertex(intE* iN, intE* oN, uintT id, uintT od):
       inNeighbors(iN), outNeighbors(oN), asymmetricVertex(id,od){}
    intE *getInNeighbors() { return inNeighbors; }
    intE *getOutNeighbors() { return outNeighbors; }
    intE getInNeighbor(uintT j) { return inNeighbors[2 * j]; }
    intE getOutNeighbor(uintT j) { return outNeighbors[2 * j]; }
    intE getInWeight(uintT j) { return inNeighbors[2 * j + 1]; }
    intE getOutWeight(uintT j) { return outNeighbors[2 * j + 1]; }
    void setInNeighbors(intE *_i) { inNeighbors = _i; }
    void setOutNeighbors(intE *_i) { outNeighbors = _i; }
    void flipEdges()
    {
       swap(inNeighbors, outNeighbors);
       swap(inDegree, outDegree);
    }
    void del()
    {
       free(inNeighbors);
       free(outNeighbors);
    }
    intE *inNeighbors, *outNeighbors; // the second position is used to store weights
};

struct symmetricVertex
{
    symmetricVertex(uintT d): degree(d){};
    inline uintT getInDegree() { return degree; }
    inline uintT getOutDegree() { return degree; }
    inline void setInDegree(uintT _d) { degree = _d; }
    inline void setOutDegree(uintT _d) { degree = _d; }
    inline void flipEdges() {}
    uintT degree;
};

struct symmetricUnweightedVertex : public symmetricVertex
{
    symmetricUnweightedVertex(uintE* n, uintT d):
       neighbors(n), symmetricVertex(d){}
    uintE *getInNeighbors() { return neighbors; }
    uintE *getOutNeighbors() { return neighbors; }
    uintE getInNeighbor(uintT j) { return neighbors[j]; }
    uintE getOutNeighbor(uintT j) { return neighbors[j]; }
    intE getInWeight(intT j) { return INVALID_WEIGHT; }
    intE getOutWeight(intT j) { return INVALID_WEIGHT; }
    void setInNeighbors(uintE *_i) { neighbors = _i; }
    void setOutNeighbors(uintE *_i) { neighbors = _i; }
    void del() { free(neighbors); }
    uintE *neighbors;
};

struct symmetricWeightedVertex : public symmetricVertex
{
    symmetricWeightedVertex(intE* n, uintT d):
        neighbors(n), symmetricVertex(d){}
    void del() { free(neighbors); }
    // weights are stored in the entry after the neighbor ID
    // so size of neighbor list is twice the degree
    intE *getInNeighbors() { return neighbors; }
    intE *getOutNeighbors() { return neighbors; }
    intE getInNeighbor(intT j) { return neighbors[2 * j]; }
    intE getOutNeighbor(intT j) { return neighbors[2 * j]; }
    intE getInWeight(intT j) { return neighbors[2 * j + 1]; }
    intE getOutWeight(intT j) { return neighbors[2 * j + 1]; }
    void setInNeighbors(intE *_i) { neighbors = _i; }
    void setOutNeighbors(intE *_i) { neighbors = _i; }
    intE *neighbors;
};

// TraitsHelper for different vertices
template <typename T>  
struct TraitsHelper {  
    static const bool isSymmetric = false;
    static const bool isWeighted = false;
};

template <>  
struct TraitsHelper<asymmetricUnweightedVertex> {  
    static const bool isSymmetric = false;
    static const bool isWeighted = false;
};

template <>  
struct TraitsHelper<asymmetricWeightedVertex> {  
    static const bool isSymmetric = false;
    static const bool isWeighted = true;
};

template <>  
struct TraitsHelper<symmetricUnweightedVertex> {  
    static const bool isSymmetric = true;
    static const bool isWeighted = false;
};

template <>  
struct TraitsHelper<symmetricWeightedVertex> {  
    static const bool isSymmetric = true;
    static const bool isWeighted = true;
};

#endif