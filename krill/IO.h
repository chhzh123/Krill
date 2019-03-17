// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen
// Original work Copyright (c) 2013 Julian Shun

#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <type_traits> // C++11 for enable_if
#include "parallel.h"
#include "blockRadixSort.h"
#include "utils.h"
#include "graph.h"
using namespace std;

typedef unsigned char uchar; // added!

typedef pair<uintE,uintE> intPair;
typedef pair<uintE, pair<uintE,intE> > intTriple;

template <class E>
struct pairFirstCmp {
    bool operator() (pair<uintE,E> a, pair<uintE,E> b) {
        return a.first < b.first;
    }
};

template <class E>
struct getFirst {uintE operator() (pair<uintE,E> a) {return a.first;} };

template <class IntType>
struct pairBothCmp {
    bool operator() (pair<uintE,IntType> a, pair<uintE,IntType> b) {
      if (a.first != b.first) return a.first < b.first;
      return a.second < b.second;
    }
};

// A structure that keeps a sequence of strings all allocated from
// the same block of memory
struct words {
    long n; // total number of characters
    char* Chars;  // array storing all strings
    long m; // number of substrings
    char** Strings; // pointers to strings (all should be null terminated)
    words() {}
    words(char* C, long nn, char** S, long mm):
        Chars(C), n(nn), Strings(S), m(mm) {}
    void del() {
        free(Chars);
        free(Strings);
    }
};
 
inline bool isSpace(char c) {
    switch (c)    {
    case '\r': 
    case '\t': 
    case '\n': 
    case 0:
    case ' ' : return true;
    default : return false;
    }
}

_seq<char> readStringFromFile(char *fileName) {
    ifstream file (fileName, ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        std::cout << "Unable to open file: " << fileName << std::endl;
        abort();
    }
    long end = file.tellg();
    file.seekg (0, ios::beg);
    long n = end - file.tellg();
    char* bytes = newA(char,n+1);
    file.read (bytes,n);
    file.close();
    return _seq<char>(bytes,n);
}

// parallel code for converting a string to words
words stringToWords(char *Str, long n) {
    parallel_for (long i = 0; i < n; i++) 
          if (isSpace(Str[i])) Str[i] = 0;

    // mark start of words
    bool *FL = newA(bool,n);
    FL[0] = Str[0];
    parallel_for (long i = 1; i < n; i++)
        FL[i] = Str[i] && !Str[i-1];
      
    // offset for each start of word
    _seq<long> Off = sequence::packIndex<long>(FL, n);
    long m = Off.n;
    long *offsets = Off.A;

    // pointer to each start of word
    char **SA = newA(char*, m);
    parallel_for (long j = 0; j < m; j++)
        SA[j] = Str+offsets[j];

    free(offsets); free(FL);
    return words(Str,n,SA,m);
}

template <class vertex>
struct Uncompressed_Mem : public Deletable
{
public:
    vertex* V;
    long n;
    long m;
    void* allocatedInplace, *inEdges;
    uintE* flags;

    Uncompressed_Mem(vertex* VV, long nn, long mm, void* ai, void* _inEdges = NULL):
        V(VV), n(nn), m(mm), allocatedInplace(ai), inEdges(_inEdges), flags(NULL) {}

    void del() {
        if (flags != NULL)
            free(flags);
        if (allocatedInplace == NULL) 
            for (long i = 0; i < n; i++)
                V[i].del();
        else
            free(allocatedInplace);
        free(V);
        if(inEdges != NULL)
            free(inEdges);
    }
};

template <class vertex>
typename enable_if<!TraitsHelper<vertex>::isWeighted && !TraitsHelper<vertex>::isSymmetric, graph<vertex> >::type
readGraphFromFile(char* fname) { // Graph<asymmetricUnweighted>
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    long len = W.m -1;
    long n = atol(W.Strings[1]);
    long m = atol(W.Strings[2]);
    if (W.Strings[0] != (string) "AdjacencyGraph"){
        cerr << "Bad input file" << endl;
        abort();
    }
    if (len != n + m + 2) {
        cerr << "Bad input file" << endl;
        abort();
    }
        
    uintT* offsets = newA(uintT,n);
    uintE* edges = newA(uintE,m);
    parallel_for(long i = 0; i < n; i++)
        offsets[i] = atol(W.Strings[i + 3]);
    parallel_for(long i = 0; i < m; i++)
        edges[i] = atol(W.Strings[i+n+3]); 
    vertex* v = newA(vertex,n);
    parallel_for (uintT i = 0; i < n; i++) {
        uintT o = offsets[i];
        uintT l = ((i == n-1) ? m : offsets[i+1])-offsets[i];
        v[i].setOutDegree(l);
        v[i].setOutNeighbors(edges+o);
    }

    uintT* tOffsets = newA(uintT,n);
    parallel_for(long i = 0; i < n; i++)
        tOffsets[i] = INT_T_MAX;
    uintE* inEdges = newA(uintE,m);
    intPair* temp = newA(intPair,m);
    parallel_for(long i = 0; i < n; i++){
        uintT o = offsets[i];
        for(uintT j = 0; j < v[i].getOutDegree(); j++)
            temp[o+j] = make_pair(v[i].getOutNeighbor(j),i);
    }
    free(offsets);
    intSort::iSort(temp,m,n+1,getFirst<uintE>());
    tOffsets[temp[0].first] = 0;
    inEdges[0] = temp[0].second;
    parallel_for(long i = 1; i < m; i++) {
        inEdges[i] = temp[i].second;
        if(temp[i].first != temp[i-1].first)
            tOffsets[temp[i].first] = i;
    }
    free(temp);
    // fill in offsets of degree 0 vertices by taking closest non-zero
    // offset to the right
    sequence::scanIBack(tOffsets,tOffsets,n,minF<uintT>(),(uintT)m);

    parallel_for(long i = 0;i < n; i++){
        uintT o = tOffsets[i];
        uintT l = ((i == n-1) ? m : tOffsets[i+1])-tOffsets[i];
        v[i].setInDegree(l);
        v[i].setInNeighbors(inEdges+o);
    }
    free(tOffsets);
    Uncompressed_Mem<vertex>* mem = new Uncompressed_Mem<vertex>(v,n,m,edges,inEdges);
    return graph<vertex>(v,n,m,false,mem);
}

template <class vertex>
typename enable_if<!TraitsHelper<vertex>::isWeighted && TraitsHelper<vertex>::isSymmetric, graph<vertex> >::type
readGraphFromFile(char* fname) { // Graph<symmetricUnweighted>
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    long len = W.m -1;
    long n = atol(W.Strings[1]);
    long m = atol(W.Strings[2]);
    if (W.Strings[0] != (string) "AdjacencyGraph"){
        cerr << "Bad input file" << endl;
        abort();
    }
    if (len != n + m + 2) {
        cerr << "Bad input file" << endl;
        abort();
    }
        
    uintT* offsets = newA(uintT,n);
    uintE* edges = newA(uintE,m);
    parallel_for(long i = 0; i < n; i++)
        offsets[i] = atol(W.Strings[i + 3]);
    parallel_for(long i = 0; i < m; i++)
        edges[i] = atol(W.Strings[i+n+3]); 
    vertex* v = newA(vertex,n);
    parallel_for (uintT i = 0; i < n; i++) {
        uintT o = offsets[i];
        uintT l = ((i == n-1) ? m : offsets[i+1])-offsets[i];
        v[i].setOutDegree(l);
        v[i].setOutNeighbors(edges+o);
    }

    free(offsets);
    Uncompressed_Mem<vertex>* mem = new Uncompressed_Mem<vertex>(v,n,m,edges);
    return graph<vertex>(v,n,m,false,mem);
}

template <class vertex>
typename enable_if<TraitsHelper<vertex>::isWeighted && !TraitsHelper<vertex>::isSymmetric, graph<vertex> >::type
readGraphFromFile(char* fname) { // Graph<asymmetricWeighted>
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    long len = W.m -1;
    long n = atol(W.Strings[1]);
    long m = atol(W.Strings[2]);
    if (W.Strings[0] != (string) "WeightedAdjacencyGraph"){
        cerr << "Bad input file" << endl;
        abort();
    }
    if (len != n + 2*m + 2) {
        cerr << "Bad input file" << endl;
        abort();
    }

    uintT* offsets = newA(uintT,n);
    intE* edges = newA(intE,2*m);
    parallel_for(long i = 0; i < n; i++)
        offsets[i] = atol(W.Strings[i + 3]);
    parallel_for(long i = 0; i < m; i++) {
        edges[2*i] = atol(W.Strings[i+n+3]); 
        edges[2*i+1] = atol(W.Strings[i+n+m+3]);
    }
    vertex* v = newA(vertex,n);
    parallel_for (uintT i = 0; i < n; i++) {
        uintT o = offsets[i];
        uintT l = ((i == n-1) ? m : offsets[i+1])-offsets[i];
        v[i].setOutDegree(l);
        v[i].setOutNeighbors(edges+2*o);
    }

    uintT* tOffsets = newA(uintT,n);
    parallel_for(long i = 0; i < n; i++)
        tOffsets[i] = INT_T_MAX;
    intE* inEdges = newA(intE,2*m);
    intTriple* temp = newA(intTriple,m);
    parallel_for(long i = 0; i < n; i++){
        uintT o = offsets[i];
        for(uintT j = 0; j < v[i].getOutDegree(); j++)
            temp[o+j] = make_pair(v[i].getOutNeighbor(j),make_pair(i,v[i].getOutWeight(j)));
    }
    free(offsets);
    intSort::iSort(temp,m,n+1,getFirst<intPair>());
    tOffsets[temp[0].first] = 0;
    inEdges[0] = temp[0].second.first;
    inEdges[1] = temp[0].second.second;
    parallel_for(long i = 1; i < m; i++) {
        inEdges[2*i] = temp[i].second.first;
        inEdges[2*i+1] = temp[i].second.second;
        if(temp[i].first != temp[i-1].first)
            tOffsets[temp[i].first] = i;
    }
    free(temp);
    // fill in offsets of degree 0 vertices by taking closest non-zero
    // offset to the right
    sequence::scanIBack(tOffsets,tOffsets,n,minF<uintT>(),(uintT)m);

    parallel_for(long i = 0;i < n; i++){
        uintT o = tOffsets[i];
        uintT l = ((i == n-1) ? m : tOffsets[i+1])-tOffsets[i];
        v[i].setInDegree(l);
        v[i].setInNeighbors(inEdges+2*o);
    }
    free(tOffsets);
    Uncompressed_Mem<vertex>* mem = new Uncompressed_Mem<vertex>(v,n,m,edges,inEdges);
    return graph<vertex>(v,n,m,true,mem);
}

template <class vertex>
typename enable_if<TraitsHelper<vertex>::isWeighted && TraitsHelper<vertex>::isSymmetric, graph<vertex> >::type
readGraphFromFile(char* fname) { // Graph<symmetricWeighted>
    _seq<char> S = readStringFromFile(fname);
    words W = stringToWords(S.A, S.n);
    long len = W.m -1;
    long n = atol(W.Strings[1]);
    long m = atol(W.Strings[2]);
    if (W.Strings[0] != (string) "WeightedAdjacencyGraph"){
        cerr << "Bad input file" << endl;
        abort();
    }
    if (len != n + 2*m + 2) {
        cerr << "Bad input file" << endl;
        abort();
    }

    uintT* offsets = newA(uintT,n);
    intE* edges = newA(intE,2*m);
    parallel_for(long i = 0; i < n; i++)
        offsets[i] = atol(W.Strings[i + 3]);
    parallel_for(long i = 0; i < m; i++) {
        edges[2*i] = atol(W.Strings[i+n+3]); 
        edges[2*i+1] = atol(W.Strings[i+n+m+3]);
    }
    vertex* v = newA(vertex,n);
    parallel_for (uintT i = 0; i < n; i++) {
        uintT o = offsets[i];
        uintT l = ((i == n-1) ? m : offsets[i+1])-offsets[i];
        v[i].setOutDegree(l);
        v[i].setOutNeighbors(edges+2*o);
    }

    free(offsets);
    Uncompressed_Mem<vertex>* mem = new Uncompressed_Mem<vertex>(v,n,m,edges);
    return graph<vertex>(v,n,m,true,mem);
}

#endif