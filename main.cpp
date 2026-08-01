#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>

#include "header/mmu.h"
#include "header/CPU.h"



int main(int argc, char* argv[]) {
		std::cout << "LC-32 Emulator written by Joshua Kitchen" << std::endl;
		
		std::string file = "";
		if (argc > 1) {
			file = std::string(argv[1]);
		}
		else {
			std::cout << "No file specified. Please provide a file to load." << std::endl;
			return 1;
		}

		std::cout << "Loading file: " << file << std::endl;
		std::ifstream fileStream(file, std::ios::binary | std::ios::ate);


		if (!fileStream.is_open()) {
			std::cout << "failed to open " << file << std::endl;
			return 2;
		}

		std::vector<uint8_t> *bytes = new std::vector<uint8_t>(TOTAL_MEMORY_SIZE, 0); // This will become the memory of the CPU 

		size_t size = fileStream.tellg();
		fileStream.seekg(0);

		if (size > GENERAL_MEMORY_SIZE) {
			std::cout << "File is too large to load into memory. Max size is " << GENERAL_MEMORY_SIZE << " bytes." << std::endl;
			return 3;
		}
		
		fileStream.read(reinterpret_cast<char*>(bytes->data() + GENERAL_MEMORY_OFFSET), size);
		if (fileStream.fail()) {
			std::cout << "Read operation failed!" << std::endl;
			return 4;
		}
		std::cout << "File loaded " << file << std::endl;
		fileStream.close();

		/*
		Load modules here
		*/

		//MMU* mmu = new MMU(bytes);
		//uint8_t testByte = mmu->readByte(GENERAL_MEMORY_OFFSET + 4);
		//uint32_t testWord = mmu->readWord(GENERAL_MEMORY_OFFSET + 4);

		//delete mmu;

		CPU* cpu = new CPU(bytes);
		cpu->mainloop();

		return 0;
}