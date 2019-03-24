// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
using namespace std;

struct Update_F : public Function
{
  	Update_F(uintE* _IDs, uintE* _prevIDs):
    	IDs(_IDs), prevIDs(_prevIDs) {}
  	inline bool operator () (uintE i) {
    	prevIDs[i] = IDs[i];
    	return 1;
	}
	uintE* IDs, *prevIDs;
};

class Components : public UnweightedTask
{
public:
	Components(long _n):
		UnweightedTask(_n), IDs(NULL), prevIDs(NULL){}
	inline bool update(uintE s, uintE d){ // Update function writes min ID
		uintE origID = IDs[d];
    	if (IDs[s] < origID) {
    		IDs[d] = min(origID,IDs[s]);
    		if (origID == prevIDs[d])
    			return 1;
    	}
    	return 0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		uintE origID = IDs[d]; // be careful that IDs[d] will be modified
    	return (writeMin(&IDs[d],IDs[s]) && origID == prevIDs[d]);
	}
	inline bool cond(uintE d){
		return cond_true(d);
	}
	inline bool finished(){
		return frontier.isEmpty();
	}
	void initialize(){
		IDs = newA(uintE,n);
		parallel_for(long i = 0; i < n; i++){
			IDs[i] = i; // initialize unique IDs
		}
		setAll<uintE>(prevIDs,(uintE)0);

		setFrontierAll();
	}
	void iniOneIter(){
		setAll<bool>(nextFrontier,0);
		vertexMap(frontier,Update_F(IDs,prevIDs));
    }
	void clear(){
		freeMem<uintE>(IDs);
		freeMem<uintE>(prevIDs);
	}
	uintE* IDs;
	uintE* prevIDs;
};