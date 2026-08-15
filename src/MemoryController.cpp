#include "../header/memoryController.h"
#include "../header/IODevice.h"

MemoryController::MemoryController() {
	addressSpace = std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0);
	IODeviceMap = std::map<uint32_t, IODevice*>();
}

MemoryController::MemoryController(std::vector<uint8_t> *buffer) {
	addressSpace = *buffer;
	IODeviceMap = std::map<uint32_t, IODevice*>();
}

void MemoryController::registerIODevice(IODevice* device, uint32_t readyRegAddress) {
	std::lock_guard<std::mutex> lock(IODeviceMapLock);
	IODeviceMap[readyRegAddress] = device;
}

void MemoryController::writeByte(uint32_t address, uint32_t value) {
	{
		std::lock_guard<std::mutex> lock(addressSpaceLock);
		addressSpace.at(address) = value & 0xFF;
	}

	auto it = IODeviceMap.find(address);
	if (it == IODeviceMap.end()) { return; };
	IODevice* mappedIO = it->second;
	mappedIO->signal();

}

void MemoryController::writeWord(uint32_t address, uint32_t value) {
	{
		std::lock_guard<std::mutex> lock(addressSpaceLock);
		addressSpace.at(address) = value & 0xFF;
		addressSpace.at(address + 1) = (value >> 8) & 0xFF;
		addressSpace.at(address + 2) = (value >> 16) & 0xFF;
		addressSpace.at(address + 3) = (value >> 24) & 0xFF;
	}

	for (uint32_t offset = 0; offset < 4; offset++) {
		auto it = IODeviceMap.find(address + offset);
		if (it != IODeviceMap.end()) {
			it->second->signal();
		}
	}
}

void MemoryController::setReadyReg(uint32_t address, uint32_t value) {
	// Enables IO to reset its ready register without signialing itself
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	addressSpace.at(address) = value & 0xFF;
}

uint32_t MemoryController::readByte(uint32_t address) {
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	return static_cast<uint32_t>(addressSpace.at(address));
}

uint32_t MemoryController::readWord(uint32_t address) {
	std::lock_guard<std::mutex> lock(addressSpaceLock);
	return  addressSpace.at(address)|
			addressSpace.at(address + 1) << 8|
			addressSpace.at(address + 2) << 16|
			addressSpace.at(address + 3) << 24;
}
