// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h"
#include "M-BFS.pb.h"
using namespace MBFS;

#include "BFS.h"
using namespace std;

extern "C"
void setKernels(graph<asymmetricUnweightedVertex>&G, Kernels& K, commandLine P)
{
	MBFS::PropertyManager prop(G.n);
	for (int i = 1; i < 9; ++i){
		int start = 91 * i;
		if (start >= G.n)
			start = i;
		BFS* bfs = new BFS(G.n, prop, start); // remember to dynamically allocate memory
		K.appendJob(bfs);
	}
	prop.initialize();
}