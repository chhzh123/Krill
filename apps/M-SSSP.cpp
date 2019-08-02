// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 1; i < 9; ++i){
		SSSP* sssp = new SSSP(G.n,211*i);
		K.appendJob(sssp);
	}
}