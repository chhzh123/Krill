Krill: An Efficient Concurrent Graph Processing System
======================

Krill is an efficient graph system for processing **concurrent graph jobs**, which equips with **graph kernel fusion** and greatly reduces the number of memory accesses.

Currently, we select [Ligra](https://github.com/jshun/ligra), a state-of-the-art shared-memory single graph processing framework, as our underlying infrastructure.

## Getting Started

To write a program to process concurrent graph jobs, you should follow the three steps below.

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

The first three functions are the same as Ligra, and the latter three are used for concurrent graph processing, since we decouple the processing logics.
All of them are pure virtual functions, which means compile error will occur if the functions are not be specified.

A detailed template can be found in `apps/template-singleton.h`.

### Integrate them into a kernel
After several graph jobs are implemented, you can place them into a container.

In the main program, you should provide the implementation of `setKernels`, where you create instances of your jobs and append them into a kernel container via `appendJob` function. For example:

```cpp
template <class vertex>
void setKernels(graph<vertex>&G, Kernels& K, commandLine P)
{
	MyJob* myjob = new MyJob(G.n);
	K.appendJob(myjob);
}
```

By default, the maximum job number is set to 128, and you can modify this number in `krill/kernel.h`.

Four basic graph algorithms including BFS, BellmanFord (SSSP), PageRank (PR), and Connect Components (CC) are provided in the `apps` folder.
We also provide simple combinations of them, shown below

| Job set | Combination |
| :---: | :---: |
| `Homo1` | {BFS, CC} * 4 |
| `Homo2` | {PR, SSSP} * 4 |
| `Heter` | {BFS, CC, PR, SSSP} * 2 |
| `M-BFS` | {BFS} * 8 |
| `M-SSSP` | {SSSP} * 8 |

A detailed template can be found in `apps/template-concurrent.h`.

### Arrange your programs
All the job header files and the kernel container main program should be placed in the `apps` folder.

To make the compiler recognize your programs, you should modify the `Makefile`.

Please append your job header file in `KERNEL` variable, and the main program should be added in the `ALL` target.

## Compilation

After organizing your jobs and modifying the makefile, you can compile the program and run for it!

Just type `make` or `make -j` for compilation in the `apps` folder.

Compilers
* Intel [icpc](https://software.intel.com/en-us/c-compilers) compiler >= 18.0.0
* g++ >= 5.3.0 with support of [Cilk Plus](https://www.cilkplus.org/) or [OpenMP](https://www.openmp.org/)

Since template metaprogramming and some C++ 11 features are used in our system, the compiler needs to support the C++ 11 standard.

To compile with g++ using Cilk Plus, define the environment variable `CILK`. To compile with icpc, define the environment variable `MKLROOT` and make sure CILK is not defined. To compile with OpenMP, define the environment variable `OPENMP` and make sure CILK and MKLROOT are not defined. To output the debugging message, define `DEBUG` variable.

Notice the system has not been thoroughly tested in other compiler settings except for Intel icpc. Please commit a issue if some bugs you have found.

## Execution

To execute the compiled program, you can run the following commands (support the program named `concurrent`):

```bash
$ ./concurrent -w ../inputs/rMatGraph_WJ_5_100
```

The command line arguments used in our system include:
* `-w`: if the input graph is weighted
* `-s`: if the input graph is symmetric
* `-b`: if the input graph is stored in binary
* `-rounds`: specify the number of rounds the program to run

## Datasets

The datasets used in our experiments can be found in the following links.

| Abbr. | Dataset | # of vertices | # of edges | source |
| :---: | :---:   | :---:         | :---:      | :---:  |
| CP | cit-Patents | 6.0 M         | 16.5M      | http://snap.stanford.edu/data/cit-Patents.html |
| LJ | LiveJournal | 4.8 M         | 69 M       | http://snap.stanford.edu/data/soc-LiveJournal1.html |
| RD | USAroad     | 24 M          | 58 M       | https://sparse.tamu.edu/DIMACS10/road_usa |
| RM | rMat24      | 33.6 M        | 168 M      | https://graph500.org/ |
| TW | Twitter     | 41.7 M        | 1.4 B      | https://sparse.tamu.edu/SNAP/twitter7 |
| FT | Friendster  | 124 M         | 1.8 B      | http://snap.stanford.edu/data/com-Friendster.html |

Notice the graph data needs to be transformed into the format of [Problem Based Benchmark Suite](http://www.cs.cmu.edu/~pbbs/benchmarks/graphIO.html). The facilities in `utils` like `SNAPtoAdj`, `MTXtoAdj`, and `adjGraphAddWeights` can be used for format transformation.

Similarly, you need to type `make` in the `utils` folder to compile the facilities first.

## Experiments

To reproduce the experiments in our paper, you should make sure
1. Ligra has been compiled in another folder at first.
2. Python 3 is installed in your system, which is needed for bash script writing and result extraction.
3. The datasets are downloaded.
4. The environment variables are properly defined, including `LIGRA_PATH` and `DATASET_PATH`.

Then follow the guidance below:

```bash
$ # in your code repository
$ cd experiments
$ # this will run all the experiments for LiveJournal (LJ)
$ make exp LJ=1
$ # only run for single job, say PageRank
$ make pr LJ=1
$ # only run for heter
$ make heter LJ=1
$ # processor counter monitor
$ make pcm LJ=1
$ # scalability of # of jobs
$ make multibfs LJ=1
$ # scalability of # of cores
$ make multicore LJ=1
$ # clean experimental results
$ make clean
```
