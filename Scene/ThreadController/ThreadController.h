#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadController {
private:
	ThreadController();

public:
	~ThreadController();

	ThreadController(ThreadController const&) = delete;
	ThreadController& operator= (ThreadController const&) = delete;

	static ThreadController& Instance() {
		static ThreadController s;
		return s;
	}


};


