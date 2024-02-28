#include "messagetask.h"

#include <iostream>

using std::cout;

void MessageTask::run() {
	cout << message_ << "\n";
}