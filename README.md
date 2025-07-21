# Summary

This library provides a TRY/CATCH style exception handling mechanism for C. 

# Dependencies

This library depends on the `SDL3` library and `stdlib`. Specifically it uses the SDL_Log method from SDL3.

# Installation

```bash
cmake -S . -B build
cd build
make
make install
```

# Philosophy of Use

This library does not actually implement true exception handling; rather, it sets up a useful simulacrum of exception handling using macros wrapped around the return codes of functions. There is no `setjmp`/`longjmp` hackery here.

This library never performs any dynamic memory allocation, ever. A maximum of 128 ErrorContexts can be utilized at the same time.

You do not need to know how the library works underneath the covers in order to use it. There is one datastructure, a few utility functions, and a bunch of macros. All you need to know is about 10 macros.

# Functions and Return Codes

This library can perform tests on any function or expression that returns an integer value.

Any function which uses the `ATTEMPT` macro should have a return type of `ErrorContext *`. The macros within this library, when they detect an unhandled error, will attempt to pass up the unhandled error to the context of the previous function in the call stack. This allows for errors to propagate up through the call stack in the same way as exceptions. (For example, if you use traditional C error handling in a call stack of `a() -> b() -> c()`, and `c()` fails because it runs out of memory, `b()` will likely detect that error and return some error to `a()`, but it may or may not return the context of what failed and why. With this, you get that context all the way up in `a()` without knowing anything about `c()`.

# Error codes

The library uses integer values to specify error codes inside of its context. These integer return codes are defined in `error.h` in the form of `ERR_xxxxx` where `xxxxx` is the name of the error code in question. See `error.h` for a list of defined errors and their descriptions. You can define additional error types by defining additional `ERR_xxxxx` values. Make sure not to clobber existing values.

# Setting up the error context

Before you can use any of these macros you must set up an error context inside of the current scope.

```c
PREPARE_ERROR(errctx);
```

This will create a ErrorContext structure inside of the current scope named `errctx` and initialize it. This structure is used for all operations of the library within the current scope. Attempting to use the library in a given scope before calling this will result in compile-time errors.

# 

# Attempting an operation

```c
ATTEMPT {
	// ... code
} CLEANUP {
} PROCESS(errctx) {
} FINISH(errctx, true)
```

`ATTEMPT { ... }` is the block within which you will perform operations which may cause errors that need to be caught. See "Capturing errors", below.

`CLEANUP { ... }` is the block within which you will perform any code which MUST be executed REGARDLESS of whether or not errors were thrown. Closing open file handles, or releasing memory, for example.

`PROCESS(errctx) { ... }` is the block within which you will handle any errors that were caught inside of the `ATTEMPT` block. See "Handling Errors" below.

`FINISH(errctx, true)` terminates the attempt operation. The `FINISH` macro takes two arguments: the name of the ErrorContext, and a boolean regarding whether or not to pass unhandled errors up to the calling function. Unless you have a good reason not to, this should be true.

# Capturing errors

Inside of an `ATTEMPT` block, any operation which could generate or represent an error should be wrapped in one of several macros.

## Capturing errors from functions which return ErrorContext *

For functions that return `ErrorContext *`, you should use the `CATCH` macro.

```c
ATTEMPT {
    CATCH(errctx, errorGeneratingFunction())
} // ...
```

This will call assign the return value of the function in question to the ErrorContext previously prepared in the current scope. If the function returns an ErrorContext that indicates any type of error, the `ATTEMPT` block is immediately exited, and the `CLEANUP` block begins.

## Setting errors from functions or expressions returning integer

For functions that return integer, such as logical comparisons or most standard library functions, use the `FAIL_ZERO_BREAK` and `FAIL_NONZERO_BREAK` macros. These macros allow you to capture an integer return code from an expression or function and set an error code in the current context based off that return.

Here is an example of checking for a NULL pointer

```c
ATTEMPT {
    FAIL_ZERO_BREAK(errctx, (somePointer == NULL), ERR_NULLPOINTER, "Someone gave me a NULL pointer")
} // ...
```

Here is an example of checking for two strings that are not equal

```c
ATTEMPT {
    FAIL_NONZERO_BREAK(errctx, strcmp("not", "equal"), ERR_VALUE, "Strings are not equal")
} // ...
```

When either of these two macros are used, the `ATTEMPT` block is immediately exited, and the `CLEANUP` block begins.

# Handling errors

Inside of the `PROCESS { ... }` block, you must handle any errors that occurred during the `ATTEMPT { ... }` block. You do this with `HANDLE`, `HANDLE_GROUP`, and `HANDLE_DEFAULT`.

## Handling a specific error with HANDLE

In order to handle a specific error code, use the `HANDLE` macro.

```c
} PROCESS(errctx) {
} HANDLE(errctx, ERR_NULLPOINTER) {
    // Something is complaining about a null pointer error. Do something about it.
} // ...
```

## Handling a group of errors with HANDLE_GROUP

In order to handle a group of related errors that all require the same failure behavior, use `HANDLE` followed by `HANDLE_GROUP`. For example, to handle a scenario where an IO error, key error, and index error all need to be handled the same way:

```c
} PROCESS(errctx) {
} HANDLE(errctx, ERR_IO) {
} HANDLE_GROUP(errctx, ERR_KEY) {
} HANDLE_GROUP(errctx, ERR_INDEX) {
    // error handling code goes here
}
```

This creates a fallthrough mechanism where all 3 errors get the same error handling code. Note that while the cases fall through, you can still (if desired) put some code specific to each error in that error's `HANDLE` or `HANDLE_GROUP` block; but this is not required, only the final handler needs to get any code.

The fallthrough behavior stops as soon as another `HANDLE` macro is encountered. For example, in this example, `ERR_IO`, `ERR_KEY` and `ERR_INDEX` are all handled as a group, but `ERR_RELATIONSHIP` is not.

```c
} PROCESS(errctx) {
} HANDLE(errctx, ERR_IO) {
} HANDLE_GROUP(errctx, ERR_KEY) {
} HANDLE_GROUP(errctx, ERR_INDEX) {
    // This code handles 3 error cases
} HANDLE(errctx, ERR_RELATIONSHIP) {
    // This code handles 1 error case
}
```

# Returning success or failure from functions returning ErrorContext *

If at all possible, when using this library, your functiions should return `ErrorContext *`. When returning from such functions, you should use the `SUCCEED_RETURN` and `FAIL_RETURN` macros.

## SUCCEED_RETURN

This macro is used when your function has reached the end of its happy code path and is prepared to exit successfully. This sets the ErrorContext to a successful state and exits the function.

```c
PREPARE_ERROR(errctx);
ATTEMPT {
    // ... stuff
} CLEANUP {
} PROCESS(errctx) {
} FINISH(errctx, true);
SUCCEED_RETURN(errctx);
```

## FAIL_RETURN

If the code path in the current function reaches a state wherein an error must be set and the function must return early, you can use `FAIL_RETURN` to accomplish this. Note that this should not be used inside of an `ATTEMPT { ... }` block; this immediately exits the function, preventing a `CLEANUP { ... }` block from executing. This can be safely used from inside of a `CLEANUP` or `PROCESS` block, or from anywhere within the function not inside of an `ATTEMPT { ... }` block.

The function allows you to provide printf-style variable arguments to provide a meaningful failure message.

```c
PREPARE_ERROR(errctx);
FAIL_RETURN(ERR_BEHAVIOR, "Something went horribly wrong!")
```

## Conditionally failing and returning

In addition to `FAIL_RETURN` you can also test for zero or non-zero conditions, set an error, and return from the function immediately. Use the `FAIL_ZERO_RETURN` and `FAIL_NONZERO_RETURN` macros for this. These macros can be used anywhere that `FAIL_RETURN` can be used.

```c
PREPARE_ERROR(errctx);
FAIL_ZERO_RETURN(errctx, (somePointer == NULL), ERR_NULLPOINTER, "Someone gave me a NULL pointer")
```

```c
PREPARE_ERROR(errctx);
FAIL_NONZERO_RETURN(errctx, strcmp("not", "equal"), ERR_VALUE, "Strings are not equal")
```

