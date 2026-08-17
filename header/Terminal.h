#ifndef TERMINAL_H
#define TERMINAL_H
#include "memoryController.h"
#include "IODevice.h"

class Terminal: IODevice {
	MemoryController* memoryController;
	uint32_t readyRegAddr;
	uint32_t charRegAddr;
	bool isRunning;
	
	void mainloop();
public:
	Terminal();
	Terminal(MemoryController* memoryController, uint32_t charRegAddr, uint32_t readyRegAddr);
	void start();
	void stop();
	void signal() override;


};

#endif