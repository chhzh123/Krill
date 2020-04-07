// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
using namespace std;

class BFS: public UnweightedJob
{
public:
	BFS(long _n, long _start = 0):
		UnweightedJob(_n), parents(NULL), start(_start){
			assert(_start < n);
		}; // call parent class constructor
	inline bool update(uintE s, uintE d){
		if (parents[d] == UINT_E_MAX){
			parents[d] = s;
			return 1; // maybe use a bitmap to record frontier
		} else
			return 0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		return (CAS(&parents[d],UINT_E_MAX,s));
	}
	inline bool cond(uintE d){ // cond function checks if vertex has been visited yet
		return (parents[d] == UINT_E_MAX); // since CAS cannot pass negtive num
	}
	inline bool finished(int){
		return frontier.isEmpty();
	}
	void initialize(){
		// creates Parents array, initialized to all UINT_E_MAX (or -1), except for start
		setAll<uintE>(parents,UINT_E_MAX); // unexplored
		parents[start] = start;
		setFrontier(n,start);
	}
	void clear(){ // only need to free members in child class
		freeMem<uintE>(parents);
	}
	long start;
	uintE* parents;
};