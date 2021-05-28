// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef SSSP_H
#define SSSP_H

#include "kernel.h"

// reset changed vertices
class Reset : public Function
{
public:
	Reset(SSSP_Prop::Changed *_changed) :
		changed(_changed) {}
	inline bool operator() (uintE i){
		changed->set(i,0);
		return 1;
	}
	SSSP_Prop::Changed *changed;
};

// Bellmanford
class SSSP: public WeightedJob
{
public:
	SSSP(long _n, PropertyManager& prop, long _start = 0):
		WeightedJob(_n), start(_start){
			assert(_start < n);
			shortestPathLen = prop.add_ShortestPathLen();
			changed = prop.add_Changed();
		}; // call parent class constructor
	inline bool update(uintE s, uintE d, intE edgeLen) final { // relax, edgeLen(s,d)
		intE newDist = shortestPathLen->get(s) + edgeLen;
		if (shortestPathLen->get(d) > newDist) { // Update shortestPathLen if found a shorter path
			shortestPathLen->get(d) = newDist;
			if (changed->get(d) == 0) { // keep track that the vertex has been changed value
				changed->set(d,1);
				return 1; // only set once in sparse mode
			}
		}
		return 0; // if no edges can be relaxed, then the algorithm terminates
	}
	inline bool updateAtomic(uintE s, uintE d, intE edgeLen) final {
		intE newDist = shortestPathLen->get(s) + edgeLen;
		return (writeMin(shortestPathLen->get_addr(d),newDist) && CAS(changed->get_addr(d),0,1));
	}
	inline bool cond(uintE d) final {
		return cond_true(d);
	}
	inline bool finished(int round) final {
		if (round == n) {
		// if the relax procedure has been executed for more than N times
		// there must exist negative weight cycle
			shortestPathLen->set_all(-0x3f3f3f3f);
			return true;
		} else {
#ifndef DEBUG
			if (frontier.isEmpty()){
				int val = shortestPathLen->get(0);
				if (val != 0x3f3f3f3f)
					cout << "Len: " << val << endl;
				else
					cout << "Len: Infinity!" << endl;
				return true;
			} else
				return false;
#else
			if (frontier.isEmpty()){
				for (int i = 0; i < 20; ++i)
					cout << shortestPathLen->get(i) << " ";
				cout << endl;
				return true;
			} else
				return false;
#endif
		}
	}
	inline void initialize() final {
		shortestPathLen->set(start,0);
		setFrontier(n,start);
	}
	void finishOneIter(bool*) { // overload
        frontier.del();
        // set new frontier
        setFrontier(n,nextFrontier);
		vertexMap(frontier,Reset(changed)); // reset all the changed vertex
    }
	SSSP_Prop::ShortestPathLen* shortestPathLen;
	SSSP_Prop::Changed* changed; // int for CAS
	long start;
};

#endif