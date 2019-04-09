# suppose all the ligra programs are built

KRILL_PATH = ../apps
LIGRA_PATH = ../../ligra/apps
DATASET_PATH = ../../../Dataset
PCM_PATH = ../../../Tools/pcm

ifdef CP
DATA = cit-Patents
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else ifdef LJ
DATA = soc-LiveJournal1
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else ifdef USA
DATA = road_usa
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else ifdef TW
DATA = twitter7
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else ifdef FT
DATA = com-friendster
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else
DATA = rMatGraph
DATASET = ../inputs/$(DATA)_J_5_100
DATASETW = ../inputs/$(DATA)_WJ_5_100
endif

PROFILE_PATH = profile
PCM_OUTPUT_PATH = pcm
TIMEK = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@k.time sh -c
PERFK = perf stat -d -o $(PROFILE_PATH)/$(DATA)/$@k.perf sh -c
TIME = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@.time sh -c
PERF = perf stat -d -o $(PROFILE_PATH)/$(DATA)/$@.perf sh -c

PCM = sudo ./$(PCM_PATH)/pcm-memory.x 0.01 -csv=$(PCM_OUTPUT_PATH)/$@.csv --external-program

# to clear the cache
#sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'

.PHONY: clean cleanpcm exp build bfs pr sssp cc homo1 homo2 heter mbfs msssp pcm bfspcm prpcm ccpcm sssppcm multibfs multicore

exp: build homo1 homo2 heter mbfs msssp
	python profiling.py $(PROFILE_PATH)/$(DATA)

build:
	-mkdir -p $(PROFILE_PATH)/$(DATA)

bfs: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -task bfs $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -task bfs $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/BFS $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/BFS $(DATASET)'

pr: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -task pr $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -task pr $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/PageRank $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/PageRank $(DATASET)'

cc: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -task cc $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -task cc $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/Components $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/Components $(DATASET)'

sssp: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -task sssp -w $(DATASETW)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -task sssp -w $(DATASETW)"
	$(TIME) './$(LIGRA_PATH)/BellmanFord $(DATASETW)'
	$(PERF) './$(LIGRA_PATH)/BellmanFord $(DATASETW)'

HOMO1S = ./$(LIGRA_PATH)/BFS -r 10 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 20 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 30 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 40 $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET)
HOMO1P = $(subst ;, &,$(HOMO1S)) & wait
homo1: build homo1k homo1s homo1p
homo1k: build
	$(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	$(PERF) "./$(KRILL_PATH)/Homo1 $(DATASET)"
homo1s: build
	$(TIME) '$(HOMO1S)'
	$(PERF) '$(HOMO1S)'
homo1p: build
	$(TIME) '$(HOMO1P)'
	$(PERF) '$(HOMO1P)'

HOMO2S = ./$(LIGRA_PATH)/BellmanFord -r 73 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 144 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 215 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 286 $(DATASETW) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET)
HOMO2P = $(subst ;, &,$(HOMO2S)) & wait
homo2: build homo2k homo2s homo2p
homo2k: build
	$(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
homo2s: build
	$(TIME) '$(HOMO2S)'
	$(PERF) '$(HOMO2S)'
homo2p: build
	$(TIME) '$(HOMO2P)'
	$(PERF) '$(HOMO2P)'

HETERS = ./$(LIGRA_PATH)/BFS -r 71 $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 102 $(DATASETW) ; \
	./$(LIGRA_PATH)/BFS -r 142 $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 203 $(DATASETW)
HETERP = $(subst ;, &,$(HETERS)) & wait
heter: build heterk heters heterp
heterk: build
	$(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
heters: build
	$(TIME) '$(HETERS)'
	$(PERF) '$(HETERS)'
heterp: build
	$(TIME) '$(HETERP)'
	$(PERF) '$(HETERP)'

MBFSS = ./$(LIGRA_PATH)/BFS -r 91 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 182 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 273 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 364 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 455 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 546 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 637 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 828 $(DATASET)
MBFSP = $(subst ;, &,$(MBFSS)) & wait
mbfs: build mbfsk mbfss mbfsp
mbfsk: build
	$(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	$(PERF) "./$(KRILL_PATH)/M-BFS $(DATASET)"
mbfss: build
	$(TIME) '$(MBFSS)'
	$(PERF) '$(MBFSS)'
mbfsp: build
	$(TIME) '$(MBFSP)'
	$(PERF) '$(MBFSP)'

MSSSPS = ./$(LIGRA_PATH)/BellmanFord -r 211 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 422 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 633 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 844 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1055 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1266 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1477 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1688 $(DATASETW)
MSSSPP = $(subst ;, &,$(MSSSPS)) & wait
msssp: build mssspk msssps mssspp
mssspk: build
	$(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
msssps: build
	$(TIME) '$(MSSSPS)'
	$(PERF) '$(MSSSPS)'

mssspp: build
	$(TIME) '$(MSSSPP)'
	$(PERF) '$(MSSSPP)'

pcm: buildpcm bfspcm prpcm ccpcm sssppcm
	python pcm.py $(PCM_OUTPUT_PATH)
buildpcm:
	sudo modprobe msr
	-mkdir $(PCM_OUTPUT_PATH)
bfspcm: buildpcm
	$(PCM) ./$(LIGRA_PATH)/BFS $(DATASET)
prpcm: buildpcm
	$(PCM) ./$(LIGRA_PATH)/PageRank $(DATASET)
ccpcm: buildpcm
	$(PCM) ./$(LIGRA_PATH)/Components $(DATASET)
sssppcm: buildpcm
	$(PCM) ./$(LIGRA_PATH)/BellmanFord $(DATASETW)

TIMEBFS = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@
multibfs: multibfsk
multibfsk: build
	$(TIMEBFS)1.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 1 $(DATASET)"
	$(TIMEBFS)2.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 2 $(DATASET)"
	$(TIMEBFS)4.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 4 $(DATASET)"
	$(TIMEBFS)8.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 8 $(DATASET)"
	$(TIMEBFS)16.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 16 $(DATASET)"
	$(TIMEBFS)32.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 32 $(DATASET)"
	$(TIMEBFS)64.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 64 $(DATASET)"
	$(TIMEBFS)128.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 128 $(DATASET)"

# multibfss:
# 	 for i in 1 2 4 8 16 32 64 128 ; do \
# 	 	src=1 ;
# 		$(TIMEBFS) sh -c "while [[ $$src -le $$i ]] ; do \
# 			./$(LIGRA_PATH)/BFS -r 10*$$i $(DATASET) ; \
# 			$$((src = src +1)) ; \
# 		done"
# 	done

TIMEMULTICORE = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@
# or env CILK_NWORKERS
multicore:
	export CILK_NWORKERS=1 && $(TIMEMULTICORE)1.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=2 && $(TIMEMULTICORE)2.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=4 && $(TIMEMULTICORE)4.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=8 && $(TIMEMULTICORE)8.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=16 && $(TIMEMULTICORE)16.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=32 && $(TIMEMULTICORE)32.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=40 && $(TIMEMULTICORE)40.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"

clean:
	-rm -rf $(PROFILE_PATH) *.prof

cleanpcm:
	-rm -rf $(PCM_OUTPUT_PATH) *.pcm