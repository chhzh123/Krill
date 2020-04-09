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
class SSSP: public WeightedJob
{
public:
	SSSP(long _n, long _start = 0):
		WeightedJob(_n), shortestPathLen(NULL), changed(NULL), start(_start), round(0){
			assert(_start < n);
		}; // call parent class constructor
	inline bool update(uintE s, uintE d, intE edgeLen){ // relax, edgeLen(s,d)
		intE newDist = shortestPathLen[s] + edgeLen;
		if (shortestPathLen[d] > newDist) { // Update shortestPathLen if found a shorter path
			shortestPathLen[d] = newDist;
			if (changed[d] == 0) { // keep track that the vertex has been changed value
				changed[d] = 1;
				return 1; // only set once in sparse mode
			}
		}
		return 0; // if no edges can be relaxed, then the algorithm terminates
	}
	inline bool updateAtomic(uintE s, uintE d, intE edgeLen){
		intE newDist = shortestPathLen[s] + edgeLen;
		return (writeMin(&shortestPathLen[d],newDist) && CAS(&changed[d],0,1));
	}
	inline bool cond(uintE d){
		return cond_true(d);
	}
	inline bool finished(int){
		if (round == n) {
		// if the relax procedure has been executed for more than N times
		// there must exist negative weight cycle
			setAll<intE>(shortestPathLen,-(INT_MAX/2));
			return true;
		} else {
#ifndef DEBUG
			return frontier.isEmpty();
#else
			if (frontier.isEmpty()){
				for (int i = 0; i < 20; ++i)
					cout << shortestPathLen[i] << " ";
				cout << endl;
				return true;
			} else
				return false;
#endif
		}
	}
	void initialize(){
		// initialize shortestPathLen to "infinity"
		setAll<intE>(shortestPathLen,INT_MAX/2); // avoid overflow
		shortestPathLen[start] = 0; // except for the source vertex
		setAll<int>(changed,0);
		setFrontier(n,start);
	}
	void finishOneIter(bool*){ // overload
        frontier.del();
        // set new frontier
        setFrontier(n,nextFrontier);
		vertexMap(frontier,Reset(changed)); // reset all the changed vertex
        round++;
    }
	void clear(){
		freeMem<intE>(shortestPathLen);
		freeMem<int>(changed);
	}
	long round;
	intE* shortestPathLen;
	int* changed; // int for CAS
	long start;
};