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

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	K.flagThreshold = false;
	int cnt = 3;
	Heter::PropertyManager prop(G.n);
	for (int i = 1; i < 3; ++i){
		int start_bfs = 71 * i + 2;
		if (start_bfs >= G.n)
			start_bfs = i;
		BFS* bfs = new BFS(G.n, prop, start_bfs); // remember to dynamically allocate memory
		double arrival_time = P.getDoubleValue(cnt++);
		bfs->arrival_time = arrival_time;

		Components* cc = new Components(G.n, prop);
		arrival_time = P.getDoubleValue(cnt++);
		cc->arrival_time = arrival_time;

		// PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
		PageRankDelta<vertex> *prd = new PageRankDelta<vertex>(G.n, G.V, prop);
		arrival_time = P.getDoubleValue(cnt++);
		prd->arrival_time = arrival_time;

		int start_sssp = 101 * i + 1;
		if (start_sssp >= G.n)
			start_sssp = i*2;
		SSSP* sssp = new SSSP(G.n, prop, start_sssp);
		arrival_time = P.getDoubleValue(cnt++);
		sssp->arrival_time = arrival_time;

		// K.appendJob({bfs,cc,pr,sssp});
		K.appendJob({bfs,cc,prd,sssp});
	}
	prop.initialize();
}