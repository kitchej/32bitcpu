#include "../header/Terminal.h"
#include <thread>
#include <iostream>

Terminal::Terminal() : IODevice(){
	this->charRegAddr = 0;
}

Terminal::Terminal(MemoryController* memoryController, uint32_t charRegAddr, uint32_t readyRegAddr): IODevice(memoryController, readyRegAddr){
	this->charRegAddr = charRegAddr;
}

void Terminal::mainloop() {
	while (true) {
		std::unique_lock<std::mutex> lock(IOLock);
		readyRegWritten.wait(lock, [this] { return ready || !isRunning; });
		if (!isRunning) {
			return;
		}

		char outChar = (char)memoryController->readByte(charRegAddr);
		std::cout << outChar;
		memoryController->setReadyReg(readyRegAddr, 0);
		ready = false; 
	}
}

void Terminal::start(){
	isRunning = true;
	std::thread listener(&Terminal::mainloop, this);
	listener.detach();
}

void Terminal::stop() {
	std::lock_guard<std::mutex> lock(IOLock);
	isRunning = false;
}

void Terminal::signal()  {
	std::lock_guard<std::mutex> lock(IOLock);
	ready = true;
	readyRegWritten.notify_one();
}