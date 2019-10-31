// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 1; i < 5; ++i){
		BFS* bfs = new BFS(G.n,71*i); // remember to dynamically allocate memory
		SSSP* sssp = new SSSP(G.n,101*i+1);
		K.appendJob({bfs,sssp});
	}
}