// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef KRILL_H
#define KRILL_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
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

inline bool inFrontierQ(vertexSubset& frontier, const long vSrc)
{
    frontier.toDense();
    if (frontier.d[vSrc]) // if vSrc in frontier
        return true;
    else
        return false;
}

// Compute one iteration, push-based
template <class vertex>
void pushEngine(graph<vertex>& G, Kernels& K)
{
    // asymmetricVertex(vertex)
    //   uintE *inNeighbors, *outNeighbors;
    //   uintT inDegree, outDegree;
    vertex* V = G.V; // list of vertices, graph structure, only one copy
    int nTasks = K.nTask;
    long n = G.n; // # of vertices
    Task** task = K.task;
    bool* taskmask = newA(bool,nTasks); // bitmap for tasks
    for (int i = 0; i < nTasks; ++i)
        // task[i]->iniOneIter(V);
        task[i]->iniOneIter();
    parallel_for (long vSrc = 0; vSrc < n; ++vSrc){ // outer parallel
        for (int i = 0; i < nTasks; ++i) // needn't parallel
            taskmask[i] = inFrontierQ(task[i]->frontier,vSrc) ? 1 : 0; // pass reference!
        // at this time, all the task should get into inner loop
        // get vSrc's ngh and degree
        vertex src = V[vSrc]; // get src vertex info
        parallel_for (long vDstOffset = 0; vDstOffset < src.getOutDegree(); ++vDstOffset){ // inner parallel
            for (int i = 0; i < nTasks; ++i)
                task[i]->condQ(taskmask[i],vSrc,src.getOutNeighbor(vDstOffset),src.getOutWeight(vDstOffset));
        }
    }
    // finish one iteration
    for (int i = 0; i < nTasks; ++i)
        task[i]->finishOneIter();
}

void scheduleTask(Task** task, int& n)
{
    int i = 0;
    while (i < n){
        if (task[i]->active && task[i]->finished()){
            cout << "Finished task " << i << "!" << endl;
            task[i]->clear(); // child
            task[i]->clearAll(); // parent
            delete task[i];
            for (int j = i; j < n-1; ++j) // simple schedule
                task[j] = task[j+1];
            n--;
        }
        i++;
    }
}

template <class vertex>
void Compute(graph<vertex>& G, Kernels& K, commandLine P)
{
    // int nTask = K.nTask;
    Task** task = K.task;
    for (int i = 0; i < K.nTask; ++i){
        task[i]->active = true;
        (task[i])->initialize();
    }
    int cnt = 0;
    while (K.nTask > 0){ // One iteration
        cnt++;
        cout << cnt << ": There are " << K.nTask << " tasks!" << endl;
        pushEngine(G,K);
        scheduleTask(task,K.nTask);
    }
}

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P); // first declare

template <class vertex>
bool checkGraphKernelsValid(graph<vertex>&G, Kernels& K)
{
    bool flag = false;
    for (int i = 0; i < K.nTask; ++i)
        if ((K.task[i])->isWeighted){
            flag = true;
            break;
        }
    if (flag && !G.isWeighted) // graph is unweighted, but there are weighted tasks
        return false;
    else
        return true;
}

template <class vertex>
void framework(graph<vertex>& G, commandLine P)
{
    Kernels K;
    setKernels(G,K,P);

    if (!checkGraphKernelsValid(G,K)){
        cerr << "Error: Unweighted graph with weighted tasks!" << endl;
        abort();
    }

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
            if (V.s[i])
                f(V.s[i]);
    }
}

#endif