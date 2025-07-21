#include "sdlerror.h"
#include "stdlib.h"

ErrorContext __error_last_ditch;
ErrorContext *__error_last_ignored;
ErrorUnhandledErrorHandler error_handler_unhandled_error;

char *__ERROR_NAMES[] = {
  "",
  "Null Pointer Error",
  "Out Of Bounds Error",
  "SDL Library Error",
  "Attribute Error",
  "Type Error",
  "Key Error",
  "Heap Error",
  "Index Error",
  "Format Error",
  "Input Output Error",
  "Registry Error",
  "Value Error",
  "Behavior Error",
  "Relationship Error"
};

ErrorContext HEAP_ERROR[MAX_HEAP_ERROR];

void error_init()
{
    static int inited = 0;
    if ( inited == 0 ) {
	for (int i = 0; i < MAX_HEAP_ERROR; i++ ) {
	    memset((void *)&HEAP_ERROR[i], 0x00, sizeof(ErrorContext));
	    HEAP_ERROR[i].heapid = i;
	    HEAP_ERROR[i].stacktracebufptr = (char *)&HEAP_ERROR[i].stacktracebuf;
	}
	__error_last_ignored = NULL;
	memset((void *)&__error_last_ditch, 0x00, sizeof(ErrorContext));
	__error_last_ditch.stacktracebufptr = (char *)&__error_last_ditch.stacktracebuf;
	error_handler_unhandled_error = &error_default_handler_unhandled_error;
	inited = 1;
    }
}

void error_default_handler_unhandled_error(ErrorContext *errctx)
{
  if ( errctx == NULL ) {
    exit(1);
  }
  exit(errctx->status);
}

ErrorContext *heap_next_error()
{
    for (int i = 0; i < MAX_HEAP_ERROR; i++ ) {
	if ( HEAP_ERROR[i].refcount == 0 ) {
	    return &HEAP_ERROR[i];
	}
    }
    return (ErrorContext *)NULL;
}

ErrorContext *heap_release_error(ErrorContext *err)
{
    int oldid = 0;
    if ( err == NULL ) {
	ErrorContext *errctx = &__error_last_ditch;
	FAIL_RETURN(errctx, ERR_NULLPOINTER, "heap_release_error got NULL context pointer");
    }
    if ( err->refcount > 0 ) {
      err->refcount -= 1;
    }
    if ( err->refcount == 0 ) {
	oldid = err->heapid;
	memset(err, 0x00, sizeof(ErrorContext));
	err->stacktracebufptr = (char *)&err->stacktracebuf;
	err->heapid = oldid;
	return NULL;
    }
    return err;
}

char *error_name_for_status(int status)
{
  return __ERROR_NAMES[status];
}
