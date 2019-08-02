// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
#include "CC.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 1; i < 5; ++i){
		BFS* bfs = new BFS(G.n,10*i); // remember to dynamically allocate memory
		K.appendJob(bfs);
	}
	for (int i = 0; i < 4; ++i){
		Components* cc = new Components(G.n);
		K.appendJob(cc);
	}
}