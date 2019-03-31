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
	string task_name = P.getOptionValue("-task","pr");
	if (task_name == "bfs"){
		BFS* bfs = new BFS(G.n);
		K.appendTask(bfs);
	} else if (task_name == "cc"){
		Components* cc = new Components(G.n);
		K.appendTask(cc);
	} else if (task_name == "pr"){
		PageRank<vertex>* pr = new PageRank<vertex>(G.n,G.V);
		K.appendTask(pr);
	} else if (task_name == "sssp"){
		SSSP* sssp = new SSSP(G.n);
		K.appendTask(sssp);
	} else {
		cerr << "Error: No this task!" << endl;
		abort();
	}
}