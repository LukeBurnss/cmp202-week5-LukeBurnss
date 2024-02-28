#include "farm.h"

// Adds a task to the farm's task queue
void Farm::add_task(Task* task) {
    std::lock_guard<std::mutex> guard(queueMutex); // Locks mutex, unlocks when out of scope
    taskQueue.push(task);
}

// Runs all tasks in the farm using parallel worker threads
void Farm::run() {
    int nCPUs = std::thread::hardware_concurrency(); // Number of concurrent threads supported
    for (int i = 0; i < nCPUs; i++) {
        // Create and start worker threads
        workers.emplace_back([this]() {
            while (true) {
                Task* task = nullptr;

                {
                    std::lock_guard<std::mutex> guard(queueMutex); // Automatically locks and unlocks
                    if (taskQueue.empty()) {
                        break; // Exit if no more tasks
                    }
                    task = taskQueue.front(); // Get the next task
                    taskQueue.pop(); // Remove it from the queue
                }

                task->run(); // Execute the task
                delete task; // Delete the task
            }
            });
    }

    // Wait for all worker threads to complete
    for (auto& worker : workers) {
        worker.join();
    }
    workers.clear(); // Clear the vector of threads
}
