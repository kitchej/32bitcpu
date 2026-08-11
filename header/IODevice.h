#ifndef IODEVICE_H
#define IODEVICE_H
#include <condition_variable>
class MemoryController;

class IODevice{
protected:
	std::condition_variable readyRegWritten;
	std::mutex IOLock;
	uint32_t readyRegAddr;
	bool ready;
	MemoryController* memoryController;
public:
	IODevice();
	IODevice(MemoryController* memoryController, uint32_t readyRegAddr);
	virtual void signal();

};

#endif
