// Task-based parallelism example
// Adam Sampson <a.sampson@abertay.ac.uk>

#include <iostream>
#include <string>

#include "farm.h"
#include "task.h"
#include "messagetask.h"

// Import things we need from the standard library
using std::cout;
using std::to_string;

int main(int argc, char *argv[])
{
	// Example: create and run a single task
	Task *t = new MessageTask("hello, world!");
	cout << "Running one task...\n";
	t->run();
	delete t;

	// Example: run a load of tasks using a farm
	Farm f;
	for (int i = 0; i < 200; ++i)
	{
		f.add_task(new MessageTask("I am task " + to_string(i)));
	}
	cout << "Running task farm...\n";
	f.run();
	cout << "Tasks complete!\n";

	return 0;
}