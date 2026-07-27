#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <string>
#include <map>
#include "mmu.h"

class CPU;

enum class Opcode : uint8_t {
	NOP = 0,
	ADD,
	ADDI,
	AND,
	ANDI,
	BRN,
	BRP,
	BRZ,
	DIV,
	JAL,
	JMP,
	JR,
	LB,
	LBI,
	LBS,
	LW,
	LWI,
	MULT,
	NOT,
	OR,
	ORI,
	RBS,
	RET,
	SB,
	SBI,
	SUB,
	SUBI,
	SW,
	SWI,
	TRAP,
	XOR,
	XORI,
	HALT
};

typedef struct Instruction {
	Opcode opcode;
	uint8_t dest;
	uint8_t src1;
	uint8_t src2;
	uint32_t imm18;
	uint32_t offset26;
} Instruction;

using OP_FPTR = void (CPU::*)(const Instruction&);

class CPU {
private:
	// Bit masks for field extraction
	static constexpr uint32_t MASK_18BIT = 0x3FFFF;      // 18 bits: 111111111111111111
	static constexpr uint32_t MASK_26BIT = 0x3FFFFFF;    // 26 bits: 11111111111111111111111111
	static constexpr uint8_t MASK_5BIT = 0x1F;
	// Flags
	bool N_FLAG;
	bool P_FLAG;
	bool Z_FLAG;
	// General Purpose Regesters
	uint32_t r0, r1, r2, r3, r4, r5, r6, r7;
	// Special Purpose Regesters
	uint32_t a0, a1, a2;
	uint32_t v0, v1;
	uint32_t sp, sf;
	// Internal Registers
	uint32_t pc, ir, mar, mdr;
	MMU* mmu;

	std::map<Opcode, std::vector<std::string>> instructionFormats;
	std::vector<OP_FPTR> op_table;

	void fetch_instruction();
	Instruction decode_instruction();
	void execute_instruction(const Instruction& instr);
	void decode_rtype(uint32_t instr, std::vector<std::string> argtypes, Instruction& instrInfo);
	void decode_itype(uint32_t instr, std::vector<std::string> argtypes, Instruction& instrInfo);
	void decode_jtype(uint32_t instr, std::vector<std::string> argtypes, Instruction& instrInfo);
	void initialize_registers();
	void initialize_instruction_tables();

public:
	

	CPU();
	CPU(std::vector<uint8_t>* bytes);
	CPU(MMU* mmu_obj);
	~CPU();

	void mainloop();
	void reset();

	// R-type Instructions
	void NOP(const Instruction& instr);
	void ADD(const Instruction& instr);
	void AND(const Instruction& instr);
	void DIV(const Instruction& instr);
	void JR(const Instruction& instr);
	void MULT(const Instruction& instr);
	void NOT(const Instruction& instr);
	void OR(const Instruction& instr);
	void SUB(const Instruction& instr);
	void XOR(const Instruction& instr);
	void HALT(const Instruction& instr);

	// I-type Instructions
	void ADDI(const Instruction& instr);
	void ANDI(const Instruction& instr);
	void LB(const Instruction& instr);
	void LBI(const Instruction& instr);
	void LBS(const Instruction& instr);
	void LW(const Instruction& instr);
	void LWI(const Instruction& instr);
	void ORI(const Instruction& instr);
	void RBS(const Instruction& instr);
	void SB(const Instruction& instr);
	void SBI(const Instruction& instr);
	void SUBI(const Instruction& instr);
	void SW(const Instruction& instr);
	void SWI(const Instruction& instr);
	void XORI(const Instruction& instr);

	// J-type Instructions
	void BRN(const Instruction& instr);
	void BRP(const Instruction& instr);
	void BRZ(const Instruction& instr);
	void JAL(const Instruction& instr);
	void JMP(const Instruction& instr);
	void RET(const Instruction& instr);
	void TRAP(const Instruction& instr);


};


#endif

