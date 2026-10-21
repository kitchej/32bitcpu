#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <string>
#include <map>
#include "memoryController.h"
#include "LC32_Globals.h"

class CPU;

using OP_FPTR = void (CPU::*)(const Instruction&);

class CPU {
private:
	// Bit masks
	static constexpr uint32_t MASK_16BIT = 0xFFFF;
	static constexpr uint32_t MASK_26BIT = 0x3FFFFFF;
	static constexpr uint8_t MASK_5BIT = 0x1F;
	static constexpr uint8_t MASK_6BIT = 0x3F;

	// General Purpose Regesters
	uint32_t r0, r1, r2, r3, r4, r5, r6, r7;
	// Temporary Registers
	uint32_t t0, t1, t2, t3, t4, t5, t6, t7;
	// Special Purpose Regesters
	uint32_t a0, a1, a2;
	uint32_t v0, v1;
	uint32_t sp, sf;
	uint32_t ra;
	// Internal Registers
	uint32_t pc, ir, mar, mdr, ira;
	// Flags
	uint32_t flags;

	// Classes
	MemoryController* memoryController;

	// Data Structures
	std::vector<OP_FPTR> opTable = std::vector<OP_FPTR>{
	&CPU::NOP,
	&CPU::ADD,
	&CPU::ADDI,
	&CPU::AND,
	&CPU::ANDI,
	&CPU::BRN,
	&CPU::BRP,
	&CPU::BRZ,
	&CPU::DIV,
	&CPU::JAL,
	&CPU::JMP,
	&CPU::JALA,
	&CPU::LB,
	&CPU::LBI,
	&CPU::LBS,
	&CPU::LW,
	&CPU::LWI,
	&CPU::MULT,
	&CPU::NOT,
	&CPU::OR,
	&CPU::ORI,
	&CPU::RBS,
	&CPU::RET,
	&CPU::SB,
	&CPU::SBI,
	&CPU::SUB,
	&CPU::SUBI,
	&CPU::SW,
	&CPU::SWI,
	&CPU::INTER,
	&CPU::XOR,
	&CPU::XORI,
	&CPU::HALT,
	&CPU::RETI
	};

	std::map<Register, uint32_t*> registerFile;
	std::vector<uint32_t> interruptq;

	// Syncronization 
	std::mutex interruptLock;

	// CPU Execution Methods
	void set_flag(Flag flag, bool value);
	void set_flags_alu(uint32_t regDestVal);
	void fetch_instruction();
	Instruction decode_instruction();
	void decode_rtype(uint32_t instr, Instruction& instrInfo);
	void decode_itype(uint32_t instr, Instruction& instrInfo);
	void decode_jtype(uint32_t instr, Instruction& instrInfo);
	void execute_instruction(const Instruction& instr);
	void sign_extend_imm16(uint32_t &value);	void sign_extend_offset26(uint32_t& value);
	void execute_jmp(const Instruction& instr);

	// Initialzation methods
	void initialize_registers();

	// Interrupt control
	void service_interrupts();
	

public:
	CPU();
	CPU(MemoryController* memoryController_obj);

	void mainloop();
	void reset();
	uint32_t peek_reg(Register address);
	void poke_reg(Register address, uint32_t value);
	void interrupt(uint32_t interVec);
	bool get_flag(Flag flag);

	// R-type Instructions
	void NOP(const Instruction& instr);
	void ADD(const Instruction& instr);
	void AND(const Instruction& instr);
	void DIV(const Instruction& instr);
	void JALA(const Instruction& instr);
	void LBS(const Instruction& instr);
	void MULT(const Instruction& instr);
	void NOT(const Instruction& instr);
	void OR(const Instruction& instr);
	void RBS(const Instruction& instr);
	void SUB(const Instruction& instr);
	void XOR(const Instruction& instr);
	void HALT(const Instruction& instr);

	// I-type Instructions
	void ADDI(const Instruction& instr);
	void ANDI(const Instruction& instr);
	void LB(const Instruction& instr);
	void LBI(const Instruction& instr);
	void LW(const Instruction& instr);
	void LWI(const Instruction& instr);
	void ORI(const Instruction& instr);
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
	void INTER(const Instruction& instr);
	void RETI(const Instruction& instr);


};


#endif
