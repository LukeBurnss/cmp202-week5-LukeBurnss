#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include "MandelbrotTask.h" // Include the task definition
#include "mandelbrot.h"

class Farm {
public:
	void add_task(const MandelbrotTask& task);
	void run();
private:
	std::queue<MandelbrotTask> taskQueue;
	std::mutex queueMutex;
};