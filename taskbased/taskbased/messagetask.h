#ifndef MESSAGETASK_H
#define MESSAGETASK_H

#include <string>

#include "task.h"

/** Task that prints a message to cout. */
class MessageTask : public Task
{
public:
	MessageTask(const std::string& message)
		: message_(message)
	{
	}

	void run();

private:
	const std::string message_;
};

#endif