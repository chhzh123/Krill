// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 0; i < 8; ++i){
		BFS* bfs = new BFS(G.n,10*i); // remember to dynamically allocate memory
		K.appendTask(bfs);
	}
}