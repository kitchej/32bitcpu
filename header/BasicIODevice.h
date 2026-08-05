#ifndef BASICIODEVICE_H
#define BASICIODEVICE_H
#include <cstdint>
#include "mmu.h"

class BasicIODevice {
	// A base class for implementing memory-mapped IO devices.
protected:
	MMU* mainMmu;
	uint32_t readyRegAddr;

public:
	BasicIODevice();
	BasicIODevice(MMU* mmu, uint32_t readyRegAddr);
	bool isReady();
};

#endif