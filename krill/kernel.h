// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef KERNEL_H
#define KERNEL_H

#include <initializer_list> // variadic arguments
#include "utils.h"
#include "vertexSubset.h"
using namespace std;

// predefined max job num in the waiting list
// determined by the width of CPU register
#define MAX_JOB_NUM 128

class Job // base class (abstract class)
{
public:
    Job(long _nVertex, bool _isWeighted, bool _isSingleton = false):
        n(_nVertex), active(false), nextFrontier(NULL),
        isWeighted(_isWeighted), isSingleton(_isSingleton){};
    ~Job() = default;

    // *pure* virtual function used for correct function call
    virtual bool cond(uintE d) = 0;
    virtual bool finished(int iter) { return frontier.isEmpty(); }
    virtual void initialize() = 0;
    virtual void condPush(uintE& out, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPush(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPull(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPullAtomic(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPullNoCond(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPullNoCondAtomic(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPushSingle(const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPullSingle(const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPullSingleAtomic(const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void iniOneIter(){
        nextFrontier = newA(bool,n); // DO NOT FREE nextFrontier
        parallel_for (long i = 0; i < n; ++i) // remember to initialize!
            nextFrontier[i] = 0;
        frontier.toDense();
#ifdef DEBUG
        frontier.print(2);
#endif
    }
    virtual void finishOneIter(bool* nextUni){
        frontier.del();
        // set new frontier
        setFrontier(n,nextFrontier);
    }
    inline void clearAll(){
        active = false;
        // frontier.del(); // free may have something wrong...
        // if (nextFrontier != NULL)
        //     free(nextFrontier);
    }
    template<class T>
    inline void freeMem(T*& ptr){
        if (ptr != NULL){
            free(ptr); ptr = NULL;
        }
    }
    template<class T>
    inline void setAll(T*& ptr, T val){ // pass reference!
        // DO NOT FREE PTR!
        ptr = newA(T,n);
        parallel_for(long i = 0; i < n; ++i)
             ptr[i] = val;
    }
    inline void setFrontierAll(){ // select all vertices
        bool* allVert = newA(bool,n);
        parallel_for(long i = 0; i < n; i++)
            allVert[i] = 1;
        setFrontier(n,n,allVert);
    }
    inline void setFrontier(long _n, intE v){
        frontier = vertexSubset(_n,v);
    }
    inline void setFrontier(long _n){
        frontier = vertexSubset(_n);
    }
    inline void setFrontier(long _n, bool* bits){
        frontier = vertexSubset(_n,bits);
    }
    inline void setFrontier(long _n, long _m, bool* bits){
        frontier = vertexSubset(_n,_m,bits);
    }
    void printNextFrontier(){
        for (long i = 0; i < n; ++i)
            cout << nextFrontier[i] << ((i != n-1) ? " " : "\n");
    }
    bool active; // be careful of the struct member order
    int ID;
    long n; // # of vertices
    vertexSubset frontier;
    bool* nextFrontier;
    bool isWeighted;
    bool isSingleton;
};

class UnweightedJob : public Job
{
public:
    UnweightedJob(long _nVertex, bool _isSingleton = false):
        Job(_nVertex, false, _isSingleton){};
    virtual bool update(uintE s, uintE d) = 0;
    virtual bool updateAtomic(uintE s, uintE d) = 0;
    // sparse
    void condPush(uintE& out, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            out = vDst;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    // dense
    void condPush(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPull(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (cond(vDst) && frontier.d[vSrc] && update(vSrc, vDst))
        {
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPullAtomic(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (cond(vDst) && frontier.d[vSrc] && updateAtomic(vSrc, vDst))
        {
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPullNoCond(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && update(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPullNoCondAtomic(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && updateAtomic(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPushSingle(const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (cond(vDst) && updateAtomic(vSrc, vDst))
            nextFrontier[vDst] = 1; // need not atomic
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPullSingle(const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && update(vSrc,vDst))
            nextFrontier[vDst] = 1; // need not atomic
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPullSingleAtomic(const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && updateAtomic(vSrc,vDst))
            nextFrontier[vDst] = 1; // need not atomic
        // DO NOT SET ELSE! some memory may be accessed several times
    }
};

class WeightedJob : public Job
{
public:
    WeightedJob(long _nVertex, bool _isSingleton = false):
        Job(_nVertex, true, _isSingleton){};
    virtual bool update(uintE s, uintE d, intE edgeVal) = 0;
    virtual bool updateAtomic(uintE s, uintE d, intE edgeVal) = 0;
    // sparse
    void condPush(uintE& out, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1; // need not atomic
            out = vDst;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    // dense
    void condPush(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1;
            nextUni[vDst] = 1;
        }
    }
    void condPull(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (cond(vDst) && frontier.d[vSrc] && update(vSrc, vDst, edgeVal))
        {
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
    }
    void condPullAtomic(bool *&nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (cond(vDst) && frontier.d[vSrc] && updateAtomic(vSrc, vDst, edgeVal))
        {
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
    }
    void condPullNoCond(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && update(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1;
            nextUni[vDst] = 1;
        }
    }
    void condPullNoCondAtomic(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && updateAtomic(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1;
            nextUni[vDst] = 1;
        }
    }
    void condPushSingle(const long vSrc, const long vDst, const intE edgeVal)
    {
        if (cond(vDst) && updateAtomic(vSrc, vDst, edgeVal))
            nextFrontier[vDst] = 1;
    }
    void condPullSingle(const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && update(vSrc,vDst,edgeVal))
            nextFrontier[vDst] = 1;
    }
    void condPullSingleAtomic(const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && updateAtomic(vSrc,vDst,edgeVal))
            nextFrontier[vDst] = 1;
    }
};

class Kernels
{
public:
    Kernels(): nJob(0),nCJob(0),nSJob(0),nextUni(NULL),nextSpUni(NULL){
        job = new Job*[MAX_JOB_NUM]; // polymorphism, using `new' may be easier for deletion
        cJob = new Job*[MAX_JOB_NUM];
        sJob = new Job*[MAX_JOB_NUM];
    };
    ~Kernels() = default;
    void appendJob(Job* tsk)
    {
        job[nJob++] = tsk;
#if defined (NOKERF) || defined(NOOPT)
        cJob[nCJob++] = tsk;
#else
        if (!tsk->isSingleton)
            cJob[nCJob++] = tsk;
        else
            sJob[nSJob++] = tsk;
#endif
    }
    void appendJob(initializer_list<Job*> list)
    {
        for (auto tsk : list)
            appendJob(tsk);
    }
    void initialize(const long nVert_, const bool isWeightedGraph){
        nVert = nVert_;
        for (int i = 0; i < nJob; ++i){
            if (job[i]->isWeighted && !isWeightedGraph){ // graph is unweighted, but there are weighted jobs
                cerr << "Error: Unweighted graph with weighted jobs!" << endl;
                abort();
            }
            if (job[i]->n != nVert){
                cerr << "Error: Inconsistent number of vertices." << endl;
                abort();
            }
        }
        bool* originalUni = newA(bool,nVert);
        parallel_for (int i = 0; i < nVert; ++i)
            originalUni[i] = 0;
        parallel_for (int i = 0; i < nJob; ++i){
            Job* t = job[i];
            t->active = true;
            t->ID = i;
            t->initialize();
            vertexSubset f = t->frontier;
            uintE* s = f.toSparse();
            long m = f.m;
            if (!t->isSingleton)
                parallel_for (long j = 0; j < m; ++j)
                    originalUni[s[j]] = 1;
        }
        UniFrontier = vertexSubset(nVert,originalUni);
    }
    void iniOneIter(int cORs = 2){
        // 0: cJob  1: sJobs  2: both
        if (cORs == 1)
            parallel_for(int i = 0; i < nSJob; ++i)
                sJob[i]
                    ->iniOneIter();
        else
        {
            if (cORs == 0)
                parallel_for(int i = 0; i < nCJob; ++i)
                    cJob[i]
                        ->iniOneIter();
            else
            {
                parallel_for(int i = 0; i < nCJob; ++i)
                    cJob[i]
                        ->iniOneIter();
                parallel_for(int i = 0; i < nSJob; ++i)
                    sJob[i]
                        ->iniOneIter();
            }
            flagSparse = false;
            nextM = 0;
        }
    }
    void finishOneIter(int cORs = 2){
        // 0: cJob  1: sJobs  2: both
        if (cORs == 1)
            parallel_for(int i = 0; i < nSJob; ++i)
                sJob[i]
                    ->finishOneIter(nextUni);
        else
        {
            if (cORs == 0)
                parallel_for(int i = 0; i < nCJob; ++i)
                    cJob[i]
                        ->finishOneIter(nextUni);
            else
            {
                parallel_for(int i = 0; i < nCJob; ++i)
                    cJob[i]
                        ->finishOneIter(nextUni);
                parallel_for(int i = 0; i < nSJob; ++i)
                    sJob[i]
                        ->finishOneIter(nextUni);
            }
            UniFrontier.del();
            // set new frontier
            if (!flagSparse)
            {
                UniFrontier = ((nextUni != NULL) ? vertexSubset(nVert, nextUni) : vertexSubset(nVert));
                nextUni = NULL;
                nextSpUni = NULL;
            }
            else
            {
                UniFrontier = ((nextSpUni != NULL) ? vertexSubset(nVert, nextM, nextSpUni) : vertexSubset(nVert));
                if (nextUni != NULL)
                {
                    UniFrontier.toDense();
                    parallel_for (long i = 0; i < nVert; ++i)
                        UniFrontier.d[i] |= nextUni[i];
                    free(UniFrontier.s);
                    UniFrontier.s = NULL;
                }
                nextUni = NULL;
                nextSpUni = NULL;
            }
        }
    }
    void finish(){
        if (nextSpUni != NULL)
            free(nextSpUni);
        delete [] job;
        // delete [] cJob;
        // delete [] sJob;
    }
    inline bool* denseMode(){
        flagSparse = false;
        nextUni = newA(bool,nVert); // DO NOT FREE nextFrontier
        parallel_for (long i = 0; i < nVert; ++i) // remember to initialize!
            nextUni[i] = 0;
        UniFrontier.toDense();
        return nextUni;
    }
    long nVert;
    int pushDenseCnt = 0;
    int pushSparseCnt = 0;
    int pullDenseCnt = 0;
    int pullSparseCnt = 0;
    bool flagThreshold = true;
    int nJob;
    Job** job; // 1D array to store pointers of the jobs
    int nCJob;
    int nSJob;
    Job** cJob; // concurrent jobs
    Job** sJob; // singleton jobs
    bool flagSparse;
    bool* nextUni;
    long nextM;
    uintE* nextSpUni;
    vertexSubset UniFrontier;
};

class Function // used for vertexMap
{
public:
    Function() = default;
    ~Function() = default;
    virtual bool operator() (uintE i) = 0;
};

#endif