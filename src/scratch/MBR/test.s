	.file	"testing.c"
	.section	.rodata
.LC0:
	.string	"sizeof(MBR_t) = %u\n"
.LC1:
	.string	"sizeof(MBR_partition_t) = %u\n"
.LC2:
	.string	"sizeof(CHS_address_t) = %u\n"
	.text
.globl main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$.LC0, %eax
	movl	$552, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$.LC1, %eax
	movl	$24, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$.LC2, %eax
	movl	$3, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.5.2 20110127 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
