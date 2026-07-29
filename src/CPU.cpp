#include <cstdint>
#include "../header/CPU.h"

void CPU::initialize_instruction_tables() {
	instructionFormats[Opcode::NOP] = {"R-type"};
	instructionFormats[Opcode::ADD] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::ADDI] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::AND] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::ANDI] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::BRN] = {"J-type", "offset26"};
	instructionFormats[Opcode::BRP] = {"J-type", "offset26"};
	instructionFormats[Opcode::BRZ] = {"J-type", "offset26"};
	instructionFormats[Opcode::DIV] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::JAL] = {"J-type", "offset26"};
	instructionFormats[Opcode::JMP] = {"J-type", "offset26"};
	instructionFormats[Opcode::JR] = {"R-type", "src1"};
	instructionFormats[Opcode::LB] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::LBI] = {"I-type", "dest", "imm18"};
	instructionFormats[Opcode::LBS] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::LW] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::LWI] = {"I-type", "dest", "imm18"};
	instructionFormats[Opcode::MULT] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::NOT] = {"R-type", "dest", "src1"};
	instructionFormats[Opcode::OR] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::ORI] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::RBS] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::RET] = {"J-type"};
	instructionFormats[Opcode::SB] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::SBI] = {"I-type", "dest", "imm18"};
	instructionFormats[Opcode::SUB] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::SUBI] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::SW] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::SWI] = {"I-type", "dest", "imm18"};
	instructionFormats[Opcode::TRAP] = {"J-type"};
	instructionFormats[Opcode::XOR] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::XORI] = {"I-type", "dest", "src1", "imm18"};
	instructionFormats[Opcode::HALT] = { "R-type"};
	op_table = std::vector<OP_FPTR>{
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
		&CPU::JR,
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
		&CPU::TRAP,
		&CPU::XOR,
		&CPU::XORI,
		&CPU::HALT
	};
}

void CPU::initialize_registers() {

	 N_FLAG = false; P_FLAG = false; Z_FLAG = false;
	 r0 = 0;  r1 = 0;  r2 = 0;  r3 = 0;  r4 = 0;  r5 = 0;  r6 = 0;  r7 = 0;
	 a0 = 0;  a1 = 0;  a2 = 0;
	 v0 = 0;  v1 = 0;
	 sp = 0;  sf = 0;  pc = 0x00005000;  ir = 0;  mar = 0;  mdr = 0;
}

CPU::CPU() {
	initialize_registers();
	initialize_instruction_tables();
	mmu = new MMU();
}

CPU::CPU(std::vector<uint8_t>* bytes) {
	initialize_registers();
	initialize_instruction_tables();
	mmu = new MMU(bytes);
}
CPU::CPU(MMU* mmu_obj) {
	initialize_registers();
	initialize_instruction_tables();
	mmu = mmu_obj;
}
CPU::~CPU() {
	delete mmu;
}

Instruction CPU::decode_instruction() {
	Instruction instrInfo;
	uint8_t opcode = (ir >> 26) & 0x3F; // Extract the opcode (6 bits)
	instrInfo.opcode = static_cast<Opcode>(opcode);
	// Determine the instruction format based on the opcode
	auto type = instructionFormats.find(instrInfo.opcode);
	if (type != instructionFormats.end()) {
		const std::vector<std::string>& format = type->second;
		if (format[0] == "R-type") {
			decode_rtype(ir, format, instrInfo);
		}
		else if (format[0] == "I-type") {
			decode_itype(ir, format, instrInfo);
		}
		else if (format[0] == "J-type") {
			decode_jtype(ir, format, instrInfo);
		}
	}
	return instrInfo;
}

void CPU::decode_rtype(uint32_t instr, std::vector<std::string> argtypes, Instruction& instrInfo) {
	instrInfo.dest = (instr >> 21) & MASK_5BIT;
	instrInfo.src1 = (instr >> 16) & MASK_5BIT;
	instrInfo.src2 = (instr >> 11) & MASK_5BIT;
	instrInfo.imm18 = 0;
	instrInfo.offset26 = 0;
}
void CPU::decode_itype(uint32_t instr, std::vector<std::string> argtypes, Instruction& instrInfo) {
	instrInfo.dest = (instr >> 21) & MASK_5BIT;
	instrInfo.src1 = (instr >> 16) & MASK_5BIT;
	instrInfo.imm18 = instr & MASK_18BIT;
	instrInfo.src2 = 0;
	instrInfo.offset26 = 0;

}
void CPU::decode_jtype(uint32_t instr, std::vector<std::string> argtypes, Instruction& instrInfo) {
	instrInfo.offset26 = instr & MASK_26BIT;
	instrInfo.dest = 0;
	instrInfo.src1 = 0;
	instrInfo.src2 = 0;
	instrInfo.imm18 = 0;
}

void CPU::fetch_instruction() {
	ir = mmu->readWord(pc);
}

void CPU::execute_instruction(const Instruction& instr) {
	size_t op_index = (size_t)instr.opcode;
	(this->*op_table[op_index])(instr);
}

void CPU::reset() {
	initialize_registers();
}

void CPU::mainloop() {
	while (true) {
		fetch_instruction();
		Instruction curr_instr = decode_instruction();
		execute_instruction(curr_instr);
		pc += 4;
	}
}

// R-type Instructions
void CPU::NOP(const Instruction& instr){}
void CPU::ADD(const Instruction& instr){
	instr->dest

}
void CPU::AND(const Instruction& instr){}
void CPU::DIV(const Instruction& instr){}
void CPU::JR(const Instruction& instr){}
void CPU::MULT(const Instruction& instr){}
void CPU::NOT(const Instruction& instr){}
void CPU::OR(const Instruction& instr){}
void CPU::SUB(const Instruction& instr){}
void CPU::XOR(const Instruction& instr){}
void CPU::HALT(const Instruction& instr){}


// I-type Instructions
void CPU::ADDI(const Instruction& instr){}
void CPU::ANDI(const Instruction& instr){}
void CPU::LB(const Instruction& instr){}
void CPU::LBI(const Instruction& instr){}
void CPU::LBS(const Instruction& instr){}
void CPU::LW(const Instruction& instr){}
void CPU::LWI(const Instruction& instr){}
void CPU::ORI(const Instruction& instr){}
void CPU::RBS(const Instruction& instr){}
void CPU::SB(const Instruction& instr){}
void CPU::SBI(const Instruction& instr){}
void CPU::SUBI(const Instruction& instr){}
void CPU::SW(const Instruction& instr){}
void CPU::SWI(const Instruction& instr){}
void CPU::XORI(const Instruction& instr){}

// J-type Instructions
void CPU::BRN(const Instruction& instr){}
void CPU::BRP(const Instruction& instr){}
void CPU::BRZ(const Instruction& instr){}
void CPU::JAL(const Instruction& instr){}
void CPU::JMP(const Instruction& instr){}
void CPU::RET(const Instruction& instr){}
void CPU::TRAP(const Instruction& instr){}