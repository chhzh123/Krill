// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "BFS.h"
#include "CC.h"
#include "PageRank.h"
#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	string job_name = P.getOptionValue("-job","pr");
	long start = P.getOptionLongValue("-r",0);
	if (job_name == "bfs"){
		BFS* bfs = new BFS(G.n,start);
		K.appendJob(bfs);
	} else if (job_name == "cc"){
		Components* cc = new Components(G.n);
		K.appendJob(cc);
	} else if (job_name == "pr"){
		PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
		K.appendJob(pr);
	} else if (job_name == "sssp"){
		SSSP* sssp = new SSSP(G.n,start);
		K.appendJob(sssp);
	} else {
		cerr << "Error: No this job!" << endl;
		abort();
	}
}