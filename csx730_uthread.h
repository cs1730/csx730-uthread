#ifndef CSX730_UTHREAD_H
#define CSX730_UTHREAD_H

#include <errno.h>

/**
 * @file
 * A thread of execution is the smallest sequence of programmed instructions that can be 
 * managed independently by a scheduler. A <b>user-mode thread</b>, sometimes referred to
 * as a <i>fiber</i>, is one that is scheduled in user mode instead of kernel mode. In user mode,
 * only a single thread can execute at a time. After some time, the current thread that
 * is executing will be temporarily interrupted by some signal, the disposition of which
 * should trigger a context switch to another thread without requiring either thread's 
 * cooperation. The basic idea is that these context switches should occur so quickly that
 * all threads appear to execute concurrently. Special care should be taken to block 
 * signals during a context switch. 
 *
 * <p>
 * Each thread gets its own stack that is separate from the stack of the calling process
 * but somewhere within the process's virtual memory space. While this new stack space
 * can be allocated using @c malloc(3), use of @c mmap(2) is recommended as it guarantees
 * the memory will be allocated at a nearby page boundary. You should 
 * <a href="https://lwn.net/Articles/294001/">actively avoid</a> use of the @c MAP_GROWSDOWN
 * flag when using @c mmap(2) for a stack. Instead simply treat the returned pointer to 
 * the mapped area as the end of the stack, then add the stack size to compute the 
 * initial stack pointer value.
 *
 * <p>
 * The @c _CS670_SOURCE feature test macro defines additional functions for user-mode threads.
 */

//------------------------------------------------------------------------------------------------//
// DEFINES                                                                                        //
//------------------------------------------------------------------------------------------------//

/** Minimum size for user-mode thread stack. */
#define UTHREAD_MIN_STACK_SIZE 131072

/** Default size for user-mode thread stack. */
#define UTHREAD_STACK_SIZE 4194304

//------------------------------------------------------------------------------------------------//
// TYPEDEFS                                                                                       //
//------------------------------------------------------------------------------------------------//

/** 
 * The implementation-defined unsigned integer type of the result of @c sizeof. A @c size_t can 
 * store the maximum size of a theoretically possible object of any type (including array). We 
 * define it here so that @c <stddef.h> does not need to be included.
 *
 * <p>
 * The exact type provided here was chosen to match the @c gcc implementation on UGA's Nike server.
 * You should be able to verify using:
 * @code
 * $ echo | gcc -E -xc -include 'stddef.h' - | grep size_t
 * @endcode
 */
typedef long unsigned int size_t;

/** User-mode thread start function argument type. */
typedef void * uthread_arg;

/** User-mode thread start function type. */
typedef void uthread_func(uthread_arg);

//------------------------------------------------------------------------------------------------//
// DATA STRUCTURES & ENUMERATIONS                                                                 //
//------------------------------------------------------------------------------------------------//

/**
 * Represents a user-mode thread state. 
 */
typedef enum {
  NEW,     /**< A thread that has not yet started is in this state. */
  RUNNING, /**< A thread that is executing is this state. */
  WAITING, /**< A thread that is has been preempted is in this state. */
  DONE     /**< A thread that has exited is in this state. */
} uthread_state;

/** 
 * Holds the metadata for a stack. On most systems, we assume that @c start is the lowest address
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

/** 
 * Holds the metadata for a user-mode thread. Implementers should feel free to use @c extra to point
 * to any implementation-dependent extra information they might need to properly implement the
 * library's functions. The @c priority member is to be ignored unless @c _CS6730_SOURCE is defined
 * (e.g., if the [PRIORITY] requirememt is being implemented).
 */
typedef struct {
  uthread_state state;    /**< thread state */
  uthread_stack stack;    /**< thread stack */
  uthread_ctx   ctx;      /**< thread context */
  void *        extra;    /**< extra information; implementation-dependent */
  unsigned long priority; /**< thread priority */
} uthread;

//------------------------------------------------------------------------------------------------//
// FUNCTION PROTOTYPES                                                                            //
//------------------------------------------------------------------------------------------------//

/**
 * Clears out all of the metadata for the thread. Formally, this function fills the first 
 * @c sizeof(uthread) bytes of the memory area pointed to by @c thread with the constant byte 
 * @c NULL.
 *
 * @param thread     pointer to user-mode thread
 */
void uthread_clear(uthread * thread);

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
 * @remark @c _CS6730_SOURCE causes the thread's priority to default to @c UTHREAD_PRIORITY_NORMAL.
 * @param thread     pointer to user-mode thread
 * @param func       pointer to start function
 * @param arg        start function argument
 * @param stack_size thread stack size; must be @c >= @c UTHREAD_MIN_STACK_SIZE
 * @return @c 0 on succes; otherwise, an error number indicating the error
 */
int uthread_create(uthread * thread, uthread_func * func, uthread_arg arg, size_t stack_size);

/**
 * Terminates the calling user-mode thread. When a thread terminates, process-shared resources 
 * (e.g., mutexes, condition variables, semaphores, and file descriptors) are not released, and 
 * functions registered using @c atexit(3) are not called. The thread's state should be
 * @c DONE once terminated, regardless of whether the termination was the result of calling
 * this function.
 */
void uthread_exit(void);

/**
 * Waits for the user-mode thread specified by @p thread to terminate. If that thread has already 
 * terminated, then this function returns immediately. While the thread is waiting, its state should
 * be @c WAITING.
 *
 * @param thread     pointer to user-mode thread
 */
void uthread_join(uthread * thread);

/**
 * Returns a pointer to the user-mode thread structure of the calling thread.
 *
 * @return a pointer to the user-mode thread structure of the calling thread
 */
uthread * uthread_self(void);

/**
 * Enqueues a user-mode thread into to the ready queue. If @c _CS6730_SOURCE is defined, then
 * the ready queue is assumed to be a max heap based on the thread's priority. Otherwise, a simple 
 * queue is assumed.
 *
 * @param thread     pointer to user-mode thread
 */
void _uthread_sched_enqueue(uthread * thread);

/**
 * Dequeues a user-mode thread into to the ready queue. If @c _CS6730_SOURCE is defined, then
 * the ready queue is assumed to be a max heap based on the thread's priority. Otherwise, a simple 
 * queue is assumed.
 *
 * @return pointer to user-mode thread
 */
uthread * _uthread_sched_dequeue(void);

//------------------------------------------------------------------------------------------------//
// _CS6730_SOURCE FEATURES
//------------------------------------------------------------------------------------------------//
#ifdef _CS6730_SOURCE

/** 
 * Low priority value.
 * @remark Requires @c _CS6730_SOURCE.
 */
#define UTHREAD_PRIORITY_LOW 10L

/** 
 * Normal priority value.
 * @remark Requires @c _CS6730_SOURCE.
 */
#define UTHREAD_PRIORITY_NORMAL 5L

/** 
 * High priority value.
 * @remark Requires @c _CS6730_SOURCE.
 */
#define UTHREAD_PRIORITY_HIGH 0L

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
 * @param priority   thread priority
 * @return @c 0 on succes; otherwise, an error number indicating the error
 */
int uthread_create_priority(uthread * thread, uthread_func * func, uthread_arg arg,
			    size_t stack_size, unsigned long priority);


#endif // _CS6730_SOURCE

#endif // CSX730_UTHREAD_H

