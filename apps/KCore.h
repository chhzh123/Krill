// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
using namespace std;

template<class vertex>
struct Deg_AtLeast_K : public Function
{
	Deg_AtLeast_K(intE* _degrees, uintE _k):
		degrees(_degrees), k(_k){}
	inline bool operator () (uintE i){
		return degrees[i] >= k;
	}
	uintE k;
	intE* degrees;
};

template<class vertex>
struct Deg_LessThan_K : public Function
{
	Deg_LessThan_K(intE* _degrees, uintE* _coreNum, uintE _k):
		degrees(_degrees), coreNum(_coreNum), k(_k){}
	inline bool operator () (uintE i){
		if (degrees[i] < (int)k && degrees[i] > 0){ // already removed
			coreNum[i] = k - 1;
			degrees[i] = 0; // mark
			return true;
		} else
			return false;
	}
	uintE k;
	intE* degrees;
	uintE* coreNum;
};

// assume undirected/symmetric graph!
template<class vertex>
class KCore : public UnweightedJob
{
public:
	KCore(long _n, vertex* _V):
		UnweightedJob(_n), V(_V), coreNum(NULL), degrees(NULL), toRemove(NULL), k(1){}
	inline bool update(uintE s, uintE d){
		degrees[s]--;
		return false;
	}
	inline bool updateAtomic(uintE s, uintE d){
		writeAdd(&degrees[s],-1);
		return false; // avoid adding into frontier
	}
	inline bool cond(uintE d){
		if (toRemove[d] == 1){ // <k
			flag = true; // there exists <k
			return true;
		} else {
			return false;
		}
	}
	inline bool finished(){
		if (frontier.isEmpty()){ // no new vertices are added to frontier, meaning all the vertices' degree are less than k
			cout << "Largest core is " << k-1 << endl;
			return true;
		} else {
			if (!flag) // found k-core
				++k;
			return false;
		}
	}
	void initialize(){
		setAll<uintE>(coreNum,0);
		degrees = newA(intE,n);
		parallel_for (long i = 0; i < n; ++i)
			degrees[i] = V[i].getOutDegree();
		setFrontierAll();
	}
	void iniOneIter(){
		nextFrontier = newA(bool,n);
		parallel_for (long i = 0; i < n; ++i) // remember to initialize!
			nextFrontier[i] = 0;
		flag = false;
		toRemove = vertexFilter(frontier,Deg_LessThan_K<vertex>(degrees,coreNum,k),1);
		nextFrontier = vertexFilter(frontier,Deg_AtLeast_K<vertex>(degrees,k),1);
	}
	void clear(){
		freeMem<intE>(degrees);
		freeMem<uintE>(coreNum);
	}
	vertex* V;
	uintE* coreNum;
	intE* degrees;
	bool* toRemove;
	bool flag;
	uintE k; // used for CAS
};