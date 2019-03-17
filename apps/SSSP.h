// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
using namespace std;

// reset changed vertices
class Reset : public Function
{
public:
	Reset(int* _changed):
		changed(_changed){}
	inline bool operator() (uintE i){
		changed[i] = 0;
		return 1;
	}
	int* changed;
};

// Bellmanford
class SSSP: public WeightedTask
{
public:
	SSSP(long _n, long _start = 0):
		WeightedTask(_n), shortestPathLen(NULL), changed(NULL), start(_start){}; // call parent class constructor
	inline bool update(uintE s, uintE d, intE edgeLen){ // relax, edgeLen(s,d)
		intE newDist = shortestPathLen[s] + edgeLen;
		if (shortestPathLen[d] > newDist) { // Update shortestPathLen if found a shorter path
			shortestPathLen[d] = newDist;
			if (changed[d] == 0) { // keep track that the vertex has been changed value
				changed[d] = 1;
				return 1;
			}
		} else
			return 0; // if no edges can be relaxed, then the algorithm terminates
	}
	inline bool updateAtomic(uintE s, uintE d, intE edgeLen){
		intE newDist = shortestPathLen[s] + edgeLen;
		return (writeMin(&shortestPathLen[d],newDist) && CAS(&changed[d],0,1));
	}
	inline bool cond(uintE d){
		return cond_true(d);
	}
	inline bool finished(){
		if (round == n) {
		// if the relax procedure has been executed for more than N times
		// there must exist negative weight cycle
			parallel_for (long i = 0; i < n; ++i)
				shortestPathLen[i] = -(INT_E_MAX/2);
			return true;
		} else {
			return frontier.isEmpty();
		}
	}
	void initialize(){
		// initialize shortestPathLen to "infinity"
		shortestPathLen = newA(intE,n);
		parallel_for(long i = 0; i < n; ++i)
			shortestPathLen[i] = INT_MAX / 2; // avoid overflow
		shortestPathLen[start] = 0; // except for the source vertex

		changed = newA(int,n);
		parallel_for(long i = 0; i < n; ++i)
			changed[i] = 0;
		// pass in 'start' from cmd?
		setFrontier(n,start);
	}
	void finishOneIter(){ // overload
        frontier.del();
        // set new frontier
        setFrontier(n,nextFrontier);
		vertexMap(frontier,Reset(changed)); // reset all the changed vertex
        round++;
    }
	void clear(){
		if (shortestPathLen != NULL)
			free(shortestPathLen);
		if (changed != NULL)
			free(changed);
	}
	long round;
	intE* shortestPathLen;
	int* changed; // int for CAS
	long start;
};