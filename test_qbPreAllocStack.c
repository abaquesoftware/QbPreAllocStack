#include "qbPreAllocStack.h"

#include <stdio.h>

int last_id=0;

typedef struct _element_
{
  int id;
  char string[100];
} element_t;

int build_func(qbPreAllocStack_t* stack, void *element_)
{
  element_t *element=(element_t*)element_;
  element->id=last_id++;
  element->string[0]='\0';
  printf("BUILD - element : %p -  id=%d\n", element , element->id );
  return QBPREALLOCSTACK_RC_OK;
}

int destroy_func(qbPreAllocStack_t* stack, void *element_)
{
  element_t *element=(element_t*)element_;
  printf("DESTROY - id=%d\n", element->id );
  return QBPREALLOCSTACK_RC_OK;
}

int alloc_func(qbPreAllocStack_t* stack, void *element_)
{
  element_t *element=(element_t*)element_;
  printf("ALLOC - id=%d\n", element->id );
  return QBPREALLOCSTACK_RC_OK;
}

int free_func(qbPreAllocStack_t* stack, void *element_)
{
  element_t *element=(element_t*)element_;
  printf("FREE - id=%d\n", element->id );
  return QBPREALLOCSTACK_RC_OK;
}

void log_func(int level, const char* format, ...)
{
  
}

int main( int nb_arg, const char **args)
{
  printf("======================\n");
  printf("Test PreAllocStack\n");
  printf("======================\n");

  qbPreAllocStack_t *stack;
  int rc=qbPreAllocStack_build( 10 , 
                             sizeof(element_t),
                             build_func,
                             destroy_func,
                             alloc_func,
                             free_func,
                             log_func,
                             "LOG-PREFIX",
                             NULL,
                             &stack);

  int i;
  element_t *element_array[11];
  for( i = 0 ; i < 2 ; i++ )
  {
    rc=qbPreAllocStack_alloc(stack,(void**)&element_array[i]);
    printf("Alloc element %p - id = %d - rc=%d\n", element_array[i], element_array[i]->id, rc );
  }

  for( i = 0 ; i < 2 ; i++ )
  {
    rc=qbPreAllocStack_free(stack,(void*)element_array[i]);
    printf("free element %d - rc=%d\n", element_array[i]->id, rc );
  }

  qbPreAllocStack_destroy(stack);

  return 0;
}


