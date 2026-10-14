"""
!!! AI GENERATED !!!
(But perfected by yours truly)
"""
import struct

MAX_UINT_32 = 0xFFFFFFFF
FILE_SIZE = 133070848
INSTRUCT_OFFSET = 0
DATA_OFFSET = 0

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
	instruction = (opcode << 26) | (dest << 21) | (src << 16) | (trgt << 11)
	return instruction & 0xFFFFFFFF

def encode_itype(opcode, dest, src, imm17):
	instruction = (opcode << 26) | (dest << 21) | (src << 17) | (imm17 & 0x1FFFF)
	return instruction & 0xFFFFFFFF

def encode_jtype(opcode, offset26):
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