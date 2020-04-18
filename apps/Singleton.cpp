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
	Heter::PropertyManager prop(G.n);
	string job_name = P.getOptionValue("-job", "bfs");
	long start = P.getOptionLongValue("-r",0);
	if (job_name == "bfs"){
		BFS* bfs = new BFS(G.n, prop, start);
		K.appendJob(bfs);
	} else if (job_name == "cc"){
		Components* cc = new Components(G.n, prop);
		K.appendJob(cc);
	} else if (job_name == "pr"){
		PageRank<vertex>* pr = new PageRank<vertex>(G.n, G.V);
		K.appendJob(pr);
	} else if (job_name == "sssp"){
		SSSP* sssp = new SSSP(G.n, prop, start);
		K.appendJob(sssp);
	} else if (job_name == "prd"){
		PageRankDelta<vertex>* prd = new PageRankDelta<vertex>(G.n, G.V, prop);
		K.appendJob(prd);
	} else {
		cerr << "Error: No this job!" << endl;
		abort();
	}
	prop.initialize();
}