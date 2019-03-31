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
	BFS* bfs = new BFS(G.n); // remember to dynamically allocate memory
	Components* cc = new Components(G.n);
	PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
	SSSP* sssp = new SSSP(G.n);
	K.appendTask({bfs,cc,pr,sssp});
}