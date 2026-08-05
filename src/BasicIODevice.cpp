#include <cstdint>
#include "../header/mmu.h"
#include "../header/BasicIODevice.h"

BasicIODevice::BasicIODevice() {
	mainMmu = nullptr;
	readyRegAddr = 0;
}

BasicIODevice::BasicIODevice(MMU* mainMmu, uint32_t readyRegAddr) {
	this->mainMmu = mainMmu;
	this->readyRegAddr = readyRegAddr;
}

bool BasicIODevice::isReady() {
	return (bool)mainMmu->readByte(readyRegAddr);
}