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
#include <thread>
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
    K.pushSparseCnt++;
    int nCJobs = K.nCJob;
    Job** job = K.cJob;
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
                for (int i = 0; i < nCJobs; ++i)
                    job[i]->condPush(outEdges[os+vDstOffset],
                        vSrc,src.getOutNeighbor(vDstOffset),
                        src.getOutWeight(vDstOffset));
            }
        } else {
            parallel_for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nCJobs; ++i)
                    job[i]->condPush(outEdges[os+vDstOffset],
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
    K.pushDenseCnt++;
    int nCJobs = K.nCJob;
    long n = K.nVert; // # of vertices
    Job** job = K.cJob;
    K.denseMode();
    parallel_for (long vSrc = 0; vSrc < n; ++vSrc){ // outer parallel
        vertex src = V[vSrc];
        uintE outDegree = src.getOutDegree();
        if (outDegree < SEQ_THRESHOLD){
            for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nCJobs; ++i)
                    job[i]->condPush(K.nextUni,
                        vSrc,src.getOutNeighbor(vDstOffset),
                        src.getOutWeight(vDstOffset));
            }
        } else {
            parallel_for (long vDstOffset = 0; vDstOffset < outDegree; ++vDstOffset){ // inner parallel
                for (int i = 0; i < nCJobs; ++i)
                    job[i]->condPush(K.nextUni,
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
    K.pullDenseCnt++;
    int nCJobs = K.nCJob;
    long n = K.nVert; // # of vertices
    Job** job = K.cJob;
    K.denseMode();
    parallel_for (long vDst = 0; vDst < n; ++vDst){
        int cntJobs = 0;
        Job** currJob = newA(Job*,nCJobs);
        for (int i = 0; i < nCJobs; ++i)
            if (job[i]->cond(vDst))
                currJob[cntJobs++] = job[i];
        vertex dst = V[vDst];
        uintE inDegree = dst.getInDegree();
        if (inDegree < SEQ_THRESHOLD){
            for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                for (int i = 0; i < cntJobs; ++i){
                    currJob[i]->condPull(K.nextUni,
                        dst.getInNeighbor(vSrcOffset),vDst,
                        dst.getInWeight(vSrcOffset));
                    if (!currJob[i]->cond(vDst)){ // early break!
                        cntJobs--;
                        for (int j = i; j < cntJobs; ++j)
                            currJob[j] = currJob[j+1];
                        continue;
                    }
                }
            }
        } else {
            parallel_for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                for (int i = 0; i < cntJobs; ++i)
                    currJob[i]->condPullAtomic(K.nextUni,
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
void pullSingle(vertex*& V, Job*& tsk)
{
#ifdef DEBUG
    auto t1 = Clock::now();
#endif
    long n = tsk->n; // # of vertices
    parallel_for (long vDst = 0; vDst < n; ++vDst){
        vertex dst = V[vDst];
        uintE inDegree = dst.getInDegree();
        if (!tsk->cond(vDst)) continue;
        if (inDegree < SEQ_THRESHOLD){
            for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                tsk->condPullSingle(dst.getInNeighbor(vSrcOffset),vDst,
                        dst.getInWeight(vSrcOffset));
                if (!tsk->cond(vDst)) // early break!
                    break;
            }
        } else {
            parallel_for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                tsk->condPullSingleAtomic(dst.getInNeighbor(vSrcOffset),vDst,
                    dst.getInWeight(vSrcOffset));
            }
        }
    }
#ifdef DEBUG
    auto t2 = Clock::now();
    cout << "pull single time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns" << endl;
#endif
}

// Compute one iteration, pull-based (dense)
template <class vertex>
void pullAll(vertex*& V, Job**& tsk, int nVert, int nJobs)
{
#ifdef DEBUG
    auto t1 = Clock::now();
#endif
    parallel_for (long vDst = 0; vDst < nVert; ++vDst){
        for (int i = 0; i < nJobs; ++i){
            Job* job = tsk[i];
            if (!job->cond(vDst)) continue;
            vertex dst = V[vDst];
            uintE inDegree = dst.getInDegree();
            if (inDegree < SEQ_THRESHOLD){
                for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                    job->condPullSingle(dst.getInNeighbor(vSrcOffset),vDst,
                                dst.getInWeight(vSrcOffset));
                    if (!job->cond(vDst)) // early break!
                        break;
                }
            } else {
                parallel_for (long vSrcOffset = 0; vSrcOffset < inDegree; ++vSrcOffset){
                    job->condPullSingleAtomic(dst.getInNeighbor(vSrcOffset),vDst,
                        dst.getInWeight(vSrcOffset));
                }
            }
        }
    }
#ifdef DEBUG
    auto t2 = Clock::now();
    cout << "pull all time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns" << endl;
#endif
}

void scheduleJob(Job**& job, int& nJob)
{
    int i = 0;
    while (i < nJob){
        if (job[i]->finished()){
            cout << "Finished job " << typeid(*(job[i])).name() << endl;
            // job[i]->clear(); // child
            // job[i]->clearAll(); // parent
            delete job[i];
            for (int j = i; j < nJob-1; ++j) // simple schedule
                job[j] = job[j+1];
            job[--nJob] = NULL;
        } else
            i++;
    }
}

template <class vertex>
void Compute(graph<vertex>& G, Kernels& K)
{
    vertex* V = G.V; // list of vertices, graph structure, only one copy
    uintT* UniFrontier = K.UniFrontier.toSparse();
    long m = K.UniFrontier.m;
    uintT* degrees = newA(uintT, m);
    parallel_for (long i = 0; i < m; ++i)
        degrees[i] = V[UniFrontier[i]].getOutDegree();
    // for each iteration, select which engine to use
    uintT outDegrees = sequence::plusReduce(degrees, m);
    intT threshold = G.m / 2; // f(# of vertices)
    if (outDegrees == 0) {
        K.denseMode();
        free(degrees);
        return;
    }
    if (m + outDegrees > threshold){
        free(degrees);
        pushDense(V,K);
    } else {
        if (m + outDegrees > G.m / 5)
            pushSparse(V,K,degrees,true);
        else
            pushSparse(V,K,degrees); // sparse index
        free(degrees);
    }
}

template <class vertex>
void Execute(graph<vertex>& G, Kernels& K, commandLine P)
{
    int iter = 0;
    while (K.nCJob + K.nSJob > 0){ // one iteration
        iter++;
#ifdef DEBUG
        cout << iter << ": # of jobs: " << K.nJob << endl;
#endif
        if (K.nSJob == 0){
            K.iniOneIter();
            Compute(G,K);
            K.finishOneIter();

            scheduleJob(K.cJob,K.nCJob);
        } else {
            K.iniOneIter();

#if defined(CILKP)
            cilk_spawn pullAll<vertex>(G.V, K.sJob, K.nVert, K.nSJob);
#else
            // thread singleJobThreads[K.nSJob];
            // for (int i = 0; i < K.nSJob; ++i)
            //     singleJobThreads[i] = thread(pullSingle<vertex>,ref(G.V),ref(K.sJob[i]));
            thread singleJobAll = thread(pullAll<vertex>, ref(G.V), ref(K.sJob), K.nVert, K.nSJob);
#endif

            if (K.nCJob != 0)
                Compute(G,K);

#if defined(CILKP)
            cilk_sync;
#else
            singleJobAll.join();
            // for (int i = 0; i < K.nSJob; ++i)
            //     singleJobThreads[i].join();
#endif

            K.finishOneIter();
            scheduleJob(K.cJob,K.nCJob);
            scheduleJob(K.sJob,K.nSJob);
        }
    }
    K.finish();
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
#ifdef DEBUG
    cout << "Push-Dense: " << K.pushDenseCnt << endl;
    cout << "Push-Sparse: " << K.pushSparseCnt << endl;
    cout << "Pull-Dense: " << K.pullDenseCnt << endl;
    cout << "Pull-Sparse: " << K.pullSparseCnt << endl;
#endif
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
            startTime();
            graph<asymmetricUnweightedVertex> G =
                readGraphFromFile<asymmetricUnweightedVertex>(iFile);
            nextTime("Graph IO time");
            framework(G,P);
        } else {
            startTime();
            graph<symmetricUnweightedVertex> G =
                readGraphFromFile<symmetricUnweightedVertex>(iFile);
            nextTime("Graph IO time");
            framework(G,P);
        }
    } else {
        if (!symmetric) {
            startTime();
            graph<asymmetricWeightedVertex> G =
                readGraphFromFile<asymmetricWeightedVertex>(iFile);
            nextTime("Graph IO time");
            framework(G,P);
        } else {
            startTime();
            graph<symmetricWeightedVertex> G =
                readGraphFromFile<symmetricWeightedVertex>(iFile);
            nextTime("Graph IO time");
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