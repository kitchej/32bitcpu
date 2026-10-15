#ifndef CPU32BITGLOBALS_H
#define CPU32BITGLOBALS_H
#include <cstdint>
#include <map>

constexpr size_t TOTAL_MEMORY_SIZE = 134217728;
constexpr size_t GENERAL_MEMORY_SIZE = 133070848; // 127 MB
constexpr size_t KERNAL_MEMORY_SIZE = 16384;     // 16 KB
constexpr size_t IO_MEMORY_SIZE = 65536;     // 64 KB

constexpr size_t KERNAL_MEMORY_OFFSET = 0x00;
constexpr size_t KERNAL_MEMORY_END = 0x3FFF;
constexpr size_t IO_MEMORY_OFFSET = 0x4000;
constexpr size_t GENERAL_MEMORY_OFFSET = 0x18000;

static_assert(KERNAL_MEMORY_OFFSET + KERNAL_MEMORY_SIZE <= IO_MEMORY_OFFSET,
	"Kernel region overlaps IO region");
static_assert(IO_MEMORY_OFFSET + IO_MEMORY_SIZE <= GENERAL_MEMORY_OFFSET,
	"IO region overlaps general memory");
static_assert(GENERAL_MEMORY_OFFSET + GENERAL_MEMORY_SIZE <= TOTAL_MEMORY_SIZE,
	"general memory region exceeds total memory");

constexpr size_t HARDWARE_INTERRUPT_TABLE = 0X3FFF;
constexpr size_t SOFTWARE_INTERRUPT_TABLE = 0x14000;

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

typedef struct Instruction {
	Opcode opcode;
	uint8_t dest;
	uint8_t src;
	uint8_t trgt;
	uint8_t shamt;
	uint32_t imm16;
	uint32_t offset26;
} Instruction;


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


#endif
