#include "../header/mmu.h"
#include <thread>

MMU::MMU() {
	memoryBuffer = new std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0);
	IOReadyBitMap = new std::map<uint32_t, uint32_t>();
}

MMU::MMU(std::vector<uint8_t> *buffer) {
	memoryBuffer = buffer;
	IOReadyBitMap = new std::map<uint32_t, uint32_t>();
}

MMU::~MMU() {
	delete memoryBuffer;
	delete IOReadyBitMap;
}

void MMU::registerIODevice(size_t size) {
	for (int i = 0; i < size; i++) {
		try {
			
		}
		catch (std::out_of_range) {
			throw std::
		}
	}
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
	return static_cast<uint32_t>(memoryBuffer->at(address));
}

uint32_t MMU::readWord(uint32_t address) {
	std::lock_guard<std::mutex> lock(memoryLock);
	return  memoryBuffer->at(address)|
			memoryBuffer->at(address + 1) << 8|
			memoryBuffer->at(address + 2) << 16|
			memoryBuffer->at(address + 3) << 24;
}

void MMU::writeByteIO(uint32_t addres, uint32_t value, bool readyValue) {
	std::lock_guard<std::mutex> lock(memoryLock);
	memoryBuffer->at(addres) = value & 0xFF;
	memoryBuffer->at(readyRegAddr) = (uint8_t)readyValue;
}

uint32_t MMU::readByteIO(uint32_t addres, bool readyValue) {
	std::lock_guard<std::mutex> lock(memoryLock);
	memoryBuffer->at(readyRegAddr) = (uint8_t)readyValue;
	return static_cast<uint32_t>(memoryBuffer->at(addres));
}

void MMU::writeWordIO(uint32_t addres, uint32_t value, bool readyValue) {
	std::lock_guard<std::mutex> lock(memoryLock);
	memoryBuffer->at(addres) = value & 0xFF;
	memoryBuffer->at(addres + 1) = (value >> 8) & 0xFF;
	memoryBuffer->at(addres + 2) = (value >> 16) & 0xFF;
	memoryBuffer->at(addres + 3) = (value >> 24) & 0xFF;
	memoryBuffer->at(readyRegAddr) = (uint8_t)readyValue;
}

uint32_t MMU::readWordIO(uint32_t addres, bool readyValue) {
	std::lock_guard<std::mutex> lock(memoryLock);
	memoryBuffer->at(readyRegAddr) = (uint8_t)readyValue;
	return  memoryBuffer->at(addres) |
		memoryBuffer->at(addres + 1) << 8 |
		memoryBuffer->at(addres + 2) << 16 |
		memoryBuffer->at(addres + 3) << 24;
}
