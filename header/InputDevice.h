#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H
#include "BasicIODevice.h"

class InputDevice : BasicIODevice {
	 

public:
	InputDevice();
	void virtual write(uint32_t value);
};

#endif