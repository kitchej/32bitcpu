#ifndef TERMINAL_H
#define TERMINAL_H
#include "memoryController.h"
#include "IODevice.h"
#include <thread>
#include <optional>

class Terminal: IODevice {
	uint32_t charRegAddr;
	bool isRunning;
	std::thread *mainloopThread;

	void mainloop();
public:
	Terminal();
	Terminal(MemoryController* memoryController, uint32_t charRegAddr, uint32_t readyRegAddr);
	~Terminal();
	void start();
	void stop();
	void signal() override;
};

#endif