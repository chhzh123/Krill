// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
using namespace std;

class BFS: public Task
{
public:
	BFS(long _n):
		Task(_n), parents(NULL){}; // call parent class constructor
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
	// cond function checks if vertex has been visited yet
	inline bool cond(uintE d){
		return (parents[d] == UINT_E_MAX); // since CAS cannot pass negtive num
	}
	inline bool finished(){
		return frontier.isEmpty();
	}
	void initialize(){
		// creates Parents array, initialized to all UINT_E_MAX (or -1), except for start
		parents = newA(uintE,n); // dynamically allocate memory
		parallel_for(long i = 0; i < n; ++i)
			parents[i] = UINT_E_MAX; // unexplored
		// pass in 'start' from cmd?
		long start = 0;
		parents[start] = start;
		setFrontier(n,start);
	}
	void clear(){
		if (parents != NULL)
			free(parents);
		if (nextFrontier != NULL)
			free(nextFrontier);
	}
	uintE* parents;
};