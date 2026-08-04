#ifndef BASICIODEVICE_H
#define BASICIODEVICE_H
#include <cstdint>
#include "mmu.h"
#include <map>

class BasicIODevice {
	// An abstract base class for implementing memory-mapped IO devices.
protected:
	MMU* mmu;
	uint32_t addrRangeStart, addrRangeEnd;

public:
	BasicIODevice();
	BasicIODevice(MMU* mmu, uint32_t addrRangeStart, uint32_t addrRangeEnd);
	virtual void write(uint32_t value);
	virtual bool isReady();
};

#endif