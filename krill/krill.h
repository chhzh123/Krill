// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef KRILL_H
#define KRILL_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <typeinfo> // class name
#include "kernel.h"
#include "graph.h"
#include "vertex.h"
#include "vertexSubset.h"
#include "utils.h"
#include "gettime.h"
#include "parseCommandLine.h"
#include "IO.h"
using namespace std;

// cond function that always returns true
inline bool cond_true (intT d) { return 1; }

inline bool inFrontierQ(const vertexSubset& frontier, const long vSrc)
{
    if (frontier.d[vSrc]) // if vSrc in frontier
        return true;
    else
        return false;
}

// Compute one iteration, push-based
template <class vertex>
void pushEngine(graph<vertex>& G, Kernels& K)
{
    vertex* V = G.V; // list of vertices, graph structure, only one copy
    int nTasks = K.nTask;
    long n = G.n; // # of vertices
    Task** task = K.task;
    K.iniOneIter();
    uintE* index = K.UniFrontier.toSparse();
    long m = K.UniFrontier.m;
    parallel_for (long j = 0; j < m; ++j){ // outer parallel
        long vSrc = index[j];
        vertex src = V[vSrc];
        uintE outDegree = src.getOutDegree();
        parallel_for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
            for (int i = 0; i < nTasks; ++i)
                task[i]->condQ(K.nextUni,vSrc,src.getOutNeighbor(vDstOffset),src.getOutWeight(vDstOffset));
        }
    }
    K.finishOneIter();
}

void scheduleTask(Task** task, int& n)
{
    int i = 0;
    while (i < n){
        if (task[i]->finished()){
            cout << "Finished task " << typeid(*(task[i])).name() << endl;
            // task[i]->clear(); // child
            // task[i]->clearAll(); // parent
            delete task[i];
            for (int j = i; j < n-1; ++j) // simple schedule
                task[j] = task[j+1];
            task[--n] = NULL;
        } else
            i++;
    }
}

template <class vertex>
void Compute(graph<vertex>& G, Kernels& K, commandLine P)
{
    Task** task = K.task; // used for passing reference
    int cnt = 0;
    while (K.nTask > 0){ // one iteration
        cnt++;
#ifdef DEBUG
        cout << cnt << ": # of tasks: " << K.nTask << endl;
#endif
        pushEngine(G,K);
        scheduleTask(task,K.nTask);
    }
}

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P); // first declare

template <class vertex>
void framework(graph<vertex>& G, commandLine P)
{
    Kernels K;
    setKernels(G,K,P);

    // check validity
    K.initialize(G.n,G.isWeighted);

    startTime();
    Compute(G,K,P);
    nextTime("Running time");
    if(G.transposed)
        G.transpose();
    G.del();
}

int main(int argc, char* argv[]){
    commandLine P(argc,argv," [-s] <inFile>");
    char* iFile = P.getArgument(0);
    bool symmetric = P.getOptionValue("-s");
    bool weighted = P.getOptionValue("-w");
    bool compressed = P.getOptionValue("-c");
    bool binary = P.getOptionValue("-b");
    long rounds = P.getOptionLongValue("-rounds",1);

    if (!weighted) {
        if (!symmetric) {
            graph<asymmetricUnweightedVertex> G =
                readGraphFromFile<asymmetricUnweightedVertex>(iFile);
            framework(G,P);
        } else {
            graph<symmetricUnweightedVertex> G =
                readGraphFromFile<symmetricUnweightedVertex>(iFile);
            framework(G,P);
        }
    } else {
        if (!symmetric) {
            graph<asymmetricWeightedVertex> G =
                readGraphFromFile<asymmetricWeightedVertex>(iFile);
            framework(G,P);
        } else {
            graph<symmetricWeightedVertex> G =
                readGraphFromFile<symmetricWeightedVertex>(iFile);
            framework(G,P);
        }
    }
}

template <class F>
void vertexMap(vertexSubset V, F f) {
    long n = V.n;
    long m = V.m;
    if (V.isDense){
        parallel_for (long i = 0; i < n; ++i)
            if (V.d[i])
                f(i);
    } else {
        parallel_for (long i = 0; i < m; ++i)
            f(V.s[i]);
    }
}

template <class F>
vertexSubset vertexFilter(vertexSubset V, F filter) {
    long n = V.n;
    long m = V.m;
    V.toDense();
    bool* d_out = newA(bool,n);
    parallel_for (long i = 0; i < n; i++)
        d_out[i] = 0;
    parallel_for (long i = 0; i < n; i++)
        if (V.d[i])
            d_out[i] = filter(i);
    return vertexSubset(n,d_out);
}

template <class F>
bool* vertexFilter(vertexSubset V, F filter, int x) {
    long n = V.n;
    long m = V.m;
    V.toDense();
    bool* d_out = newA(bool,n);
    parallel_for (long i = 0; i < n; i++)
        d_out[i] = 0;
    parallel_for (long i = 0; i < n; i++)
        if (V.d[i])
            d_out[i] = filter(i);
    return d_out;
}

#endif