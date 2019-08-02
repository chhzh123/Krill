# suppose all the ligra programs are built

KRILL_PATH = ../apps
LIGRA_PATH = ../../ligra/apps
DATASET_PATH = ../../Dataset
PCM_PATH = ../../../Tools/pcm

ifdef CP
DATA = cit-Patents
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else ifdef LJ
DATA = soc-LiveJournal1
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w
else ifdef RD
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
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job bfs $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job bfs $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/BFS $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/BFS $(DATASET)'

pr: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job pr $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job pr $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/PageRank $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/PageRank $(DATASET)'

cc: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job cc $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job cc $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/Components $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/Components $(DATASET)'

sssp: build
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job sssp -w $(DATASETW)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job sssp -w $(DATASETW)"
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
multibfs: build multibfsk multibfss multibfsp
multibfsk: build
	$(TIMEBFS)1.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 1 $(DATASET)"
	$(TIMEBFS)2.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 2 $(DATASET)"
	$(TIMEBFS)4.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 4 $(DATASET)"
	$(TIMEBFS)8.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 8 $(DATASET)"
	$(TIMEBFS)16.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 16 $(DATASET)"
	$(TIMEBFS)32.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 32 $(DATASET)"
	$(TIMEBFS)64.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 64 $(DATASET)"
	$(TIMEBFS)128.time sh -c "./$(KRILL_PATH)/Multi-BFS -n 128 $(DATASET)"
multibfss: build
	-mkdir multibfs
	-python write_sh.py
	chmod u+x multibfs/multibfss1.sh multibfs/multibfss2.sh multibfs/multibfss4.sh multibfs/multibfss8.sh multibfs/multibfss16.sh multibfs/multibfss32.sh multibfs/multibfss64.sh multibfs/multibfss128.sh
	$(TIMEBFS)1.time sh -c "./multibfs/multibfss1.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)2.time sh -c "./multibfs/multibfss2.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)4.time sh -c "./multibfs/multibfss4.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)8.time sh -c "./multibfs/multibfss8.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)16.time sh -c "./multibfs/multibfss16.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)32.time sh -c "./multibfs/multibfss32.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)64.time sh -c "./multibfs/multibfss64.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)128.time sh -c "./multibfs/multibfss128.sh $(LIGRA_PATH) $(DATASET)"
multibfsp: build
	-mkdir multibfs
	-python write_sh.py
	chmod u+x multibfs/multibfsp1.sh multibfs/multibfsp2.sh multibfs/multibfsp4.sh multibfs/multibfsp8.sh multibfs/multibfsp16.sh multibfs/multibfsp32.sh multibfs/multibfsp64.sh multibfs/multibfsp128.sh
	$(TIMEBFS)1.time sh -c "./multibfs/multibfsp1.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)2.time sh -c "./multibfs/multibfsp2.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)4.time sh -c "./multibfs/multibfsp4.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)8.time sh -c "./multibfs/multibfsp8.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)16.time sh -c "./multibfs/multibfsp16.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)32.time sh -c "./multibfs/multibfsp32.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)64.time sh -c "./multibfs/multibfsp64.sh $(LIGRA_PATH) $(DATASET)"
	$(TIMEBFS)128.time sh -c "./multibfs/multibfsp128.sh $(LIGRA_PATH) $(DATASET)"

TIMEMULTICORE = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@
# or env CILK_NWORKERS
multicore: build multicorek multicores multicorep
multicorek: build
	export CILK_NWORKERS=1 && $(TIMEMULTICORE)1.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=2 && $(TIMEMULTICORE)2.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=4 && $(TIMEMULTICORE)4.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=8 && $(TIMEMULTICORE)8.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=16 && $(TIMEMULTICORE)16.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=32 && $(TIMEMULTICORE)32.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	export CILK_NWORKERS=40 && $(TIMEMULTICORE)40.time sh -c "./$(KRILL_PATH)/Heter -w $(DATASETW)"
multicores: build
	env CILK_NWORKERS=1 $(TIMEMULTICORE)1.time sh -c '$(HETERS)'
	env CILK_NWORKERS=2 $(TIMEMULTICORE)2.time sh -c '$(HETERS)'
	env CILK_NWORKERS=4 $(TIMEMULTICORE)4.time sh -c '$(HETERS)'
	env CILK_NWORKERS=8 $(TIMEMULTICORE)8.time sh -c '$(HETERS)'
	env CILK_NWORKERS=16 $(TIMEMULTICORE)16.time sh -c '$(HETERS)'
	env CILK_NWORKERS=32 $(TIMEMULTICORE)32.time sh -c '$(HETERS)'
	env CILK_NWORKERS=40 $(TIMEMULTICORE)40.time sh -c '$(HETERS)'
multicorep: build
	taskset -c 0 env CILK_NWORKERS=1 $(TIMEMULTICORE)1.time sh -c '$(HETERP)'
	taskset -c 0-1 env CILK_NWORKERS=2 $(TIMEMULTICORE)2.time sh -c '$(HETERP)'
	taskset -c 0-3 env CILK_NWORKERS=4 $(TIMEMULTICORE)4.time sh -c '$(HETERP)'
	taskset -c 0-7 env CILK_NWORKERS=8 $(TIMEMULTICORE)8.time sh -c '$(HETERP)'
	taskset -c 0-15 env CILK_NWORKERS=16 $(TIMEMULTICORE)16.time sh -c '$(HETERP)'
	taskset -c 0-31 env CILK_NWORKERS=32 $(TIMEMULTICORE)32.time sh -c '$(HETERP)'
	taskset -c 0-39 env CILK_NWORKERS=40 $(TIMEMULTICORE)40.time sh -c '$(HETERP)'

clean:
	-rm -rf $(PROFILE_PATH) *.prof
	-rm -rf $(PCM_OUTPUT_PATH) *.pcm