// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "Heter.pb.h"
using namespace Heter;

#include "BFS.h"
#include "CC.h"
#include "PageRank.h"
#include "PageRankDelta.h"
#include "SSSP.h"
using namespace std;

extern "C"
void setKernels(graph<asymmetricWeightedVertex>&G, Kernels& K, commandLine P)
{
	K.flagThreshold = false;
	Heter::PropertyManager prop(G.n);
	for (int i = 1; i < 3; ++i){
		int start_bfs = 71 * i + 2;
		if (start_bfs >= G.n)
			start_bfs = i;
		BFS* bfs = new BFS(G.n, prop, start_bfs); // remember to dynamically allocate memory

		Components* cc = new Components(G.n, prop);

		// PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
		PageRankDelta<asymmetricWeightedVertex> *prd = new PageRankDelta<asymmetricWeightedVertex>(G.n, G.V, prop);

		int start_sssp = 101 * i + 1;
		if (start_sssp >= G.n)
			start_sssp = i*2;
		SSSP* sssp = new SSSP(G.n, prop, start_sssp);

		// K.appendJob({bfs,cc,pr,sssp});
		K.appendJob({bfs,cc,prd,sssp});
	}
	prop.initialize();
}