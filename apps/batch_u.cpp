// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
#include "SSSP.h"
#include "PageRank.h"
#include "CC.h"
#include "KCore.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	BFS* bfs = new BFS(G.n); // remember to dynamically allocate memory
	BFS* bfs2 = new BFS(G.n,10);
	Components* cc = new Components(G.n);
	KCore<vertex>* kcore = new KCore<vertex>(G.n,G.V);
	K.appendTask({bfs, bfs2, cc, kcore});
}