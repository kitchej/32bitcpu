#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__
#include <vector>
#include "header/LC32_globals.h"
#include "header/CPU.h"

class Pixel {
public:
	char character;
	int x;
	int y;
	Pixel(char character, int x, int y) {
		this->character = character;
		this->x = x;
		this->y = y;
	}
};

typedef std::vector<std::vector<Pixel>> PixelVector2d;

class Debugger {
	CPU* cpu;
	PixelVector2d screen_buffer;
	int rows;
	int cols;
	char background;
	// Functionality for screen buffer
	void init_screen();
	void write();
	void write_line();
	void display();
	void refresh();
	void clear_buffer();

	// Wrappers for cpu functions
	void step();
	void fetch_instruction();
	void reset();
	void poke_reg();
	void peek_reg();
	

public:
	Debugger(CPU* cpu);
	void mainloop();

};


#endif
