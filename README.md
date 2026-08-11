# LC-32 - Little Computer 32
Written by Joshua Kitchen

## Architecture Overview

LC-32 is RISC style architecture based on the LC-3 and MIPS architectures

|
| ----------- | -----------
|**Word Size**|32 Bits|
|**Endianess**|Little|
|**Addressing modes**|PC relative and register indirect|

### Programmable registers
| Name | Value | Purpose | Persists After Subroutine Call
| ----------- | ----------- | ----------- | ----------- 
|$R0-$R7|0-7|General Purpose | No
|$RA|8| Subroutine Return Address | No
|$A0-$A2|9-11|Subroutine Argument Registers | Yes
|$V0-$V1|12-13|Subroutine Output Registers | No
|$SP|14| Stack Pointer | Yes
|$SF|15| Stack Frame Pointer | Yes
|$R8-$R15|16-23| General Purpose | No
### Internal Registers
| Name | Value | Purpose 
| -----------|----------- | ----------- |
|$IRA| 26| Interrupt Return Addresss
|$PC| 27 | Program Counter
|$FLAGS| 28 | BIT 31: Zero, BIT 30: Negative, BIT 29: Postive, BIT 28: D_ZERO, BIT 27: SERVICING_INTERRUPT, BIT 26: HALT
|$IR| 29 |Instruction Register
|$MAR| 30 |Memory Address Register
|$MDR| 31 |Memory Data Register


#### Direct Addressing
256 MB of address space (0x00000000-0x0FFFFFFF) is directly addressable by J-type instruction offsets
##### Memory Map of the  LC-32
|Region|Range|Size|
| ----------- | ----------- | ----------- | ----------- |
|Kernel Code|0x00000000-0x00003FFF | 16,384 bytes (16 KB)
|Reserved for I/O|0x00004000-0x00013FFF | 65,536 bytes (64 KB)
|General Purpose|0x00014000-0x0FFFFFFF | 268,353,536 bytes (256 MB)
|TOTAL|0x00000000 - 0x0FFFFFFF|268,435,456 bytes (256 MB)|

## Interrupts
Both hardware and software can send interrupts. Interrupt vectors are 32-bit. Vectors 0x00 - 0xFF are reserved for hardware interrupts with the remaining vectors avalible for software interrupts.
The hardware interrupt table starts at the top of kernel ROM at addresss 0x3FFF and grows downward. The software interrupt table starts at the top of General Purpose memory at 0xFFFFFFF and grows downward.
***All interrupt handlers must end with RETI to ensure other interrupts can be serviced!***
## Instruction Formats

### R-TYPE INSTRUCTIONS
**OPCODE** [31-26: 6 bits] | **DEST** [25-21: 5 bits] | **SRC** [21-16: 5 bits] | **TRGT** [15-11: 5 bits] | **Unused** [10-0: 11 bits]

### I-TYPE INSTRUCTIONS
**OPCODE** [31-26: 6 bits] | **DEST** [25-21: 5 bits] | **SRC** [20-16: 5 bits] | **IMM-17** [16-0: 17 bits]

### J-TYPE INSTRUCTIONS
**OPCODE** [31-26: 6 bits] | **OFFSET-26** [25-0: 26 bits]

#### Example encodings

|Assembly|Hex|Binary|
| ----------- | ----------- | ----------- |
|add r0, r1, r2|0x04024000|00000100000000010001000000000000
|addi r0, r1, 4|0x08020004|00001000000000100000000000000100
|jmp 0x10|0x28000010|00101000000000000000000000010000

## Instruction Set
33 of 64 possible 6-bit opcode values are assigned; 31 remain free for future instructions. Ordered by opcode.

| FORMAT | NAME | MNEMONIC | OPCODE | OPERATION | ARG1 | ARG2 | ARG3 | 
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
|R|No Operation|NOP|0x00|No state change||||
|R|Add|ADD|0x01|DEST=SRC+TRGT|DEST|SRC|TRGT
|I|Add Immediate|ADDI|0x02|DEST=SRC+imm17|DEST|SRC|imm17
|R|And|AND|0x03|DEST=SRC&TRGT|DEST|SRC|TRGT
|I|And Immediate|ANDI|0x04|DEST=SRC&imm17|DEST|SRC|imm17
|J|Branch if Negative|BRN|0x05|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Branch if Positive|BRP|0x06|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Branch if Zero|BRZ|0x07|PC=PC+4+(OFFSET*4)|OFFSET26||
|R|Divide|DIV|0x08|DEST=SRC/TRGT (rounds toward zero)|DEST|SRC|TRGT
|J|Jump and Link|JAL|0x09|PC=PC+4+(OFFSET*4); ra=pc + 4|OFFSET26||
|J|Jump|JMP|0x0A|PC=PC+4+(OFFSET*4)|OFFSET26||
|R|Jump and Link Absolute|JAlA|0x0B|PC=SRC + 4; ra=pc = 4|SRC||
|I|Load Byte|LB|0x0C|DEST=MEM[PC+4+OFFSET]|DEST|OFFSET18|
|I|Load Byte Indirect|LBI|0x0D|DEST=MEM[BASE REG]|DEST|BASE REG|
|I|Left Bit Shift|LBS|0x0E|DEST=SRC<<SHIFT AMT|DEST|SRC|SHIFT AMT
|I|Load Word|LW|0x0F|DEST=MEM[PC+4+(OFFSET*4)]|DEST|OFFSET18|
|I|Load Word Indirect|LWI|0x10|DEST=MEM[BASE REG]|DEST|BASE REG|
|R|Multiply|MULT|0x11|DEST=SRC*TRGT|DEST|SRC|TRGT
|R|Logical Not|NOT|0x12|DEST=~SRC|DEST|SRC|
|R|Inclusive Or|OR|0x13|DEST=SRC\|TRGT|DEST|SRC|TRGT
|I|Inclusive Or Immediate|ORI|0x14|DEST=SRC\|imm17|DEST|SRC|imm17
|I|Right Bit Shift|RBS|0x15|DEST=SRC>>SHIFT AMT|DEST|SRC|SHIFT AMT
|J|Return from Subroutine|RET|0x16|PC=$RA||||
|I|Store Byte|SB|0x17|MEM[PC+4+OFFSET]=SRC|SRC|OFFSET18|
|I|Store Byte Indirect|SBI|0x18|MEM[BASE REG]=SRC|SRC|BASE REG|
|R|Subtract|SUB|0x19|DEST=SRC-TRGT|DEST|SRC|TRGT
|I|Subtract Immediate|SUBI|0x1A|DEST=SRC-imm17|DEST|SRC|imm17
|I|Store Word|SW|0x1B|MEM[PC+4+(OFFSET*4)]=SRC|SRC|OFFSET18|
|I|Store Word Indirect|SWI|0x1C|MEM[BASE REG]=SRC|SRC|BASE REG|
|J|Interrupt|INTER|0x1D|PC=*(0x0FFFFFFF - OFFSET26)|OFFSET26||
|R|Exclusive Or|XOR|0x1E|DEST=SRC^TRGT|DEST|SRC|TRGT
|I|Exclusive Or Immediate|XORI|0x1F|DEST=SRC^imm17|DEST|SRC|
|R|Halt|HALT|0x20|Stop instruction execution||||+96
|J|Return from Interrupt|RETI|0x20|PC=RA; SERVICING_INTERRUPTS=0||||