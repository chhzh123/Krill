// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	long cnt = P.getOptionLongValue("-n",1);
	cout << cnt << " BFSs" << endl;
	for (int i = 1; i < cnt+1; ++i){
		BFS* bfs = new BFS(G.n,10*i); // remember to dynamically allocate memory
		K.appendTask(bfs);
	}
}