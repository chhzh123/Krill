// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "Homo2.pb.h"
using namespace Homo2;

#include "PageRankDelta.h"
#include "SSSP.h"
using namespace std;

extern "C"
void setKernels(graph<asymmetricWeightedVertex>&G, Kernels& K, commandLine P)
{
	K.flagThreshold = false;
	Homo2::PropertyManager prop(G.n);
	for (int i = 0; i < 4; ++i){
		PageRankDelta<asymmetricWeightedVertex> *prd = new PageRankDelta<asymmetricWeightedVertex>(G.n, G.V, prop);
		K.appendJob(prd);
	}
	for (int i = 1; i < 5; ++i){
		int start = 71*i + 2;
		if (start >= G.n)
			start = i;
		SSSP* sssp = new SSSP(G.n, prop, start);
		K.appendJob(sssp);
	}
	prop.initialize();
}