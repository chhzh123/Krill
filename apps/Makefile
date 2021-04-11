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
ifdef CILK
PCC = g++
PCFLAGS = -fcilkplus -lcilkrts -O2 -DCILK $(INTT) $(INTE) $(CODE) $(PD)
PLFLAGS = -fcilkplus -lcilkrts

else ifdef ONEAPI_ROOT
PCC = dpcpp
PCFLAGS = -fopenmp -O3 -DOPENMP $(INTT) $(INTE) $(CODE) $(PD)

else ifdef OPENMP
PCC = g++
PCFLAGS = -fopenmp -O3 -DOPENMP $(INTT) $(INTE) $(CODE) $(PD)

else
PCC = g++
PCFLAGS = -O2 $(INTT) $(INTE) $(CODE) $(PD)
endif

COMMON_FLAGS = -std=c++11 -pthread
ALG_FLAGS = -fPIC -shared

.SUFFIXES:            # Delete the default suffixes

# to clear the cache
#sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'

ALL = Homo1 Homo2 Heter M-BFS M-SSSP Multi-BFS
# ALL += M-CC M-PageRank Singleton Heter2

all: main all_so

all_so: $(addsuffix .so, $(ALL))

Singleton: Singleton.cpp Heter.pb.h
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) $(COMMON_FLAGS) $(NOKERFFLAG) $(NOOPTFLAG) $(ALG_FLAGS) $< -o $@

main: main.cpp
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) $(COMMON_FLAGS) $(NOKERFFLAG) $(NOOPTFLAG) $< -ldl -o $@


%.so : %.cpp %.pb.h
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) $(COMMON_FLAGS) $(NOKERFFLAG) $(NOOPTFLAG) $(ALG_FLAGS) $< -o $@

all_pbs: $(addsuffix .pb.h, $(ALL))
%.pb.h: %.prop
	python ../propbuf/propbuf.py $< $(LAYOUT)

.PHONY: clean
clean:
	-rm -f *.o $(ALL) *.pb.h *.gcov *.gcno *.gcda *.out.* *.out *.log