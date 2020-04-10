// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
#include "Homo1.pb.h"
using namespace std;
using namespace Homo1;

class BFS: public UnweightedJob
{
public:
	BFS(long _n, BFS_parents* _parents, long _start = 0):
		UnweightedJob(_n), parents(_parents), start(_start){
			assert(_start < n);
		}; // call parent class constructor
	inline bool update(uintE s, uintE d){
		if ((*parents)[d] == UINT_E_MAX){
			(*parents)[d] = s;
			return 1; // maybe use a bitmap to record frontier
		} else
			return 0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		return (CAS(&(parents->data[d]),UINT_E_MAX,s));
	}
	inline bool cond(uintE d){ // cond function checks if vertex has been visited yet
		return ((*parents)[d] == UINT_E_MAX); // since CAS cannot pass negtive num
	}
	inline bool finished(int){
		return frontier.isEmpty();
	}
	void initialize(){
		(*parents)[start] = start;
		setFrontier(n,start);
	}
	long start;
	BFS_parents* parents;
};