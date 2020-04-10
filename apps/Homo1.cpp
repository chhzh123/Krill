// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "Homo1.pb.h"
using namespace Homo1;

#include "BFS.h"
#include "CC.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	Homo1::Property prop(G.n);
	for (int i = 1; i < 5; ++i){
		BFS *bfs = new BFS(G.n, prop, 10 * i); // remember to dynamically allocate memory
		K.appendJob(bfs);
	}
	for (int i = 0; i < 4; ++i){
		Components* cc = new Components(G.n, prop);
		K.appendJob(cc);
	}
}