// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef PAGERANK_H
#define PAGERANK_H

#include "kernel.h"
#include <cmath>

// vertex map function to update its p value according to PageRank equation
struct Update_PR : public Function
{
	Update_PR(double* _p_curr, double* _p_next, double _damping, intE n):
		p_curr(_p_curr), p_next(_p_next), damping(_damping), // be carful of underline!
		addedConstant((1 - _damping) * (1 / (double)n)){}
	inline bool operator () (uintE i) {
		p_next[i] = damping * p_next[i] + addedConstant;
		return true;
	}
	double addedConstant;
	double* p_curr;
	double* p_next;
	double damping;
};

struct Reset_PR : public Function
{
	Reset_PR(double* _p_curr): p_curr(_p_curr) {}
	inline bool operator () (uintE i) {
		p_curr[i] = 0.0;
		return true;
	}
	double* p_curr;
};

template <class vertex>
class PageRank : public UnweightedJob
{
public:
	PageRank(long _n, vertex* _V, long _maxIters = 15):
		UnweightedJob(_n, true), p_curr(NULL), p_next(NULL), V(_V),
		maxIters(_maxIters){}; // call parent class constructor
	inline bool update(uintE s, uintE d){ // update function applies PageRank equation
		p_next[d] += p_curr[s] / V[s].getOutDegree();
		return true;
	}
	inline bool updateAtomic(uintE s, uintE d){
		writeAdd(&p_next[d], p_curr[s] / V[s].getOutDegree());
		return true;
	}
	inline bool cond(uintE d){
		return cond_true(d);
	}
	inline bool finished(int iter){
		if (iter >= maxIters || L1_norm < epsilon){
#ifdef DEBUG
    	for (long i = 0; i < 20; ++i)
    		cout << p_curr[i] << " ";
    	cout << endl;
#endif
			return true;
		}
		else
			return false;
	}
	void initialize(){
		double one_over_n = 1/(double)n;
		setAll<double>(p_curr,one_over_n);
		setAll<double>(p_next,0); // 0 if unchanged
		setFrontierAll();
	}
	void finishOneIter(bool*){ // overload
		vertexMap(frontier,Update_PR(p_curr,p_next,damping,n));

		// compute L1-norm between p_curr and p_next
		parallel_for(long i = 0; i < n; i++)
      		p_curr[i] = fabs(p_curr[i] - p_next[i]);
    	L1_norm = sequence::plusReduce(p_curr,n);
    	if (L1_norm < epsilon)
    		return;
    	// reset p_curr
    	vertexMap(frontier,Reset_PR(p_curr));
    	swap(p_curr,p_next);

        frontier.del();
        setFrontier(n,nextFrontier);
    }
	void clear(){
		freeMem<double>(p_curr);
		freeMem<double>(p_next);
	}
	long maxIters;
	double* p_curr;
	double* p_next;
	vertex* V;
	double L1_norm;
	const double damping = 0.85;
	const double epsilon = 0.0000001;
};

#endif // PAGERANK_H