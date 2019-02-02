#ifndef CSX730_UTHREAD_H
#define CSX730_UTHREAD_H

#include <errno.h>

/**
 * @file
 * Testing...
 */

/** Minimum size for user-mode thread stack. */
#define UTHREAD_MIN_STACK_SIZE 131072

/** Default size for user-mode thread stack. */
#define UTHREAD_STACK_SIZE 4194304

/** User-mode thread start function argument type. */
typedef void * uthread_arg;

/** User-mode thread start function type. */
typedef void uthread_func(uthread_arg);

typedef enum { NEW,      /**< new */
	       RUNNING,
	       WAITING,
	       DONE
} uthread_state;

/** 
 * Holds the metadata for a stack. On most systems, we assume that @start is the lowest address
 * in the allocated stack space (e.g., returned by @c malloc(3) or @c mmap(2)), and that
 * @c rsp is the highest address in the space.  
 */
typedef struct {
  void * start; /**< Start address of allocated memory. */
  void * rsp;   /**< Stack pointer address. */
  size_t size;  /**< Stack size, in bytes. */
} uthread_stack;

/**
 * Holds the context information about the user-mode thread. Students may add additional registers
 * to the structure if they find that the ones included are not sufficient to save the context.
 */
typedef struct {
  void * rsp; /**< register stack pointer */
  void * rbp; /**< register base pointer */
  void * rbx; /**< register b extended */
  void * r12; /**< register 12 */
  void * r13; /**< register 13 */
  void * r14; /**< register 14 */
  void * r15; /**< register 15 */
  // YOU MAY ADD ADDITIONAL REGISTERS BELOW THIS COMMENT
} uthread_ctx;

typedef struct {
  uthread_state state;
  uthread_stack stack;
} uthread;

/**
 * Initializes and runs a user-mode thread within the calling process. The new thread starts in the
 * function @p func which has a single argument @p arg. The new thread has its own stack space, the
 * size of which, in bytes, is specified by @p stack_size. The macro constant @c UTHREAD_STACK_SIZE
 * is available to provide a good default stack size. Upon termination of the thread, its stack
 * is deallocated.
 *
 * <p>
 * If the function succeeds, it will return @c 0 and run the thread; otherwise, an error number
 * shall be returned indicating the error:
 *   - @c EAGAIN resources unavailable for stack allocation
 *   - @c EINVAL the value of @p thread or @p func is @c NULL
 *   - @c ENOMEM the value of @p stack_size is less than @c UTHREAD_MIN_STACK_SIZE
 * This function does not, itself, change the value of @c errno.
 *
 * @param thread     pointer to user-mode thread
 * @param func       pointer to start function
 * @param arg        start function argument
 * @param stack_size thread stack size; must be @c >= @c UTHREAD_MIN_STACK_SIZE
 * @return @c 0 on succes; otherwise, an error number indicating the error
 */
int uthread_create(uthread * thread, uthread_func * func, uthread_arg arg, size_t stack_size);

/**
 *
 */
void uthread_exit(void * status);

void uthread_join(uthread * thread, void ** status);

uthread * uthread_self(void);



#ifdef _CS6730_UTHREAD_PRIORITY
#include "csx730_uthread_priority.h"
#endif

#endif // CSX730_UTHREAD_H

