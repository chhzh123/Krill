#include "krill.h"

#define CHECK()                                             \
{                                                           \
    const char* dlsym_error = dlerror();                    \
    if (dlsym_error) {                                      \
        cerr << "Cannot load libray or symbol: "            \
             << dlsym_error << '\n';                        \
        exit(1);                                            \
    }                                                       \
}                                                           \

template <class vertex>
void framework(graph<vertex>& G, commandLine P)
{
    char* algFile = P.getArgument(1); // second last
    strcat(algFile,".so");
    void* handle = dlopen(algFile, RTLD_LAZY);
    CHECK();

    void (*setKernels)(graph<vertex>&, Kernels&, commandLine);
    setKernels = (void (*)(graph<vertex>&, Kernels&, commandLine))dlsym(handle, "setKernels");
    CHECK();

    startTime();
    Kernels K;
    setKernels(G,K,P);

    // check validity
    K.initialize(G.n,G.isWeighted);
    reportTime("Initialization time");

    startTime();
    Execute(G,K,P);
    reportTime("Running time");
#ifdef DEBUG
    cout << "Push-Dense: " << K.pushDenseCnt << endl;
    cout << "Push-Sparse: " << K.pushSparseCnt << endl;
    cout << "Pull-Dense: " << K.pullDenseCnt << endl;
    cout << "Pull-Sparse: " << K.pullSparseCnt << endl;
#endif
    G.del();
}

int main(int argc, char* argv[]){
    commandLine P(argc,argv," [-s] <inFile>");
    char* iFile = P.getArgument(0);
    bool symmetric = P.getOptionValue("-s");
    bool weighted = P.getOptionValue("-w");
    bool compressed = P.getOptionValue("-c");
    bool binary = P.getOptionValue("-b");
    long rounds = P.getOptionLongValue("-rounds",1);

    if (!weighted) {
        if (!symmetric) {
            startTime();
            graph<asymmetricUnweightedVertex> G =
                readGraphFromFile<asymmetricUnweightedVertex>(iFile);
            reportTime("Graph IO time");
            framework(G,P);
        } else {
            startTime();
            graph<symmetricUnweightedVertex> G =
                readGraphFromFile<symmetricUnweightedVertex>(iFile);
            reportTime("Graph IO time");
            framework(G,P);
        }
    } else {
        if (!symmetric) {
            startTime();
            graph<asymmetricWeightedVertex> G =
                readGraphFromFile<asymmetricWeightedVertex>(iFile);
            reportTime("Graph IO time");
            framework(G,P);
        } else {
            startTime();
            graph<symmetricWeightedVertex> G =
                readGraphFromFile<symmetricWeightedVertex>(iFile);
            reportTime("Graph IO time");
            framework(G,P);
        }
    }
}