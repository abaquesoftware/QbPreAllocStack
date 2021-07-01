#include "qbPreAllocStack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

const char* QBPREALLOCSTACK_RC_STRING[] = 
{ 
  "OK",                        // 0
  "Unknwown error",            // 1
  "Build error",               // 2
  "Out of memory" ,            // 3
  "Stack deleted"              // 4
};
										

const char* QBPREALLOCSTACK_LOGLEVEL_STRING[] = 
{ 
  "----",                     // 0
  "ERR ",                     // 1
  "WARN",                     // 2
  "INFO",                     // 3
  "DBUG"                      // 4
};

void qbPreAllocStack_default_log_function( const int level_, const char* format, ... )
{
  int level=level_;
  if(level<0) level=0;
  if(level>4) level=4;
  printf( "%s-" , QBPREALLOCSTACK_LOGLEVEL_STRING[level] );
  va_list arglist;
  va_start( arglist, format );
  vprintf( format , arglist );
  va_end( arglist );
  printf( "\n" );
}

int qbPreAllocStack_build(  long nb_elements,
                          long element_size,
                          int (*build_func  )(qbPreAllocStack_t* stack, void *element) ,
                          int (*destroy_func)(qbPreAllocStack_t* stack, void *element) ,
                          int (*alloc_func  )(qbPreAllocStack_t* stack, void *element) ,
                          int (*free_func   )(qbPreAllocStack_t* stack, void *element) ,
                          void (*log_func   )( const int level, const char* format, ...),
                          const char *log_prefix,
                          void* stack_data,
                          qbPreAllocStack_t **newStack )
{
  void (*log)( const int level, const char* format, ...) = log_func;
  if( log == NULL ) log = qbPreAllocStack_default_log_function; 

  log( 3 , "%s -> qbPreAllocStack_build" , log_prefix );
  int rc=QBPREALLOCSTACK_RC_OK;

  // Check input parameters
  if(nb_elements <= 0)
  {
        log( 1 ,"%s ERROR - invalid parameter: nb_elements=%ld" , log_prefix, nb_elements );
	rc=QBPREALLOCSTACK_RC_BUILD;
  }
  if(element_size <= 0)
  {
        log( 1 ,"%s ERROR - invalid parameter: element_size=%ld" , log_prefix, element_size );
	rc=QBPREALLOCSTACK_RC_BUILD;
  }
  if( rc==QBPREALLOCSTACK_RC_OK)
  {
    // Allocate stack itself
    *newStack=(qbPreAllocStack_t *)malloc(sizeof(qbPreAllocStack_t));
    if(*newStack==NULL)
    {
        log( 1 ,"%s ERROR-cannot allocate qbPreAllocStack" , log_prefix );
	rc=QBPREALLOCSTACK_RC_BUILD;
    }
    else
    {
        // Initialize attributes
	(*newStack)->nb_elements=nb_elements;
	(*newStack)->element_size=element_size;
	(*newStack)->build_func=build_func;
	(*newStack)->destroy_func=destroy_func;
	(*newStack)->alloc_func=alloc_func;
	(*newStack)->free_func=free_func;
	(*newStack)->log_func=log;
	(*newStack)->log_prefix[0]='\0';
	(*newStack)->stack_data=stack_data;
	(*newStack)->element_array=NULL;
	(*newStack)->stackElement_array=NULL;
        (*newStack)->first_stackElement=NULL;
	(*newStack)->nb_used_elements=0;
	if(log_prefix)
	{
		strncat((*newStack)->log_prefix,log_prefix,QBPREALLOCSTACK_LOG_PREFIX_MAX_SIZE);
		(*newStack)->log_prefix[QBPREALLOCSTACK_LOG_PREFIX_MAX_SIZE-1]='\0';
	}
    }
  }
  if(rc==QBPREALLOCSTACK_RC_OK)
  {
    // Allocate element_array
    (*newStack)->element_array=(qbPreAllocStack_t *)malloc(element_size*nb_elements);
    printf( "oooooo : allocate element_array : pointer = %p - size=%ld\n", (*newStack)->element_array , element_size*nb_elements );
    if((*newStack)->element_array==NULL)
    {
        log( 1 ,"%s ERROR-cannot allocate element_array" , log_prefix );
	rc=QBPREALLOCSTACK_RC_BUILD;
    } 
  }

  if(rc==QBPREALLOCSTACK_RC_OK)
  {
    // Allocate stackElement_array
    (*newStack)->stackElement_array=(qbPreAllocStackElement_t *)malloc(sizeof(qbPreAllocStackElement_t)*nb_elements);
    printf( "oooooo : allocate stackElement_array : pointer = %p - size=%ld\n", (*newStack)->stackElement_array , (sizeof(qbPreAllocStackElement_t)*nb_elements) );
    if((*newStack)->stackElement_array==NULL)
    {
        log( 1 ,"%s ERROR-cannot allocate stackElement_array" , log_prefix );
	rc=QBPREALLOCSTACK_RC_BUILD;
    } 
  }

  if(rc==QBPREALLOCSTACK_RC_OK)
  {
    // initialize mutex
    if(pthread_mutex_init(&(*newStack)->mutex, NULL)!=0)
    {
        log( 1 ,"%s ERROR-cannot initialize mutex" , log_prefix );
	rc=QBPREALLOCSTACK_RC_BUILD;
    }
  }

  if(rc==QBPREALLOCSTACK_RC_OK)
  {
    int i;
    qbPreAllocStackElement_t *stackElement, *prevStackElement=NULL;
    for(i=0; i< (*newStack)->nb_elements; i++)
    {
	stackElement=(qbPreAllocStackElement_t*)(*newStack)->stackElement_array + (sizeof(qbPreAllocStackElement_t) * i);
	stackElement->prev=prevStackElement;
	stackElement->next=NULL;
	if(prevStackElement) prevStackElement->next=stackElement;
	void *element=(*newStack)->element_array + (element_size * i);
	if( (*newStack)->build_func != NULL)
		rc = build_func(*newStack, element);
        prevStackElement=stackElement;
    }
    (*newStack)->first_stackElement=(*newStack)->stackElement_array;
    printf( "oooooo : first_stackElement = %p\n" , (*newStack)->first_stackElement );
    if(rc!=QBPREALLOCSTACK_RC_OK)
    {
        log( 1 ,"%s ERROR-cannot allocate one or several elements" , log_prefix );
    }  
  }

  // if an error occured, 
  if(rc!=QBPREALLOCSTACK_RC_OK)
  {
    qbPreAllocStack_destroy(*newStack);
    *newStack=NULL;
  }

/*
  // debug
  qbPreAllocStackElement_t *stackElement = (*newStack)->first_stackElement;
  while( stackElement )
  {
    printf( "oooooo - StackElement=%p\n", stackElement );
    stackElement=stackElement->next;
  } 
*/
  log( 3 , "%s <- qbPreAllocStack_build" , log_prefix );
  return rc;
}

int qbPreAllocStack_alloc(qbPreAllocStack_t *stack, void **element)
{
    int rc=QBPREALLOCSTACK_RC_OK;

    pthread_mutex_lock(&stack->mutex);

    printf( "oooooo -> qbPreAllocStack_alloc -------------------------\n" );
    printf( "oooooo    stack->stackElement_array = %p\n" , stack->stackElement_array );
    printf( "oooooo    stack->element_array = %p\n" , stack->element_array );
    printf( "oooooo    stack->first_stackElement = %p\n" , stack->first_stackElement );
    qbPreAllocStackElement_t *stackElement=stack->first_stackElement;
    if( stackElement == NULL )
      rc=QBPREALLOCSTACK_RC_OUTOFMEM;
    if(rc==QBPREALLOCSTACK_RC_OK)
    {
      stack->first_stackElement=stackElement->next;
      long index = (stackElement - stack->stackElement_array) / sizeof(qbPreAllocStackElement_t);
      *element = stack->element_array + index * stack->element_size;
      printf( "oooooo    index = %ld\n" , index );
      printf( "oooooo    *element = %p\n" , *element );
      printf( "oooooo    element_size = %ld\n" , stack->element_size );
      stackElement->prev=NULL;
      stackElement->next=NULL;
      if( stack->first_stackElement ) stack->first_stackElement->prev=NULL;
      stack->nb_used_elements++;

  // debug
  stackElement = stack->first_stackElement;
  while( stackElement )
  {
    printf( "oooooo - StackElement=%p\n", stackElement );
    stackElement=stackElement->next;
  } 

      if( stack->alloc_func )
        rc = stack->alloc_func( stack, *element ); 
   }
   pthread_mutex_unlock(&stack->mutex);
   return rc;
}

int qbPreAllocStack_free(qbPreAllocStack_t *stack, void *element)
{
    int rc=QBPREALLOCSTACK_RC_OK;
    pthread_mutex_lock(&stack->mutex);
    long index = (element - stack->element_array) / stack->element_size;
    qbPreAllocStackElement_t *stackElement = &(stack->stackElement_array[index]);
    if( stack->first_stackElement ) stack->first_stackElement->prev=stackElement;
    stackElement->next=stack->first_stackElement;
    stack->first_stackElement=stackElement;
    stack->nb_used_elements--;
    if( stack->free_func )
      rc = stack->free_func( stack, element );
    pthread_mutex_unlock(&stack->mutex);
    return rc;
}

int qbPreAllocStack_destroy(qbPreAllocStack_t *stack)
{
    printf( "oooooo -> qbPreAllocStack_destroy\n" );
    if( stack == NULL )
      return QBPREALLOCSTACK_RC_UNKNOWN;
    int rc=QBPREALLOCSTACK_RC_OK;
    pthread_mutex_lock(&stack->mutex);
    int i;
    qbPreAllocStackElement_t *stackElement;
    if( stack->destroy_func != NULL )
    {
      for(i=0; i<stack->nb_elements; i++)
      {
	void *element=stack->element_array + (stack->element_size * i);
        if(stack->destroy_func)
 	  stack->destroy_func(stack, element);
      }
    }
    if( stack->element_array      != NULL ) free( stack->element_array      );
    if( stack->stackElement_array != NULL ) free( stack->stackElement_array );
    free(stack);
    printf( "oooooo <- qbPreAllocStack_destroy\n" );
    pthread_mutex_unlock(&stack->mutex);
    return rc;
}

