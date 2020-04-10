// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "Homo2.pb.h"
using namespace Homo2;

#include "PageRankDelta.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	Homo2::Property prop(G.n);
	for (int i = 0; i < 4; ++i){
		PageRankDelta<vertex> *prd = new PageRankDelta<vertex>(G.n, G.V, prop);
		K.appendJob(prd);
	}
	for (int i = 1; i < 5; ++i){
		int start = 71*i + 2;
		if (start >= G.n)
			start = i;
		SSSP* sssp = new SSSP(G.n, prop, start);
		K.appendJob(sssp);
	}
}