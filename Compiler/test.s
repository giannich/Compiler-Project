	.globl	_main
_main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$115, -8(%rbp)
	movl	$4, -12(%rbp)
	movl	$7, -16(%rbp)
	movl	$1, -20(%rbp)
whileLoopLabel_0:
	movl	-8(%rbp), %ebx
	movl	%ebx, %eax
	cmp	$100, %eax
	jle	whileContLabel_0
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	movl	-8(%rbp), %eax
	subl	$5, %eax
	movl	%eax, -8(%rbp)
	jmp	whileLoopLabel_0
whileContLabel_0:
doLoopLabel_0:
	movl	-20(%rbp), %edi
	movl	$2, %esi
	callq	_powerFunction
	movl	%eax, -16(%rbp)
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	movl	-16(%rbp), %ebx
	movl	%ebx, %eax
	cmp	$1000, %eax
	jl	doLoopLabel_0
retlabel_0:
	movl	$2, %eax
	imull	-12(%rbp), %eax
	addl	-8(%rbp), %eax
	subl	-16(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq

	.globl	_powerFunction
_powerFunction:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %ebx
	movl	%ebx, %eax
	cmp	$0, %eax
	jg	ifLabel_0
	jmp	elseLabel_0
ifLabel_0:
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
forLoopLabel_0:
	movl	-16(%rbp), %ebx
	movl	%ebx, %eax
	cmp	$0, %eax
	jle	forContLabel_0
	movl	-12(%rbp), %eax
	imull	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-16(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	forLoopLabel_0
forContLabel_0:
	jmp	contIfLabel_0
elseLabel_0:
	movl	$1, -12(%rbp)
contIfLabel_0:
retlabel_1:
	movl	-12(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq