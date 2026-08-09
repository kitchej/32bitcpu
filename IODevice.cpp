#include "header/IODevice.h"
#include <thread>

IODevice::IODevice() {
	readyRegAddr = 0;
	ready = false;
	memoryController = nullptr;
}

IODevice::IODevice(MemoryController* memoryController, uint32_t readyRegAddr) {
	this->readyRegAddr = readyRegAddr;
	this->ready = false;
	this->memoryController = memoryController;
}
