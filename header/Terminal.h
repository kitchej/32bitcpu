#ifndef TERMINAL_H
#define TERMINAL_H
#include "BasicIODevice.h"
#include "mmu.h"
#include <thread>
#include <atomic>

class Terminal : public BasicIODevice {
	uint32_t charRegAddr;
	std::thread listenerThread;
	std::atomic<bool> shouldStop{false};

	void mainloop();

public:
	Terminal();
	Terminal(MMU *mainMmu, uint32_t charRegAddr, uint32_t readyRegAddr);
	void start();
	void stop();
	~Terminal();


};

#endif