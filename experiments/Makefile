# suppose all the ligra programs are built

LIGRA_PATH = ../../ligra/apps
DATASET_PATH = ~/Dataset

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
else
DATA = rMatGraph
DATASET = ../inputs/$(DATA)_J_5_100
DATASETW = ../inputs/$(DATA)_WJ_5_100
endif

TIME = /usr/bin/time -p -o timing/$(DATA)/$@.time sh -c

exp: build homo1 homo2 heter mbfs msssp
build:
	-mkdir -p timing/$(DATA)

bfs: build
	$(TIME) './$(LIGRA_PATH)/BFS $(DATASET)'

pr: build
	$(TIME) './$(LIGRA_PATH)/PageRank $(DATASET)'

cc: build
	$(TIME) './$(LIGRA_PATH)/Components $(DATASET)'

sssp: build
	$(TIME) './$(LIGRA_PATH)/BellmanFord $(DATASET)'

homo1: build homo1s homo1p
homo1s: build
	$(TIME) './$(LIGRA_PATH)/BFS -r 0 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 10 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 20 $(DATASET); \
	./$(LIGRA_PATH)/BFS -r 30 $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET); \
	./$(LIGRA_PATH)/Components $(DATASET)'

homo1p: build
	$(TIME) './$(LIGRA_PATH)/BFS -r 0 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 10 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 20 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 30 $(DATASET) & \
	./$(LIGRA_PATH)/Components $(DATASET) & \
	./$(LIGRA_PATH)/Components $(DATASET) & \
	./$(LIGRA_PATH)/Components $(DATASET) & \
	./$(LIGRA_PATH)/Components $(DATASET) & \
	wait'

homo2: build homo2s homo2p
homo2s: build
	$(TIME) './$(LIGRA_PATH)/BellmanFord -r 2 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 12 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 22 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 32 $(DATASETW) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET)'

homo2p: build
	$(TIME) './$(LIGRA_PATH)/BellmanFord -r 2 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 12 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 22 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 32 $(DATASETW) & \
	./$(LIGRA_PATH)/PageRank $(DATASET) & \
	./$(LIGRA_PATH)/PageRank $(DATASET) & \
	./$(LIGRA_PATH)/PageRank $(DATASET) & \
	./$(LIGRA_PATH)/PageRank $(DATASET) & \
	wait'

heter: build heters heterp
heters: build
	$(TIME) './$(LIGRA_PATH)/BFS -r 0 $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 1 $(DATASETW) ; \
	./$(LIGRA_PATH)/BFS -r 2 $(DATASET) ; \
	./$(LIGRA_PATH)/Components $(DATASET) ; \
	./$(LIGRA_PATH)/PageRank $(DATASET) ; \
	./$(LIGRA_PATH)/BellmanFord -r 3 $(DATASETW)'

heterp: build
	$(TIME) './$(LIGRA_PATH)/BFS -r 0 $(DATASET) & \
	./$(LIGRA_PATH)/Components $(DATASET) & \
	./$(LIGRA_PATH)/PageRank $(DATASET) & \
	./$(LIGRA_PATH)/BellmanFord -r 1 $(DATASETW) & \
	./$(LIGRA_PATH)/BFS -r 2 $(DATASET) & \
	./$(LIGRA_PATH)/Components $(DATASET) & \
	./$(LIGRA_PATH)/PageRank $(DATASET) & \
	./$(LIGRA_PATH)/BellmanFord -r 3 $(DATASETW) & \
	wait'

mbfs: build mbfss mbfsp
mbfss: build
	$(TIME) './$(LIGRA_PATH)/BFS -r 0 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 10 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 20 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 30 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 40 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 50 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 60 $(DATASET) ; \
	./$(LIGRA_PATH)/BFS -r 70 $(DATASET)'

mbfsp: build
	$(TIME) './$(LIGRA_PATH)/BFS -r 0 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 10 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 20 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 30 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 40 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 50 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 60 $(DATASET) & \
	./$(LIGRA_PATH)/BFS -r 70 $(DATASET) & \
	wait'

msssp: build msssps mssspp
msssps: build
	$(TIME) './$(LIGRA_PATH)/BellmanFord -r 0 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 10 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 20 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 30 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 40 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 50 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 60 $(DATASETW) ; \
	./$(LIGRA_PATH)/BellmanFord -r 70 $(DATASETW)'

mssspp: build
	$(TIME) './$(LIGRA_PATH)/BellmanFord -r 0 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 10 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 20 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 30 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 40 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 50 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 60 $(DATASETW) & \
	./$(LIGRA_PATH)/BellmanFord -r 70 $(DATASETW) & \
	wait'