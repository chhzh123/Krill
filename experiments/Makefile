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

.PHONY: clean cleanpcm exp build bfs pr sssp cc homo1 homo2 heter mbfs msssp pcm bfspcm prpcm ccpcm sssppcm

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
	$(TIME) './$(LIGRA_PATH)/BellmanFord $(DATASET)'
	$(PERF) './$(LIGRA_PATH)/BellmanFord $(DATASET)'

HOMO1S = ./$(LIGRA_PATH)/BFS -r 311 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 622 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 933 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 1244 $(DATASET); \
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

clean:
	-rm -rf $(PROFILE_PATH) *.prof

cleanpcm:
	-rm -rf $(PCM_OUTPUT_PATH) *.pcm