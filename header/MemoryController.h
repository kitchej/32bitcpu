#ifndef memoryController_H
#define memoryController_H

#include <vector>
#include <cstdint>
#include <mutex>
#include <map>

class IODevice;

class MemoryController {
	
	std::vector<uint8_t> *addressSpace; // IO, kernel, and main memory all share this container
	std::map<std::uint32_t, IODevice*> IODeviceMap;
	std::mutex addressSpaceLock;
	std::mutex IODeviceMapLock;
	uint32_t romStart;
	uint32_t romEnd;
public:
	MemoryController();
	MemoryController(std::vector<uint8_t>* buffer, uint32_t romStart, uint32_t romEnd);
	~MemoryController();
	void registerIODevice(IODevice* device, uint32_t readyRegAddress);
	void writeByte(uint32_t address, uint32_t value);
	void writeWord(uint32_t address, uint32_t value);
	void setReadyReg(uint32_t address, uint32_t value);
	uint32_t readByte(uint32_t address);
	uint32_t readWord(uint32_t address);
	

};


#endif
