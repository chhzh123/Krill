// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#include "kernel.h" // base job class
using namespace std;

/**
 * Write your own job class below,
 * which should be inherent from two base job classes,
 * i.e. `UnweightedJob` or `WeightedJob`.

 * For weighted jobs, you should comment the previous line
 * and uncomment the following line.
 */
class MyJob: public UnweightedJob
// class MyJob: public WeightedJob
{
public:
	/**
	 * The constructor of this job.
	 * Notice the first argument of the job should be `_n`,
	 * i.e. the number of vertices of the input graph.
	 * Parent constructor should be called at first.
	 * Other initialization facility can be defined by yourself.
	 * Also, for weighted jobs, you should delete the `UnweightedJob`
	 * and uncomment the `WeightedJob` line.
	 */
	MyJob(long _n):
		UnweightedJob(_n){};
		// WeightedJob(_n){};

	/**
	 * Input:
	 ** d: the index of the destination vertex
	 * This function checks if the destination vertex satisfies your condition.
	 */
	inline bool cond(uintE d){
		// write your condition function here
	}

	/**
	 * Input:
	 ** s: the index of the source vertex
	 ** d: the index of the destination vertex
	 *
	 * This function checks if the src-dst vertex-pair should be updated.
	 * If true, update the properties value of them, and append the destination
	 * vertex into next frontier.
	 * If false, nothing will be done to this vertex-pair.
	 */
	inline bool update(uintE s, uintE d){
		// write your update function here
	}

	/**
	 * Input:
	 ** s: the index of the source vertex
	 ** d: the index of the destination vertex
	 *
	 * This function is the atomic version of `update`.
	 * The basic facility of some atomic operations including `CAS`
	 * and `writeMin` can be found in `krill/util.h`.
	 */
	inline bool updateAtomic(uintE s, uintE d){
		// write your atomic update function here
	}

	/**
	 * This function justifies whether your job is finished and also should be overridden.
	 * The default implementation returns whether the frontier set is empty.
	 */
	inline bool finished(int){
		// you can change the finished condition
		return frontier.isEmpty();
	}

	/**
	 * This method will be functioned before all the iterations.
	 * You can initialize your private property values here.
	 * We provide facilities like
	 *
	 * > void setAll(T*& ptr, T val)
	 *
	 * which can be used to set all the values in the `ptr` array to `val`.
	 */
	void initialize(){
		// write your initialize function here
	}

	/**
	 * If you create instances of some member data using dynamic allocation,
	 * you should free the memory here. Otherwise, directly return.
	 * Notice you only need to free the private property values you created,
	 * the frontier and other basic infrastructures need not be
	 * managed manually.
	 * We provide facilities like
	 *
	 * > void freeMem(T*& ptr)
	 *
	 * to quickly free the allocated memory.
	 */
	void clear(){
		// write your clear function here
	}

	/**
	 * `iniOneIter` is a virtual function but is not a pure virtual function,
	 * meaning you DO NOT NECESSARILY need to specify it.
	 * But for some specific jobs, operations like `vertexMap`
	 * need to be done before the core computation part,
	 * thus this kind of operations can be implemented in this function.
	 *
	 * You can overload the function by uncommenting the following part
	 * and adding the operations you want to implement.
	 */
	// void iniOneIter(){
	//  // WARNING: DO NOT DELETE THE BLOCK BELOW
	//  //          UNLESS YOU KNOW WHAT YOU ARE DOING
	//  // ***** DO NOT DELETE BEGIN *****
	// 	nextFrontier = newA(bool,n); // DO NOT FREE nextFrontier
	// 	setAll<bool>(nextFrontier,0);
	// 	frontier.toDense();
	//  // ***** DO NOT DELELE END *****
	// }

	/**
	 * Similar to `iniOneIter`, `finishOneIter` is also a virtual function
	 * used for some specific jobs,
	 * and you DO NOT NECESSARILY need to specify it.
	 *
	 * You can overload the function by uncommenting the following part
	 * and adding the operations you want to implement.
	 */
	// void finishOneIter(){
	//  // WARNING: DO NOT DELETE THE BLOCK BELOW
	//  //          UNLESS YOU KNOW WHAT YOU ARE DOING
	//  // ***** DO NOT DELETE BEGIN *****
	// 	frontier.del();
	// 	setFrontier(n,nextFrontier); // set new frontier
	//  // ***** DO NOT DELELE END *****
	// }

private:
	/**
	 * Specify your job property values here
	 */
};