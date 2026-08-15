#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <string>
#include <map>
#include "memoryController.h"

#define HARDWARE_INTERRUPT_TABLE 0X3FFF
#define SOFTWARE_INTERRUPT_TABLE  0xFFFFFFF

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
	JALA,
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
	INTER,
	XOR,
	XORI,
	HALT,
	RETI
};

enum class Register : uint8_t {
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	RA,
	A0,
	A1,
	A2,
	V0,
	V1,
	SP,
	SF,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15

};

enum Flag : uint32_t {
	ZERO = 31,
	NEG = 30,
	POS = 29,
	D_ZERO = 28,
	SERVICING_INTERRUPT = 27,
	HALT = 26
};

enum class CmdFmt {
	R_TYPE,
	I_TYPE,
	J_TYPE
};

typedef struct R_Instruction {
	Opcode opcode;
	uint8_t dest;
	uint8_t src;
	uint8_t trgt;
	uint8_t shamt;
} R_Instruction;

typedef struct I_Instruction {
	Opcode opcode;
	uint8_t dest;
	uint8_t src;
	uint32_t imm17;
} I_Instruction;


typedef struct J_Instruction {
	Opcode opcode;
	uint32_t offset26;
} J_Instruction;


// I plan to migrate to the above structs, but until then below is what you should use
// BUT IF YOU CHAGNE YOUR MIND, DONT DELETE THEM! THE ASSEMBLER USES THEM!

typedef struct Instruction {
	Opcode opcode;
	uint8_t dest;
	uint8_t src;
	uint8_t trgt;
	uint8_t shamt;
	uint32_t imm17;
	uint32_t offset26;
} Instruction;

using OP_FPTR = void (CPU::*)(const Instruction&);

const std::map<Opcode, CmdFmt> instructionFormats = {
	{ Opcode::NOP,   CmdFmt::R_TYPE },
	{ Opcode::ADD,   CmdFmt::R_TYPE },
	{ Opcode::ADDI,  CmdFmt::I_TYPE },
	{ Opcode::AND,   CmdFmt::R_TYPE },
	{ Opcode::ANDI,  CmdFmt::I_TYPE },
	{ Opcode::BRN,   CmdFmt::J_TYPE },
	{ Opcode::BRP,   CmdFmt::J_TYPE },
	{ Opcode::BRZ,   CmdFmt::J_TYPE },
	{ Opcode::DIV,   CmdFmt::R_TYPE },
	{ Opcode::JAL,   CmdFmt::J_TYPE },
	{ Opcode::JMP,   CmdFmt::J_TYPE },
	{ Opcode::JALA,  CmdFmt::R_TYPE },
	{ Opcode::LB,    CmdFmt::I_TYPE },
	{ Opcode::LBI,   CmdFmt::I_TYPE },
	{ Opcode::LBS,   CmdFmt::R_TYPE },
	{ Opcode::LW,    CmdFmt::I_TYPE },
	{ Opcode::LWI,   CmdFmt::I_TYPE },
	{ Opcode::MULT,  CmdFmt::R_TYPE },
	{ Opcode::NOT,   CmdFmt::R_TYPE },
	{ Opcode::OR,    CmdFmt::R_TYPE },
	{ Opcode::ORI,   CmdFmt::I_TYPE },
	{ Opcode::RBS,   CmdFmt::R_TYPE },
	{ Opcode::RET,   CmdFmt::J_TYPE },
	{ Opcode::SB,    CmdFmt::I_TYPE },
	{ Opcode::SBI,   CmdFmt::I_TYPE },
	{ Opcode::SUB,   CmdFmt::R_TYPE },
	{ Opcode::SUBI,  CmdFmt::I_TYPE },
	{ Opcode::SW,    CmdFmt::I_TYPE },
	{ Opcode::SWI,   CmdFmt::I_TYPE },
	{ Opcode::INTER, CmdFmt::J_TYPE },
	{ Opcode::XOR,   CmdFmt::R_TYPE },
	{ Opcode::XORI,  CmdFmt::I_TYPE },
	{ Opcode::HALT,  CmdFmt::R_TYPE },
	{ Opcode::RETI,  CmdFmt::J_TYPE },
};

class CPU {
private:
	// Bit masks
	static constexpr uint32_t MASK_17BIT = 0x1FFFF;
	static constexpr uint32_t MASK_26BIT = 0x3FFFFFF;
	static constexpr uint8_t MASK_5BIT = 0x1F;
	static constexpr uint8_t MASK_6BIT = 0x3F;

	// General Purpose Regesters
	uint32_t r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
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
	std::vector<OP_FPTR> opTable;
	std::map<Register, uint32_t*> registerFile;
	std::vector<uint32_t> interruptq;

	// Syncronization 
	std::mutex interruptLock;

	// CPU Execution Methods
	void set_flag(Flag flag, bool value);
	void set_flags_alu(uint32_t regDestVal);
	void clear_flags();
	void fetch_instruction();
	Instruction decode_instruction();
	void decode_rtype(uint32_t instr, Instruction& instrInfo);
	void decode_itype(uint32_t instr, Instruction& instrInfo);
	void decode_jtype(uint32_t instr, Instruction& instrInfo);
	void execute_instruction(const Instruction& instr);
	void sign_extend_imm17(uint32_t &value);
	void sign_extend_offset26(uint32_t& value);
	void execute_jmp(const Instruction& instr);

	// Initialzation methods
	void initialize_registers();
	void initialize_instruction_tables();

	// Interrupt control
	void service_interrupts();
	

public:
	

	CPU();
	CPU(MemoryController* memoryController_obj);
	~CPU();

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
	void INTER(const Instruction& instr);
	void RETI(const Instruction& instr);


};


#endif

