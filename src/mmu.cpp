#include "../header/mmu.h"
#include <stdexcept>

MMU::MMU() {
	memoryBuffer = new std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0);
}


MMU::MMU(std::vector<uint8_t> *buffer) {
	memoryBuffer = buffer;
}

MMU::~MMU() {
	delete memoryBuffer;
}

void MMU::writeByte(uint32_t address, uint8_t value) {
	memoryBuffer->at(address) = value;
}

void MMU::writeWord(uint32_t address, uint32_t value) {
	memoryBuffer->at(address) = value & 0xFF;
	memoryBuffer->at(address + 1) = (value >> 8) & 0xFF;
	memoryBuffer->at(address  + 2) = (value >> 16) & 0xFF;
	memoryBuffer->at(address + 3) = (value >> 24) & 0xFF;
}

uint8_t MMU::readByte(uint32_t address) {
	return memoryBuffer->at(address);
}

uint32_t MMU::readWord(uint32_t address) {
	return  (static_cast<uint8_t>(memoryBuffer->at(address)))|
			(static_cast<uint8_t>(memoryBuffer->at(address + 1)) << 8)|
			(static_cast<uint8_t>(memoryBuffer->at(address + 2)) << 16)|
			(static_cast<uint8_t>(memoryBuffer->at(address + 3)) << 24);
}



