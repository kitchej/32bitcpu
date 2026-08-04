#include <cstdint>
#include "../header/mmu.h"
#include "../header/BasicIODevice.h"
#include <thread>
#include <mutex>

BasicIODevice::BasicIODevice() {
	mmu = nullptr;
	addrRangeStart = 0;
	addrRangeEnd = 0;
}

BasicIODevice::BasicIODevice(MMU* mmu, uint32_t addrRangeStart, uint32_t addrRangeEnd) {
	this->mmu = mmu;
	this->addrRangeStart = 0;
	this->addrRangeEnd = 0;
}