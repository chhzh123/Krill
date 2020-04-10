// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
#include "Homo1.pb.h"
using namespace std;
using namespace Homo1;

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

class Components : public UnweightedJob
{
public:
	Components(long _n, Property& prop):
		UnweightedJob(_n){
			IDs = prop.add_IDs();
			prevIDs = prop.add_prevIDs();
		}
	inline bool update(uintE s, uintE d){ // Update function writes min ID
		uintE origID = (*IDs)[d];
    	if ((*IDs)[s] < origID) {
    		(*IDs)[d] = min(origID,(*IDs)[s]);
    		if (origID == (*prevIDs)[d])
    			return 1; // only added to the frontier once
    	}
    	return 0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		uintE origID = (*IDs)[d]; // be careful that (*IDs)[d] will be modified
    	return (writeMin(&(*IDs)[d],(*IDs)[s]) && origID == (*prevIDs)[d]);
	}
	inline bool cond(uintE d){
		return cond_true(d);
	}
	inline bool finished(int){
		// return frontier.isEmpty();
		if (frontier.isEmpty()){
			bool* res;
			setAll<bool>(res,0);
			parallel_for (int i = 0; i < n; ++i)
				res[(*IDs)[i]] = 1;
  			int cnt = 0;
  			parallel_for (int i = 0; i < n; ++i)
				if (res[i])
      				writeAdd<int>(&cnt,1);
  			cout << "CC: " << cnt << endl;
  			return true;
		} else
			return false;
	}
	void initialize(){
		setFrontierAll();
	}
	void iniOneIter(){
		nextFrontier = newA(bool,n); // DO NOT FREE nextFrontier
        parallel_for (long i = 0; i < n; ++i) // remember to initialize!
            nextFrontier[i] = 0;
        frontier.toDense();
		vertexMap(frontier,Update_F(IDs->data,prevIDs->data));
#ifdef DEBUG
        frontier.print(20);
#endif
    }
	Components_IDs *IDs;
	Components_prevIDs* prevIDs;
};