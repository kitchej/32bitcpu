#ifndef TERMINAL_H
#define TERMINAL_H
#include "BasicIODevice.h"
#include <thread>
#include <atomic>

class Terminal : BasicIODevice {
	uint32_t charRegAddr;
	std::thread listenerThread;
	std::atomic<bool> shouldStop{false};

	void mainloop();

public:



};

#endif