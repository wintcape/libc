# libc wrapper / replacement
Ongoing project to add robustness to libc for improved programming of C applications.

## About

This project is not intended to rewrite the entirety of libc; it is mostly just a learning exercise for me that doubles as a method of creating useful tools for future projects.

There are some standard libc functions whose implementation is typically already highly optimized for the target platform, and other features which provide detailed platform-specific error handling out-of-the-box. As such, there are some standard libc headers that this library assumes are available on the host platform, regardless of what platform layer is used:

### `<stdlib.h>`

Requesting memory from the host platform:
- `malloc`
- `free`

Pseudorandom number generation:
- `rand`
- `srand`

### `<string.h>`

Highly-optimized string operations:
- `strlen`
- `strnlen`
- `memset`
- `memcpy`
- `memmove`
- `memcmp`

Platform-dependent error reporting:
- `strerror_r` (GNU/Linux, macOS)
- `strerror_s` (Windows)

### `<math.h>`

Performing algebraic operations on 32-bit floating point numbers:
- `fabsf`
- `floorf`
- `ceilf`
- `powf`
- `sqrtf`
- `expf`
- `logf`
- `log10f`
- `sinf`
- `cosf`
- `tanf`
- `asinf`
- `acosf`
- `atanf`
- `sinhf`
- `coshf`
- `tanhf`

Performing algebraic operations on 64-bit floating point numbers:
- `fabs`
- `floor`
- `ceil`
- `pow`
- `sqrt`
- `exp`
- `log`
- `log10`
- `sin`
- `cos`
- `tan`
- `asin`
- `acos`
- `atan`
- `sinh`
- `cosh`
- `tanh`

### **TEMPORARY**: `<stdio.h>`

 Crutch for printing fixed-precision floating point numbers (until `string_f64` is implemented):
- `snprintf`

## Testing

### System requirements
~6.00 GiB free disk space.

~2.81 GiB free RAM.

### System dependencies
Requires GNU make.

Makefiles use gcc by default. The library has been tested on Clang as well.

### Windows
To test on Windows:
```
make windows-test
```

### GNU/Linux
To test on GNU/Linux:
```
make linux-test
```

### macOS/OSX
To test on macOS/OSX:
```
make macos-test
```

## To-do
- Implement `string_f64`, removing dependency `<stdio.h>`.
- Implement unbuffered file I/O for Windows platform layer; currently lets Windows handle alignment and buffering.
- Implement `platform_thread_wait`, `platform_thread_wait_timeout`, and `platform_thread_active` for macOS/Linux platform layers.
- Change implementation of `platform/test_filesystem.c` such that one failed test doesn't result in all of the other filesystem I/O tests failing due to broken pipe (Currently, when a test fails, the test function exits early, meaning the file handle never gets closed before the next test tries to open the same handle again).
- Improve robustness of tests in `platform/filesystem.c`.
- Implement `FILE_MODE_APPEND`.
- Verify thread safety for `platform/filesystem.h`.
- Tests for `platform/thread.h` and `platform/mutex.h`.
- Make general improvements to the API design; provide more options for choosing between use of API-provided data structures versus `void*` handles the user chooses how to allocate, etc.

## Changelog

### 0.4.3
- Added lines to `platform/test_filesystem.c` to ensure `FILE_MODE_READ` is detected for `file_exists` on all the test input files.

### 0.4.2
- Swapped the meaning of `%p` and `%P` format specifiers for `string_format`, simply because I thought the convention would be clearer.
- Standalone `%f` format specifier behavior has changed; it now truncates the value to not show any decimal point or fractional part if it evaluates to 0.
- New `%F` format specifier preserves the old behavior of `%f` (always shows decimal point and fractional part)
- Fixed a bug with `string_format` padding format specifier where the incorrect number of padding characters would be computed from the characters of the formatting string due to an order-of-arithmetic error.

### 0.4.1
- Fixed a long standing bug where every time I ran the test program on Linux, a file called NUL got written to the working directory. It was actually a bug in the Linux/macOS Makefiles, as I had copied the Windows ones but forgotten to change one of the lines of shell code.
- Fixed a long standing bug where extra zero-bytes were getting written into the middle of the log file; it was because `logger_file_append` was passing the wrong message length to `file_write_line` (off by one byte!).
- Added a clause to `__string_replace` to exit early for a particular edge case when both the string to replace and its replacement are empty. Should hopefully improve performance.

### 0.4.0
- Commented all the code in the `test/` library, so it is now easy to scan through which tests I do and do not already have. This helped me realize that I was missing some tests. I added a few and fixed a few minor bugs related to this.
- `memory_free` logs an error if an illegal size is provided for a given memory tag.
- New function `memory_amount_allocated` for querying number of bytes allocated for a particular tag. Many of the functions in the `test/` library now make use of this in conjunction with `memory_allocation_count` and `memory_free_count` to (hopefully) improve the level of memory safety verification that each test involving memory allocations can individually perform.
- Added function `array_size` to calculate size in bytes of an array.
- Many of the tests in `platform/filesystem.c` now make use of `file_position_get` and `file_position_set`.
- *BUG FIXING TIME!!* Fixed several nasty memory allocator bugs related to string allocation (see below).
- Fixed a bug where the wrong amount of memory would be freed by `_array_resize` when resizing strings; this has led to a function signature change for `_array_resize`.
- Fixed a bug with pointer arithmetic when freeing strings via `string_free`.
- As a result of several of the changes above, `memory_stat` now works as intended. It gets invoked automatically to provide memory reporting if there is an error during `memory_shutdown`.
- `dynamic_allocator_clear` can handle an uninitialized `dynamic_allocator_t` buffer.
- A few function signatures have changed to remove `const` requirement, even when the current implementation holds those variables constant. List of affected functions:
    - `memory_startup`
    - `_array_field_set` and `_array_field_get`
    - `hashtable_create`
    - `queue_create`
    - `logger_log`
    - `string_contains`
    - `string_reverse`
    - `string_f64`
    - `string_bytesize`
    - `string_allocate`
    - `file_position_set`
    - `thread_wait_timeout`
    - `thread_sleep`
    - `platform_string_length_clamped`
    - `platform_error_message`
    - `platform_sleep`
    - `platform_file_position_set`
    - `platform_thread_wait_timeout`
    - `platform_thread_sleep`

Regarding the final change: going forward, I'll try to standardize use of `const` to the following cases:
1. Within the signatures of library-internal functions, within the signatures of functions labeled `INLINE`, and within the implementations of all functions, everything I can possibly label `const` should be labeled `const` explicitly.
2. Within the signatures of useful API functions, `const` will generally be omitted, unless it is explicitly documenting something important like a read-only pointer or maximum string length.

### 0.3.4
- Fixed a bug in `string_format` pointer arithmetic that would cause the function to fail at random (depending on operating system factors).
- Moved preprocessor binding `platform_log_error` for logging platform-specific error messages into the `core/logger.h` header so it is visible across the library.
- Changes to platform-layer implementation of `platform/filesystem.h` to make the internal platform-dependent file data structures more robust (they now keep track of size and file position). This also fixes a bug where `file_read`, `file_read_line`, `file_read_all`, `file_write`, and `file_write_line` would fail depending on where the host platform file pointer was positioned within the file.
- Added functions `file_position_get` and `file_position_set` for querying file pointer info.
- General clarity improvements made to the implementation of `platform/filesystem.h`.

### 0.3.3
- Added a 'dev' branch so I can: (1) push stuff prior to testing if it works cross-platform, (2) pull it on other platforms and run the tests, (3) update the 'master' branch only if it has been tested on all of them. This should resolve issues like those mentioned in the changelog for versions 0.2.1 and 0.3.1.
- New function `string_strip_ansi` to strip ANSI formatting codes. +tests for it.
- Log file is now plaintext; strips the ANSI color codes using `string_strip_ansi`.
- Additional tests for empty files in `platform/test_filesystem.c`.

### 0.3.2
- Added explicit types `arg_t` and `args_t` in `common/args.h` to make the usage of `ARGS(...)` preprocessor binding more clear from the function signature (albeit still cumbersome).

### 0.3.1
- Fixed GNU/Linux and macOS platform layers so they compile and pass all tests. I need to set up a branch so I can push one platform layer at a time wthout modifying the others until I can test them.

### 0.3.0
- `platform/filesystem.h` now uses the platform layer internally, rather than `<stdio.h>`. With it comes much more extensive testing done by `platform/test_filesystem.c`.
- Function signature changes: `file_open` no longer accepts a `bool` argument indicating text- versus binary- encoding (if converting between LF and CRLF newlines for text files, you need to handle this yourself; `file_read_line` and `file_write_line` consider a single LF as the newline delimiter). `file_write_line` no longer accepts a `u64*` argument indicating an output buffer for the number of bytes written.
- Added preprocessor binding `_file_write` as shorthand for writing null-terminated strings to a file.
- `core/thread.h` and `core/mutex.h` are now `platform/thread.h` and `platform/mutex.h`.
- New function `string_replace` to replace substrings within a string. + tests for it.
- Fixed a bug with `logger_log` so it correctly writes to the log file, as long as `logger_startup` has succeeded prior to its invocation.
- `string_format` no longer crashes if passed null pointers for `%s`, `%S`, `%f`, `%e`, or `%d`.

### 0.2.4
-`file_read_line` returns on `\r` or `\n` (previously was just `\n`)

### 0.2.3
- Added 64-bit variants of all the `math/` functions and preprocessor bindings.

### 0.2.2
- `freelist_init` and `test_register` now return a `bool` indicating success.
- Better null-pointer handling and error messages across the library.
- All documentation now explicitly specifies `Must be non-zero.` for all pointer-valued parameters for which null checks are *not* performed by the function.

### 0.2.1
- Fixed and tested Linux platform layer with version 0.2. Updated macOS platform layer to coincide with this, but it is still untested. Among the bugs was the invocation of `memory_allocate` when creating a mutex during the initialization of the memory subsystem; because the state was only halfway initialized, the check done by `memory_allocate` prior to invoking the allocator, which should have failed and invoked `malloc`, would pass, and then all Hell would break loose.
- Fixed a bunch of name conflicts that the math library was causing with standard libc headers by appending a `math_` prefix to all the function names.
- `platform_error_string` now uses `strerror_s` (Windows) and `strerror_r` (POSIX) from `<string.h>` on the backend.

### 0.2.0
- Standardized platform-layer error reporting across the library via two new functions: `platform_error_code` and `platform_error_string` (both thread-safe).
- New functions in `core/string.h`: `string_i64` and `string_u64`, for converting various types of integer values to string; `string_format` uses these now instead of being dependent on `snprintf` from `<stdio.h>` for that. Still working on `string_f64` to completely remove the `snprintf` dependency.
- Included a lot more functionality from `<math.h>` into `math/math.h`.
- `memory_equal` now calls new function `platform_memory_equal` which is a wrapper for `memcmp`, instead of using the naive string comparison implementation I used previously
- Added explicit documentation specifying when `array` and `string` class functions (from `container/array.h`, `container/string.h`, and `core/string.h`) do and do not support passing null pointers as arguments. This is very different from other containers in `container/` and `memory/`, which always have explicit error handling if any of the arguments are null pointers.
- `array_remove`, `array_pop`, and `string_remove` now all support passing empty arrays and null output buffers as arguments (but *not* null arrays!).
- New function `string_reverse` and tests, for reversing a string.
- Type definition name change: `file_handle_t` -> `file_t` because I wanted it to be less tedious to type
- Fixed some spacing and other issues in various logger messages across the library.
- Fixed some bugs in `test_array.h`

### 0.1.3
- Improved error handling in `core/memory.h`.

### 0.1.2
- Improvements to `core/memory.h` to verify thread safety using mutual exclusion operations.
- Added a Mac OS platform layer and makefile, although I cannot test it because I do not have a machine that runs Mac OS (pretty please someone tell me if this works, especially because I have no idea how Mac OS and Objective-C works and basically copied all the non-POSIX sections of the implementation directly from here: https://github.com/travisvroman/kohi/blob/main/engine/src/platform/platform_macos.m).
- Added function `platform_processor_core_count` to query host platform processor count.
- Truncated function signature, for consistency with `platform_processor_core_count`: `platform_get_absolute_time` -> `platform_absolute_time`.
- Removed unused memory tag.

### 0.1.1
- Forgot a `.gitignore` when I pushed the Linux version of the repo yesterday, resulting in a bunch of binaries being written to the repo. Fixed this.
- Fixed `display_time` so it correctly handles seconds-to-minutes and minutes-to-hours overflow.

### 0.1.0
- Added `core/thread.h` and `core/mutex.h` to provide multithreading utilities.
- Added `container/hashtable.h` and corresponding tests to provide a hashtable data structure.
- Added `container/queue.h` and corresponding tests to provide a queue data structure.
- Changed some function invocations: `dynamic_allocator_create`, `freelist_create` -> `dynamic_allocator_init`, `freelist_init`. `dynamic_allocator_destroy`, `freelist_destroy` -> `dynamic_allocator_clear`, `freelist_clear`. Mostly for consistency, since these functions did not use implicit memory allocation, and all other `create` and `destroy` style functions do.
- `linear_allocator_create` now returns a `bool` indicating success or failure.
- Slightly more robust error handling for `memory/linear_allocator.h` and `memory/dynamic_allocator.h`.
- Use of `<errno.h>` in platform layer to give detailed platform-dependent error messages.
- Added inline function `display_time` in `math/math.h` for converting elapsed time in seconds to appropriate units for display (see `math/conversion.h`).
- Added new `string_format` format specifiers `%d` for floating point mantissa and `%P` for minimum width padding. Added a couple of tests for these, as well as `%e` which was previously missing any tests whatsoever.
- Fixed documentation in `memory/linear_allocator.h` and `memory/dynamic_allocator.h` (and possibly a few other places) that was incorrect or contained typos.
- Confirmed that all tests pass when using the GNU/Linux platform layer, and added a Linux makefile.
- Removed extra aliases for calling `_array_field_set`, since this function should really only ever be called if you know what you're getting into.

### 0.0.9
- Hotfixes to resolve some faulty functionality of `string_format` due to missing tests (forgot to make sure integers worked with sign format modifier, forgot to invalidate multiple redundant format modifiers which overwrite each other). Also cleaned up the implementation a bit, improving some clarity in the code.
- Added `string_format` support for `%e` abbreviated notation floating-point format specifier.

### 0.0.8
- Added a bunch of functionality to `string_format` (see `container/string/format.h` for details). + additional tests.

### 0.0.7
- Changed signature of `string_bytesize`; first argument was `u64`, now `const u64`.
- Abstracted all dependencies on the standard libc headers into the platform layer.
- Fixed an error message typo in `linear_allocator.c`
- `memory_startup` now fails if called more than once in the same state lifetime.
- Test functions no longer use `LOG_TRACE` log elevation for logging info about processes which are memory-intensive - uses `LOG_DEBUG` instead.

### 0.0.6
- `file_read` no longer checks for buffer overflow but correctly returns `false` on file error and `true` otherwise.
- Added tests for `platform/filesystem.h`.
- Swapped definitions of `string_push` and `string_insert` with `_string_push` and `_string_insert` to keep the convention for implicitly computing null-terminated string length standardized across the library.
- New variant of `file_write_line` called `_file_write_line` which implicitly computes null-terminated string length of its null-terminated string argument
### 0.0.5
- Added `string_contains` and `_string_contains` for finding a substring within a string.
- Added test for `string_contains`.
- Changed the invocation of `string_equal` and `_string_equal` to align better with that of `string_contains` and `_string_contains`.
- The implementation of `string_equal` can successfully handle strings containing zero-bytes; since length is explicitly specified at the function call, it ends up just being a wrapper for `memory_equal`.
- Improved documentation of `core/string.h`.
- Updated `LOG_LEVEL_COUNT` to its correct value (`7`, not `8`).
- Switched some instances throughout the library where I was using a literal `1` to now use `sizeof ( char )`, mostly for clarity purposes.

### 0.0.4
- Added new function `file_size` and other small improvements to `platform/filesystem.h`. Tests for `platform/filesystem.h` are in development.
- Changes to `core/string.h`: added `string_allocate` for allocating an empty string of the provided length. The implementation of `string_allocate_from` now calls `string_allocate`. When the memory is allocated, the length of the string is stored in an internal header, which will be accessed by `string_free` when the string is freed, to read the number of bytes to free. This fixes a bug where the output of `file_read_all` would not work with `string_free` if the file contained any zero-bytes (since `string_free` previously worked by looking for a zero).
- Added back `file_write_line`, since I guess it can come in handy to avoid typing extra `\n` characters.

### 0.0.3
- Removed `file_write_line` since it is redundant now.
- `file_read_line` now correctly returns false if `fgets` fails.
- Removed a bug where `logger_log` (and thus `string_format`, which is now dependent on `memory_allocate` calls), would be run if `platform_memory_allocate` failed, meaning the logger would crash the program if `memory_startup` (or other invocations of `platform_memory_allocate`) ever failed.

### 0.0.2
- Migrated `platform/filesystem.h` from using `core/string.h` to using `container/string.h`. Use of mutable strings removes write limit from `file_write_line` and returns a string whose length is precalculated (see `container/string.h` for more info).
- Standardized and completed documentation across all files in the repository so far.

### 0.0.1
- Initial commit.
