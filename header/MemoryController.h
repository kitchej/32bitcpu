#ifndef memoryController_H
#define memoryController_H

#include <vector>
#include <cstdint>
#include <mutex>
#include <map>

#include "IODevice.h"

#define TOTAL_MEMORY_SIZE 268435456 // ~256 MB
#define GENERAL_MEMORY_SIZE 268353536  // ~256 MB
#define KERNAL_MEMORY_SIZE 16384  // ~16 KB
#define IO_MEMORY_SIZE 65536 // ~64 KB
#define KERNAL_MEMORY_OFFSET 0x00000000
#define IO_MEMORY_OFFSET 0x4000
#define GENERAL_MEMORY_OFFSET 0x14000

class MemoryController {
	
	std::vector<uint8_t> addressSpace; // IO, kernel, and main memory all share this container
	std::map<std::uint32_t, IODevice*> IODeviceMap;
	std::mutex addressSpaceLock;
	std::mutex IODeviceMapLock;


public:
	MemoryController();
	MemoryController(std::vector<uint8_t>* buffer);
	void registerIODevice(IODevice* device, uint32_t readyRegAddress);
	void writeByte(uint32_t address, uint32_t value);
	void writeWord(uint32_t address, uint32_t value);
	void setReadyReg(uint32_t address, bool value);
	uint32_t readByte(uint32_t address);
	uint32_t readWord(uint32_t address);
	

};


#endif
