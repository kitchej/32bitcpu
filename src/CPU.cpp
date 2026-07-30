#include <cstdint>
#include <iostream>
#include "../header/CPU.h"

void CPU::initialize_instruction_tables() {
	instructionFormats[Opcode::NOP] = {"R-type"};
	instructionFormats[Opcode::ADD] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::ADDI] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::AND] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::ANDI] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::BRN] = {"J-type", "offset26"};
	instructionFormats[Opcode::BRP] = {"J-type", "offset26"};
	instructionFormats[Opcode::BRZ] = {"J-type", "offset26"};
	instructionFormats[Opcode::DIV] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::JAL] = {"J-type", "offset26"};
	instructionFormats[Opcode::JMP] = {"J-type", "offset26"};
	instructionFormats[Opcode::JR] = {"R-type", "src1"};
	instructionFormats[Opcode::LB] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::LBI] = {"I-type", "dest", "imm17"};
	instructionFormats[Opcode::LBS] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::LW] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::LWI] = {"I-type", "dest", "imm17"};
	instructionFormats[Opcode::MULT] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::NOT] = {"R-type", "dest", "src1"};
	instructionFormats[Opcode::OR] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::ORI] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::RBS] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::RET] = {"J-type"};
	instructionFormats[Opcode::SB] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::SBI] = {"I-type", "dest", "imm17"};
	instructionFormats[Opcode::SUB] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::SUBI] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::SW] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::SWI] = {"I-type", "dest", "imm17"};
	instructionFormats[Opcode::TRAP] = {"J-type"};
	instructionFormats[Opcode::XOR] = {"R-type", "dest", "src1", "src2"};
	instructionFormats[Opcode::XORI] = {"I-type", "dest", "src1", "imm17"};
	instructionFormats[Opcode::HALT] = { "R-type"};
	opTable = std::vector<OP_FPTR>{
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
		&CPU::HALT,

	};
}

void CPU::initialize_registers() {
	 N_FLAG = false; P_FLAG = false; Z_FLAG = false; D_ZERO = false; HALT_FLAG = false;
	 r0 = 0;  r1 = 0;  r2 = 0;  r3 = 0;  r4 = 0;  r5 = 0;  r6 = 0;  r7 = 0;
	 a0 = 0;  a1 = 0;  a2 = 0;
	 v0 = 0;  v1 = 0; ra = 0;
	 sp = 0;  sf = 0;
	 pc = 0x00005000;  ir = 0;  mar = 0;  mdr = 0;

	 registerFile[Register::R0] = &r0;
	 registerFile[Register::R1] = &r1;
	 registerFile[Register::R2] = &r2;
	 registerFile[Register::R3] = &r3;
	 registerFile[Register::R4] = &r4;
	 registerFile[Register::R5] = &r5;
	 registerFile[Register::R6] = &r6;
	 registerFile[Register::R7] = &r7;
	 registerFile[Register::RA] = &ra;
	 registerFile[Register::A0] = &a0;
	 registerFile[Register::A1] = &a1;
	 registerFile[Register::A2] = &a2;
	 registerFile[Register::V0] = &v0;
	 registerFile[Register::V1] = &v1;
	 registerFile[Register::SP] = &sp;
	 registerFile[Register::SF] = &sf;
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
	uint8_t opcode = (ir >> 26) & MASK_6BIT;
	instrInfo.opcode = static_cast<Opcode>(opcode);
	// Determine the instruction format based on the opcode
	auto type = instructionFormats.find(instrInfo.opcode);
	if (type != instructionFormats.end()) {
		const std::vector<std::string>& format = type->second;
		if (format[0] == "R-type") {
			decode_rtype(ir, instrInfo);
		}
		else if (format[0] == "I-type") {
			decode_itype(ir, instrInfo);
		}
		else if (format[0] == "J-type") {
			decode_jtype(ir, instrInfo);
		}
	}
	return instrInfo;
}

void CPU::decode_rtype(uint32_t instr, Instruction& instrInfo) {
	instrInfo.dest = (instr >> 21) & MASK_5BIT;
	instrInfo.src = (instr >> 16) & MASK_5BIT;
	instrInfo.trgt = (instr >> 11) & MASK_5BIT;
	instrInfo.imm17 = 0;
	instrInfo.offset26 = 0;
}
void CPU::decode_itype(uint32_t instr, Instruction& instrInfo) {
	instrInfo.dest = (instr >> 21) & MASK_5BIT;
	instrInfo.src = (instr >> 16) & MASK_5BIT;
	instrInfo.imm17 = instr & MASK_17BIT;
	instrInfo.trgt = 0;
	instrInfo.offset26 = 0;

}
void CPU::decode_jtype(uint32_t instr, Instruction& instrInfo) {
	instrInfo.offset26 = instr & MASK_26BIT;
	instrInfo.dest = 0;
	instrInfo.src = 0;
	instrInfo.trgt = 0;
	instrInfo.imm17 = 0;
}

void CPU::fetch_instruction() {
	ir = mmu->readWord(pc);
}

void CPU::execute_instruction(const Instruction& instr) {
	size_t op_index = (size_t)instr.opcode;
	(this->*opTable[op_index])(instr);
}

void CPU::reset() {
	initialize_registers();
}

void CPU::mainloop() {
	/*DEBUG CODE*/
	//poke_reg(Register::R2, 1);
	//poke_reg(Register::R3, 2);
	/*END DEBUG CODE*/
	while (!HALT_FLAG) {
		fetch_instruction();
		Instruction curr_instr = decode_instruction();
		execute_instruction(curr_instr);
		pc += 4;
	}
}

uint32_t CPU::peek_reg(Register address) {
	return *registerFile[address];
}
void CPU::poke_reg(Register address, uint32_t value) {
	*registerFile[address] = value;
}

void CPU::clear_flags() {
	N_FLAG = false;
	P_FLAG = false;
	Z_FLAG = false;
	D_ZERO = false;
}

void CPU::set_flags(Register address) {
	clear_flags();
	uint32_t rvalue = *registerFile[address];
	if (rvalue == 0) {
		Z_FLAG = true;
		return;
	}

	uint32_t sign = rvalue & 0x1;
	if (sign == 1) {
		N_FLAG = true;
	}
	else {
		P_FLAG = true;
	}
	
}

// R-type Instructions (Except JR)
void CPU::NOP(const Instruction& instr) { clear_flags(); }
void CPU::ADD(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) + *registerFile[(Register)instr.trgt];
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::AND(const Instruction& instr){
	*registerFile[(Register)instr.dest] = *registerFile[(Register)instr.src] & *registerFile[(Register)instr.trgt];
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::DIV(const Instruction& instr){
	// All this casting crap is needed to get true signed divsion in cpp
	int32_t dest = (int32_t)*registerFile[(Register)instr.dest];
	int32_t src = (int32_t)*registerFile[(Register)instr.src];
	int32_t trgt = (int32_t)*registerFile[(Register)instr.trgt];
	if (trgt == 0) {
		D_ZERO = true;
		return;
	}
	*registerFile[(Register)instr.dest] = (uint32_t)(src / trgt);
	clear_flags();
	set_flags((Register)instr.dest);
}

void CPU::MULT(const Instruction& instr){
	// All this casting crap is needed to get true signed mulitplcation in cpp
	int32_t dest = (int32_t)*registerFile[(Register)instr.dest];
	int32_t src = (int32_t)*registerFile[(Register)instr.src];
	int32_t trgt = (int32_t)*registerFile[(Register)instr.trgt];
	*registerFile[(Register)instr.dest] = (uint32_t)(src * trgt);
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::NOT(const Instruction& instr){
	*registerFile[(Register)instr.dest] = ~(*registerFile[(Register)instr.src]);
	clear_flags();
}
void CPU::OR(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) | (*registerFile[(Register)instr.trgt]);
	clear_flags();
}
void CPU::SUB(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) - (*registerFile[(Register)instr.trgt]);
	clear_flags();
}
void CPU::XOR(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) ^ (*registerFile[(Register)instr.trgt]);
	clear_flags();
}
void CPU::HALT(const Instruction& instr){
	clear_flags();
	HALT_FLAG = true;
}


// I-type Instructions
// TO-DO: SIGN EXTEND IMM17 VALUES
void CPU::ADDI(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) + instr.imm17;
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::ANDI(const Instruction& instr) {
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) + instr.imm17;
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::LB(const Instruction& instr){
	mar = instr.imm17 + pc;
	mdr = mmu->readByte(mar);
	*registerFile[(Register)instr.dest] = mdr;
	clear_flags();
}
void CPU::LBI(const Instruction& instr){
	mar = *registerFile[(Register)instr.src];
	mdr = mmu->readByte(mar);
	*registerFile[(Register)instr.dest] = mdr;
	clear_flags();
}
void CPU::LBS(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) << instr.imm17;
	clear_flags();
}
void CPU::LW(const Instruction& instr){
	mar = instr.imm17 + pc;
	mdr = mmu->readWord(mar);
	*registerFile[(Register)instr.dest] = mdr;
	clear_flags();
}
void CPU::LWI(const Instruction& instr){
	mar = *registerFile[(Register)instr.src];
	mdr = mmu->readWord(mar);
	*registerFile[(Register)instr.dest] = mdr;
	clear_flags();
}
void CPU::ORI(const Instruction& instr){ 
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) | instr.imm17;
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::RBS(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) >> instr.imm17;
	clear_flags();
}
void CPU::SB(const Instruction& instr){
	mar = instr.imm17 + pc;
	mdr = *registerFile[(Register)instr.src];
	mmu->writeByte(mar, mdr);
	clear_flags(); 
}
void CPU::SBI(const Instruction& instr){
	// THESE OPCODE ARGUMENTS ARE CORRECT!
	// DO NOT CHANGE THEM!
	mar = *registerFile[(Register)instr.src]; // < CORRECT!
	mdr = *registerFile[(Register)instr.dest]; // < CORRECT!
	mmu->writeByte(mar, mdr);
	clear_flags();
}
void CPU::SUBI(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) - instr.imm17;
	clear_flags();
	set_flags((Register)instr.dest);
}
void CPU::SW(const Instruction& instr){
	mar = instr.imm17 + pc;
	mdr = *registerFile[(Register)instr.src];
	mmu->writeByte(mar, mdr);
	clear_flags();
}
void CPU::SWI(const Instruction& instr){
	// THESE OPCODE ARGUMENTS ARE CORRECT!
	// DO NOT CHANGE THEM!
	mar = *registerFile[(Register)instr.src]; // < CORRECT!
	mdr = *registerFile[(Register)instr.dest]; // < CORRECT!
	mmu->writeWord(mar, mdr);
	clear_flags();
	clear_flags();
}
void CPU::XORI(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) ^ instr.imm17;
	clear_flags();
	set_flags((Register)instr.dest);
}

#if !DISABLE_JUMPS

// J-type Instructions
void CPU::JR(const Instruction& instr) {
	pc = *registerFile[(Register)instr.src];
	clear_flags();
}
void CPU::BRN(const Instruction& instr) {
	clear_flags();
}
void CPU::BRP(const Instruction& instr) {
	clear_flags();
}
void CPU::BRZ(const Instruction& instr) {
	clear_flags();
}
void CPU::JAL(const Instruction& instr) {
	clear_flags();
}
void CPU::JMP(const Instruction& instr) {
	clear_flags();
}
void CPU::RET(const Instruction& instr) {
	clear_flags();
}
void CPU::TRAP(const Instruction& instr) {
	clear_flags();
}

#else

void CPU::JR(const Instruction& instr) {
	clear_flags();
}
void CPU::BRN(const Instruction& instr) {
	clear_flags();
}
void CPU::BRP(const Instruction& instr) {
	clear_flags();
}
void CPU::BRZ(const Instruction& instr) {
	clear_flags();
}
void CPU::JAL(const Instruction& instr) {
	clear_flags();
}
void CPU::JMP(const Instruction& instr) {
	clear_flags();
}
void CPU::RET(const Instruction& instr) {
	clear_flags();
}
void CPU::TRAP(const Instruction& instr) {
	clear_flags();
}

#endif