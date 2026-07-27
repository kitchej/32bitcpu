# LC-32 - Little Computer 32
## Architecture Overview
### 16 Accessible Registers
| Name | Value | Purpose | Persists After Subroutine Call
| ----------- | ----------- | ----------- | ----------- |
|$R0-$R7|0-7|General Purpose | No
|$RA|8| Subroutine Return Address | No
|$A0-$A2|9-11|Subroutine Argument Registers | Yes
|$V0-$V1|12-13|Subroutine Output Registers | No
|$SP|14| Stack Pointer | Yes
|$SF|15| Stack Frame Pointer | Yes
### Internal Registers
| Name | Purpose 
| ----------- | ----------- |
|PC| Program Counter
|Z| Zero Flag
|P| Positive Flag
|N| Negative Flag
|IR| Instruction Register
|MAR| Memory Address Register|
|MDR| Memory Data Register
### Endianness
Little Endian: the least significant byte of a multi-byte value is stored at the lowest memory address. This applies uniformly to both the instruction word format (see Instruction Formats below) and to data accessed via LW/SW/LWI/SWI.

**Example:** store a 32-bit word `0x12345678` with `SW` at base address `0x1000`:

|Address|0x1000|0x1001|0x1002|0x1003|
| ----------- | ----------- | ----------- | ----------- | ----------- |
|Byte stored|0x78|0x56|0x34|0x12|

The least significant byte (`0x78`) lands at the lowest address (`0x1000`), and the most significant byte (`0x12`) lands at the highest address (`0x1003`).
However, for `LB`/`SB`/`LBI`/`SBI`, this does not matter as a single byte has no internal ordering
### Direct Addressing
256 MB of address space (0x00000000-0x0FFFFFFF) is directly addressable by J-type instructions
#### Memory Map of the  LC-32
|Region|Start|End|Size|
| ----------- | ----------- | ----------- | ----------- |
|Kernel Code|0x00000000-0x00000FFF | 4,096 bytes (4 KB)
|Reserved for I/O|0x00001000-0x00004FFF | 16,384 bytes (16 KB)
|General Purpose|0x00005000-0x0FFFFFFF | 268,414,976 bytes (~256 MB)
## Instruction Formats

### R-TYPE INSTRUCTIONS
**OPCODE** [31-26] | **DEST** [25-22] | **SRC** [21-17] | **TRGT** [16-12] | **Unused** [11-0]
### I-TYPE INSTRUCTIONS
**OPCODE** [31-26] | **DEST** [25-22] | **SRC** [21-18] | **IMM-16** [17-0]
### J-TYPE INSTRUCTIONS
**OPCODE** [26-31] | **OFFSET-26** [0-25]
## Instruction Set
33 of 64 possible 6-bit opcode values are assigned; 31 remain free for future instructions. Ordered by opcode.

| FORMAT | NAME | MNEMONIC | OPCODE | OPERATION | ARG1 | ARG2 | ARG3 | 
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
|R|No Operation|NOP|0x00|No state change||||
|R|Add|ADD|0x01|DEST=SRC+TRGT|DEST|SRC|TRGT
|I|Add Immediate|ADDI|0x02|DEST=SRC+IMM18|DEST|SRC|IMM18
|R|And|AND|0x03|DEST=SRC&TRGT|DEST|SRC|TRGT
|I|And Immediate|ANDI|0x04|DEST=SRC&IMM18|DEST|SRC|IMM18
|J|Branch if Negative|BRN|0x05|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Branch if Positive|BRP|0x06|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Branch if Zero|BRZ|0x07|PC=PC+4+(OFFSET*4)|OFFSET26||
|R|Divide|DIV|0x08|DEST=SRC/TRGT (rounds toward zero)|DEST|SRC|TRGT
|J|Jump and Link|JAL|0x09|PC=PC+4+(OFFSET*4)|OFFSET26||
|J|Jump|JMP|0x0A|PC=PC+4+(OFFSET*4)|OFFSET26||
|R|Jump Register|JR|0x0B|PC=SRC|SRC||
|I|Load Byte|LB|0x0C|DEST=MEM[PC+4+OFFSET]|DEST|OFFSET18|
|I|Load Byte Indirect|LBI|0x0D|DEST=MEM[BASE REG]|DEST|BASE REG|
|I|Left Bit Shift|LBS|0x0E|DEST=SRC<<SHIFT AMT|DEST|SRC|SHIFT AMT
|I|Load Word|LW|0x0F|DEST=MEM[PC+4+(OFFSET*4)]|DEST|OFFSET18|
|I|Load Word Indirect|LWI|0x10|DEST=MEM[BASE REG]|DEST|BASE REG|
|R|Multiply|MULT|0x11|DEST=SRC*TRGT|DEST|SRC|TRGT
|R|Logical Not|NOT|0x12|DEST=~SRC|DEST|SRC|
|R|Inclusive Or|OR|0x13|DEST=SRC\|TRGT|DEST|SRC|TRGT
|I|Inclusive Or Immediate|ORI|0x14|DEST=SRC\|IMM18|DEST|SRC|IMM18
|I|Right Bit Shift|RBS|0x15|DEST=SRC>>SHIFT AMT|DEST|SRC|SHIFT AMT
|J|Return from Subroutine|RET|0x16|PC=$RA||||
|I|Store Byte|SB|0x17|MEM[PC+4+OFFSET]=SRC|SRC|OFFSET18|
|I|Store Byte Indirect|SBI|0x18|MEM[BASE REG]=SRC|SRC|BASE REG|
|R|Subtract|SUB|0x19|DEST=SRC-TRGT|DEST|SRC|TRGT
|I|Subtract Immediate|SUBI|0x1A|DEST=SRC-IMM18|DEST|SRC|IMM18
|I|Store Word|SW|0x1B|MEM[PC+4+(OFFSET*4)]=SRC|SRC|OFFSET18|
|I|Store Word Indirect|SWI|0x1C|MEM[BASE REG]=SRC|SRC|BASE REG|
|J|Trap|TRAP|0x1D|PC=0+(OFFSET*4)|OFFSET26||
|R|Exclusive Or|XOR|0x1E|DEST=SRC^TRGT|DEST|SRC|TRGT
|I|Exclusive Or Immediate|XORI|0x1F|DEST=SRC^IMM18|DEST|SRC|
|R|Halt|HALT|0x20|Stop instruction execution||||