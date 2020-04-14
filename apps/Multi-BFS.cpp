// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "Multi-BFS.pb.h"
using namespace MultiBFS;

#include "BFS.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	long cnt = P.getOptionLongValue("-n",1);
	cout << cnt << " BFSs" << endl;
	MultiBFS::Property prop(G.n);
	for (int i = 1; i < cnt+1; ++i){
		BFS* bfs = new BFS(G.n, prop, 10*i); // remember to dynamically allocate memory
		K.appendJob(bfs);
	}
	prop.initialize();
}