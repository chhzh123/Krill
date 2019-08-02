// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "PageRank.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 0; i < 4; ++i){
		PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V); // remember to dynamically allocate memory
		K.appendJob(pr);
	}
	for (int i = 1; i < 5; ++i){
		SSSP* sssp = new SSSP(G.n,71*i+2);
		K.appendJob(sssp);
	}
}