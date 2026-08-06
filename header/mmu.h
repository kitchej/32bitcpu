#ifndef MMU_H
#define MMU_H

#include <vector>
#include <cstdint>
#include <mutex>
#include <map>

#define TOTAL_MEMORY_SIZE 268435456 // ~256 MB
#define GENERAL_MEMORY_SIZE 268353536  // ~256 MB
#define KERNAL_MEMORY_SIZE 16384  // ~16 KB
#define IO_MEMORY_SIZE 65536 // ~64 KB
#define KERNAL_MEMORY_OFFSET 0x00000000
#define IO_MEMORY_OFFSET 0x00004000
#define GENERAL_MEMORY_OFFSET 0x00014000

class MMU {
	
	std::vector<uint8_t>* memoryBuffer;
	std::mutex memoryLock;
	std::map<std::uint32_t, uint32_t> *IOReadyBitMap;



public:
	MMU();
	MMU(std::vector<uint8_t>* buffer);
	~MMU();
	void registerIODevice(std::vector<uint32_t> addresses, uint32_t readyBitAddr);
	void writeByte(uint32_t address, uint32_t value);
	void writeWord(uint32_t address, uint32_t value);
	uint32_t readByte(uint32_t address);
	uint32_t readWord(uint32_t address);
	void writeByteIO(uint32_t address, uint32_t value, bool readyValue);
	uint32_t readByteIO(uint32_t address, bool readyValue);
	void writeWordIO(uint32_t address, uint32_t value, bool readyValue);
	uint32_t readWordIO(uint32_t address, bool readyValue);

};


#endif
