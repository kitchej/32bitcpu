#!/usr/bin/env python3
"""
!!! AI GENERATED !!!
(But perfected by yours truly)

Generates a binary file with encoded instructions for each format type.
Creates a byte array spanning from 0x00005000 to 0x0FFFFFFF
"""

import struct
from sys import byteorder
MAX_UINT_32 = 0xFFFFFFFF
JUMPS_ONLY = False
FILE_SIZE = 268353536
INSTRUCT_OFFSET = 0
DATA_OFFSET = 0

# Opcode definitions (matching the CPU's Opcode enum)
OPCODES = {
	'NOP': 0,
	'ADD': 1,
	'ADDI': 2,
	'AND': 3,
	'ANDI': 4,
	'BRN': 5,
	'BRP': 6,
	'BRZ': 7,
	'DIV': 8,
	'JAL': 9,
	'JMP': 10,
	'JALA': 11,
	'LB': 12,
	'LBI': 13,
	'LBS': 14,
	'LW': 15,
	'LWI': 16,
	'MULT': 17,
	'NOT': 18,
	'OR': 19,
	'ORI': 20,
	'RBS': 21,
	'RET': 22,
	'SB': 23,
	'SBI': 24,
	'SUB': 25,
	'SUBI': 26,
	'SW': 27,
	'SWI': 28,
	'INTER': 29,
	'XOR': 30,
	'XORI': 31,
	'HALT': 32,
	'RETI': 33,
}

def encode_rtype(opcode, dest, src, trgt):
	"""
	R-type format:
	**OPCODE** [31-26: 6 bits] | **DEST** [25-21: 5 bits] | **SRC** [21-16: 5 bits] | **TRGT** [15-11: 5 bits] | **Unused** [10-0: 11 bits]
	"""
	instruction = (opcode << 26) | (dest << 21) | (src << 16) | (trgt << 11)
	return instruction & 0xFFFFFFFF

def encode_itype(opcode, dest, src, imm17):
	"""
	I-type format:
	**OPCODE** [31-26: 6 bits] | **DEST** [25-21: 5 bits] | **SRC** [20-16: 5 bits] | **IMM-17** [16-0: 17 bits]
	"""
	instruction = (opcode << 26) | (dest << 21) | (src << 17) | (imm17 & 0x1FFFF)
	return instruction & 0xFFFFFFFF

def encode_jtype(opcode, offset26):
	"""
	J-type format:
	[opcode:6][offset26:26]
	Bits: 31-26  25-0
	"""
	instruction = (opcode << 26) | (offset26 & 0x3FFFFFF)
	return instruction & 0xFFFFFFFF

def write_uint32_to_buffer(buffer, offset, value):
	"""
	Write a 32-bit unsigned integer to a byte buffer in little-endian format.

	Args:
		buffer: bytearray to write to
		offset: byte offset in the buffer
		value: 32-bit unsigned integer value
	"""
	buffer[offset:offset+4] = struct.pack('<I', value & 0xFFFFFFFF)

def print_word_hexdump(buffer, start_offset, end_offset):
	"""
	Print a 4-byte-per-row hexdump of the buffer, reading each row as a
	little-endian uint32_t (matching memoryController::readWord's byte assembly), so the
	printed value is exactly what the CPU will decode -- no manual byte
	reversal needed to compare against expected instruction values.

	Equivalent to: hexdump -e '"%08_ax: " 1/4 "%08x" "\\n"' dummy_file.bin
	"""
	for offset in range(start_offset, end_offset, 4):
		word = struct.unpack_from('<I', buffer, offset)[0]
		print(f"{offset:08x}: {word:08x}")

def main():
	# File configuration
	print(f"Creating buffer of {FILE_SIZE:,} bytes ({FILE_SIZE / (1024*1024):.2f} MB)...")

	# Create byte buffer
	buffer = bytearray(FILE_SIZE)
	offset = INSTRUCT_OFFSET

	if JUMPS_ONLY:
		instructions = [
			encode_jtype(OPCODES['JMP'], offset26=1),
			MAX_UINT_32,
			encode_jtype(OPCODES['JAL'], offset26=1),
			MAX_UINT_32,
			encode_jtype(OPCODES['JALA'], offset26=1),
			MAX_UINT_32,

		]
	else:
		instructions = [
			encode_jtype(OPCODES['INTER'], offset26=4),
			encode_rtype(OPCODES['ADD'], dest=1, src=1, trgt=2),
		]

	print(f"Writing {len(instructions)} instructions to buffer.")
	for instr in instructions:
		write_uint32_to_buffer(buffer, offset, instr)
		offset += 4

	# Write HALT at the end
	halt_instr = encode_rtype(OPCODES['HALT'], dest=0, src=0, trgt=0)
	write_uint32_to_buffer(buffer, offset, halt_instr)
	offset += 4

	print(f"Writing to dummy_file.bin...")
	with open('dummy_file.bin', 'wb') as f:
		f.write(buffer)

	print(f"Successfully wrote dummy_file.bin")
	print(f"\nHexdump of instructions (0x0 - 0x{offset:X}):")
	print_word_hexdump(buffer, INSTRUCT_OFFSET, offset)

if __name__ == '__main__':
	main()