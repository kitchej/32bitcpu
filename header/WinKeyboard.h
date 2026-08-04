#ifndef WINKEYBOARD_H
#define WINKEYBOARD_H
#include "../header/BasicIODevice.h"
#include <cstdint>

class WinKeyboard : BasicIODevice {
	bool readyBitAddr;
	uint32_t charRegAddr;

public:
	WinKeyboard();
	WinKeyboard(MMU* mmu, uint32_t addrRangeStart, uint32_t addrRangeEnd);
	void write(uint32_t value) override;
	bool isReady() override;
	void mainloop();
};

#endif