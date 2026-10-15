# LC-32 - Little Computer 32
Designed by Joshua Kitchen

## Architecture Overview

LC-32 is RISC style architecture based on the LC-3 and MIPS architectures

| | |
| ----------- | ----------- |
|**Word Size**|32 Bits|
|**Endianess**|Little|
|**Addressing modes**|PC relative and register indirect|

### Programmable registers
| Name | Value | Purpose | Responsible For Restoring
| ----------- | ----------- | ----------- | ----------- 
|$R0-$R7|0-7| General Purpose | Callee
|$RA|8| Subroutine Return Address | Callee
|$A0-$A2|9-11|Subroutine Argument Registers | Caller
|$V0-$V1|12-13|Subroutine Output Registers | Caller
|$SP|14| Stack Pointer | Callee
|$SF|15| Stack Frame Pointer | Callee
|$T0-$T7| 16-23 | Temporaries | Caller
### Internal Registers
| Name | Value | Purpose 
| -----------|----------- | ----------- |
|$IRA| 26| Interrupt Return Address
|$PC| 27 | Program Counter
|$FLAGS| 28 | BIT 31: Zero, BIT 30: Negative, BIT 29: Positive, BIT 28: D_ZERO, BIT 27: SERVICING_INTERRUPT, BIT 26: HALT
|$IR| 29 | Instruction Register
|$MAR| 30 | Memory Address Register
|$MDR| 31 | Memory Data Register


#### Direct Addressing
128 MB of address space is directly addressable by positive J-type instruction offsets. Offsets represent the number of words before (negative offsets) or after (postive offsets) the PC + 4.


#### Memory Map of the  LC-32


| Region | Range | Size |
|---|---|---|
| Kernel Code | 0x00000000–0x00003FFF | 16,384 bytes (16 KB) |
| Reserved for I/O | 0x00004000–0x00013FFF | 65,536 bytes (64 KB) |
| Software Interrupt Table | 0x00014000–0x00017FFF | 16,384 bytes (16 KB or 4026 4-byte entries) |
| General Purpose RAM | 0x00018000–0x07EFFFFC | 133,070,848 bytes (127 MB) |
| Stack | 0x07F00000–0x07FFFFFC | 1,048,576 bytes (1 MB) |

## Interrupts
Both hardware and software can send interrupts. Interrupt vectors are 26-bit. Vectors 0x00 - 0x1FF (0-511) are reserved for hardware interrupts and the remaning vectors starting at 0x200 are avalible for software interrupts.
The hardware interrupt table starts at the top of kernel ROM at address 0x3FFF and is indexed via *subtracting* the vector from 0x3FFF. The software interrupt table starts at 0x00014000 and is indexed by *adding*. 
There are 4096 possible software interrupts if the table is confined to addresses 0x00014000–0x00017FFF.


***All interrupt handlers must end with RETI to ensure code returns to the correct address and to allow other interrupts to be serviced.***

## The Stack
The global stack starts at 0xFFFFFFFC and is the default value of the $SF and $SP registers. The stack grows downward so the $SP register should be incremented to add space and deincremented to remove space. 

## Instruction Formats


### R-TYPE Instructions

| OPCODE | DEST | SRC | TRGT | SHAMT | UNUSED |
|---|---|---|---|---|---|
| [31-26: 6 bits] | [25-21: 5 bits] | [20-16: 5 bits] | [15-11: 5 bits] | [10-6: 5 bits] | [5-0: 6 bits] |

### I-TYPE Instructions

| OPCODE | DEST | SRC | IMM-17 |
|---|---|---|---|
| [31-26: 6 bits] | [25-21: 5 bits] | [20-16: 5 bits] | [15-0: 16 bits] |

### J-TYPE Instructions

| OPCODE | OFFSET-26 |
|---|---|
| [31-26: 6 bits] | [25-0: 26 bits] |

## Instruction Set

| FORMAT | NAME | MNEMONIC | OPCODE | OPERATION | ARG1 | ARG2 | ARG3 | 
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
|R|No Operation|NOP|0x00|No state change||||
|R|Add|ADD|0x01|DEST=SRC+TRGT|DEST|SRC|TRGT
|I|Add Immediate|ADDI|0x02|DEST=SRC+IMM16|DEST|SRC|imm16
|R|And|AND|0x03|DEST=SRC&TRGT|DEST|SRC|TRGT
|I|And Immediate|ANDI|0x04|DEST=SRC&IMM16|DEST|SRC|imm16
|J|Branch if Negative|BRN|0x05|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Branch if Positive|BRP|0x06|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Branch if Zero|BRZ|0x07|PC=PC+4+(OFFSET*4)|OFFSET26||
|R|Divide|DIV|0x08|DEST=SRC/TRGT (rounds toward zero)|DEST|SRC|TRGT
|J|Jump and Link|JAL|0x09|PC=PC+4+(OFFSET*4); ra=pc + 4|OFFSET26||
|J|Jump|JMP|0x0A|PC=PC+4+(OFFSET*4)|OFFSET26||
|R|Jump and Link Absolute|JAlA|0x0B|PC=SRC + 4; ra=pc = 4|SRC||
|I|Load Byte|LB|0x0C|DEST=MEM[PC+4+OFFSET]|DEST|OFFSET17|
|I|Load Byte Indirect|LBI|0x0D|DEST=MEM[BASE REG]|DEST|BASE REG|
|R|Left Bit Shift|LBS|0x0E|DEST=SRC<<SHIFT AMT|DEST|SRC|SHIFT AMT
|I|Load Word|LW|0x0F|DEST=MEM[PC+4+(OFFSET*4)]|DEST|OFFSET16|
|I|Load Word Indirect|LWI|0x10|DEST=MEM[BASE REG]|DEST|BASE REG|
|R|Multiply|MULT|0x11|DEST=SRC*TRGT|DEST|SRC|TRGT
|R|Logical Not|NOT|0x12|DEST=~SRC|DEST|SRC|
|R|Inclusive Or|OR|0x13|DEST=SRC\|TRGT|DEST|SRC|TRGT
|I|Inclusive Or Immediate|ORI|0x14|DEST=SRC\|imm16|DEST|SRC|imm16
|R|Right Bit Shift|RBS|0x15|DEST=SRC>>SHIFT AMT|DEST|SRC|SHIFT AMT
|J|Return from Subroutine|RET|0x16|PC=$RA||||
|I|Store Byte|SB|0x17|MEM[PC+4+OFFSET]=SRC|SRC|OFFSET17|
|I|Store Byte Indirect|SBI|0x18|MEM[BASE REG]=SRC|SRC|BASE REG|
|R|Subtract|SUB|0x19|DEST=SRC-TRGT|DEST|SRC|TRGT
|I|Subtract Immediate|SUBI|0x1A|DEST=SRC-imm16|DEST|SRC|imm16
|I|Store Word|SW|0x1B|MEM[PC+4+(OFFSET*4)]=SRC|SRC|OFFSET17|
|I|Store Word Indirect|SWI|0x1C|MEM[BASE REG]=SRC|SRC|BASE REG|
|J|Interrupt|INTER|0x1D|PC=*(0x0FFFFFFF - OFFSET26)|OFFSET26||
|R|Exclusive Or|XOR|0x1E|DEST=SRC^TRGT|DEST|SRC|TRGT
|I|Exclusive Or Immediate|XORI|0x1F|DEST=SRC^imm16|DEST|SRC|IMM16
|R|Halt|HALT|0x20|Stop instruction execution||||+96
|J|Return from Interrupt|RETI|0x21|PC=IRA; SERVICING_INTERRUPTS=0||||
