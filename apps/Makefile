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

# compilers
ifdef CILK
PCC = g++
PCFLAGS = -fcilkplus -lcilkrts -O3 -DCILK $(INTT) $(INTE) $(CODE) $(PD)
PLFLAGS = -fcilkplus -lcilkrts

else ifdef MKLROOT
PCC = icpc
PCFLAGS = -O3 -DCILKP $(INTT) $(INTE) $(CODE) $(PD)

else ifdef OPENMP
PCC = g++
PCFLAGS = -fopenmp -O3 -DOPENMP $(INTT) $(INTE) $(CODE) $(PD)

else
PCC = g++
PCFLAGS = -O2 $(INTT) $(INTE) $(CODE) $(PD)
endif

COMMON_FLAGS = -std=c++11 -pthread

# to clear the cache
#sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'

# make sure modifying header files will lead to recompilation 
COMMON := krill.h kernel.h graph.h vertex.h utils.h IO.h
COMMON += parallel.h gettime.h transpose.h parseCommandLine.h vertexSubset.h
COMMON += blockRadixSort.h

KERNEL = BFS.h SSSP.h PageRank.h CC.h KCore.h

ALL = Homo1 Homo2 Heter M-BFS M-SSSP Singleton Multi-BFS

all: $(ALL)

% : %.cpp
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) $(COMMON_FLAGS) $< -o $@

.PHONY: clean
clean:
	-rm -f *.o $(ALL) *.gcov *.gcno *.gcda *.out.* *.out