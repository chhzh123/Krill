// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "krill.h"
#include "M-SSSP.pb.h"
using namespace MSSSP;

#include "SSSP.h"
using namespace std;

template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	K.flagThreshold = false;
	MSSSP::PropertyManager prop(G.n);
	int cnt = 3;
	for (int i = 1; i < 9; ++i){
		int start = 211 * i;
		if (start >= G.n)
			start = i;
		SSSP* sssp = new SSSP(G.n, prop, start);
		double arrival_time = P.getDoubleValue(cnt++);
		sssp->arrival_time = arrival_time;
		K.appendJob(sssp);
	}
	prop.initialize();
}