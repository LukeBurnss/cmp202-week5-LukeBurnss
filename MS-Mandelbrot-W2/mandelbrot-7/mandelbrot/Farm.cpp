#include "Farm.h"
#include <iostream>
typedef std::chrono::steady_clock the_clock;

// Adds a MandelbrotTask to the task queue in a thread-safe manner
void Farm::add_task(const MandelbrotTask& task) {
	// Lock the mutex to ensure thread-safe access to the queue
	std::lock_guard<std::mutex> lock(queueMutex);
	// Add the task to the queue
	taskQueue.push(task);
}

// Runs all the tasks in the task queue using multiple threads
void Farm::run() {
	// Determine the number of hardware threads available
	int numThreads = std::thread::hardware_concurrency();
	// Create a vector to hold all the worker threads
	std::vector<std::thread> threads(numThreads);

	// Lambda function that each thread will execute
	auto executeTask = [&]() {
		while (true) {
			MandelbrotTask task;
			{
				// Lock the mutex for safe access to the task queue
				std::lock_guard<std::mutex> lock(queueMutex);
				// Check if there are no more tasks, then break from the loop
				if (taskQueue.empty()) break;
				// Get the next task from the queue
				task = taskQueue.front();
				// Remove the task from the queue
				taskQueue.pop();
			}
			// Compute the Mandelbrot set for the given task
			compute_mandelbrot_row(task.left, task.right, task.top, task.bottom, task.row);
		}
		};
	std::cout << "Running parallel Mandelbrot with one thread per row..." << std::endl;
	auto start = the_clock::now();

	// Launch threads to execute tasks
	for (int i = 0; i < numThreads; ++i) {
		threads[i] = std::thread(executeTask);
	}
	// Wait for all threads to complete
	for (auto& thread : threads) {
		thread.join();
	}

	auto end = the_clock::now();
	auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "With " << numThreads << " threads, parallel execution took " << time_taken << " ms." << std::endl;
	write_tga("output_parallel_row_farm.tga");
}