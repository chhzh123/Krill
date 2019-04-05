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
#include <chrono> // timing
#include "kernel.h"
#include "graph.h"
#include "vertex.h"
#include "vertexSubset.h"
#include "utils.h"
#include "gettime.h"
#include "parseCommandLine.h"
#include "IO.h"
using namespace std;

using Clock = std::chrono::high_resolution_clock;

#define SEQ_THRESHOLD 1000

// cond function that always returns true
inline bool cond_true (intT d) { return 1; }

// Compute one iteration, push-based (sparse, indices)
template <class vertex>
void pushSparse(vertex*& V, Kernels& K, uintT* degrees, bool remDups = false)
{
#ifdef DEBUG
    auto t1 = Clock::now();
#endif
    K.countPush++;
    int nTasks = K.nTask;
    Task** task = K.task;
    uintE* indices = K.UniFrontier.toSparse();
    long m = K.UniFrontier.m;
    K.flagSparse = true;

    uintT* offsets = degrees;
    long nOutEdges = sequence::plusScan(offsets,degrees,m); // accumulate offsets
    uintE* outEdges = newA(uintE,nOutEdges);
    parallel_for (long j = 0; j < m; ++j){ // outer parallel
        long vSrc = indices[j];
        uintT os = offsets[j];
        vertex src = V[vSrc];
        uintE outDegree = src.getOutDegree();
        if (outDegree < SEQ_THRESHOLD){
            for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nTasks; ++i)
                    task[i]->condPush(outEdges[os+vDstOffset],
                        vSrc,src.getOutNeighbor(vDstOffset),
                        src.getOutWeight(vDstOffset));
            }
        } else {
            parallel_for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nTasks; ++i)
                    task[i]->condPush(outEdges[os+vDstOffset],
                        vSrc,src.getOutNeighbor(vDstOffset),
                        src.getOutWeight(vDstOffset));
            }
        }
    }
    uintE* nextIndices = newA(uintE, nOutEdges);
    if (remDups)
        remDuplicates(outEdges,NULL,nOutEdges,K.nVert);
    // Filter out the empty slots (marked with -1)
    long nextM = sequence::filter(outEdges,nextIndices,nOutEdges,nonMaxF());
    K.nextSpUni = nextIndices;
    K.nextM = nextM;

    free(outEdges);
#ifdef DEBUG
    auto t2 = Clock::now();
    cout << "push sparse time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns" << endl;
#endif
}

template <class vertex>
void pushDense(vertex*& V, Kernels& K)
{
#ifdef DEBUG
    auto t1 = Clock::now();
#endif
    K.countPush++;
    int nTasks = K.nTask;
    long n = K.nVert; // # of vertices
    Task** task = K.task;
    K.UniFrontier.toDense();
    parallel_for (long vSrc = 0; vSrc < n; ++vSrc){ // outer parallel
        vertex src = V[vSrc];
        uintE outDegree = src.getOutDegree();
        if (outDegree < SEQ_THRESHOLD){
            for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nTasks; ++i)
                    task[i]->condPush(K.nextUni,
                        vSrc,src.getOutNeighbor(vDstOffset),
                        src.getOutWeight(vDstOffset));
            }
        } else {
            parallel_for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nTasks; ++i)
                    task[i]->condPush(K.nextUni,
                        vSrc,src.getOutNeighbor(vDstOffset),
                        src.getOutWeight(vDstOffset));
            }
        }
    }
#ifdef DEBUG
    auto t2 = Clock::now();
    cout << "push dense time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns" << endl;
#endif
}

// Compute one iteration, pull-based (dense, 0-1)
template <class vertex>
void pullDense(vertex*& V, Kernels& K)
{
#ifdef DEBUG
    auto t1 = Clock::now();
#endif
    K.countPull++;
    int nTasks = K.nTask;
    long n = K.nVert; // # of vertices
    Task** task = K.task;
    K.UniFrontier.toDense();
    parallel_for (long vDst = 0; vDst < n; ++vDst){
        int cntTasks = 0;
        Task** currTask = newA(Task*,nTasks);
        for (int i = 0; i < nTasks; ++i)
            if (task[i]->cond(vDst))
                currTask[cntTasks++] = task[i];
        vertex dst = V[vDst];
        uintE inDegree = dst.getInDegree();
        if (inDegree < SEQ_THRESHOLD){
            for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                for (int i = 0; i < cntTasks; ++i){
                    currTask[i]->condPull(K.nextUni,
                        dst.getInNeighbor(vSrcOffset),vDst,
                        dst.getInWeight(vSrcOffset));
                    if (!currTask[i]->cond(vDst)){ // early break!
                        cntTasks--;
                        for (int j = i; j < cntTasks; ++j)
                            currTask[j] = currTask[j+1];
                        continue;
                    }
                }
            }
        } else {
            parallel_for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                for (int i = 0; i < cntTasks; ++i)
                    currTask[i]->condPull(K.nextUni,
                        dst.getInNeighbor(vSrcOffset),vDst,
                        dst.getInWeight(vSrcOffset));
            }
        }
    }
#ifdef DEBUG
    auto t2 = Clock::now();
    cout << "pull dense time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns" << endl;
#endif
}

// Compute one iteration, pull-based (dense)
template <class vertex>
void pullSingle(vertex*& V, Task*& tsk, bool*& nextUni)
{
#ifdef DEBUG
    auto t1 = Clock::now();
#endif
    long n = tsk->n; // # of vertices
    parallel_for (long vDst = 0; vDst < n; ++vDst){
        vertex dst = V[vDst];
        uintE inDegree = dst.getInDegree();
        if (inDegree < SEQ_THRESHOLD){
            for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                tsk->condPull(nextUni,
                        dst.getInNeighbor(vSrcOffset),vDst,
                        dst.getInWeight(vSrcOffset));
                if (!tsk->cond(vDst)) // early break!
                    break;
            }
        } else {
            parallel_for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                tsk->condPull(nextUni,
                    dst.getInNeighbor(vSrcOffset),vDst,
                    dst.getInWeight(vSrcOffset));
            }
        }
    }
#ifdef DEBUG
    auto t2 = Clock::now();
    cout << "pullSingle time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns" << endl;
#endif
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
void Compute(graph<vertex>& G, Kernels& K)
{
    vertex* V = G.V; // list of vertices, graph structure, only one copy
    K.iniOneIter();

    uintT* UniFrontier = K.UniFrontier.toSparse();
    long m = K.UniFrontier.m;
    uintT* degrees = newA(uintT, m);
    parallel_for (long i = 0; i < m; ++i)
        degrees[i] = V[UniFrontier[i]].getOutDegree();
    // for each iteration, select which engine to use
    uintT outDegrees = sequence::plusReduce(degrees,m);
    if (outDegrees != 0){
        intT threshold = G.m / 20; // # of edges / 20
        if (m + outDegrees > threshold)
            pushDense(V,K);
        else
            pushSparse(V,K,degrees); // sparse index
    }
    //// single task
    // int nTasks = K.nTask;
    // Task** task = K.task;
    // parallel_for (int i = 0; i < nTasks; ++i)
    //     pullSingle(V,K.task[i],K.nextUni);

    free(degrees);
    K.finishOneIter();
}

template <class vertex>
void Execute(graph<vertex>& G, Kernels& K, commandLine P)
{
    Task** task = K.task; // used for passing reference
    int cnt = 0;
    while (K.nTask > 0){ // one iteration
        cnt++;
#ifdef DEBUG
        cout << cnt << ": # of tasks: " << K.nTask << endl;
#endif
        Compute(G,K);
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
    Execute(G,K,P);
    nextTime("Running time");
    cout << "Push: " << K.countPush << endl;
    cout << "Pull: " << K.countPull << endl;
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