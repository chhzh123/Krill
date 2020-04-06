// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
#include <cmath>
using namespace std;

// vertex map function to update its p value according to PageRank equation
struct Update_PageRankDelta : public Function
{
	Update_PageRankDelta(double* _p, double* _neigh_sum, double* _delta, double _damping, double _factor, double _one_over_n, int _round):
		p(_p), neigh_sum(_neigh_sum), delta(_delta), damping(_damping), factor(_factor), one_over_n(_one_over_n), round(_round){}
	inline bool operator () (uintE i) {
        if (round != 0){
            delta[i] = damping * neigh_sum[i]; // refresh the old delta
            if (fabs(delta[i]) > factor * p[i]){
                p[i] += delta[i];
                return true;
            } else
                return false;
        } else { // round 0
            p[i] += damping * neigh_sum[i];
            delta[i] = p[i] - one_over_n;
            return (fabs(delta[i]) > factor * p[i]);
        }
	}
    int round;
	double* p;
	double* neigh_sum;
    double* delta;
	double damping;
    double factor;
    double one_over_n;
};

template <class vertex>
class PageRankDelta : public UnweightedJob
{
public:
	PageRankDelta(long _n, vertex* _V, long _maxIters = 10):
		UnweightedJob(_n), p(NULL), neigh_sum(NULL), V(_V),
		iter(0), maxIters(_maxIters){}; // call parent class constructor
	inline bool update(uintE s, uintE d){ // update function applies PageRank equation
        double oldVal = neigh_sum[d];
		neigh_sum[d] += delta[s] / V[s].getOutDegree();
		return oldVal == 0.0;
	}
	inline bool updateAtomic(uintE s, uintE d){
		// writeAdd(&neigh_sum[d], delta[s] / V[s].getOutDegree());
        volatile double oldVal, newVal;
        do{
            oldVal = neigh_sum[d];
            newVal = oldVal + delta[s] / V[s].getOutDegree();
        } while (!CAS(&neigh_sum[d],oldVal,newVal));
		return oldVal == 0.0;
	}
	inline bool cond(uintE d){
		return true;
	}
	inline bool finished(){
		if (iter > maxIters || L1_norm < epsilon){
#ifdef DEBUG
    	for (long i = 0; i < 10; ++i)
    		cout << p[i] << " ";
    	cout << endl;
#endif
			return true;
		}
		else
			return false;
	}
	void initialize(){
		setAll<double>(p,(1-damping)/(double)n);
        setAll<double>(delta,1/(double)n);
        setAll<double>(neigh_sum, 0); // delta_dst
        setFrontierAll();
        setAll<bool>(tmp, true);
    }
	void finishOneIter(bool* nextUni){ // overload
        if (nextUni == NULL)
            setAll<bool>(nextUni, true);
        vertexSubset active = vertexFilter<Update_PageRankDelta>(vertexSubset(n, tmp), Update_PageRankDelta(p, neigh_sum, delta, damping, factor, 1/(double)n, iter), nextUni);

        // compute L1-norm
        double* delta_abs = newA(double,n);
        parallel_for (long i = 0; i < n; ++i)
            delta_abs[i] = fabs(delta[i]);
    	L1_norm = sequence::plusReduce(delta_abs,n);
    	if (L1_norm < epsilon){
#ifdef DEBUG
            cout << "early break" << endl;
#endif
    		return;
        }

        frontier.del();
        frontier = active;
        setAll<double>(neigh_sum, 0); // delta_dst
        iter++;
    }
	void clear(){
		freeMem<double>(p);
        freeMem<double>(delta);
		freeMem<double>(neigh_sum);
        freeMem<bool>(tmp);
	}
	long iter;
	long maxIters;
	double* p;
	double* neigh_sum;
    double* delta;
	vertex* V;
    bool* tmp;
	double L1_norm;
	const double damping = 0.85;
	const double epsilon = 0.0000001;
    const double factor = 0.01;
};