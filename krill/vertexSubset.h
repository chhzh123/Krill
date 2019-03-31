// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen
// Original work Copyright (c) 2013 Julian Shun

#ifndef VERTEX_SUBSET_H
#define VERTEX_SUBSET_H

//*****VERTEX OBJECT*****
struct vertexSubset {
    bool isDense; // 3Bytes are wasted
    bool* d;
    uintE* s;
    long n, m; // n: maximum # of vertices, m: current # of vertices

    // new: null constructor
    vertexSubset():
        n(0), m(0), isDense(0), d(NULL), s(NULL){}

    // new: copy constructor
    vertexSubset(const vertexSubset& other):
        n(other.n), m(other.m), isDense(other.isDense), d(other.d), s(other.s){}

    // new: overload =, ensure other != (*this) SHALLOW COPY!
    vertexSubset& operator=(const vertexSubset& other){
        n = other.n; m = other.m; isDense = other.isDense;
        d = other.d; s = other.s;
        return *this;
    }

    // make a singleton vertex in range of n
    vertexSubset(long _n, intE v):
        n(_n), m(1), d(NULL), isDense(0) {
        s = newA(uintE,1);
        s[0] = v;
    }

    //empty vertex set
    vertexSubset(long _n):
        n(_n), m(0), d(NULL), s(NULL), isDense(0) {}
    // make vertexSubset from array of vertex indices
    // n is range, and m is size of array
    vertexSubset(long _n, long _m, uintE* indices):
        n(_n), m(_m), s(indices), d(NULL), isDense(0) {}
    // make vertexSubset from boolean array, where n is range
    vertexSubset(long _n, bool* bits):
        n(_n), d(bits), s(NULL), isDense(1) {
        m = sequence::sum(bits,_n);
    }
    // make vertexSubset from boolean array giving number of true values
    vertexSubset(long _n, long _m, bool* bits):
        n(_n), m(_m), s(NULL), d(bits), isDense(1) {}

    // delete the contents
    void del(){
        if (d != NULL) {free(d); d = NULL;}
        if (s != NULL) {free(s); s = NULL;}
    }
    inline long numRows() { return n; }
    inline long numNonzeros() { return m; }
    inline bool isEmpty() { return m == 0; }

    // converts to dense but keeps sparse representation if there
    bool* toDense() { // 01
        if (d == NULL) {
            d = newA(bool,n);
            {parallel_for (long i = 0; i < n; i++)
                d[i] = 0;}
            {parallel_for (long i = 0; i < m; i++)
                d[s[i]] = 1;}
        }
        isDense = true;
        return d; // Add return value
    }

    // converts to sparse but keeps dense representation if there
    uintE* toSparse() { // integer
        if (s == NULL) {
            _seq<uintE> R = sequence::packIndex<uintE>(d,n);
            if (m != R.n) {
                cerr << "Error: Expected stored value m = " << m << ", "
                     << "but get R.n = " << R.n << "!" << endl;
                abort();
            }
            s = R.A;
        }
        isDense = false;
        return s; // Add return value
    }

    // check for equality
    bool eq (vertexSubset& b) {
        toDense();
        b.toDense();
        bool* c = newA(bool,n);
        {parallel_for (long i = 0; i < b.n; i++)
            c[i] = (d[i] != b.d[i]);}
        bool equal = (sequence::sum(c,n) == 0);
        free(c);
        return equal;
    }

    void print() {
        if (isDense) {
            cout << "D:";
            for (long i = 0; i < n; i++)
                if (d[i])
                    cout << i << " ";
            cout << endl;
        } else {
            cout << "S:";
            for (long i = 0; i < m; i++)
                cout << s[i] << " ";
            cout << endl;
        }
    }
};

#endif