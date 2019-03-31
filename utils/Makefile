ifdef LONG
INTT = -DLONG
endif

ifdef EDGELONG
INTE = -DEDGELONG
endif

ifdef DEBUG
DEBUGFLAG = -g -DDEBUG
endif

#compilers
ifdef CILK
PCC = g++
PCFLAGS = -fcilkplus -lcilkrts -O3 -DCILK $(INTT) $(INTE)
PLFLAGS = -fcilkplus -lcilkrts

else ifdef MKLROOT
PCC = icpc
PCFLAGS = -O3 -DCILKP $(INTT) $(INTE)

else ifdef OPENMP
PCC = g++
PCFLAGS = -fopenmp -O3 -DOPENMP $(INTT) $(INTE)

else
PCC = g++
PCFLAGS = -O3 $(INTT) $(INTE)
endif

COMMON = utils.h parseCommandLine.h parallel.h quickSort.h blockRadixSort.h transpose.h
LOCAL_COMMON = graphIO.h
GENERATORS = rMatGraph gridGraph randLocalGraph SNAPtoAdj adjGraphAddWeights adjToBinary

.PHONY: all clean
all: $(GENERATORS)

$(COMMON):
	ln -s ../krill/$@ .

% : %.C $(COMMON) $(LOCAL_COMMON)
	$(PCC) $(PCFLAGS) $(DEBUGFLAG) -o $@ $<

clean :
	rm -f *.o $(GENERATORS)

cleansrc :
	make -s clean
	rm -f $(COMMON)