#ifndef CODE_H
#define CODE_H

/*
 * CSCI 112 Assignment 9 - Dynamic Memory and Memory Functions
 *
 * This header is complete: it contains the ByteBuffer type and the prototypes
 * of all 22 functions. Do NOT change any signature - the automated tests
 * rely on them. You implement the functions in code.c.
 *
 * Each prototype is preceded by a short description of what the function
 * must do. The detailed step-by-step notes are in code.c.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/* ======================================================================
 * PART 1: Raw dynamic memory (malloc, calloc, realloc, free)
 * ====================================================================== */

/*
 * STEP 1: alloc_ints
 * Allocates room for `count` ints on the heap with malloc(). The memory is
 * not initialized. Returns NULL if count is 0 or if the allocation fails.
 * The caller owns the returned block and must free it.
 */
int *alloc_ints(size_t count);
/*
 * STEP 2: alloc_zeroed_ints
 * Does the same as alloc_ints(), but every byte of the block is zero
 * (use calloc()). Returns NULL if count is 0 or if the allocation fails.
 */
int *alloc_zeroed_ints(size_t count);
/*
 * STEP 3: safe_free
 * Frees the block that a pointer points to AND sets that pointer to NULL, so
 * it can never become a dangling pointer. It receives the address of the
 * pointer, e.g. safe_free((void **)&p). Passing NULL, or the address of a
 * pointer that is already NULL, does nothing. Calling it twice on the same
 * pointer is harmless.
 */
void safe_free(void **ptr);

/*
 * STEP 4: resize_ints
 * Changes a block of `old_count` ints to a block of `new_count` ints and
 * returns the (possibly moved) block. Old values are kept; if the block grew,
 * the new ints are set to zero. If new_count is 0 the block is freed and NULL
 * is returned. If arr is NULL it behaves like alloc_zeroed_ints(new_count).
 * If realloc() fails it returns NULL and the original block stays valid and
 * untouched (use a temporary pointer!).
 */
int *resize_ints(int *arr, size_t old_count, size_t new_count);

/*
 * STEP 5: duplicate_ints
 * Returns a new heap block that contains a copy of the first `count` ints
 * of src (malloc + memcpy). Returns NULL if src is NULL, count is 0 or the
 * allocation fails. Changing the copy never changes src.
 */
int *duplicate_ints(const int *src, size_t count);

/* ======================================================================
 * PART 2: ByteBuffer - a growing block of bytes
 * ====================================================================== */

/*
 * A ByteBuffer is a block of bytes that grows when more data is added than it
 * can hold.
 *   data     - heap block of `capacity` bytes
 *   size     - number of bytes in use (the content)
 *   capacity - number of bytes allocated
 *
 * Invariants of a valid buffer: size <= capacity, capacity >= 1, and every
 * byte of data from index `size` on (the spare room) is zero.
 * A buffer is NOT '\0'-terminated.
 */
typedef struct
{
    unsigned char *data;
    size_t size;
    size_t capacity;
} ByteBuffer;

/*
 * STEP 6: buffer_create
 * Creates an empty buffer: allocates the struct and a zero-filled data block
 * of `capacity` bytes, and sets size to 0. Returns NULL if capacity is 0 or
 * any allocation fails; if only the data block failed, the struct must be
 * freed first so nothing leaks.
 */
ByteBuffer *buffer_create(size_t capacity);

/*
 * STEP 7: buffer_reserve
 * Makes sure the buffer can hold at least `min_capacity` bytes. If it already
 * can, nothing changes. Otherwise the capacity is doubled until it is large
 * enough (4 -> 8 -> 16 -> 32 ...), the data block is reallocated and the new
 * bytes are zeroed. Returns true on success, false if buf is NULL or realloc()
 * fails (the buffer is then unchanged).
 */
bool buffer_reserve(ByteBuffer *buf, size_t min_capacity);

/*
 * STEP 8: buffer_append
 * Appends `n` bytes from `data` after the current content, growing the buffer
 * with buffer_reserve() when needed. Returns false if buf is NULL, or data is
 * NULL while n > 0, or the buffer cannot grow. Appending 0 bytes succeeds and
 * changes nothing.
 */
bool buffer_append(ByteBuffer *buf, const void *data, size_t n);

/*
 * STEP 9: buffer_append_byte
 * Appends a single byte to the buffer. Returns the same result as
 * buffer_append() would.
 */
bool buffer_append_byte(ByteBuffer *buf, unsigned char byte);

/*
 * STEP 10: buffer_append_string
 * Appends the characters of a C string WITHOUT its terminating '\0'.
 * Returns false if str is NULL or the append fails. An empty string succeeds
 * and changes nothing.
 */
bool buffer_append_string(ByteBuffer *buf, const char *str);

/*
 * STEP 11: buffer_find
 * Returns the index of the first occurrence of `byte` in the content (only
 * the first `size` bytes are searched, never the spare room). Returns -1 if
 * the byte is not found or buf is NULL.
 */
int buffer_find(const ByteBuffer *buf, unsigned char byte);

/*
 * STEP 12: buffer_equals
 * Returns true if both buffers have the same size and exactly the same
 * content. Capacities do not matter, two empty buffers are equal. Returns
 * false if either pointer is NULL.
 */
bool buffer_equals(const ByteBuffer *a, const ByteBuffer *b);

/*
 * STEP 13: buffer_insert
 * Inserts `n` bytes from `data` so that they start at index `pos`; the old
 * bytes from pos onward move to the right. pos may be 0 ... size (pos == size
 * appends). Grows the buffer when needed. Returns false, leaving the buffer
 * unchanged, if buf is NULL, pos > size, or data is NULL while n > 0.
 * Inserting 0 bytes succeeds and changes nothing.
 */
bool buffer_insert(ByteBuffer *buf, size_t pos, const void *data, size_t n);

/*
 * STEP 14: buffer_remove
 * Removes `n` bytes starting at index `pos`; the following bytes move to the
 * left, and the n bytes that became unused at the end are set to zero.
 * Returns false, leaving the buffer unchanged, if buf is NULL or the range
 * pos ... pos+n-1 is not completely inside the content. Removing 0 bytes
 * succeeds and changes nothing.
 */
bool buffer_remove(ByteBuffer *buf, size_t pos, size_t n);

/*
 * STEP 15: buffer_clear
 * Empties the buffer: size becomes 0 and the whole data block is zeroed.
 * The capacity stays the same. Does nothing if buf is NULL.
 */
void buffer_clear(ByteBuffer *buf);

/*
 * STEP 16: buffer_destroy
 * Frees the data block, then the struct, and sets the caller's pointer to
 * NULL. It receives the address of the pointer, e.g. buffer_destroy(&buf).
 * Passing NULL, or the address of a pointer that is already NULL, does
 * nothing. Calling it twice is harmless.
 */
void buffer_destroy(ByteBuffer **buf);

/* ======================================================================
 * PART 3: Deep copies, searching and arrays of strings
 * ====================================================================== */

/*
 * STEP 17: buffer_clone
 * Returns a deep copy of a buffer: a new struct with its own data block, the
 * same capacity and the same content. Changing the clone never changes the
 * source. Returns NULL if src is NULL or the allocation fails.
 */
ByteBuffer *buffer_clone(const ByteBuffer *src);

/*
 * STEP 18: buffer_concat
 * Returns a new buffer holding the content of `a` followed by the content of
 * `b`. Its capacity is exactly a->size + b->size (but at least 1). Neither
 * source is modified. Returns NULL if an argument is NULL or an allocation
 * fails.
 */
ByteBuffer *buffer_concat(const ByteBuffer *a, const ByteBuffer *b);

/*
 * STEP 19: buffer_count
 * Returns how many times `byte` occurs in the content (the first `size`
 * bytes only). Returns 0 if buf is NULL.
 */
size_t buffer_count(const ByteBuffer *buf, unsigned char byte);

/*
 * STEP 20: buffer_shrink_to_fit
 * Reduces the capacity to the current size (but at least 1) with realloc();
 * the content is kept. Returns true on success (also when nothing had to
 * change), false if buf is NULL or realloc() fails (buffer then unchanged).
 */
bool buffer_shrink_to_fit(ByteBuffer *buf);

/*
 * STEP 21: buffer_split
 * Splits the content at every `delim` byte and returns a heap-allocated array
 * of heap-allocated, '\0'-terminated strings (tokens). A buffer with k
 * delimiters gives k + 1 tokens and empty tokens are kept, e.g. "a,bb,,ccc"
 * split at ',' gives "a", "bb", "", "ccc"; an empty buffer gives exactly one
 * empty token. Tokens are separate copies and the buffer is not modified.
 * *out_count receives the number of tokens. On any failure (also when buf or
 * out_count is NULL) it returns NULL, sets *out_count to 0 when out_count is
 * not NULL, and frees everything it had already allocated.
 */
char **buffer_split(const ByteBuffer *buf, unsigned char delim, size_t *out_count);

/*
 * STEP 22: free_tokens
 * Frees an array returned by buffer_split(): first every token, then the
 * array itself, then sets the caller's pointer to NULL. It receives the
 * address of the pointer, e.g. free_tokens(&tokens, n). Passing NULL, or the
 * address of a pointer that is already NULL, does nothing. Calling it twice
 * is harmless.
 */
void free_tokens(char ***tokens, size_t count);

#endif
