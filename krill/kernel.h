// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef KERNEL_H
#define KERNEL_H

#include <initializer_list> // variadic arguments
#include "utils.h"
#include "vertexSubset.h"
using namespace std;

#define MAX_TASK_NUM 20 // predefined max task num in the waiting list

class Task // base class (abstract class)
{
public:
    Task(long _nVertex, bool _isWeighted):
        n(_nVertex), active(false), nextFrontier(NULL), isWeighted(_isWeighted){};
    ~Task() = default;

    // *pure* virtual function used for correct function call
    virtual bool cond(uintE d) = 0;
    virtual bool finished() = 0;
    virtual void initialize() = 0;
    virtual void clear() = 0;
    virtual void condQ(const bool taskValid, const long vSrc, const long vDst, const intE edgeVal) = 0;
    virtual void iniOneIter(){
        nextFrontier = newA(bool,n);
        parallel_for (long i = 0; i < n; ++i) // remember to initialize!
            nextFrontier[i] = 0;
#ifdef DEBUG
        frontier.print();
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
    inline void freeMem(T* ptr){
        if (ptr != NULL)
            free(ptr);
    }
    template<class T>
    inline void setAll(T*& ptr, T val){ // pass reference!
        if (ptr == NULL)
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
    long n; // # of vertices
    vertexSubset frontier;
    bool* nextFrontier;
    bool isWeighted;
};

class UnweightedTask : public Task
{
public:
    UnweightedTask(long _nVertex):
        Task(_nVertex, false){};
    virtual bool update(uintE s, uintE d) = 0;
    virtual bool updateAtomic(uintE s, uintE d) = 0;
    void condQ(const bool taskValid, const long vSrc, const long vDst, const intE edgeVal = 0) // edgeVal is useless
    {
        if (taskValid && cond(vDst) && updateAtomic(vSrc,vDst))
            nextFrontier[vDst] = 1;
        // DO NOT SET ELSE! some memory may be accessed several times
    }
};

class WeightedTask : public Task
{
public:
    WeightedTask(long _nVertex):
        Task(_nVertex, true){};
    virtual bool update(uintE s, uintE d, intE edgeVal) = 0;
    virtual bool updateAtomic(uintE s, uintE d, intE edgeVal) = 0;
    void condQ(const bool taskValid, const long vSrc, const long vDst, const intE edgeVal)
    {
        if (taskValid && cond(vDst) && updateAtomic(vSrc,vDst,edgeVal))
            nextFrontier[vDst] = 1;
    }
};

class Kernels
{
public:
    Kernels(): nTask(0){
        task = new Task*[MAX_TASK_NUM]; // polymorphism, using `new' may be easier for deletion
    };
    ~Kernels() = default;
    void appendTask(Task* tsk)
    {
        task[nTask++] = tsk;
    }
    void appendTask(initializer_list<Task*> list)
    {
        for (auto tsk : list)
            task[nTask++] = tsk;
    }
    int nTask;
    Task** task; // 1D array to store pointers of the tasks
};

class Function // used for vertexMap
{
public:
    Function() = default;
    ~Function() = default;
    virtual bool operator() (uintE i) = 0;
};

#endif