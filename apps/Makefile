ifdef LONG
INTT = -DLONG
endif

ifdef EDGELONG
INTE = -DEDGELONG
endif

ifdef PD
PD = -DPD
endif

ifdef DEBUG
DEBUGFLAG = -g -DDEBUG
endif

ifdef PROFILE
PROFILEFLAG = -pg -fprofile-arcs -ftest-coverage --coverage
endif

ifdef NOKERF
NOKERFFLAG = -DNOKERF
endif

ifdef NOOPT
NOOPTFLAG = -DNOOPT
endif

# compilers
ifdef ONEAPI_ROOT
PCC = dpcpp
PCFLAGS = -fopenmp -O3 -DOPENMP $(INTT) $(INTE) $(CODE) $(PD)

else ifdef OPENMP
PCC = g++
PCFLAGS = -fopenmp -O3 -DOPENMP $(INTT) $(INTE) $(CODE) $(PD)

else
PCC = g++
PCFLAGS = -fcilkplus -lcilkrts -O2 -DCILK $(INTT) $(INTE) $(CODE) $(PD)
PLFLAGS = -fcilkplus -lcilkrts
endif

COMMON_FLAGS = -std=c++11 -pthread

.SUFFIXES:            # Delete the default suffixes

# to clear the cache
#sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'

# make sure modifying header files will lead to recompilation
COMMON := krill.h kernel.h graph.h vertex.h utils.h IO.h
COMMON += parallel.h gettime.h transpose.h parseCommandLine.h vertexSubset.h
COMMON += blockRadixSort.h

KERNEL = BFS.h SSSP.h PageRank.h CC.h

ALL = Homo1 Homo2 Heter M-BFS M-SSSP Multi-BFS
# ALL += M-CC M-PageRank Singleton Heter2

all: $(ALL)

Singleton: Singleton.cpp Heter.pb.h
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) $(COMMON_FLAGS) $(NOKERFFLAG) $(NOOPTFLAG) $< -o $@

% : %.cpp %.pb.h
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) $(COMMON_FLAGS) $(NOKERFFLAG) $(NOOPTFLAG) $< -o $@

all_pbs: $(addsuffix .pb.h, $(ALL))
%.pb.h: %.prop
	python ../propbuf/propbuf.py $< $(LAYOUT)

.PHONY: clean
clean:
	-rm -f *.o $(ALL) *.pb.h *.gcov *.gcno *.gcda *.out.* *.out *.log