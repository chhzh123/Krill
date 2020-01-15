# suppose all the ligra programs are built

KRILL_PATH = ../apps
LIGRA_PATH = ../../ligra/apps
DATASET_PATH = ../../Dataset
PCM_PATH = ../../pcm

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

else ifdef RMAT
DATA = rMatGraph24
DATASET = $(DATASET_PATH)/$(DATA)
DATASETW = $(DATASET_PATH)/$(DATA)-w

else
DATA = rMatGraph
DATASET = ../inputs/$(DATA)_J_5_100
DATASETW = ../inputs/$(DATA)_WJ_5_100
endif

PROFILE_PATH = profile
PCM_OUTPUT_PATH = pcm-results
TIMEK = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@k.time sh -c
PERFK = perf stat -d -o $(PROFILE_PATH)/$(DATA)/$@k.perf sh -c
TIME = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@.time sh -c
PERF = perf stat -d -o $(PROFILE_PATH)/$(DATA)/$@.perf sh -c

PCM = sudo ./$(PCM_PATH)/pcm-memory.x 0.01 -csv=$(PCM_OUTPUT_PATH)/$@.csv --external-program

# to clear the cache
#sudo sh -c 'echo 1 >/proc/sys/vm/drop_caches'

.PHONY: clean exp build bfs pr sssp cc homo1 homo2 heter mbfs msssp pcm bfspcm prpcm ccpcm sssppcm multibfs multicore

# Something needs to be noticed:
# 1. be careful that cold start of the first attempt will lead to performance degradation
# 2. `time` and `perf` cannot be done together, since `perf` may also degrade performance
exp: build homo1 homo2 heter mbfs msssp
	python profiling.py $(PROFILE_PATH)/$(DATA)

expk: build homo1k homo2k heterk mbfsk mssspk
	python profiling.py $(PROFILE_PATH)/$(DATA)

exps: build homo1s homo2s heters mbfss msssps
	python profiling.py $(PROFILE_PATH)/$(DATA)

expp: build homo1p homo2p heterp mbfsp mssspp
	python profiling.py $(PROFILE_PATH)/$(DATA)

expnokerf: build homo1nokerf homo2nokerf heternokerf mbfsnokerf mssspnokerf
	python profiling.py $(PROFILE_PATH)/$(DATA)

expnoopt: build homo1noopt homo2noopt heternoopt mbfsnoopt mssspnoopt
	python profiling.py $(PROFILE_PATH)/$(DATA)

expsingle: build bfs pr cc sssp

expkhomo: build mcc mpr

build:
	-mkdir -p $(PROFILE_PATH)/$(DATA)

bfs: build
	# first try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job bfs -r 1001 $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job bfs -r 1001 $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/BFS -r 1001 $(DATASET)'
	# second try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job bfs -r 1001 $(DATASET)"

pr: build
	# first try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job pr $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job pr $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/PageRank $(DATASET)'
	# second try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job pr $(DATASET)"

cc: build
	# first try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job cc $(DATASET)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job cc $(DATASET)"
	$(TIME) './$(LIGRA_PATH)/Components $(DATASET)'
	# second try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job cc $(DATASET)"

sssp: build
	# first try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job sssp -r 1001 -w $(DATASETW)"
	$(PERFK) "./$(KRILL_PATH)/Singleton -job sssp -r 1001 -w $(DATASETW)"
	$(TIME) './$(LIGRA_PATH)/BellmanFord -r 1001 $(DATASETW)'
	# second try
	$(TIMEK) "./$(KRILL_PATH)/Singleton -job sssp -r 1001 -w $(DATASETW)"

HOMO1S = ./$(LIGRA_PATH)/BFS -r 10 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 20 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 30 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 40 $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET)
HOMO1P = $(subst ;, &,$(HOMO1S)) & wait
ifneq "$(or $(TW), $(FT))" ""
homo1: build homo1k homo1s
else
homo1: build homo1k homo1s homo1p
endif
homo1k: build
	# first try
	$(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
homo1nokerf: build
	# first try
	# Need to recompile! The execution file is different!
	# $(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	# second try
	$(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	$(PERF) "./$(KRILL_PATH)/Homo1 $(DATASET)"
homo1noopt: build
	# first try
	# Need to recompile! The execution file is different!
	# $(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	# second try
	$(TIME) "./$(KRILL_PATH)/Homo1 $(DATASET)"
	$(PERF) "./$(KRILL_PATH)/Homo1 $(DATASET)"
homo1s: build
	# first try
	$(TIME) '$(HOMO1S)'
	# second try
	$(PERF) '$(HOMO1S)'
	$(TIME) '$(HOMO1S)'
homo1p: build
	# first try
	$(TIME) '$(HOMO1P)'
	# second try
	$(PERF) '$(HOMO1P)'
	$(TIME) '$(HOMO1P)'

HOMO2S = ./$(LIGRA_PATH)/BellmanFord -r 73 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 144 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 215 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 286 $(DATASETW) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET)
HOMO2P = $(subst ;, &,$(HOMO2S)) & wait
ifneq "$(or $(TW), $(FT))" ""
homo2: build homo2k homo2s
else
homo2: build homo2k homo2s homo2p
endif
homo2k: build
	# first try
	$(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	# second try
	$(PERF) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	$(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
homo2nokerf: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	# second try
	$(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
homo2noopt: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	# second try
	$(TIME) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Homo2 -w $(DATASETW)"
homo2s: build
	# first try
	$(TIME) '$(HOMO2S)'
	# second try
	$(PERF) '$(HOMO2S)'
	$(TIME) '$(HOMO2S)'
homo2p: build
	# first try
	$(TIME) '$(HOMO2P)'
	# second try
	$(PERF) '$(HOMO2P)'
	$(TIME) '$(HOMO2P)'

HETERS = ./$(LIGRA_PATH)/BFS -r 71 $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 102 $(DATASETW) ; \
	./$(LIGRA_PATH)/BFS -r 142 $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 203 $(DATASETW)
HETERP = $(subst ;, &,$(HETERS)) & wait
ifneq "$(or $(TW), $(FT))" ""
heter: build heterk heters
else
heter: build heterk heters heterp
endif
heterk: build
	# first try
	$(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	# second try
	$(PERF) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	$(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
heternokerf: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	# second try
	$(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
heternoopt: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	# second try
	$(TIME) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Heter -w $(DATASETW)"
heters: build
	# first try
	$(TIME) '$(HETERS)'
	# second try
	$(PERF) '$(HETERS)'
	$(TIME) '$(HETERS)'
heterp: build
	# first try
	$(TIME) '$(HETERP)'
	# second try
	$(PERF) '$(HETERP)'
	$(TIME) '$(HETERP)'

MBFSS = ./$(LIGRA_PATH)/BFS -r 91 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 182 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 273 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 364 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 455 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 546 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 637 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 728 $(DATASET)
MBFSP = $(subst ;, &,$(MBFSS)) & wait
ifneq "$(or $(TW), $(FT))" ""
mbfs: build mbfsk mbfss
else
mbfs: build mbfsk mbfss mbfsp
endif
mbfsk: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
mbfsnokerf: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	# second try
	$(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	$(PERF) "./$(KRILL_PATH)/M-BFS $(DATASET)"
mbfsnoopt: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	# second try
	$(TIME) "./$(KRILL_PATH)/M-BFS $(DATASET)"
	$(PERF) "./$(KRILL_PATH)/M-BFS $(DATASET)"
mbfss: build
	# first try
	$(TIME) '$(MBFSS)'
	# second try
	$(PERF) '$(MBFSS)'
	$(TIME) '$(MBFSS)'
mbfsp: build
	# first try
	$(TIME) '$(MBFSP)'
	# second try
	$(PERF) '$(MBFSP)'
	$(TIME) '$(MBFSP)'

MSSSPS = ./$(LIGRA_PATH)/BellmanFord -r 211 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 422 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 633 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 844 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1055 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1266 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1477 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1688 $(DATASETW)
MSSSPP = $(subst ;, &,$(MSSSPS)) & wait
ifneq "$(or $(TW), $(FT))" ""
msssp: build mssspk msssps
else
msssp: build mssspk msssps mssspp
endif
mssspk: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	$(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
mssspnokerf: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	# second try
	$(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
mssspnoopt: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	# second try
	$(TIME) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/M-SSSP -w $(DATASETW)"
msssps: build
	# first try
	$(TIME) '$(MSSSPS)'
	# second try
	$(PERF) '$(MSSSPS)'
	$(TIME) '$(MSSSPS)'
mssspp: build
	# first try
	$(TIME) '$(MSSSPP)'
	# second try
	$(PERF) '$(MSSSPP)'
	$(TIME) '$(MSSSPP)'

MCCS = ./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET)
MCCP = $(subst ;, &,$(MCCS)) & wait
ifneq "$(or $(TW), $(FT))" ""
mcc: build mcck mccs
else
mcc: build mcck mccs mccp
endif
mcck: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-CC $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-CC $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-CC $(DATASET)"
mccnokerf: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-CC $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-CC $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-CC $(DATASET)"
mccnoopt: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-CC $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-CC $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-CC $(DATASET)"
mccs: build
	# first try
	$(TIME) '$(MCCS)'
	# second try
	$(PERF) '$(MCCS)'
	$(TIME) '$(MCCS)'
mccp: build
	# first try
	$(TIME) '$(MCCP)'
	# second try
	$(PERF) '$(MCCP)'
	$(TIME) '$(MCCP)'

MPRS = ./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET)
MPRP = $(subst ;, &,$(MPRS)) & wait
ifneq "$(or $(TW), $(FT))" ""
mpr: build mprk mprs
else
mpr: build mprk mprs mprp
endif
mprk: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
mprnokerf: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
mprnoopt: build
	# first try
	$(TIME) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
	# second try
	$(PERF) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
	$(TIME) "./$(KRILL_PATH)/M-PageRank $(DATASET)"
mprs: build
	# first try
	$(TIME) '$(MPRS)'
	# second try
	$(PERF) '$(MPRS)'
	$(TIME) '$(MPRS)'
mprp: build
	# first try
	$(TIME) '$(MPRP)'
	# second try
	$(PERF) '$(MPRP)'
	$(TIME) '$(MPRP)'

HETER2S = ./$(LIGRA_PATH)/BFS -r 71 $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 102 $(DATASETW) ; \
	./$(LIGRA_PATH)/BFS -r 142 $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 203 $(DATASETW) ; \
	./$(LIGRA_PATH)/BFS -r 213 $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 304 $(DATASETW) ; \
	./$(LIGRA_PATH)/BFS -r 284 $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 405 $(DATASETW)
HETER2P = $(subst ;, &,$(HETER2S)) & wait
ifneq "$(or $(TW), $(FT))" ""
heter2: build heter2k heter2s
else
heter2: build heter2k heter2s heter2p
endif
heter2k: build
	# first try
# 	$(TIME) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
	# second try
# 	$(PERF) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
	$(TIME) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
heter2nokerf: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
	# second try
# 	$(TIME) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
heter2noopt: build
	# first try
	# $(TIME) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
	# second try
# 	$(TIME) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
	$(PERF) "./$(KRILL_PATH)/Heter2 -w $(DATASETW)"
heter2s: build
	# first try
# 	$(TIME) '$(HETER2S)'
	# second try
# 	$(PERF) '$(HETER2S)'
	$(TIME) '$(HETER2S)'
heter2p: build
	# first try
# 	$(TIME) '$(HETER2P)'
	# second try
# 	$(PERF) '$(HETER2P)'
	$(TIME) '$(HETER2P)'

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

pcmk: buildpcm homo1pcm homo2pcm heterpcm mbfspcm msssppcm

homo1pcm: buildpcm
	$(PCM) ./$(KRILL_PATH)/Homo1 $(DATASET)
homo2pcm: buildpcm
	$(PCM) ./$(KRILL_PATH)/Homo2 -w $(DATASETW)
heterpcm: buildpcm
	$(PCM) ./$(KRILL_PATH)/Heter -w $(DATASETW)
mbfspcm: buildpcm
	$(PCM) ./$(KRILL_PATH)/M-BFS $(DATASET)
msssppcm: buildpcm
	$(PCM) ./$(KRILL_PATH)/M-SSSP -w $(DATASETW)

TIMEBFS = /usr/bin/time -v -o $(PROFILE_PATH)/$(DATA)/$@
multibfs: build multibfsk multibfss multibfsp
	python profiling.py $(PROFILE_PATH)/$(DATA) 1
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
	-rm -rf $(PROFILE_PATH) *.prof *.log
	-rm -rf $(PCM_OUTPUT_PATH) *.pcm