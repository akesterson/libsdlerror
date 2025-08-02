#include "sdlerror.h"
#include "stdlib.h"

ErrorContext __error_last_ditch;
ErrorContext *__error_last_ignored;
ErrorUnhandledErrorHandler error_handler_unhandled_error;

char __ERROR_NAMES[MAX_ERR_VALUE][MAX_ERROR_NAME_LENGTH];

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
	memset((void *)&__ERROR_NAMES[0], 0x00, ((MAX_ERR_VALUE+1) * MAX_ERROR_NAME_LENGTH));

	error_name_for_status(ERR_NULLPOINTER, "Null Pointer Error");
	error_name_for_status(ERR_OUTOFBOUNDS, "Out Of Bounds Error");
	error_name_for_status(ERR_SDL, "SDL Library Error");
	error_name_for_status(ERR_ATTRIBUTE, "Attribute Error");
	error_name_for_status(ERR_TYPE, "Type Error");
	error_name_for_status(ERR_KEY, "Key Error");
	error_name_for_status(ERR_HEAP, "Heap Error");
	error_name_for_status(ERR_INDEX, "Index Error");
	error_name_for_status(ERR_FORMAT, "Format Error");
	error_name_for_status(ERR_IO, "Input Output Error");
	error_name_for_status(ERR_REGISTRY, "Registry Error");
	error_name_for_status(ERR_VALUE, "Value Error");
	error_name_for_status(ERR_BEHAVIOR, "Behavior Error");
	error_name_for_status(ERR_RELATIONSHIP, "Relationship Error");
	
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


// returns or sets the name for the given status.
// Call with name = NULL to retrieve a status.
char *error_name_for_status(int status, char *name)
{
    if ( status > MAX_ERR_VALUE ) {
	return "Unknown Error";
    }
    if ( name != NULL ) {
	strncpy(&__ERROR_NAMES[status], name, MAX_ERROR_NAME_LENGTH);	
    }
    return &__ERROR_NAMES[status];
}
