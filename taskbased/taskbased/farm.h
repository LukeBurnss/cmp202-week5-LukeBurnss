#ifndef FARM_H
#define FARM_H

#include "task.h"

// FIXME - You will need to add #includes here (probably <mutex> at least)
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

/** A collection of tasks that should be performed in parallel. */
class Farm {
public:
	// DO NOT CHANGE the public interface of this class.
	// You only need to implement the existing methods.

	/** Add a task to the farm.
	    The task will be deleted once it has been run. */
	void add_task(Task *task);

	/** Run all the tasks in the farm.
	    This method only returns once all the tasks in the farm
		have been completed. */
	void run();

private:
	// FIXME - You will need to add private member variables here
	std::queue<Task*> taskQueue;    // Queue of tasks to be executed
	std::mutex queueMutex;          // Mutex for protecting access to the task queue
	std::vector<std::thread> workers; // Vector to hold worker threads
};

#endif
