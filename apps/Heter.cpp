// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
#include "CC.h"
#include "PageRank.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	for (int i = 0; i < 2; ++i){
		BFS* bfs = new BFS(G.n,2*i); // remember to dynamically allocate memory
		Components* cc = new Components(G.n);
		PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
		SSSP* sssp = new SSSP(G.n,2*i+1);
		K.appendTask({bfs,cc,pr,sssp});
	}
}