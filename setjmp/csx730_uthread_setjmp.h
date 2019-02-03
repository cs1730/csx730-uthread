#ifndef CSX730_UTHREAD_SETJMP
#define CSX730_UTHREAD_SETJMP

#include "csx730_uthread.h"

/** 
 * Saves various information about the calling execution context (typically, the stack pointer, the
 * instruction pointer, and possibly the values of other registers) in the buffer @p env for later
 * use by @c _uthread_restore(). In this case, the function returns @c 0.
 *
 * @param ctx pointer to structure that can hold the execution context information
 * @return @c 0 when called directly; on the "fake" return that occurs after @c _uthread_restor(),
 *         the nonzero value specified in @c value is returned.
 */
int _uthread_save(uthread_ctx * ctx);

/**
 * Uses the information saved in @c ctx to transfer control back to the point where 
 * @c _uthread_save() was called and to restore ("rewind") the stack to its state at the time of the
 * @c _uthread_save() call. In addition, the values of some other registers may be restored to their
 * state at the time of the @c _uthread_save()) call. Following a successful @c _uthread_restore(), 
 * execution continues as if @c _uthread_save() had returned for a second time. This "fake" return 
 * can be distinguished from a true @c _uthread_save() call because the "fake" return returns the 
 * value provided in @c value.  If the programmer mistakenly passes the value @c 0 in @c value, the 
 * "fake" return will instead return @c 1.
 *
 * @param ctx   pointer to structure that can hold the execution context information
 * @param value value for "fake" return of @c _uthread_save()
 */
void _uthread_restore(uthread_ctx * ctx, int value);

#endif // CSX730_UTHREAD_SETJMP


