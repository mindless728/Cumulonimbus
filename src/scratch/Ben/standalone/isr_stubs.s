# 1 "isr_stubs.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "isr_stubs.S"
# 20 "isr_stubs.S"
# 1 "./bootstrap.h" 1
# 21 "isr_stubs.S" 2







 .text
# 49 "isr_stubs.S"
 .globl __isr_table
 .globl __isr_restore
# 67 "isr_stubs.S"
isr_save:
# 80 "isr_stubs.S"
 pusha
 pushl %ds
 pushl %es
 pushl %fs
 pushl %gs
 pushl %ss
# 97 "isr_stubs.S"
 movl 52(%esp),%eax
 movl 56(%esp),%ebx
 pushl %ebx
 pushl %eax




 movl __isr_table(,%eax,4),%ebx
 call *%ebx
 addl $8,%esp





__isr_restore:
# 139 "isr_stubs.S"
 popl %ss
 popl %gs
 popl %fs
 popl %es
 popl %ds
 popa
 addl $8, %esp
 iret
# 165 "isr_stubs.S"
.globl __isr_0x00 ; __isr_0x00: ; pushl $0 ; pushl $0x00 ; jmp isr_save; .globl __isr_0x01 ; __isr_0x01: ; pushl $0 ; pushl $0x01 ; jmp isr_save; .globl __isr_0x02 ; __isr_0x02: ; pushl $0 ; pushl $0x02 ; jmp isr_save; .globl __isr_0x03 ; __isr_0x03: ; pushl $0 ; pushl $0x03 ; jmp isr_save;
.globl __isr_0x04 ; __isr_0x04: ; pushl $0 ; pushl $0x04 ; jmp isr_save; .globl __isr_0x05 ; __isr_0x05: ; pushl $0 ; pushl $0x05 ; jmp isr_save; .globl __isr_0x06 ; __isr_0x06: ; pushl $0 ; pushl $0x06 ; jmp isr_save; .globl __isr_0x07 ; __isr_0x07: ; pushl $0 ; pushl $0x07 ; jmp isr_save;
.globl __isr_0x08 ; __isr_0x08: ; pushl $0x08 ; jmp isr_save; .globl __isr_0x09 ; __isr_0x09: ; pushl $0 ; pushl $0x09 ; jmp isr_save; .globl __isr_0x0a ; __isr_0x0a: ; pushl $0x0a ; jmp isr_save; .globl __isr_0x0b ; __isr_0x0b: ; pushl $0x0b ; jmp isr_save;
.globl __isr_0x0c ; __isr_0x0c: ; pushl $0x0c ; jmp isr_save; .globl __isr_0x0d ; __isr_0x0d: ; pushl $0x0d ; jmp isr_save; .globl __isr_0x0e ; __isr_0x0e: ; pushl $0x0e ; jmp isr_save; .globl __isr_0x0f ; __isr_0x0f: ; pushl $0 ; pushl $0x0f ; jmp isr_save;
.globl __isr_0x10 ; __isr_0x10: ; pushl $0 ; pushl $0x10 ; jmp isr_save; .globl __isr_0x11 ; __isr_0x11: ; pushl $0x11 ; jmp isr_save; .globl __isr_0x12 ; __isr_0x12: ; pushl $0 ; pushl $0x12 ; jmp isr_save; .globl __isr_0x13 ; __isr_0x13: ; pushl $0 ; pushl $0x13 ; jmp isr_save;
.globl __isr_0x14 ; __isr_0x14: ; pushl $0 ; pushl $0x14 ; jmp isr_save; .globl __isr_0x15 ; __isr_0x15: ; pushl $0 ; pushl $0x15 ; jmp isr_save; .globl __isr_0x16 ; __isr_0x16: ; pushl $0 ; pushl $0x16 ; jmp isr_save; .globl __isr_0x17 ; __isr_0x17: ; pushl $0 ; pushl $0x17 ; jmp isr_save;
.globl __isr_0x18 ; __isr_0x18: ; pushl $0 ; pushl $0x18 ; jmp isr_save; .globl __isr_0x19 ; __isr_0x19: ; pushl $0 ; pushl $0x19 ; jmp isr_save; .globl __isr_0x1a ; __isr_0x1a: ; pushl $0 ; pushl $0x1a ; jmp isr_save; .globl __isr_0x1b ; __isr_0x1b: ; pushl $0 ; pushl $0x1b ; jmp isr_save;
.globl __isr_0x1c ; __isr_0x1c: ; pushl $0 ; pushl $0x1c ; jmp isr_save; .globl __isr_0x1d ; __isr_0x1d: ; pushl $0 ; pushl $0x1d ; jmp isr_save; .globl __isr_0x1e ; __isr_0x1e: ; pushl $0 ; pushl $0x1e ; jmp isr_save; .globl __isr_0x1f ; __isr_0x1f: ; pushl $0 ; pushl $0x1f ; jmp isr_save;
.globl __isr_0x20 ; __isr_0x20: ; pushl $0 ; pushl $0x20 ; jmp isr_save; .globl __isr_0x21 ; __isr_0x21: ; pushl $0 ; pushl $0x21 ; jmp isr_save; .globl __isr_0x22 ; __isr_0x22: ; pushl $0 ; pushl $0x22 ; jmp isr_save; .globl __isr_0x23 ; __isr_0x23: ; pushl $0 ; pushl $0x23 ; jmp isr_save;
.globl __isr_0x24 ; __isr_0x24: ; pushl $0 ; pushl $0x24 ; jmp isr_save; .globl __isr_0x25 ; __isr_0x25: ; pushl $0 ; pushl $0x25 ; jmp isr_save; .globl __isr_0x26 ; __isr_0x26: ; pushl $0 ; pushl $0x26 ; jmp isr_save; .globl __isr_0x27 ; __isr_0x27: ; pushl $0 ; pushl $0x27 ; jmp isr_save;
.globl __isr_0x28 ; __isr_0x28: ; pushl $0 ; pushl $0x28 ; jmp isr_save; .globl __isr_0x29 ; __isr_0x29: ; pushl $0 ; pushl $0x29 ; jmp isr_save; .globl __isr_0x2a ; __isr_0x2a: ; pushl $0 ; pushl $0x2a ; jmp isr_save; .globl __isr_0x2b ; __isr_0x2b: ; pushl $0 ; pushl $0x2b ; jmp isr_save;
.globl __isr_0x2c ; __isr_0x2c: ; pushl $0 ; pushl $0x2c ; jmp isr_save; .globl __isr_0x2d ; __isr_0x2d: ; pushl $0 ; pushl $0x2d ; jmp isr_save; .globl __isr_0x2e ; __isr_0x2e: ; pushl $0 ; pushl $0x2e ; jmp isr_save; .globl __isr_0x2f ; __isr_0x2f: ; pushl $0 ; pushl $0x2f ; jmp isr_save;
.globl __isr_0x30 ; __isr_0x30: ; pushl $0 ; pushl $0x30 ; jmp isr_save; .globl __isr_0x31 ; __isr_0x31: ; pushl $0 ; pushl $0x31 ; jmp isr_save; .globl __isr_0x32 ; __isr_0x32: ; pushl $0 ; pushl $0x32 ; jmp isr_save; .globl __isr_0x33 ; __isr_0x33: ; pushl $0 ; pushl $0x33 ; jmp isr_save;
.globl __isr_0x34 ; __isr_0x34: ; pushl $0 ; pushl $0x34 ; jmp isr_save; .globl __isr_0x35 ; __isr_0x35: ; pushl $0 ; pushl $0x35 ; jmp isr_save; .globl __isr_0x36 ; __isr_0x36: ; pushl $0 ; pushl $0x36 ; jmp isr_save; .globl __isr_0x37 ; __isr_0x37: ; pushl $0 ; pushl $0x37 ; jmp isr_save;
.globl __isr_0x38 ; __isr_0x38: ; pushl $0 ; pushl $0x38 ; jmp isr_save; .globl __isr_0x39 ; __isr_0x39: ; pushl $0 ; pushl $0x39 ; jmp isr_save; .globl __isr_0x3a ; __isr_0x3a: ; pushl $0 ; pushl $0x3a ; jmp isr_save; .globl __isr_0x3b ; __isr_0x3b: ; pushl $0 ; pushl $0x3b ; jmp isr_save;
.globl __isr_0x3c ; __isr_0x3c: ; pushl $0 ; pushl $0x3c ; jmp isr_save; .globl __isr_0x3d ; __isr_0x3d: ; pushl $0 ; pushl $0x3d ; jmp isr_save; .globl __isr_0x3e ; __isr_0x3e: ; pushl $0 ; pushl $0x3e ; jmp isr_save; .globl __isr_0x3f ; __isr_0x3f: ; pushl $0 ; pushl $0x3f ; jmp isr_save;
.globl __isr_0x40 ; __isr_0x40: ; pushl $0 ; pushl $0x40 ; jmp isr_save; .globl __isr_0x41 ; __isr_0x41: ; pushl $0 ; pushl $0x41 ; jmp isr_save; .globl __isr_0x42 ; __isr_0x42: ; pushl $0 ; pushl $0x42 ; jmp isr_save; .globl __isr_0x43 ; __isr_0x43: ; pushl $0 ; pushl $0x43 ; jmp isr_save;
.globl __isr_0x44 ; __isr_0x44: ; pushl $0 ; pushl $0x44 ; jmp isr_save; .globl __isr_0x45 ; __isr_0x45: ; pushl $0 ; pushl $0x45 ; jmp isr_save; .globl __isr_0x46 ; __isr_0x46: ; pushl $0 ; pushl $0x46 ; jmp isr_save; .globl __isr_0x47 ; __isr_0x47: ; pushl $0 ; pushl $0x47 ; jmp isr_save;
.globl __isr_0x48 ; __isr_0x48: ; pushl $0 ; pushl $0x48 ; jmp isr_save; .globl __isr_0x49 ; __isr_0x49: ; pushl $0 ; pushl $0x49 ; jmp isr_save; .globl __isr_0x4a ; __isr_0x4a: ; pushl $0 ; pushl $0x4a ; jmp isr_save; .globl __isr_0x4b ; __isr_0x4b: ; pushl $0 ; pushl $0x4b ; jmp isr_save;
.globl __isr_0x4c ; __isr_0x4c: ; pushl $0 ; pushl $0x4c ; jmp isr_save; .globl __isr_0x4d ; __isr_0x4d: ; pushl $0 ; pushl $0x4d ; jmp isr_save; .globl __isr_0x4e ; __isr_0x4e: ; pushl $0 ; pushl $0x4e ; jmp isr_save; .globl __isr_0x4f ; __isr_0x4f: ; pushl $0 ; pushl $0x4f ; jmp isr_save;
.globl __isr_0x50 ; __isr_0x50: ; pushl $0 ; pushl $0x50 ; jmp isr_save; .globl __isr_0x51 ; __isr_0x51: ; pushl $0 ; pushl $0x51 ; jmp isr_save; .globl __isr_0x52 ; __isr_0x52: ; pushl $0 ; pushl $0x52 ; jmp isr_save; .globl __isr_0x53 ; __isr_0x53: ; pushl $0 ; pushl $0x53 ; jmp isr_save;
.globl __isr_0x54 ; __isr_0x54: ; pushl $0 ; pushl $0x54 ; jmp isr_save; .globl __isr_0x55 ; __isr_0x55: ; pushl $0 ; pushl $0x55 ; jmp isr_save; .globl __isr_0x56 ; __isr_0x56: ; pushl $0 ; pushl $0x56 ; jmp isr_save; .globl __isr_0x57 ; __isr_0x57: ; pushl $0 ; pushl $0x57 ; jmp isr_save;
.globl __isr_0x58 ; __isr_0x58: ; pushl $0 ; pushl $0x58 ; jmp isr_save; .globl __isr_0x59 ; __isr_0x59: ; pushl $0 ; pushl $0x59 ; jmp isr_save; .globl __isr_0x5a ; __isr_0x5a: ; pushl $0 ; pushl $0x5a ; jmp isr_save; .globl __isr_0x5b ; __isr_0x5b: ; pushl $0 ; pushl $0x5b ; jmp isr_save;
.globl __isr_0x5c ; __isr_0x5c: ; pushl $0 ; pushl $0x5c ; jmp isr_save; .globl __isr_0x5d ; __isr_0x5d: ; pushl $0 ; pushl $0x5d ; jmp isr_save; .globl __isr_0x5e ; __isr_0x5e: ; pushl $0 ; pushl $0x5e ; jmp isr_save; .globl __isr_0x5f ; __isr_0x5f: ; pushl $0 ; pushl $0x5f ; jmp isr_save;
.globl __isr_0x60 ; __isr_0x60: ; pushl $0 ; pushl $0x60 ; jmp isr_save; .globl __isr_0x61 ; __isr_0x61: ; pushl $0 ; pushl $0x61 ; jmp isr_save; .globl __isr_0x62 ; __isr_0x62: ; pushl $0 ; pushl $0x62 ; jmp isr_save; .globl __isr_0x63 ; __isr_0x63: ; pushl $0 ; pushl $0x63 ; jmp isr_save;
.globl __isr_0x64 ; __isr_0x64: ; pushl $0 ; pushl $0x64 ; jmp isr_save; .globl __isr_0x65 ; __isr_0x65: ; pushl $0 ; pushl $0x65 ; jmp isr_save; .globl __isr_0x66 ; __isr_0x66: ; pushl $0 ; pushl $0x66 ; jmp isr_save; .globl __isr_0x67 ; __isr_0x67: ; pushl $0 ; pushl $0x67 ; jmp isr_save;
.globl __isr_0x68 ; __isr_0x68: ; pushl $0 ; pushl $0x68 ; jmp isr_save; .globl __isr_0x69 ; __isr_0x69: ; pushl $0 ; pushl $0x69 ; jmp isr_save; .globl __isr_0x6a ; __isr_0x6a: ; pushl $0 ; pushl $0x6a ; jmp isr_save; .globl __isr_0x6b ; __isr_0x6b: ; pushl $0 ; pushl $0x6b ; jmp isr_save;
.globl __isr_0x6c ; __isr_0x6c: ; pushl $0 ; pushl $0x6c ; jmp isr_save; .globl __isr_0x6d ; __isr_0x6d: ; pushl $0 ; pushl $0x6d ; jmp isr_save; .globl __isr_0x6e ; __isr_0x6e: ; pushl $0 ; pushl $0x6e ; jmp isr_save; .globl __isr_0x6f ; __isr_0x6f: ; pushl $0 ; pushl $0x6f ; jmp isr_save;
.globl __isr_0x70 ; __isr_0x70: ; pushl $0 ; pushl $0x70 ; jmp isr_save; .globl __isr_0x71 ; __isr_0x71: ; pushl $0 ; pushl $0x71 ; jmp isr_save; .globl __isr_0x72 ; __isr_0x72: ; pushl $0 ; pushl $0x72 ; jmp isr_save; .globl __isr_0x73 ; __isr_0x73: ; pushl $0 ; pushl $0x73 ; jmp isr_save;
.globl __isr_0x74 ; __isr_0x74: ; pushl $0 ; pushl $0x74 ; jmp isr_save; .globl __isr_0x75 ; __isr_0x75: ; pushl $0 ; pushl $0x75 ; jmp isr_save; .globl __isr_0x76 ; __isr_0x76: ; pushl $0 ; pushl $0x76 ; jmp isr_save; .globl __isr_0x77 ; __isr_0x77: ; pushl $0 ; pushl $0x77 ; jmp isr_save;
.globl __isr_0x78 ; __isr_0x78: ; pushl $0 ; pushl $0x78 ; jmp isr_save; .globl __isr_0x79 ; __isr_0x79: ; pushl $0 ; pushl $0x79 ; jmp isr_save; .globl __isr_0x7a ; __isr_0x7a: ; pushl $0 ; pushl $0x7a ; jmp isr_save; .globl __isr_0x7b ; __isr_0x7b: ; pushl $0 ; pushl $0x7b ; jmp isr_save;
.globl __isr_0x7c ; __isr_0x7c: ; pushl $0 ; pushl $0x7c ; jmp isr_save; .globl __isr_0x7d ; __isr_0x7d: ; pushl $0 ; pushl $0x7d ; jmp isr_save; .globl __isr_0x7e ; __isr_0x7e: ; pushl $0 ; pushl $0x7e ; jmp isr_save; .globl __isr_0x7f ; __isr_0x7f: ; pushl $0 ; pushl $0x7f ; jmp isr_save;
.globl __isr_0x80 ; __isr_0x80: ; pushl $0 ; pushl $0x80 ; jmp isr_save; .globl __isr_0x81 ; __isr_0x81: ; pushl $0 ; pushl $0x81 ; jmp isr_save; .globl __isr_0x82 ; __isr_0x82: ; pushl $0 ; pushl $0x82 ; jmp isr_save; .globl __isr_0x83 ; __isr_0x83: ; pushl $0 ; pushl $0x83 ; jmp isr_save;
.globl __isr_0x84 ; __isr_0x84: ; pushl $0 ; pushl $0x84 ; jmp isr_save; .globl __isr_0x85 ; __isr_0x85: ; pushl $0 ; pushl $0x85 ; jmp isr_save; .globl __isr_0x86 ; __isr_0x86: ; pushl $0 ; pushl $0x86 ; jmp isr_save; .globl __isr_0x87 ; __isr_0x87: ; pushl $0 ; pushl $0x87 ; jmp isr_save;
.globl __isr_0x88 ; __isr_0x88: ; pushl $0 ; pushl $0x88 ; jmp isr_save; .globl __isr_0x89 ; __isr_0x89: ; pushl $0 ; pushl $0x89 ; jmp isr_save; .globl __isr_0x8a ; __isr_0x8a: ; pushl $0 ; pushl $0x8a ; jmp isr_save; .globl __isr_0x8b ; __isr_0x8b: ; pushl $0 ; pushl $0x8b ; jmp isr_save;
.globl __isr_0x8c ; __isr_0x8c: ; pushl $0 ; pushl $0x8c ; jmp isr_save; .globl __isr_0x8d ; __isr_0x8d: ; pushl $0 ; pushl $0x8d ; jmp isr_save; .globl __isr_0x8e ; __isr_0x8e: ; pushl $0 ; pushl $0x8e ; jmp isr_save; .globl __isr_0x8f ; __isr_0x8f: ; pushl $0 ; pushl $0x8f ; jmp isr_save;
.globl __isr_0x90 ; __isr_0x90: ; pushl $0 ; pushl $0x90 ; jmp isr_save; .globl __isr_0x91 ; __isr_0x91: ; pushl $0 ; pushl $0x91 ; jmp isr_save; .globl __isr_0x92 ; __isr_0x92: ; pushl $0 ; pushl $0x92 ; jmp isr_save; .globl __isr_0x93 ; __isr_0x93: ; pushl $0 ; pushl $0x93 ; jmp isr_save;
.globl __isr_0x94 ; __isr_0x94: ; pushl $0 ; pushl $0x94 ; jmp isr_save; .globl __isr_0x95 ; __isr_0x95: ; pushl $0 ; pushl $0x95 ; jmp isr_save; .globl __isr_0x96 ; __isr_0x96: ; pushl $0 ; pushl $0x96 ; jmp isr_save; .globl __isr_0x97 ; __isr_0x97: ; pushl $0 ; pushl $0x97 ; jmp isr_save;
.globl __isr_0x98 ; __isr_0x98: ; pushl $0 ; pushl $0x98 ; jmp isr_save; .globl __isr_0x99 ; __isr_0x99: ; pushl $0 ; pushl $0x99 ; jmp isr_save; .globl __isr_0x9a ; __isr_0x9a: ; pushl $0 ; pushl $0x9a ; jmp isr_save; .globl __isr_0x9b ; __isr_0x9b: ; pushl $0 ; pushl $0x9b ; jmp isr_save;
.globl __isr_0x9c ; __isr_0x9c: ; pushl $0 ; pushl $0x9c ; jmp isr_save; .globl __isr_0x9d ; __isr_0x9d: ; pushl $0 ; pushl $0x9d ; jmp isr_save; .globl __isr_0x9e ; __isr_0x9e: ; pushl $0 ; pushl $0x9e ; jmp isr_save; .globl __isr_0x9f ; __isr_0x9f: ; pushl $0 ; pushl $0x9f ; jmp isr_save;
.globl __isr_0xa0 ; __isr_0xa0: ; pushl $0 ; pushl $0xa0 ; jmp isr_save; .globl __isr_0xa1 ; __isr_0xa1: ; pushl $0 ; pushl $0xa1 ; jmp isr_save; .globl __isr_0xa2 ; __isr_0xa2: ; pushl $0 ; pushl $0xa2 ; jmp isr_save; .globl __isr_0xa3 ; __isr_0xa3: ; pushl $0 ; pushl $0xa3 ; jmp isr_save;
.globl __isr_0xa4 ; __isr_0xa4: ; pushl $0 ; pushl $0xa4 ; jmp isr_save; .globl __isr_0xa5 ; __isr_0xa5: ; pushl $0 ; pushl $0xa5 ; jmp isr_save; .globl __isr_0xa6 ; __isr_0xa6: ; pushl $0 ; pushl $0xa6 ; jmp isr_save; .globl __isr_0xa7 ; __isr_0xa7: ; pushl $0 ; pushl $0xa7 ; jmp isr_save;
.globl __isr_0xa8 ; __isr_0xa8: ; pushl $0 ; pushl $0xa8 ; jmp isr_save; .globl __isr_0xa9 ; __isr_0xa9: ; pushl $0 ; pushl $0xa9 ; jmp isr_save; .globl __isr_0xaa ; __isr_0xaa: ; pushl $0 ; pushl $0xaa ; jmp isr_save; .globl __isr_0xab ; __isr_0xab: ; pushl $0 ; pushl $0xab ; jmp isr_save;
.globl __isr_0xac ; __isr_0xac: ; pushl $0 ; pushl $0xac ; jmp isr_save; .globl __isr_0xad ; __isr_0xad: ; pushl $0 ; pushl $0xad ; jmp isr_save; .globl __isr_0xae ; __isr_0xae: ; pushl $0 ; pushl $0xae ; jmp isr_save; .globl __isr_0xaf ; __isr_0xaf: ; pushl $0 ; pushl $0xaf ; jmp isr_save;
.globl __isr_0xb0 ; __isr_0xb0: ; pushl $0 ; pushl $0xb0 ; jmp isr_save; .globl __isr_0xb1 ; __isr_0xb1: ; pushl $0 ; pushl $0xb1 ; jmp isr_save; .globl __isr_0xb2 ; __isr_0xb2: ; pushl $0 ; pushl $0xb2 ; jmp isr_save; .globl __isr_0xb3 ; __isr_0xb3: ; pushl $0 ; pushl $0xb3 ; jmp isr_save;
.globl __isr_0xb4 ; __isr_0xb4: ; pushl $0 ; pushl $0xb4 ; jmp isr_save; .globl __isr_0xb5 ; __isr_0xb5: ; pushl $0 ; pushl $0xb5 ; jmp isr_save; .globl __isr_0xb6 ; __isr_0xb6: ; pushl $0 ; pushl $0xb6 ; jmp isr_save; .globl __isr_0xb7 ; __isr_0xb7: ; pushl $0 ; pushl $0xb7 ; jmp isr_save;
.globl __isr_0xb8 ; __isr_0xb8: ; pushl $0 ; pushl $0xb8 ; jmp isr_save; .globl __isr_0xb9 ; __isr_0xb9: ; pushl $0 ; pushl $0xb9 ; jmp isr_save; .globl __isr_0xba ; __isr_0xba: ; pushl $0 ; pushl $0xba ; jmp isr_save; .globl __isr_0xbb ; __isr_0xbb: ; pushl $0 ; pushl $0xbb ; jmp isr_save;
.globl __isr_0xbc ; __isr_0xbc: ; pushl $0 ; pushl $0xbc ; jmp isr_save; .globl __isr_0xbd ; __isr_0xbd: ; pushl $0 ; pushl $0xbd ; jmp isr_save; .globl __isr_0xbe ; __isr_0xbe: ; pushl $0 ; pushl $0xbe ; jmp isr_save; .globl __isr_0xbf ; __isr_0xbf: ; pushl $0 ; pushl $0xbf ; jmp isr_save;
.globl __isr_0xc0 ; __isr_0xc0: ; pushl $0 ; pushl $0xc0 ; jmp isr_save; .globl __isr_0xc1 ; __isr_0xc1: ; pushl $0 ; pushl $0xc1 ; jmp isr_save; .globl __isr_0xc2 ; __isr_0xc2: ; pushl $0 ; pushl $0xc2 ; jmp isr_save; .globl __isr_0xc3 ; __isr_0xc3: ; pushl $0 ; pushl $0xc3 ; jmp isr_save;
.globl __isr_0xc4 ; __isr_0xc4: ; pushl $0 ; pushl $0xc4 ; jmp isr_save; .globl __isr_0xc5 ; __isr_0xc5: ; pushl $0 ; pushl $0xc5 ; jmp isr_save; .globl __isr_0xc6 ; __isr_0xc6: ; pushl $0 ; pushl $0xc6 ; jmp isr_save; .globl __isr_0xc7 ; __isr_0xc7: ; pushl $0 ; pushl $0xc7 ; jmp isr_save;
.globl __isr_0xc8 ; __isr_0xc8: ; pushl $0 ; pushl $0xc8 ; jmp isr_save; .globl __isr_0xc9 ; __isr_0xc9: ; pushl $0 ; pushl $0xc9 ; jmp isr_save; .globl __isr_0xca ; __isr_0xca: ; pushl $0 ; pushl $0xca ; jmp isr_save; .globl __isr_0xcb ; __isr_0xcb: ; pushl $0 ; pushl $0xcb ; jmp isr_save;
.globl __isr_0xcc ; __isr_0xcc: ; pushl $0 ; pushl $0xcc ; jmp isr_save; .globl __isr_0xcd ; __isr_0xcd: ; pushl $0 ; pushl $0xcd ; jmp isr_save; .globl __isr_0xce ; __isr_0xce: ; pushl $0 ; pushl $0xce ; jmp isr_save; .globl __isr_0xcf ; __isr_0xcf: ; pushl $0 ; pushl $0xcf ; jmp isr_save;
.globl __isr_0xd0 ; __isr_0xd0: ; pushl $0 ; pushl $0xd0 ; jmp isr_save; .globl __isr_0xd1 ; __isr_0xd1: ; pushl $0 ; pushl $0xd1 ; jmp isr_save; .globl __isr_0xd2 ; __isr_0xd2: ; pushl $0 ; pushl $0xd2 ; jmp isr_save; .globl __isr_0xd3 ; __isr_0xd3: ; pushl $0 ; pushl $0xd3 ; jmp isr_save;
.globl __isr_0xd4 ; __isr_0xd4: ; pushl $0 ; pushl $0xd4 ; jmp isr_save; .globl __isr_0xd5 ; __isr_0xd5: ; pushl $0 ; pushl $0xd5 ; jmp isr_save; .globl __isr_0xd6 ; __isr_0xd6: ; pushl $0 ; pushl $0xd6 ; jmp isr_save; .globl __isr_0xd7 ; __isr_0xd7: ; pushl $0 ; pushl $0xd7 ; jmp isr_save;
.globl __isr_0xd8 ; __isr_0xd8: ; pushl $0 ; pushl $0xd8 ; jmp isr_save; .globl __isr_0xd9 ; __isr_0xd9: ; pushl $0 ; pushl $0xd9 ; jmp isr_save; .globl __isr_0xda ; __isr_0xda: ; pushl $0 ; pushl $0xda ; jmp isr_save; .globl __isr_0xdb ; __isr_0xdb: ; pushl $0 ; pushl $0xdb ; jmp isr_save;
.globl __isr_0xdc ; __isr_0xdc: ; pushl $0 ; pushl $0xdc ; jmp isr_save; .globl __isr_0xdd ; __isr_0xdd: ; pushl $0 ; pushl $0xdd ; jmp isr_save; .globl __isr_0xde ; __isr_0xde: ; pushl $0 ; pushl $0xde ; jmp isr_save; .globl __isr_0xdf ; __isr_0xdf: ; pushl $0 ; pushl $0xdf ; jmp isr_save;
.globl __isr_0xe0 ; __isr_0xe0: ; pushl $0 ; pushl $0xe0 ; jmp isr_save; .globl __isr_0xe1 ; __isr_0xe1: ; pushl $0 ; pushl $0xe1 ; jmp isr_save; .globl __isr_0xe2 ; __isr_0xe2: ; pushl $0 ; pushl $0xe2 ; jmp isr_save; .globl __isr_0xe3 ; __isr_0xe3: ; pushl $0 ; pushl $0xe3 ; jmp isr_save;
.globl __isr_0xe4 ; __isr_0xe4: ; pushl $0 ; pushl $0xe4 ; jmp isr_save; .globl __isr_0xe5 ; __isr_0xe5: ; pushl $0 ; pushl $0xe5 ; jmp isr_save; .globl __isr_0xe6 ; __isr_0xe6: ; pushl $0 ; pushl $0xe6 ; jmp isr_save; .globl __isr_0xe7 ; __isr_0xe7: ; pushl $0 ; pushl $0xe7 ; jmp isr_save;
.globl __isr_0xe8 ; __isr_0xe8: ; pushl $0 ; pushl $0xe8 ; jmp isr_save; .globl __isr_0xe9 ; __isr_0xe9: ; pushl $0 ; pushl $0xe9 ; jmp isr_save; .globl __isr_0xea ; __isr_0xea: ; pushl $0 ; pushl $0xea ; jmp isr_save; .globl __isr_0xeb ; __isr_0xeb: ; pushl $0 ; pushl $0xeb ; jmp isr_save;
.globl __isr_0xec ; __isr_0xec: ; pushl $0 ; pushl $0xec ; jmp isr_save; .globl __isr_0xed ; __isr_0xed: ; pushl $0 ; pushl $0xed ; jmp isr_save; .globl __isr_0xee ; __isr_0xee: ; pushl $0 ; pushl $0xee ; jmp isr_save; .globl __isr_0xef ; __isr_0xef: ; pushl $0 ; pushl $0xef ; jmp isr_save;
.globl __isr_0xf0 ; __isr_0xf0: ; pushl $0 ; pushl $0xf0 ; jmp isr_save; .globl __isr_0xf1 ; __isr_0xf1: ; pushl $0 ; pushl $0xf1 ; jmp isr_save; .globl __isr_0xf2 ; __isr_0xf2: ; pushl $0 ; pushl $0xf2 ; jmp isr_save; .globl __isr_0xf3 ; __isr_0xf3: ; pushl $0 ; pushl $0xf3 ; jmp isr_save;
.globl __isr_0xf4 ; __isr_0xf4: ; pushl $0 ; pushl $0xf4 ; jmp isr_save; .globl __isr_0xf5 ; __isr_0xf5: ; pushl $0 ; pushl $0xf5 ; jmp isr_save; .globl __isr_0xf6 ; __isr_0xf6: ; pushl $0 ; pushl $0xf6 ; jmp isr_save; .globl __isr_0xf7 ; __isr_0xf7: ; pushl $0 ; pushl $0xf7 ; jmp isr_save;
.globl __isr_0xf8 ; __isr_0xf8: ; pushl $0 ; pushl $0xf8 ; jmp isr_save; .globl __isr_0xf9 ; __isr_0xf9: ; pushl $0 ; pushl $0xf9 ; jmp isr_save; .globl __isr_0xfa ; __isr_0xfa: ; pushl $0 ; pushl $0xfa ; jmp isr_save; .globl __isr_0xfb ; __isr_0xfb: ; pushl $0 ; pushl $0xfb ; jmp isr_save;
.globl __isr_0xfc ; __isr_0xfc: ; pushl $0 ; pushl $0xfc ; jmp isr_save; .globl __isr_0xfd ; __isr_0xfd: ; pushl $0 ; pushl $0xfd ; jmp isr_save; .globl __isr_0xfe ; __isr_0xfe: ; pushl $0 ; pushl $0xfe ; jmp isr_save; .globl __isr_0xff ; __isr_0xff: ; pushl $0 ; pushl $0xff ; jmp isr_save;

 .data






 .globl __isr_stub_table
__isr_stub_table:
 .long __isr_0x00, __isr_0x01, __isr_0x02, __isr_0x03
 .long __isr_0x04, __isr_0x05, __isr_0x06, __isr_0x07
 .long __isr_0x08, __isr_0x09, __isr_0x0a, __isr_0x0b
 .long __isr_0x0c, __isr_0x0d, __isr_0x0e, __isr_0x0f
 .long __isr_0x10, __isr_0x11, __isr_0x12, __isr_0x13
 .long __isr_0x14, __isr_0x15, __isr_0x16, __isr_0x17
 .long __isr_0x18, __isr_0x19, __isr_0x1a, __isr_0x1b
 .long __isr_0x1c, __isr_0x1d, __isr_0x1e, __isr_0x1f
 .long __isr_0x20, __isr_0x21, __isr_0x22, __isr_0x23
 .long __isr_0x24, __isr_0x25, __isr_0x26, __isr_0x27
 .long __isr_0x28, __isr_0x29, __isr_0x2a, __isr_0x2b
 .long __isr_0x2c, __isr_0x2d, __isr_0x2e, __isr_0x2f
 .long __isr_0x30, __isr_0x31, __isr_0x32, __isr_0x33
 .long __isr_0x34, __isr_0x35, __isr_0x36, __isr_0x37
 .long __isr_0x38, __isr_0x39, __isr_0x3a, __isr_0x3b
 .long __isr_0x3c, __isr_0x3d, __isr_0x3e, __isr_0x3f
 .long __isr_0x40, __isr_0x41, __isr_0x42, __isr_0x43
 .long __isr_0x44, __isr_0x45, __isr_0x46, __isr_0x47
 .long __isr_0x48, __isr_0x49, __isr_0x4a, __isr_0x4b
 .long __isr_0x4c, __isr_0x4d, __isr_0x4e, __isr_0x4f
 .long __isr_0x50, __isr_0x51, __isr_0x52, __isr_0x53
 .long __isr_0x54, __isr_0x55, __isr_0x56, __isr_0x57
 .long __isr_0x58, __isr_0x59, __isr_0x5a, __isr_0x5b
 .long __isr_0x5c, __isr_0x5d, __isr_0x5e, __isr_0x5f
 .long __isr_0x60, __isr_0x61, __isr_0x62, __isr_0x63
 .long __isr_0x64, __isr_0x65, __isr_0x66, __isr_0x67
 .long __isr_0x68, __isr_0x69, __isr_0x6a, __isr_0x6b
 .long __isr_0x6c, __isr_0x6d, __isr_0x6e, __isr_0x6f
 .long __isr_0x70, __isr_0x71, __isr_0x72, __isr_0x73
 .long __isr_0x74, __isr_0x75, __isr_0x76, __isr_0x77
 .long __isr_0x78, __isr_0x79, __isr_0x7a, __isr_0x7b
 .long __isr_0x7c, __isr_0x7d, __isr_0x7e, __isr_0x7f
 .long __isr_0x80, __isr_0x81, __isr_0x82, __isr_0x83
 .long __isr_0x84, __isr_0x85, __isr_0x86, __isr_0x87
 .long __isr_0x88, __isr_0x89, __isr_0x8a, __isr_0x8b
 .long __isr_0x8c, __isr_0x8d, __isr_0x8e, __isr_0x8f
 .long __isr_0x90, __isr_0x91, __isr_0x92, __isr_0x93
 .long __isr_0x94, __isr_0x95, __isr_0x96, __isr_0x97
 .long __isr_0x98, __isr_0x99, __isr_0x9a, __isr_0x9b
 .long __isr_0x9c, __isr_0x9d, __isr_0x9e, __isr_0x9f
 .long __isr_0xa0, __isr_0xa1, __isr_0xa2, __isr_0xa3
 .long __isr_0xa4, __isr_0xa5, __isr_0xa6, __isr_0xa7
 .long __isr_0xa8, __isr_0xa9, __isr_0xaa, __isr_0xab
 .long __isr_0xac, __isr_0xad, __isr_0xae, __isr_0xaf
 .long __isr_0xb0, __isr_0xb1, __isr_0xb2, __isr_0xb3
 .long __isr_0xb4, __isr_0xb5, __isr_0xb6, __isr_0xb7
 .long __isr_0xb8, __isr_0xb9, __isr_0xba, __isr_0xbb
 .long __isr_0xbc, __isr_0xbd, __isr_0xbe, __isr_0xbf
 .long __isr_0xc0, __isr_0xc1, __isr_0xc2, __isr_0xc3
 .long __isr_0xc4, __isr_0xc5, __isr_0xc6, __isr_0xc7
 .long __isr_0xc8, __isr_0xc9, __isr_0xca, __isr_0xcb
 .long __isr_0xcc, __isr_0xcd, __isr_0xce, __isr_0xcf
 .long __isr_0xd0, __isr_0xd1, __isr_0xd2, __isr_0xd3
 .long __isr_0xd4, __isr_0xd5, __isr_0xd6, __isr_0xd7
 .long __isr_0xd8, __isr_0xd9, __isr_0xda, __isr_0xdb
 .long __isr_0xdc, __isr_0xdd, __isr_0xde, __isr_0xdf
 .long __isr_0xe0, __isr_0xe1, __isr_0xe2, __isr_0xe3
 .long __isr_0xe4, __isr_0xe5, __isr_0xe6, __isr_0xe7
 .long __isr_0xe8, __isr_0xe9, __isr_0xea, __isr_0xeb
 .long __isr_0xec, __isr_0xed, __isr_0xee, __isr_0xef
 .long __isr_0xf0, __isr_0xf1, __isr_0xf2, __isr_0xf3
 .long __isr_0xf4, __isr_0xf5, __isr_0xf6, __isr_0xf7
 .long __isr_0xf8, __isr_0xf9, __isr_0xfa, __isr_0xfb
 .long __isr_0xfc, __isr_0xfd, __isr_0xfe, __isr_0xff
