#include "../header/Terminal.h"
#include <thread>
#include <iostream>

void Terminal::mainloop() {
	while (!shouldStop) {
		while (!isReady() && !shouldStop) {}
		if (!shouldStop) {
			std::cout << (char)mainMmu->readByteIO(charRegAddr, readyRegAddr, false);
		}
	}
}

Terminal::Terminal(): BasicIODevice() {
	charRegAddr = 0;
}

Terminal::Terminal(MMU* mainMmu, uint32_t charRegAddr, uint32_t readyRegAddr) : BasicIODevice(mainMmu, readyRegAddr) {
	this->charRegAddr = charRegAddr;
}

void Terminal::start() {
	listenerThread = std::thread(&Terminal::mainloop, this);
}

void Terminal::stop() {
	shouldStop = true;
	if (listenerThread.joinable()) {
		listenerThread.join();
	}
}

Terminal::~Terminal() {
	stop();
}