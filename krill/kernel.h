// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef KERNEL_H
#define KERNEL_H

#include <vector>
#include "utils.h"
#include "vertexSubset.h"
using namespace std;

#define MAX_TASK_NUM 20 // predefined max task num in the waiting list

class Task // base class (abstract class)
{
public:
    Task(long _nVertex):
        n(_nVertex), active(false), nextFrontier(){};
    ~Task() = default;

    // *pure* virtual function used for correct function call
    virtual bool update(uintE s, uintE d) = 0;
    virtual bool updateAtomic(uintE s, uintE d) = 0;
    virtual bool cond(uintE d) = 0;
    virtual bool finished() = 0;
    virtual void initialize() = 0;
    virtual void clear() = 0;
    // template <class vertex>
    // void iniOneIter(vertex* Vertex){
        // frontier.toSparse();
        // long sizeFrontier = frontier.numNonzeros();
        // uintT* degrees = newA(uintT, sizeFrontier);
        // parallel_for (long i = 0; i < sizeFrontier; ++i)
        //     degrees[i] = Vertex[frontier.s[i]].getOutDegree();
        // uintT outDegrees = sequence::plusReduce(degrees,sizeFrontier);
        // nextFrontier = newA(uintE,outDegrees);
        // nVF = 0;
        // free(degrees);
    void iniOneIter(){
        nextFrontier = newA(bool,n);
        parallel_for (long i = 0; i < n; ++i) // remember to initialize!
            nextFrontier[i] = 0;
        frontier.print();
    }
    void finishOneIter(){
        frontier.del();
        // set new frontier
        setFrontier(n,nextFrontier);
        // frontier = vertexSubset(n,nVF,nextFrontier);
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
    // long nVF; // # of vertices in nextFrontier
    // uintE* nextFrontier; // temporarily store next frontier
    bool* nextFrontier;
};

class Kernels
{
public:
    Kernels(): nTask(0){
        // task = newA(Task*,MAX_TASK_NUM); // polymorphism
        task = new Task*[MAX_TASK_NUM];
    };
    ~Kernels() = default;
    void appendTask(Task* tsk) // maybe add a list
    {
        task[nTask++] = tsk;
    }
    int nTask;
    Task** task; // 1D array to store pointers of the tasks
};

#endif