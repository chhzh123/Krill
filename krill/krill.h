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

inline void condQ(const bool taskValid, Task& task, const long vSrc, const long vDst)
{
    if (taskValid && task.cond(vDst) && task.updateAtomic(vSrc,vDst))
        task.nextFrontier[vDst] = 1;
    // DO NOT SET ELSE! some memory may be accessed several times
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
                condQ(taskmask[i],*(task[i]),vSrc,src.getOutNeighbor(vDstOffset));
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
            task[i]->clear();
            for (int j = i; j < n-1; ++j)
                task[j] = task[j+1];
            n--;
        }
        i++;
    }
}

template <class vertex>
void Compute(graph<vertex>& G, Kernels& K, commandLine P)
{
    int nTask = K.nTask;
    Task** task = K.task;
    for (int i = 0; i < nTask; ++i){
        task[i]->active = true;
        (task[i])->initialize();
    }
    int cnt = 0;
    while (nTask > 0){
        pushEngine(G,K);
        scheduleTask(task,nTask);
    }
}

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P); // first declare

int main(int argc, char* argv[]){
    commandLine P(argc,argv," [-s] <inFile>");
    char* iFile = P.getArgument(0);
    bool symmetric = P.getOptionValue("-s");
    bool compressed = P.getOptionValue("-c");
    bool binary = P.getOptionValue("-b");
    long rounds = P.getOptionLongValue("-rounds",1);

    graph<asymmetricVertex> G =
        readGraph<asymmetricVertex>(iFile,compressed,symmetric,binary);

    Kernels K;
    setKernels(G,K,P);

    startTime();
    Compute(G,K,P);
    nextTime("Running time");
    if(G.transposed)
        G.transpose();
    G.del();
}

#endif