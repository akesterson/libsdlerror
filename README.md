WIP : This file was written by AI. It needs to be cleaned by a human who actually understands the library (me).

This README file is intended to serve as a guide for developers of this C library. The purpose of the library is to provide TRY/CATCH style error handling in C programs, while also capturing stack traces and context information for debugging purposes. The following steps describe how you can use this library:

1. Include the necessary headers: `error.h` and `stdlib.h`.
2. Call `error_init()` at the start of your program to initialize error handling.
3. Use `PREPARE_ERROR(ptr)` macro to reserve an error slot for use.
4. Use `ATTEMPT/CATCH/CLEANUP/PROCESS/HANDLE/FINISH` macros around sections of code that you want to protect from errors. The `ATTEMPT` macro starts a block, the `CATCH` macro handles exceptions, and `FINISH` is used to clean up resources after an error.
5. Use the `FAIL/SUCCEED` macros inside `ATTEMPT` blocks to raise or clear errors respectively.
6. If an exception is caught by a CATCH macro, it sets the status of the error context object and moves on to the next error slot if one isn't available.
7. The HANDLE, HANDLE_GROUP and HANDLE_DEFAULT macros are used to specify what action should be taken when an error occurs.
8. If there is no handler for a particular error status, it will call the default handler which by default prints an unhandled error message and exists with that code.
9. Use `FINISH` at the end of each block to clean up resources after an error. 

This library can help you write robust C programs which are easier to debug when things go wrong.

