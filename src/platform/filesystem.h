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
file_handle_t;

/** @brief Type and instance definitions for file modes. */
typedef enum
{
    FILE_MODE_READ  = 0x1
,   FILE_MODE_WRITE = 0x2
}
FILE_MODE;

/**
 * @brief Tests if a file exists.
 * 
 * @param path The filepath to test.
 * @return true if file exists; false otherwise.
 */
bool
file_exists
(   const char* path
);

/**
 * @brief Attempts to open a file.
 * 
 * @param path The filepath.
 * @param mode Mode flag.
 * @param binary Open in binary mode? Y/N
 * @param file Output buffer for file handle.
 * @return true if file opened successfully; false otherwise.
 */
bool
file_open
(   const char*     path
,   FILE_MODE       mode
,   bool            binary
,   file_handle_t*  file
);

/**
 * @brief Closes a file.
 * 
 * @param file Handle to the file to close.
 */
void
file_close
(   file_handle_t* file
);

/**
 * @brief Computes the size of a file.
 * 
 * @param file The file.
 * @return The filesize of file in bytes.
 */
u64
file_size
(   file_handle_t* file
);

/**
 * @brief Reads a specified amount of data into an output buffer.
 * 
 * @param file Handle to the file to read.
 * @param size Number of bytes to read.
 * @param dst Output buffer for the read data.
 * @param read Output buffer to hold number of bytes read.
 * @return true if file read into dst successfully; false otherwise.
 */
bool
file_read
(   file_handle_t*  file
,   u64             size
,   void*           dst
,   u64*            read
);

/**
 * @brief Reads a file into a mutable string buffer until EOF or line break
 * encountered (see container/string.h).
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @param file Handle to the file to read.
 * @param dst Output buffer to hold the handle to the mutable output string.
 * @return true if the handle stored in dst is valid; false otherwise.
 */
bool
file_read_line
(   file_handle_t*  file
,   char**          dst
);

/**
 * @brief Generates a copy of the entire file contents.
 * 
 * Uses dynamic memory allocation. Call string_free to free.
 * (see core/string.h)
 * 
 * @param file Handle to the file to read.
 * @param dst Output buffer to hold the handle to the null-terminated output
 * string.
 * @param read Output buffer to hold number of bytes read.
 * @return true if the handle stored in dst is valid; false otherwise.
 */
bool
file_read_all
(   file_handle_t*  file
,   u8**            dst
,   u64*            read
);

/**
 * @brief Writes a specified amount of data to a file.
 * 
 * @param file Handle to the file to write to.
 * @param size Number of bytes to write.
 * @param src The data to write.
 * @param written Output buffer to hold number of bytes written.
 * @return true if src written to file successfully; false otherwise.
 */
bool
file_write
(   file_handle_t*  file
,   u64             size
,   const void*     src
,   u64*            written
);

/**
 * @brief Writes a string to file and appends the `\n` character.
 * 
 * Use file_write_line to explicitly specify string length, or _file_write_line
 * to compute the length of a null-terminated string before passing it to
 * file_write_line.
 * 
 * @param file Handle to the file to write to.
 * @param size Number of bytes to write.
 * @param src The string to write.
 * @param written Output buffer to hold number of bytes written.
 * @return true if src written to file successfully; false otherwise.
 */
bool
file_write_line
(   file_handle_t*  file
,   u64             size
,   const char*     src
,   u64*            written
);

#define _file_write_line(file,src,written) \
    file_write_line ( (file) , _string_length ( src ) , (src) , (written) )

/**
 * @brief Obtains a handle to stdin.
 * 
 * @param file Output buffer.
 */
void
file_stdin
(   file_handle_t* file
);

/**
 * @brief Obtains a handle to stdout.
 * 
 * @param file Output buffer.
 */
void
file_stdout
(   file_handle_t* file
);

/**
 * @brief Obtains a handle to stderr.
 * 
 * @param file Output buffer.
 */
void
file_stderr
(   file_handle_t* file
);

#endif  // FILESYSTEM_H