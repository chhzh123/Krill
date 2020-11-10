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
	Homo1::PropertyManager prop(G.n);
	int cnt = 2;
	for (int i = 1; i < 5; ++i){
		BFS *bfs = new BFS(G.n, prop, 10 * i); // remember to dynamically allocate memory
		double arrival_time = P.getDoubleValue(cnt++);
		bfs->arrival_time = arrival_time;
		K.appendJob(bfs);
	}
	for (int i = 0; i < 4; ++i){
		Components* cc = new Components(G.n, prop);
		double arrival_time = P.getDoubleValue(cnt++);
		cc->arrival_time = arrival_time;
		K.appendJob(cc);
	}
	prop.initialize();
}