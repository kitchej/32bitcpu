#include <cstdint>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include "../header/CPU.h"
#include "../header/LC32_Globals.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#endif

constexpr bool DISABLE_JUMPS = false; // Replaces all jumps with empty function bodies. Useful when you only want to test decoding
constexpr uint32_t CLOCK_SPEED_HZ = 1'000'000;
constexpr auto TIME_SLICE = std::chrono::milliseconds(1);
constexpr uint32_t BATCH_SIZE = CLOCK_SPEED_HZ / 1000; // How many instructions must complete per ms to stay in time

void CPU::initialize_registers() {
	flags = 0;
	r0 = 0;  r1 = 0;  r2 = 0;  r3 = 0;  r4 = 0;  r5 = 0;  r6 = 0;  r7 = 0;
	t0 = 0; t1 = 0; t2 = 0; t3 = 0; t4 = 0; t5 = 0; t6 = 0; t7 = 0;
	a0 = 0;  a1 = 0;  a2 = 0;
	v0 = 0;  v1 = 0; ra = 0;
	sp = 0;  sf = 0;
	pc = 0;  ir = 0;  mar = 0;  mdr = 0, ira = 0;

	registerFile = {
	   { Register::R0,  &r0  },
	   { Register::R1,  &r1  },
	   { Register::R2,  &r2  },
	   { Register::R3,  &r3  },
	   { Register::R4,  &r4  },
	   { Register::R5,  &r5  },
	   { Register::R6,  &r6  },
	   { Register::R7,  &r7  },
	   { Register::R8,  &t0  },
	   { Register::R9,  &t1  },
	   { Register::R10, &t2 },
	   { Register::R11, &t3 },
	   { Register::R12, &t4 },
	   { Register::R13, &t5 },
	   { Register::R14, &t6 },
	   { Register::R15, &t7 },
	   { Register::RA,  &ra  },
	   { Register::A0,  &a0  },
	   { Register::A1,  &a1  },
	   { Register::A2,  &a2  },
	   { Register::V0,  &v0  },
	   { Register::V1,  &v1  },
	   { Register::SP,  &sp  },
	   { Register::SF,  &sf  },
	};
}

CPU::CPU() {
	initialize_registers();
	memoryController = new MemoryController();
}

CPU::CPU(MemoryController* memoryController_obj) {
	initialize_registers();
	memoryController = memoryController_obj;
}

void CPU::step() { 
	/*
	Nesscary for controlling the CPU via a debugger
	Pulls the next instruction from this->ir, therefore 
	fetch_instruction() must be called before step()!
	*/
	Instruction curr_instr = decode_instruction();
	execute_instruction(curr_instr);
	service_interrupts();
	pc += 4;
}

void CPU::mainloop() {
	
	using cpu_clock = std::chrono::steady_clock;
	auto deadline = cpu_clock::now() + TIME_SLICE; // Deadline to complete a batch of instructions
	uint32_t instructions_completed = 0;
	while (!get_flag(Flag::HALT)) {
		fetch_instruction();
		step();
		instructions_completed++;
		if (instructions_completed == BATCH_SIZE) {
			instructions_completed = 0;
			std::this_thread::sleep_until(deadline);	    // Throttle CPU if we are done early
			deadline += TIME_SLICE;						    // Then just add a new TIME_SLICE to what the deadline was
			if (cpu_clock::now() > deadline + TIME_SLICE) { // But if we have gotten out of sync, we don't want the CPU to ramp up 
														    // as that would defeat the purpose of this clock
				deadline = cpu_clock::now() + TIME_SLICE;   // So instead we ignore the missed time and base new deadline off current time

			}
		}
	}
}

Instruction CPU::decode_instruction() {
	Instruction instrInfo;
	instrInfo.opcode = (Opcode)((ir >> 26) & MASK_6BIT);
	auto it = instructionFormats.find(instrInfo.opcode);
	if (it != instructionFormats.end()) {
		switch (it->second) {
		case CmdFmt::R_TYPE: decode_rtype(ir, instrInfo); break;
		case CmdFmt::I_TYPE: decode_itype(ir, instrInfo); break;
		case CmdFmt::J_TYPE: decode_jtype(ir, instrInfo); break;
		}
	}
	return instrInfo;
}

void CPU::decode_rtype(uint32_t instr, Instruction& instrInfo) {
	instrInfo.dest = (instr >> 21) & MASK_5BIT;
	instrInfo.src = (instr >> 16) & MASK_5BIT;
	instrInfo.trgt = (instr >> 11) & MASK_5BIT;
	instrInfo.shamt = (instr >> 6) & MASK_5BIT;
	instrInfo.imm16 = 0;
	instrInfo.offset26 = 0;
}

void CPU::decode_itype(uint32_t instr, Instruction& instrInfo) {
	instrInfo.dest = (instr >> 21) & MASK_5BIT;
	instrInfo.src = (instr >> 16) & MASK_5BIT;
	instrInfo.imm16 = instr & MASK_16BIT;
	instrInfo.trgt = 0;
	instrInfo.offset26 = 0;

}

void CPU::decode_jtype(uint32_t instr, Instruction& instrInfo) {
	instrInfo.offset26 = instr & MASK_26BIT;
	instrInfo.dest = 0;
	instrInfo.src = 0;
	instrInfo.trgt = 0;
	instrInfo.imm16 = 0;
}

void CPU::fetch_instruction() {
	ir = memoryController->readWord(pc);
}

void CPU::execute_instruction(const Instruction& instr) {
	size_t op_index = (size_t)instr.opcode;
	(this->*opTable[op_index])(instr);
}

void CPU::sign_extend_imm16(uint32_t &value) {
	int32_t imm16 = (int32_t)value;
	imm16 = imm16 << 16;
	imm16 = imm16 >> 16;
	value = (uint32_t)imm16;
}

void CPU::sign_extend_offset26(uint32_t& value) {
	int32_t offset26 = (int32_t)value;
	offset26 = offset26 << 6; // 32-26 = 6
	offset26 = offset26 >> 6;
	value = (uint32_t)offset26;
}

void CPU::reset() {
	initialize_registers();
}

void CPU::interrupt(uint32_t interVec) {
	std::lock_guard<std::mutex> lock(interruptLock);
	interruptq.insert(interruptq.begin(), interVec);
}

void CPU::service_interrupts() {
	std::lock_guard<std::mutex> lock(interruptLock);
	if (get_flag(Flag::SERVICING_INTERRUPT)) {
		return;
	}
	else if (!interruptq.empty()) {
		uint32_t interVec = interruptq.back();
		interruptq.pop_back();
		uint32_t interJmp = 0;
		if (interVec <= 0xFF) {
			interJmp = memoryController->readWord(HARDWARE_INTERRUPT_TABLE - (interVec * 4));
		}
		else {
			interJmp = memoryController->readWord(SOFTWARE_INTERRUPT_TABLE + (interVec * 4));
		}
		ira = pc + 4;
		pc = interJmp - 4; // to account for hardcoded pc + 4
		set_flag(Flag::SERVICING_INTERRUPT, true);
	}
	else {
		set_flag(Flag::SERVICING_INTERRUPT, false);
	}
}

uint32_t CPU::peek_reg(Register address) {
	return *registerFile[address];
}

void CPU::poke_reg(Register address, uint32_t value) {
	*registerFile[address] = value;
}

void CPU::set_flag(Flag flag, bool value) {
	flags = (flags & ~(1u << flag)) | (static_cast<uint32_t>(value) << flag);
}

void CPU::set_flags_alu(uint32_t regDestVal) {
	
	int32_t val = static_cast<int32_t>(regDestVal);
	if (val == 0) {
		set_flag(Flag::POS, 0);
		set_flag(Flag::NEG, 0);
		set_flag(Flag::ZERO, 1);
	}
	else if (val < 0) {
		set_flag(Flag::POS, 0);
		set_flag(Flag::NEG, 1);
		set_flag(Flag::ZERO, 0);
	}
	else {
		set_flag(Flag::POS, 1);
		set_flag(Flag::NEG, 0);
		set_flag(Flag::ZERO, 0);
	}
}

bool CPU::get_flag(Flag flag) {
	return flags & (1 << flag);
}

void CPU::execute_jmp(const Instruction& instr) {
	uint32_t offset26 = instr.offset26;
	sign_extend_offset26(offset26);
	pc += (offset26 * 4);
}

// R-type Instructions (Except JALA)
void CPU::NOP(const Instruction& instr) {  }
void CPU::ADD(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) + *registerFile[(Register)instr.trgt];
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::AND(const Instruction& instr){
	*registerFile[(Register)instr.dest] = *registerFile[(Register)instr.src] & *registerFile[(Register)instr.trgt];
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::DIV(const Instruction& instr){
	// All this casting crap is needed to get true signed divsion in cpp
	int32_t dest = (int32_t)*registerFile[(Register)instr.dest];
	int32_t src = (int32_t)*registerFile[(Register)instr.src];
	int32_t trgt = (int32_t)*registerFile[(Register)instr.trgt];
	if (trgt == 0) {
		set_flag(Flag::D_ZERO, 1);
		return;
	}
	*registerFile[(Register)instr.dest] = (uint32_t)(src / trgt);
	set_flags_alu(*registerFile[(Register)instr.dest]);
}


void CPU::LBS(const Instruction& instr) {
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) << instr.shamt;
	
}

void CPU::MULT(const Instruction& instr){
	// All this casting crap is needed to get true signed mulitplcation in cpp
	int32_t dest = (int32_t)*registerFile[(Register)instr.dest];
	int32_t src = (int32_t)*registerFile[(Register)instr.src];
	int32_t trgt = (int32_t)*registerFile[(Register)instr.trgt];
	*registerFile[(Register)instr.dest] = (uint32_t)(src * trgt);
	set_flags_alu(*registerFile[(Register)instr.dest]);
}
void CPU::NOT(const Instruction& instr){
	*registerFile[(Register)instr.dest] = ~(*registerFile[(Register)instr.src]);
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::OR(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) | (*registerFile[(Register)instr.trgt]);
	set_flags_alu(*registerFile[(Register)instr.dest]);
}


void CPU::RBS(const Instruction& instr) {
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) >> instr.shamt;
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::SUB(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) - (*registerFile[(Register)instr.trgt]);
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::XOR(const Instruction& instr){
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) ^ (*registerFile[(Register)instr.trgt]);
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::HALT(const Instruction& instr){
	set_flag(Flag::HALT, true);
}


// I-type Instructions
void CPU::ADDI(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) + imm16;
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::ANDI(const Instruction& instr) {
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) & imm16;
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::LB(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	mar = imm16 + pc;
	mdr = memoryController->readByte(mar);
	*registerFile[(Register)instr.dest] = mdr;
	
}

void CPU::LBI(const Instruction& instr){
	mar = *registerFile[(Register)instr.src];
	mdr = memoryController->readByte(mar);
	*registerFile[(Register)instr.dest] = mdr;
	
}

void CPU::LW(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	mar = imm16 + pc;
	mdr = memoryController->readWord(mar);
	*registerFile[(Register)instr.dest] = mdr;
	
}

void CPU::LWI(const Instruction& instr){
	mar = *registerFile[(Register)instr.src];
	mdr = memoryController->readWord(mar);
	*registerFile[(Register)instr.dest] = mdr;
}

void CPU::ORI(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) | imm16;
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::SB(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	mar = imm16 + pc;
	mdr = *registerFile[(Register)instr.dest];
	memoryController->writeByte(mar, mdr);
}

void CPU::SBI(const Instruction& instr){
	mar = *registerFile[(Register)instr.dest];
	mdr = *registerFile[(Register)instr.src];
	memoryController->writeByte(mar, mdr);
}

void CPU::SUBI(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) - instr.imm16;
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

void CPU::SW(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	mar = imm16 + pc;
	mdr = *registerFile[(Register)instr.dest];
	memoryController->writeWord(mar, mdr);
}

void CPU::SWI(const Instruction& instr){
	mar = *registerFile[(Register)instr.dest];
	mdr = *registerFile[(Register)instr.src];
	memoryController->writeWord(mar, mdr);
	
}

void CPU::XORI(const Instruction& instr){
	uint32_t imm16 = instr.imm16;
	sign_extend_imm16(imm16);
	*registerFile[(Register)instr.dest] = (*registerFile[(Register)instr.src]) ^ imm16;
	set_flags_alu(*registerFile[(Register)instr.dest]);
}

#if !DISABLE_JUMPS

// J-type Instructions
void CPU::JALA(const Instruction& instr) {
	ra = pc + 4;
	pc = *registerFile[(Register)instr.src] - 4;
	
}

void CPU::BRN(const Instruction& instr) {
	if (get_flag(Flag::NEG)) {
		execute_jmp(instr);
	}
	
}

void CPU::BRP(const Instruction& instr) {
	if (get_flag(Flag::POS)) {
		execute_jmp(instr);
	}
	
}

void CPU::BRZ(const Instruction& instr) {
	if (get_flag(Flag::ZERO)) {
		execute_jmp(instr);
	}
	
}

void CPU::JAL(const Instruction& instr) {
	ra = pc + 4;
	execute_jmp(instr);
	
}

void CPU::JMP(const Instruction& instr) {
	execute_jmp(instr);
	
}

void CPU::RET(const Instruction& instr) {
	pc = ra - 4;
	
}

void CPU::INTER(const Instruction& instr) {
	
	interrupt(instr.offset26);
}

void CPU::RETI(const Instruction& instr) {
	pc = ira - 4;
	set_flag(Flag::SERVICING_INTERRUPT, false);
	
}

#else

void CPU::JALA(const Instruction& instr) {
	
}

void CPU::BRN(const Instruction& instr) {
	
}

void CPU::BRP(const Instruction& instr) {
	
}

void CPU::BRZ(const Instruction& instr) {
	
}

void CPU::JAL(const Instruction& instr) {
	
}

void CPU::JMP(const Instruction& instr) {
	
}

void CPU::RET(const Instruction& instr) {
	
}

void CPU::INTER(const Instruction& instr) {
	
}

void CPU::RETI(const Instruction& instr) {
	
}

#endif