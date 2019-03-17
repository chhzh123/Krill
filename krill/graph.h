// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen
// Original work Copyright (c) 2013 Julian Shun

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "vertex.h"
// #include "compressedVertex.h"
#include "parallel.h"
using namespace std;

// **************************************************************
//      ADJACENCY ARRAY REPRESENTATION
// **************************************************************

// Class that handles implementation specific freeing of memory 
// owned by the graph 
struct Deletable {
public:
    virtual void del() = 0;
};

template <class vertex>
struct graph
{
    vertex* V;
    long n; // # of vertices
    long m; // # of edges
    bool transposed;
    bool isWeighted; // weighted graph?
    uintE* flags;
    Deletable *D;
    graph(vertex* VV, long nn, long mm, bool _isWeighted, Deletable* DD):
        V(VV), n(nn), m(mm), isWeighted(_isWeighted),
        D(DD), flags(NULL), transposed(0) {}
    graph(vertex* VV, long nn, long mm, bool _isWeighted, Deletable* DD, uintE* _flags):
        V(VV), n(nn), m(mm), isWeighted(_isWeighted),
        D(DD), flags(_flags), transposed(0) {}

    void del() {
        D->del();
        free(D);
    }

    void transpose() {
        // if ((sizeof(vertex) == sizeof(asymmetricVertex)) || 
            // (sizeof(vertex) == sizeof(compressedAsymmetricVertex))) {
        if (sizeof(vertex) == sizeof(asymmetricVertex)) {
            parallel_for(long i = 0; i < n; i++) {
                V[i].flipEdges();
            }
            transposed = !transposed;
        }
    }

    void print() {
        cout << "# of vertices: " << endl;
        cout << "# of edges: " << endl;
        cout << "Out Neighbors:" << endl;
        for (long i = 0; i < n; ++i) {
            cout << "Vertex " << i << ": ";
            for (long j = 0; j < V[i].getOutDegree(); ++j)
                cout << V[i].getOutNeighbor(j) << " ";
            cout << endl;
        }
    }
};

#endif