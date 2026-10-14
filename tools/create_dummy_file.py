"""
!!! AI GENERATED !!!
(But perfected by yours truly)
"""

from sys import byteorder

from utils import *
from utils import OPCODES
from utils import encode_jtype

master_instruction_list = [
			encode_itype(OPCODES['ADDI'], dest=1, src=1, imm17=-1),
			encode_rtype(OPCODES['NOP'], 0, 0, 0),
			encode_rtype(OPCODES['ADD'], dest=1, src=1, trgt=2),
			encode_rtype(OPCODES['AND'], dest=1, src=2, trgt=3),
			encode_itype(OPCODES['ANDI'], dest=1, src=2, imm17=255),
			encode_jtype(OPCODES['BRN'], offset26=6),
			encode_jtype(OPCODES['BRP'], offset26=5),
			encode_jtype(OPCODES['BRZ'], offset26=4),
			encode_rtype(OPCODES['DIV'], dest=1, src=2, trgt=3),
			encode_jtype(OPCODES['JAL'], offset26=3),
			encode_jtype(OPCODES['JMP'], offset26=2),
			encode_rtype(OPCODES['JALA'], dest=0, src=1, trgt=0),
			encode_itype(OPCODES['LB'], dest=1, src=0, imm17=50),
			encode_itype(OPCODES['LBI'], dest=1, src=2, imm17=0),
			encode_itype(OPCODES['LBS'], dest=1, src=2, imm17=2),
			encode_itype(OPCODES['LW'], dest=1, src=0, imm17=200),
			encode_itype(OPCODES['LWI'], dest=1, src=2, imm17=0),
			encode_rtype(OPCODES['MULT'], dest=1, src=2, trgt=3),
			encode_rtype(OPCODES['NOT'], dest=1, src=2, trgt=0),
			encode_rtype(OPCODES['OR'], dest=1, src=2, trgt=3),
			encode_itype(OPCODES['ORI'], dest=1, src=2, imm17=128),
			encode_itype(OPCODES['RBS'], dest=1, src=2, imm17=3),
			encode_jtype(OPCODES['RET'], offset26=0),
			encode_itype(OPCODES['SB'], dest=1, src=0, imm17=50),
			encode_itype(OPCODES['SBI'], dest=1, src=2, imm17=0),
			encode_rtype(OPCODES['SUB'], dest=1, src=2, trgt=3),
			encode_itype(OPCODES['SUBI'], dest=1, src=2, imm17=50),
			encode_itype(OPCODES['SW'], dest=1, src=0, imm17=200),
			encode_itype(OPCODES['SWI'], dest=1, src=2, imm17=0),
			encode_jtype(OPCODES['INTER'], offset26=1),
			encode_rtype(OPCODES['XOR'], dest=1, src=2, trgt=3),
			encode_itype(OPCODES['XORI'], dest=1, src=2, imm17=64),
			encode_jtype(OPCODES['RETI'], offset26=16)
]

def main(): 
	buffer = bytearray(FILE_SIZE)
	offset = INSTRUCT_OFFSET
	instructions = [
		encode_rtype(OPCODES["XOR"], dest=10, src=10, trgt=10),
		encode_rtype(OPCODES["XOR"], dest=11, src=11, trgt=11),
		encode_itype(OPCODES["ADDI"], dest=10, src=10, imm17=0x4000),
		encode_itype(OPCODES["ADDI"], dest=11, src=11, imm17=0x4001),
		encode_itype(OPCODES["SBI"], dest=10, src=9, imm17=0),
		encode_rtype(OPCODES["XOR"], dest=9, src=9, trgt=9),
		encode_itype(OPCODES["ADDI"], dest=9, src=9, imm17=0x1),
		encode_itype(OPCODES["SBI"], dest=11, src=9, imm17=0)

		encode_jtype(OPCODES["RETI"], offset26=0)
	]

	"""
	encode_rtype(OPCODES["XOR"], dest=0, src=0, trgt=0),
		encode_rtype(OPCODES["XOR"], dest=1, src=1, trgt=1),
		encode_rtype(OPCODES["XOR"], dest=2, src=2, trgt=2),
		encode_rtype(OPCODES["XOR"], dest=3, src=3, trgt=3),
		encode_itype(OPCODES["ADDI"], dest=0, src=0, imm17=0x61),
		encode_itype(OPCODES["ADDI"], dest=1, src=1, imm17=0x4000),
		encode_itype(OPCODES["ADDI"], dest=2, src=2, imm17=0x4001),
		encode_itype(OPCODES["ADDI"], dest=3, src=3, imm17=0x1),
		encode_itype(OPCODES["SBI"], dest=1, src=0, imm17=0),
		encode_itype(OPCODES["SBI"], dest=2, src=3, imm17=0)
	"""

	print(f"Writing {len(instructions)} instructions to buffer.")
	write_uint32_to_buffer(buffer, offset, b'Hello World\n')
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