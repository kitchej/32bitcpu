#include "../header/memoryController.h"
#include <thread>

MemoryController::MemoryController() {
	addressSpace = new std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0);
	IODeviceMap = new std::map<uint32_t, uint32_t>();
}

MemoryController::MemoryController(std::vector<uint8_t> *buffer) {
	addressSpace = buffer;
	IODeviceMap = new std::map<uint32_t, uint32_t>();
}

MemoryController::~MemoryController() {
	delete addressSpace;
	delete IODeviceMap;
}

void MemoryController::writeByte(uint32_t address, uint32_t value) {
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	addressSpace->at(address) = value & 0xFF;
}

void MemoryController::writeWord(uint32_t address, uint32_t value) {
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	addressSpace->at(address) = value & 0xFF;
	addressSpace->at(address + 1) = (value >> 8) & 0xFF;
	addressSpace->at(address  + 2) = (value >> 16) & 0xFF;
	addressSpace->at(address + 3) = (value >> 24) & 0xFF;
}

uint32_t MemoryController::readByte(uint32_t address) {
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	return static_cast<uint32_t>(addressSpace->at(address));
}

uint32_t MemoryController::readWord(uint32_t address) {
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	return  addressSpace->at(address)|
			addressSpace->at(address + 1) << 8|
			addressSpace->at(address + 2) << 16|
			addressSpace->at(address + 3) << 24;
}
