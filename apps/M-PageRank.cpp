// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "PageRank.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 1; i < 9; ++i){
		PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
		pr->isSingleton = false;
		K.appendJob(pr);
	}
}