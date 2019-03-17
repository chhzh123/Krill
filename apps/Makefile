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
DEBUGFLAG = -g
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
PCFLAGS = -O3 $(INTT) $(INTE) $(CODE) $(PD)
endif

COMMON := krill.h kernel.h graph.h vertex.h utils.h IO.h
COMMON += parallel.h gettime.h transpose.h parseCommandLine.h vertexSubset.h
COMMON += blockRadixSort.h

KERNEL= BFS.h SSSP.h

ALL= batch

all: $(ALL)

% : %.cpp $(COMMON) $(KERNEL)
	$(PCC) $(PCFLAGS) $(DEBUGFLAG) $< -o $@

$(COMMON):
	ln -s ../krill/$@ .

.PHONY : clean

clean :
	rm -f *.o $(ALL)

cleansrc :
	rm -f *.o $(ALL)
	rm $(COMMON)