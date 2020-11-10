// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "M-BFS.pb.h"
using namespace MBFS;

#include "BFS.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	MBFS::PropertyManager prop(G.n);
	int cnt = 2;
	for (int i = 1; i < 9; ++i){
		int start = 91 * i;
		if (start >= G.n)
			start = i;
		BFS* bfs = new BFS(G.n, prop, start); // remember to dynamically allocate memory
		double arrival_time = P.getDoubleValue(cnt++);
		bfs->arrival_time = arrival_time;
		K.appendJob(bfs);
	}
	prop.initialize();
}