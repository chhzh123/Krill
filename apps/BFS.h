// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef BFS_H
#define BFS_H

#include "kernel.h"

class BFS: public UnweightedJob
{
public:
	BFS(long _n, PropertyManager& prop, long _start = 0):
		UnweightedJob(_n), start(_start){
			assert(_start < n);
			parents = prop.add_Parents();
		}; // call parent class constructor
	inline bool update(uintE s, uintE d){
		if (parents->get(d) == UINT_E_MAX){
			parents->set(d,s);
			return 1; // maybe use a bitmap to record frontier
		} else
			return 0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		return (CAS(parents->get_addr(d),UINT_E_MAX,s));
	}
	inline bool cond(uintE d){ // cond function checks if vertex has been visited yet
		return (parents->get(d) == UINT_E_MAX); // since CAS cannot pass negtive num
	}
	inline bool finished(int){
		return frontier.isEmpty();
	}
	void initialize(){
		parents->set(start,start);
		setFrontier(n,start);
	}
	long start;
	BFS_Prop::Parents* parents;
};

#endif // BFS_H