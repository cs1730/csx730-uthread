	
/*------------------------------------------------------------------------------------------------*/
	.text
	.globl   _uthread_save
	.type    _uthread_save, @function
/*------------------------------------------------------------------------------------------------*	

/* int _uthread_save(uthread_ctx * ctx); */
_uthread_save:

	xorq    %rax, %rax
	leaveq
	retq
	pop    %rbp
	
/*------------------------------------------------------------------------------------------------*/
	.text
	.globl   _uthread_restore
	.type    _uthread_restore, @function
/*------------------------------------------------------------------------------------------------*/
	
/* void _uthread_restore(uthread_ctx * ctx, int value); */	
_uthread_restore:
	
	xorq    %rax, %rax
	leaveq
	retq
	pop    %rbp
	
