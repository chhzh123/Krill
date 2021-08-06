Krill: An Efficient Concurrent Graph Processing System
======================

Krill is an efficient graph system for processing **concurrent graph jobs**, which consists of a high-level compiler and a runtime system. We provide an interface called **property buffer** to easily manage the property data. The corresponding description file will be compiled by our compiler, and a header file will be generated for users to use. The runtime system is equipped with **graph kernel fusion**, which greatly reduces the number of memory accesses.

Currently, we select [Ligra](https://github.com/jshun/ligra), a state-of-the-art shared-memory single graph processing framework, as our underlying infrastructure.

## Getting Started

To write a program to process concurrent graph jobs, you should follow the steps below.

### Declare the required property data
Inspired by Google's [protocol buffer](https://developers.google.com/protocol-buffers/docs/proto3), we provide a clean interface to declare your property data for your graph jobs.

For example, in BFS, you need a `parent` array to store the parents of each vertex, then you can declare your property buffer as below:
```csharp
property BFS {
    int Parent = -1;
}
```

and save it as a `.prop` file. Our property buffer compiler will generate a header file `.pb.h` which consists of some common data access functions and a property manager for you to call.

### Write each graph job in a class
We provide two base classes named `UnweightedJob` and `WeightedJob`, and your custom graph job should be encapsulated in a class and inherit from these two bases.
For those jobs running on graphs with unweighted edges, you should publicly inherit from `UnweightedJob`.
For jobs running on graphs with weighted edges, inherit your class from `WeightedJob`.

Some basic functions should be overridden in the inherent classes, including
* `cond`: the condition needed to be satisfied for the destination vertices in each iteration.
* `update`: specify how to update the values of the src-dst vertex-pairs satisfying conditions, and also specify whether the destination vertex should be added in the next frontier.
* `updateAtomic`: the atomic version of `update`. Notice you should make sure the correctness of this function when running in parallel.
* `finished`: justify when your job is viewed as finished.
* `initialize`: initialize the private property values of your job.
* `clear`: if you create instances of some member data using dynamic allocation, you should free the memory in this function.

The first three functions are the same as Ligra, and the latter three are used for concurrent graph processing, since we decouple the processing logic.
All of them are pure virtual functions, which means compile error will occur if the functions are not be specified.

A detailed template can be found in `apps/template-singleton.h`.

### Integrate them into a kernel
After several graph jobs are implemented, you can place them into a container.

In the main program, you should provide the implementation of `setKernels`, where you create instances of your jobs and append them into a kernel container via `appendJob` function. For example:

```cpp
template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	PropertyManager prop(G.n); // declare property manager

	// you jobs here
	MyJob* myjob = new MyJob(G.n);
	K.appendJob(myjob);

	prop.initialize(); // do initialization
}
```

Notice you should first include the generated property buffer header file and the algorithm descriptions in the prelude. By default, the maximum job number is set to 128, and you can modify this number in `krill/kernel.h`.

Four basic graph algorithms including BFS, BellmanFord (SSSP), PageRank (PR), and Connect Components (CC) are provided in the `apps` folder.
We also provide simple combinations of them, shown below

| Job set | Composition |
| :---: | :---: |
| `Heter` | {BFS, CC, PR, SSSP} * 2 |
| `Homo1` | {BFS, CC} * 4 |
| `Homo2` | {PR, SSSP} * 4 |
| `M-BFS` | {BFS} * 8 |
| `M-SSSP` | {SSSP} * 8 |

A detailed template can be found in `apps/template-concurrent.h`.

### Arrange your programs
All the job header files and the kernel container main program should be placed in the `apps` folder.

To make the compiler recognize your programs, you should modify the `Makefile`.

Please append your job header file in `KERNEL` variable, and the main program should be added in the `ALL` target.

## Compilation

After organizing your jobs and modifying the makefile, you can compile the program and run for it!

Just type `make` or `make -j` for compilation in the `apps` folder. (If you need to make property fusion for multiple jobs, you need to add `LAYOUT=2` after the `make` command.)

Python 3 and C++ Compilers are needed. For C++, we suppot
* g++ >= 5.3.0 with support of [Cilk Plus](https://www.cilkplus.org/) or [OpenMP](https://www.openmp.org/)
* Intel [ICPC](https://software.intel.com/en-us/c-compilers) compiler >= 18.0.0
	* Note: Since Intel ICPC has been integrated into [oneAPI](https://software.intel.com/content/www/us/en/develop/tools/oneapi.html#gs.z7812c), the compiler is not thoroughly tested. Thus, using g++ with Cilk Plus is the most efficient way now.

Since template metaprogramming and some C++ 11 features are used in our system, the compiler needs to support the C++ 11 standard.

The default setting is to compile with g++ using Cilk Plus, you should not define `ONEAPI_ROOT` and `OPENMP` in the environment. To compile with Intel oneAPI compiler, define the environment variable `ONEAPI_ROOT`. To compile with OpenMP, define the environment variable `OPENMP` and make sure `ONEAPI_ROOT`is not defined. To output the debugging message, define `DEBUG` variable.

(For reference, our experiments use g++ 7.5.0 with Cilk Plus.)

## Execution

To execute the compiled program, you can run the following commands (suppose the program named `concurrent`):

```bash
$ ./concurrent -w ../inputs/rMatGraph_WJ_5_100
```

The command line arguments used in our system include:
* `-w`: if the input graph is weighted
* `-s`: if the input graph is symmetric
* `-b`: if the input graph is stored in binary
* `-rounds`: specify the number of rounds the program to run

## Experiments
### Datasets

The datasets used in our experiments can be found in the following links.

| Abbr. | Dataset | # of vertices | # of edges | source | Original format |
| :---: | :---:   | :---:         | :---:      | :---:  | :---: |
| CP | cit-Patents | 6.0 M         | 16.5M      | http://snap.stanford.edu/data/cit-Patents.html | SNAP |
| LJ | LiveJournal | 4.8 M         | 69 M       | http://snap.stanford.edu/data/soc-LiveJournal1.html | SNAP |
| RMAT | rMat24      | 33.6 M        | 168 M      | https://graph500.org/ | - |
| TW | Twitter     | 41.7 M        | 1.4 B      | https://sparse.tamu.edu/SNAP/twitter7 | MTX |
| FT | Friendster  | 124 M         | 1.8 B      | http://snap.stanford.edu/data/com-Friendster.html | SNAP |

Notice the graph data needs to be transformed into the format of **[Problem Based Benchmark Suite](http://www.cs.cmu.edu/~pbbs/benchmarks/graphIO.html)**. The facilities in `utils` like `SNAPtoAdj`, `MTXtoAdj`, and `adjGraphAddWeights` can be used for format transformation.

Similarly, you need to type `make` in the `utils` folder to compile the facilities first.

### Dataset generation
We provide several useful commands in `experiments/Makefile` enabling you to generate the datasets.

To generate the required datasets for Krill and Ligra, please follow the instructions below.

```bash
mkdir Dataset
# suppose "Dataset" and the "Krill" repo are in the same folder
cd Dataset

# CP
wget http://snap.stanford.edu/data/cit-Patents.txt.gz
gunzip cit-Patents.txt.gz
./../Krill/utils/SNAPtoAdj cit-Patents.txt cit-Patents

# LJ
wget http://snap.stanford.edu/data/soc-LiveJournal1.txt.gz
gunzip soc-LiveJournal1.txt.gz
./../Krill/utils/SNAPtoAdj soc-LiveJournal1.txt soc-LiveJournal1

# RM
./../Krill/utils/rMatGraph -a .5 -b .1 -c .1 -d .3 16800000 rMatGraph24

# TW
wget https://suitesparse-collection-website.herokuapp.com/MM/SNAP/twitter7.tar.gz
tar -xzvf twitter7.tar.gz
./../Krill/utils/MTXtoAdj soc-LiveJournal1.txt twitter7

# FT
wget http://snap.stanford.edu/data/bigdata/communities/com-friendster.ungraph.txt.gz
gunzip com-friendster.ungraph.txt.gz
./../Krill/utils/SNAPtoAdj com-friendster.ungraph.txt com-friendster

# add weights for the unweighted graph
cd ../Krill/experiments
make add_weights CP=1
make add_weights LJ=1
make add_weights RMAT=1
make add_weights TW=1
make add_weights FT=1
```

To generate the datasets required for GraphM, please run preprocessing as shown below. It may take hours for large datasets.

```bash
# run preprocess (generate binary files, transform to grid format, and relabel) for GraphM
# both unweighted and weighted graphs should be generated first
make run_preprocess LJ=1
```


### Execution
To reproduce the experiments in our paper, you should make sure
1. [Ligra](https://github.com/jshun/ligra) & [GraphM](https://github.com/chhzh123/GraphM) has been compiled in another folder at first.
2. Python 3 is installed in your system, which is needed for bash script writing and result extraction.
3. The datasets are downloaded and preprocessed for GraphM.
4. The environment variables are properly defined, including `LIGRA_PATH`, `GRAPHM_PATH` and `DATASET_PATH`.

Then follow the guidance below:

```bash
# in your code repository
cd apps
make LAYOUT=2 -j # for property fusion

cd ../experiments
# run all the experiments for all the datasets
chmod +x run.sh
./run.sh

# this will run all the experiments for LiveJournal (LJ)
# heter, homo1, homo2, mbfs, msssp
make exp LJ=1

# only run for a single job, say PageRank
make pr LJ=1
# only run for heter
make heter LJ=1

# scalability of # of jobs
make multibfs LJ=1
# scalability of # of cores
make multicore LJ=1

# clean experimental results
make clean
```

The raw profiling results can be found in the `profile` folder. If the programs run faultlessly, you will see the `.prof` results to be generated.

Most of the experimental results in the paper can be retrived from the `.prof` file. The data source of each figure is listed below:
* Figure 8: "Real time / Wall clock time (s)". For Krill without property buffers, you need to clean the binary, recompile using `make -j`, and run the experiments again. For GraphM, preprocessing time is also needed to be counted.
* Figure 9: Need to set up the `DEBUG` compilation flag.
* Figure 10: We use "L1 load" as the indicator of the number of memory accesses, which should be the number of load instructions issued by CPUs.
* Figure 11: "LLC miss rate".
* Figure 12: "LLC miss count".
* Figure 13: Take out the execution time of each job from the log and calculate the response latency.
* Figure 14: Use the `dynamic_batching` branch and run `./experiments/dynamic_batching.py`.
* Figure 15: For the distributed version of Krill (which is based on [Gemini](https://github.com/thu-pacman/GeminiGraph)), please refer to [this repository](https://github.com/chhzh123/GeminiGraph).
* Figure 16: Run `make multibfs` and `make multicore`.