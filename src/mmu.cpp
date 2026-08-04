#include "../header/mmu.h"
#include <thread>

MMU::MMU() {
	memoryBuffer = new std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0);
}

MMU::MMU(std::vector<uint8_t> *buffer) {
	memoryBuffer = buffer;
}

MMU::~MMU() {
	delete memoryBuffer;
}

void MMU::writeByte(uint32_t address, uint32_t value) {
	std::lock_guard<std::mutex> lock(memoryLock);
	memoryBuffer->at(address) = value & 0xFF;
}

void MMU::writeWord(uint32_t address, uint32_t value) {
	std::lock_guard<std::mutex> lock(memoryLock);
	memoryBuffer->at(address) = value & 0xFF;
	memoryBuffer->at(address + 1) = (value >> 8) & 0xFF;
	memoryBuffer->at(address  + 2) = (value >> 16) & 0xFF;
	memoryBuffer->at(address + 3) = (value >> 24) & 0xFF;
}

uint32_t MMU::readByte(uint32_t address) {
	std::lock_guard<std::mutex> lock(memoryLock);
	return static_cast<uint8_t>(memoryBuffer->at(address));
}

uint32_t MMU::readWord(uint32_t address) {
	std::lock_guard<std::mutex> lock(memoryLock);
	return  memoryBuffer->at(address)|
			memoryBuffer->at(address + 1) << 8|
			memoryBuffer->at(address + 2) << 16|
			memoryBuffer->at(address + 3) << 24;
}



