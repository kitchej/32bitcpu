#ifndef BASICIODEVICE_H
#define BASICIODEVICE_H
#include <cstdint>
#include "memoryController.h"

class BasicIODevice {
	// A base class for implementing memory-mapped IO devices.
public:
	BasicIODevice();
	virtual void signal(); // Called by the MemoryController to notify that IO device has been written/read to

};

#endif