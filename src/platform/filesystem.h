/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/filesystem.h
 * @brief Provides an interface for interacting with files on the host platform.
 */
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "common.h"

/** @brief Type definition for a file handle. */
typedef struct
{
    void*   handle;
    bool    valid;
}
file_t;

/** @brief Type and instance definitions for file modes. */
typedef enum
{
    FILE_MODE_ACCESS = 0x0
,   FILE_MODE_READ   = 0x1
,   FILE_MODE_WRITE  = 0x2
}
FILE_MODE;

/**
 * @brief Tests if a file with the provided mode exists at the provided path on
 * the host platform.
 * 
 * FILE_MODE_ACCESS : Test only for file existence.
 * FILE_MODE_READ   : Test for read-only file.
 * FILE_MODE_WRITE  : Test for write-only file.
 * FILE_MODE_READ |
 * FILE_MODE_WRITE  : Test for file with both read and write permission.
 * 
 * @param path The filepath to test. Must be non-zero.
 * @param mode Mode flag.
 * @return true if file exists; false otherwise.
 */
bool
file_exists
(   const char* path
,   FILE_MODE   mode
);

/**
 * @brief Attempts to open a file on the host platform.
 * 
 * @param path The filepath. Must be non-zero.
 * @param mode Mode flag.
 * @param binary Open in binary mode? Y/N
 * @param file Output buffer for file handle. Must be non-zero.
 * @return true if file opened successfully; false otherwise.
 */
bool
file_open
(   const char* path
,   FILE_MODE   mode
,   bool        binary
,   file_t*     file
);

/**
 * @brief Attempts to close a file on the host platform.
 * 
 * @param file Handle to the file to close. Must be non-zero.
 */
void
file_close
(   file_t* file
);

/**
 * @brief Computes the size of a file on the host platform.
 * 
 * @param file Handle to a file. Must be non-zero.
 * @return The filesize of file in bytes.
 */
u64
file_size
(   file_t* file
);

/**
 * @brief Reads a specified amount of content from a file on the host platform
 * into an output buffer.
 * 
 * @param file Handle to the file to read. Must be non-zero.
 * @param size Number of bytes to read.
 * @param dst Output buffer for the content. Must be non-zero.
 * @param read Output buffer to hold number of bytes read. Must be non-zero.
 * @return true if file read into dst successfully; false otherwise.
 */
bool
file_read
(   file_t* file
,   u64     size
,   void*   dst
,   u64*    read
);

/**
 * @brief Reads content from a file from the host platform into a mutable string
 * buffer until EOF or line break encountered (see container/string.h).
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @param file Handle to the file to read. Must be non-zero.
 * @param dst Output buffer to hold the handle to the mutable output string.
 * Must be non-zero.
 * @return true if the handle stored in dst is valid; false otherwise.
 */
bool
file_read_line
(   file_t* file
,   char**  dst
);

/**
 * @brief Generates a copy of the entire contents of a file on the host
 * platform.
 * 
 * Uses dynamic memory allocation. Call string_free to free.
 * (see core/string.h)
 * 
 * @param file Handle to the file to read. Must be non-zero.
 * @param dst Output buffer to hold the handle to the null-terminated output
 * string. Must be non-zero.
 * @param read Output buffer to hold number of bytes read. Must be non-zero.
 * @return true if the handle stored in dst is valid; false otherwise.
 */
bool
file_read_all
(   file_t* file
,   u8**    dst
,   u64*    read
);

/**
 * @brief Writes a specified amount of data to a file on the host platform.
 * 
 * @param file Handle to the file to write to. Must be non-zero.
 * @param size Number of bytes to write.
 * @param src The data to write. Must be non-zero.
 * @param written Output buffer to hold number of bytes written.
 * Must be non-zero.
 * @return true if src written to file successfully; false otherwise.
 */
bool
file_write
(   file_t*     file
,   u64         size
,   const void* src
,   u64*        written
);

/**
 * @brief Writes a string to file on the host platform and appends the `\n`
 * character.
 * 
 * Use file_write_line to explicitly specify string length, or _file_write_line
 * to compute the length of a null-terminated string before passing it to
 * file_write_line.
 * 
 * @param file Handle to the file to write to. Must be non-zero.
 * @param size Number of bytes to write.
 * @param src The string to write. Must be non-zero.
 * @param written Output buffer to hold number of bytes written.
 * Must be non-zero.
 * @return true if src written to file successfully; false otherwise.
 */
bool
file_write_line
(   file_t*     file
,   u64         size
,   const char* src
,   u64*        written
);

#define _file_write_line(file,src,written) \
    file_write_line ( (file) , _string_length ( src ) , (src) , (written) )

/**
 * @brief Obtains a handle to stdin.
 * 
 * @param file Output buffer. Must be non-zero.
 */
void
file_stdin
(   file_t* file
);

/**
 * @brief Obtains a handle to stdout.
 * 
 * @param file Output buffer. Must be non-zero.
 */
void
file_stdout
(   file_t* file
);

/**
 * @brief Obtains a handle to stderr.
 * 
 * @param file Output buffer. Must be non-zero.
 */
void
file_stderr
(   file_t* file
);

#endif  // FILESYSTEM_H