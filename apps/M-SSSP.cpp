// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 1; i < 9; ++i){
		int start = 211 * i;
		if (start >= G.n)
			start = i;
		SSSP* sssp = new SSSP(G.n, start);
		K.appendJob(sssp);
	}
}