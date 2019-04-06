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

ifdef LJ
DATASET = ~/Dataset/soc-LiveJournal1
DATASETW = ~/Dataset/soc-LiveJournal1-w
else ifdef USA
DATASET = ~/Dataset/road_usa
DATASETW = ~/Dataset/road_usa-w
else
DATASET = ../inputs/rMatGraph_J_5_100
DATASETW = ../inputs/rMatGraph_WJ_5_100
endif

TIME = time -p

# make sure modifying header files will lead to recompilation 
COMMON := krill.h kernel.h graph.h vertex.h utils.h IO.h
COMMON += parallel.h gettime.h transpose.h parseCommandLine.h vertexSubset.h
COMMON += blockRadixSort.h

KERNEL = BFS.h SSSP.h PageRank.h CC.h KCore.h

ALL = Homo1 Homo2 Heter M-BFS M-SSSP Singleton

all: $(ALL)

% : %.cpp
	$(PCC) -I../krill $(PCFLAGS) $(DEBUGFLAG) $(PROFILEFLAG) -pthread $< -o $@

.PHONY : clean bfs pr sssp cc homo1 homo2 heter mbfs msssp
clean :
	rm -f *.o $(ALL) *.gcov *.gcno *.gcda *.out.* *.out

bfs: Singleton
	$(TIME) ./Singleton -task bfs $(DATASET)

pr: Singleton
	$(TIME) ./Singleton -task pr $(DATASET)

sssp: Singleton
	$(TIME) ./Singleton -task sssp -w $(DATASETW)

cc: Singleton
	$(TIME) ./Singleton -task cc $(DATASET)

homo1: Homo1
	$(TIME) ./Homo1 $(DATASET)

homo2: Homo2
	$(TIME) ./Homo2 -w $(DATASETW)

heter: Heter
	$(TIME) ./Heter -w $(DATASETW)

mbfs: M-BFS
	$(TIME) ./M-BFS $(DATASET)

msssp: M-SSSP
	$(TIME) ./M-SSSP -w $(DATASETW)