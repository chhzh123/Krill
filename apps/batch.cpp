// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	BFS* bfs = new BFS(G.n); // remember to dynamically allocate memory
	SSSP* sssp = new SSSP(G.n);
	K.appendTask(bfs);
	K.appendTask(sssp);
}