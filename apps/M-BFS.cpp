// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 1; i < 9; ++i){
		BFS* bfs = new BFS(G.n,91*i); // remember to dynamically allocate memory
		K.appendJob(bfs);
	}
}