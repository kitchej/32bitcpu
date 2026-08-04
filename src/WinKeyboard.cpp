#include "../header/WinKeyboard.h"

WinKeyboard::WinKeyboard() : BasicIODevice() {
	readyBitAddr = 0;
	charRegAddr = 0;
}

WinKeyboard::WinKeyboard(MMU* mmu, uint32_t readyBitAddr, uint32_t charRegAddr): BasicIODevice(mmu, readyBitAddr, charRegAddr) {
	readyBitAddr = readyBitAddr;
	charRegAddr = 0;
}


void WinKeyboard::write(uint32_t value) {
	mmu->writeByte(charRegAddr, value);
	mmu->writeByte(readyBitAddr, 1);
}

bool WinKeyboard::isReady() {
	return mmu->readByte(readyBitAddr);
}