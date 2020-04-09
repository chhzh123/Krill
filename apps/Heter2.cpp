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
		int start_bfs = 71 * i;
		if (start_bfs >= G.n)
			start_bfs = i;
		BFS* bfs = new BFS(G.n, start_bfs); // remember to dynamically allocate memory
		int start_sssp = 101 * i + 1;
		if (start_sssp >= G.n)
			start_sssp = i;
		SSSP* sssp = new SSSP(G.n, start_sssp);
		K.appendJob({bfs,sssp});
	}
}