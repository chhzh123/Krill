// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef KERNEL_H
#define KERNEL_H

#include <initializer_list> // variadic arguments
#include "utils.h"
#include "vertexSubset.h"
using namespace std;

// predefined max task num in the waiting list
// determined by the width of CPU register
#define MAX_TASK_NUM 64

class Task // base class (abstract class)
{
public:
    Task(long _nVertex, bool _isWeighted, bool _isSingleton = false):
        n(_nVertex), active(false), nextFrontier(NULL),
        isWeighted(_isWeighted), isSingleton(_isSingleton){};
    ~Task() = default;

    // *pure* virtual function used for correct function call
    virtual bool cond(uintE d) = 0;
    virtual bool finished() = 0;
    virtual void initialize() = 0;
    virtual void clear() = 0;
    virtual void condPush(uintE& out, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPush(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPull(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void condPullSingle(const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void iniOneIter(){
        nextFrontier = newA(bool,n); // DO NOT FREE nextFrontier
        parallel_for (long i = 0; i < n; ++i) // remember to initialize!
            nextFrontier[i] = 0;
        frontier.toDense();
#ifdef DEBUG
        frontier.print(2);
#endif
    }
    virtual void finishOneIter(){
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

class UnweightedTask : public Task
{
public:
    UnweightedTask(long _nVertex, bool _isSingleton = false):
        Task(_nVertex, false, _isSingleton){};
    virtual bool update(uintE s, uintE d) = 0;
    virtual bool updateAtomic(uintE s, uintE d) = 0;
    void condPush(uintE& out, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            out = vDst;
        } else
            out = UINT_E_MAX;
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPush(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPull(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && update(vSrc,vDst)){
            nextFrontier[vDst] = 1; // need not atomic
            nextUni[vDst] = 1;
        }
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPullSingle(const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && update(vSrc,vDst))
            nextFrontier[vDst] = 1; // need not atomic
        // DO NOT SET ELSE! some memory may be accessed several times
    }
};

class WeightedTask : public Task
{
public:
    WeightedTask(long _nVertex, bool _isSingleton = false):
        Task(_nVertex, true, _isSingleton){};
    virtual bool update(uintE s, uintE d, intE edgeVal) = 0;
    virtual bool updateAtomic(uintE s, uintE d, intE edgeVal) = 0;
    void condPush(uintE& out, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1; // need not atomic
            out = vDst;
        } else
            out = UINT_E_MAX;
        // DO NOT SET ELSE! some memory may be accessed several times
    }
    void condPush(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && cond(vDst) && updateAtomic(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1;
            nextUni[vDst] = 1;
        }
    }
    void condPull(bool*& nextUni, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && update(vSrc,vDst,edgeVal)){
            nextFrontier[vDst] = 1;
            nextUni[vDst] = 1;
        }
    }
    void condPullSingle(const long vSrc, const long vDst, const intE edgeVal)
    {
        if (frontier.d[vSrc] && update(vSrc,vDst,edgeVal))
            nextFrontier[vDst] = 1;
    }
};

class Kernels
{
public:
    Kernels(): nTask(0),nCTask(0),nSTask(0),nextUni(NULL),nextSpUni(NULL){
        task = new Task*[MAX_TASK_NUM]; // polymorphism, using `new' may be easier for deletion
        cTask = new Task*[MAX_TASK_NUM];
        sTask = new Task*[MAX_TASK_NUM];
    };
    ~Kernels() = default;
    void appendTask(Task* tsk)
    {
        task[nTask++] = tsk;
        if (!tsk->isSingleton)
            cTask[nCTask++] = tsk;
        else
            sTask[nSTask++] = tsk;
    }
    void appendTask(initializer_list<Task*> list)
    {
        for (auto tsk : list)
            appendTask(tsk);
    }
    void initialize(const long nVert_, const bool isWeightedGraph){
        nVert = nVert_;
        for (int i = 0; i < nTask; ++i){
            if (task[i]->isWeighted && !isWeightedGraph){ // graph is unweighted, but there are weighted tasks
                cerr << "Error: Unweighted graph with weighted tasks!" << endl;
                abort();
            }
            if (task[i]->n != nVert){
                cerr << "Error: Inconsistent number of vertices." << endl;
                abort();
            }
        }
        bool* originalUni = newA(bool,nVert);
        parallel_for (int i = 0; i < nVert; ++i)
            originalUni[i] = 0;
        parallel_for (int i = 0; i < nTask; ++i){
            Task* t = task[i];
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
    void iniOneIter(){
        parallel_for (int i = 0; i < nCTask; ++i)
            cTask[i]->iniOneIter();
        parallel_for (int i = 0; i < nSTask; ++i)
            sTask[i]->iniOneIter();
        flagSparse = false;
        nextM = 0;
    }
    void finishOneIter(){
        parallel_for (int i = 0; i < nCTask; ++i)
            cTask[i]->finishOneIter();
        parallel_for (int i = 0; i < nSTask; ++i)
            sTask[i]->finishOneIter();
        UniFrontier.del();
        // set new frontier
        if (!flagSparse){
            UniFrontier = ((nextUni != NULL) ?
                vertexSubset(nVert,nextUni) :
                vertexSubset(nVert));
            nextUni = NULL; nextSpUni = NULL;
        } else {
            UniFrontier = ((nextSpUni != NULL) ?
                vertexSubset(nVert,nextM,nextSpUni) :
                vertexSubset(nVert));
            nextUni = NULL; nextSpUni = NULL;
        }
    }
    void finish(){
        if (nextSpUni != NULL)
            free(nextSpUni);
        delete [] task;
        // delete [] cTask;
        // delete [] sTask;
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
    int nTask;
    Task** task; // 1D array to store pointers of the tasks
    int nCTask;
    int nSTask;
    Task** cTask; // concurrent tasks
    Task** sTask; // singleton tasks
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