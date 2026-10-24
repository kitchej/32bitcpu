#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstdio>

#include "header/memoryController.h"
#include "header/CPU.h"
#include "header/Terminal.h"
#include "header/IODevice.h"

constexpr bool debug = true;

constexpr uint32_t TERM_CHAR_REG = 0x4000;
constexpr uint32_t TERM_READY_REG = 0x4001;

void load_program(std::string file, std::vector<uint8_t>* memory) {
	std::cout << "Loading file: " << file << std::endl;
	std::ifstream fileStream(file, std::ios::binary | std::ios::ate);


	if (!fileStream.is_open()) {
		std::cout << "failed to open " << file << std::endl;
		exit(2);
	}

	size_t size = fileStream.tellg();
	fileStream.seekg(0);

	if (size > GENERAL_MEMORY_SIZE) {
		std::cout << "File is too large to load into memory. Max size is " << GENERAL_MEMORY_SIZE << " bytes." << std::endl;
		exit(3);
	}

	fileStream.read(reinterpret_cast<char*>(memory->data() + GENERAL_MEMORY_OFFSET), size);
	if (fileStream.fail()) {
		std::cout << "Read operation failed!" << std::endl;
		exit(4);
	}
	std::cout << "File loaded " << std::endl;
	fileStream.close();
}

void load_kernal(std::string kernal_file, std::vector<uint8_t>* memory) {
	std::cout << "Loading kernal file: " << kernal_file << std::endl;
	std::ifstream fileStream(kernal_file, std::ios::binary | std::ios::ate);


	if (!fileStream.is_open()) {
		std::cout << "failed to open " << kernal_file << std::endl;
		exit(2);
	}

	size_t size = fileStream.tellg();
	fileStream.seekg(0);

	if (size > KERNAL_MEMORY_SIZE) {
		std::cout << "File is too large to load into kernal ROM. Max size is " << KERNAL_MEMORY_SIZE << " bytes." << std::endl;
		exit(3);
	}

	fileStream.read(reinterpret_cast<char*>(memory->data()), size);
	if (fileStream.fail()) {
		std::cout << "Read operation failed!" << std::endl;
		exit(4);
	}
	std::cout << "Kernal loaded " << std::endl;
	fileStream.close();

}

int main(int argc, char* argv[]) {
		std::cout << "LC-32 Emulator written by Joshua Kitchen" << std::endl;
		std::string kernal_file = "kernal.bin";
		std::string file = "";
		if (argc > 1) {
			file = std::string(argv[1]);
		}
		else {
			std::cout << "No file specified. Please provide a file to load." << std::endl;
			return 1;
		}
		std::vector<uint8_t>* memory = new std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0); // This will become the memory of the CPU 
		load_program(file, memory);
		load_kernal(kernal_file, memory);

		/*
		Load modules here
		*/



		MemoryController *memoryController = new MemoryController(memory, KERNAL_MEMORY_OFFSET, KERNAL_MEMORY_END);
		CPU *cpu = new CPU(memoryController);
		Terminal* terminal = new Terminal(memoryController, TERM_CHAR_REG, TERM_READY_REG);
		memoryController->registerIODevice((IODevice*)terminal, TERM_READY_REG);

		
		if (debug) {

		}
		else {
			terminal->start();
			cpu->mainloop();
			terminal->stop();
			std::cout << "System halted, press any key to exit" << std::endl;
			std::getchar();
		}
		
		return 0;
}