/* coro_ctx_swap.S */
.text

.globl coro_ctx_swap
coro_ctx_swap:

    leaq (%rsp),%rax

    movq %rax, 104(%rdi)

    movq %rbx, 96(%rdi)

    movq %rcx, 88(%rdi)

    movq %rdx, 80(%rdi)

	movq 0(%rax), %rax

	movq %rax, 72(%rdi) 

    movq %rsi, 64(%rdi)

	movq %rdi, 56(%rdi)

    movq %rbp, 48(%rdi)

    movq %r8, 40(%rdi)

    movq %r9, 32(%rdi)

    movq %r12, 24(%rdi)

    movq %r13, 16(%rdi)

    movq %r14, 8(%rdi)

    movq %r15, (%rdi)

	xorq %rax, %rax





    movq (%rsi), %r15

    movq 8(%rsi), %r14

    movq 16(%rsi), %r13

    movq 24(%rsi), %r12

    movq 32(%rsi), %r9

    movq 40(%rsi), %r8
        
    movq 48(%rsi), %rbp

    movq 56(%rsi), %rdi

    movq 80(%rsi), %rdx

    movq 88(%rsi), %rcx

    movq 96(%rsi), %rbx
        
    movq 104(%rsi), %rsp    

	movq 72(%rsi),%rax
    movq %rax,(%rsp)

	ret

