#ifndef __QBPREALLOCSTACK_H_
#define __QBPREALLOCSTACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

// Return codes
#define QBPREALLOCSTACK_RC_OK         0
#define QBPREALLOCSTACK_RC_UNKNOWN    1
#define QBPREALLOCSTACK_RC_BUILD      2
#define QBPREALLOCSTACK_RC_OUTOFMEM   3
#define QBPREALLOCSTACK_RC_DELETED    4

//const char* QBPREALLOCSTACK_RC_STRING[] = { "OK", "Unknwown error", "Build error", "Out of memory" , "Stack deleted" };
extern const char* QBPREALLOCSTACK_RC_STRING[];

#define QBPREALLOCSTACK_LOG_PREFIX_MAX_SIZE 1024

typedef struct qbPreAllocStackElement_ {
   struct qbPreAllocStackElement_ *prev;
   struct qbPreAllocStackElement_ *next;
} qbPreAllocStackElement_t;

typedef struct qbPreAllocStack_ {
        long nb_elements;
        long element_size;
	int  (*build_func  )(struct qbPreAllocStack_* stack, void *element) ;
	int  (*destroy_func)(struct qbPreAllocStack_* stack, void *element) ;
	int  (*alloc_func  )(struct qbPreAllocStack_* stack, void *element) ;
	int  (*free_func   )(struct qbPreAllocStack_* stack, void *element) ;
	void (*log_func)( const int level, const char* format, ...);
	char log_prefix[QBPREALLOCSTACK_LOG_PREFIX_MAX_SIZE];
        void *stack_data;
	void *element_array;
	qbPreAllocStackElement_t *stackElement_array;
	qbPreAllocStackElement_t *first_stackElement;
        long nb_used_elements;
        pthread_mutex_t mutex;
} qbPreAllocStack_t;

int qbPreAllocStack_build(  long nb_elements,
			  long element_size,
                          int (*build_func  )(qbPreAllocStack_t* stack, void *element) ,
                          int (*destroy_func)(qbPreAllocStack_t* stack, void *element) ,
                          int (*alloc_func  )(qbPreAllocStack_t* stack, void *element) ,
                          int (*free_func   )(qbPreAllocStack_t* stack, void *element) ,
			  void (*log)( const int level, const char* format, ...),
			  const char *log_prefix,
                          void *stack_data,
			  qbPreAllocStack_t **newStack );

int qbPreAllocStack_alloc(qbPreAllocStack_t *stack, void **element);

int qbPreAllocStack_free(qbPreAllocStack_t *stack, void *element);

int qbPreAllocStack_destroy(qbPreAllocStack_t *stack);

#ifdef __cplusplus
}
#endif

#endif // __QBPREALLOCSTACK_H_

