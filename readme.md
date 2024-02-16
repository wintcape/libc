# libc wrapper / replacement
Ongoing project to add robustness to libc for improved programming of C applications.

## Changelog

### 0.1.2
- Improvements to `core/memory.h` to verify thread safety using mutual exclusion operations.
- Added a Mac OS platform layer and makefile, although I cannot test it because I do not have a machine that runs Mac OS (pretty please someone tell me if this works, especially because I have no idea how Mac OS and Objective-C works and basically copied all the non-POSIX sections of the implementation directly from here: ).
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
- Hotfixes adding some faulty functionality of `string_format` due to missing tests (forgot to make sure integers worked with sign format modifier, forgot to invalidate multiple redundant format modifiers which overwrite each other). Also cleaned up the implementation a bit, improving some clarity in the code.
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