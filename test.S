.code16 #generate 16-bit code 
.text #executable code location 
	.globl _start;
_start: #code entry point 
	movb $'X' , %al #character to print 
	movb $0x0e, %ah #bios service code to print
	int $0x10 #interrupt the cpunow
	. = _start + 510 #mov to 510th bytefrom 0pos
	.byte 0x55 #append boot signature 
	.byte 0xaa #append boot signature
	