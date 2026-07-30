#ifndef MMU_H
#define MMU_H

#include <vector>
#include <cstdint>
#define TOTAL_MEMORY_SIZE 268435456 // ~256 MB
#define GENERAL_MEMORY_SIZE 268414976  // ~256 MB
#define KERNAL_MEMORY_SIZE 4096 // ~4 KB
#define IO_MEMORY_SIZE 16384 // ~16 KB
#define KERNAL_MEMORY_OFFSET 0x00000000
#define IO_MEMORY_OFFSET 0x00001000
#define GENERAL_MEMORY_OFFSET 0x00005000

class MMU {
	
	std::vector<uint8_t>* memoryBuffer;


public:
	MMU();
	MMU(std::vector<uint8_t>* buffer);
	~MMU();
	void writeByte(uint32_t address, uint32_t value);
	void writeWord(uint32_t address, uint32_t value);
	uint32_t readByte(uint32_t address);
	uint32_t readWord(uint32_t address);

};


#endif
