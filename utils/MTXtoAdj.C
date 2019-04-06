// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "parseCommandLine.h"
#include "graphIO.h"
#include "parallel.h"

int parallel_main(int argc, char* argv[]) {
  commandLine P(argc,argv,"[-s] <input MTX file> <output Ligra file>");
  char* iFile = P.getArgument(1);
  char* oFile = P.getArgument(0);
  bool sym = P.getOption("-s");
  edgeArray<uintT> G = readMTX<uintT>(iFile);
  writeGraphToFile<uintT>(graphFromEdges(G,sym),oFile);
}
