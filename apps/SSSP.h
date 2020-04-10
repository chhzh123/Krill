// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
#include "Homo2.pb.h"
using namespace std;
using namespace Homo2;

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
	SSSP(long _n, Property& prop, long _start = 0):
		WeightedJob(_n), start(_start){
			assert(_start < n);
			shortestPathLen = prop.add_shortestPathLen();
			changed = prop.add_changed();
		}; // call parent class constructor
	inline bool update(uintE s, uintE d, intE edgeLen){ // relax, edgeLen(s,d)
		intE newDist = (*shortestPathLen)[s] + edgeLen;
		if ((*shortestPathLen)[d] > newDist) { // Update shortestPathLen if found a shorter path
			(*shortestPathLen)[d] = newDist;
			if ((*changed)[d] == 0) { // keep track that the vertex has been changed value
				(*changed)[d] = 1;
				return 1; // only set once in sparse mode
			}
		}
		return 0; // if no edges can be relaxed, then the algorithm terminates
	}
	inline bool updateAtomic(uintE s, uintE d, intE edgeLen){
		intE newDist = (*shortestPathLen)[s] + edgeLen;
		return (writeMin(&(*shortestPathLen)[d],newDist) && CAS(&(*changed)[d],0,1));
	}
	inline bool cond(uintE d){
		return cond_true(d);
	}
	inline bool finished(int round){
		if (round == n) {
		// if the relax procedure has been executed for more than N times
		// there must exist negative weight cycle
			setAll<intE>(shortestPathLen->data,-(INT_MAX/2));
			return true;
		} else {
#ifndef DEBUG
			if (frontier.isEmpty()){
				if ((*shortestPathLen)[0] != INT_MAX / 2)
					cout << "Len: " << (*shortestPathLen)[0] << endl;
				else
					cout << "Len: Infinity!" << endl;
				return true;
			} else
				return false;
#else
			if (frontier.isEmpty()){
				for (int i = 0; i < 20; ++i)
					cout << (*shortestPathLen)[i] << " ";
				cout << endl;
				return true;
			} else
				return false;
#endif
		}
	}
	void initialize(){
		(*shortestPathLen)[start] = 0; // except for the source vertex
		setFrontier(n,start);
	}
	void finishOneIter(bool*){ // overload
        frontier.del();
        // set new frontier
        setFrontier(n,nextFrontier);
		vertexMap(frontier,Reset(changed->data)); // reset all the changed vertex
    }
	SSSP_shortestPathLen* shortestPathLen;
	SSSP_changed* changed; // int for CAS
	long start;
};