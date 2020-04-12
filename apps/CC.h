// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef COMPONENT_H
#define COMPONENT_H

#include "kernel.h"

struct Update_F : public Function
{
	Update_F(Components_Prop::CurrIDs *_IDs, Components_Prop::PrevIDs *_prevIDs) :
		IDs(_IDs), prevIDs(_prevIDs) {}
	inline bool operator () (uintE i) {
    	prevIDs->set(i,IDs->get(i));
    	return 1;
	}
	Components_Prop::CurrIDs *IDs;
	Components_Prop::PrevIDs *prevIDs;
};

class Components : public UnweightedJob
{
public:
	Components(long _n, Property& prop):
		UnweightedJob(_n){
			IDs = prop.add_CurrIDs();
			prevIDs = prop.add_PrevIDs();
		}
	inline bool update(uintE s, uintE d){ // Update function writes min ID
		uintE origID = IDs->get(d);
    	if (IDs->get(s) < origID) {
    		IDs->set(d, min(origID,IDs->get(s)));
    		if (origID == prevIDs->get(d))
    			return 1; // only added to the frontier once
    	}
    	return 0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		uintE origID = IDs->get(d); // be careful that IDs->get(d) will be modified
    	return (writeMin(IDs->get_addr(d),IDs->get(s)) && origID == prevIDs->get(d));
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
				res[IDs->get(i)] = 1;
			std::atomic<int> cnt(0); // !
			parallel_for (int i = 0; i < n; ++i)
				if (res[i])
      				cnt += 1; // this is atomic!
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
		vertexMap(frontier,Update_F(IDs,prevIDs));
#ifdef DEBUG
        frontier.print(20);
#endif
    }
	Components_Prop::CurrIDs* IDs;
	Components_Prop::PrevIDs* prevIDs;
};

#endif // COMPONENT_H