#include "../header/Terminal.h"
#include <thread>
#include <iostream>

Terminal::Terminal() : IODevice(){
	this->charRegAddr = 0;
	mainloopThread = nullptr;
}

Terminal::Terminal(MemoryController* memoryController, uint32_t charRegAddr, uint32_t readyRegAddr): IODevice(memoryController, readyRegAddr){
	this->charRegAddr = charRegAddr;
	mainloopThread = nullptr;
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
	mainloopThread = new std::thread(&Terminal::mainloop, this);
}

void Terminal::stop() {
	std::lock_guard<std::mutex> lock(IOLock);
	char outChar = (char)memoryController->readByte(charRegAddr);
	std::cout << outChar;
	memoryController->setReadyReg(readyRegAddr, 0);
	isRunning = false;
}

Terminal::~Terminal() {
	stop();
	if (mainloopThread->joinable() && (mainloopThread != nullptr)) {
		mainloopThread->join();
	}
}

void Terminal::signal()  {
	std::lock_guard<std::mutex> lock(IOLock);
	ready = true;
	readyRegWritten.notify_one();
}