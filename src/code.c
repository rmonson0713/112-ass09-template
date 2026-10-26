#include <stdlib.h>
#include <string.h>
#include "code.h"

/*
 * CSCI 112 Assignment 9 - Dynamic Memory and Memory Functions
 * 
 * Every function below is a skeleton that already compiles. Replace the
 * "// TODO" line (and the "(void)..." lines under it, which only silence
 * "unused parameter" warnings) with your implementation.
 */

/*
 * RULES (read them once, they apply to every function):
 *  - Check the result of every malloc/calloc/realloc. On failure return NULL / false
 *    and leak nothing (free what you already allocated).
 *  - Never write  p = realloc(p, ...);  - use a temporary pointer.
 *  - NULL arguments must never crash your function (see each description).
 *  - Only the standard library: <stdlib.h>, <string.h>, <stdbool.h>, <stddef.h>.
 *  - Compile with:  gcc -g -Wall -Wextra -fsanitize=address,undefined main.c code.c -o main
 */

/* ======================================================================
 * PART 1: Raw dynamic memory (malloc, calloc, realloc, free)
 * ====================================================================== */

/*
 * STEP 1: alloc_ints
 *
 * Allocate room for `count` ints with malloc().
 *  - count == 0 or allocation failure -> return NULL.
 *  - The memory is NOT initialized.
 */

int *alloc_ints(size_t count)
{
    if (count == 0)
    {
        return NULL;
    }
    int *arr = malloc(count * sizeof(int));

    if (arr == NULL)
    {
        return NULL;
    }

    return arr;
    (void)count;
    return NULL;
}

/*
 * STEP 2: alloc_zeroed_ints
 *
 * Same as alloc_ints, but every byte of the block must be zero (use calloc()).
 *  - count == 0 or allocation failure -> return NULL.
 */

int *alloc_zeroed_ints(size_t count)
{
    if (count == 0)
    {
        return NULL;
    }
    int *arr = calloc(count, sizeof(int));

    if (arr == NULL)
    {
        return NULL;
    }

    return arr;
    (void)count;
    return NULL;
}

/*
 * STEP 3: safe_free
 *
 * Receives the ADDRESS of a pointer (usage:  safe_free((void **)&p); ).
 * Free the block that *ptr points to and set *ptr to NULL (nullify!).
 *  - safe_free(NULL) does nothing.
 *  - If *ptr is already NULL, nothing happens.
 *  - Calling it twice on the same pointer must be harmless.
 */

void safe_free(void **ptr)
{
    if (ptr == NULL || *ptr == NULL)
    {
        return;
    }

    free(*ptr);
    *ptr = NULL;
    (void)ptr;
}

/*
 * STEP 4: resize_ints
 *
 * Change a block holding `old_count` ints so that it holds `new_count` ints.
 *  - new_count == 0: free arr and return NULL.
 *  - arr == NULL: behave like alloc_zeroed_ints(new_count).
 *  - Otherwise use realloc() through a TEMPORARY pointer. If it fails,
 *    return NULL and leave arr untouched and still valid.
 *  - Old values are kept. If the block grew, the new ints
 *    (old_count ... new_count - 1) must be set to zero with memset().
 *  - Return the (possibly moved) block.
 */

int *resize_ints(int *arr, size_t old_count, size_t new_count)
{
    if (new_count == 0)
    {
        free(arr);
        return NULL;
    }

    if (arr == NULL)
    {
        return alloc_zeroed_ints(new_count);
    }

    int *temp = realloc(arr, new_count * sizeof(int));

    if (temp == NULL)
    {
        return NULL;
    }

    if (new_count > old_count)
    {
        memset(temp + old_count, 0,
               (new_count - old_count) * sizeof(int));
    }

    return temp;
    (void)arr;
    (void)old_count;
    (void)new_count;
    return NULL;
}

/*
 * STEP 5: duplicate_ints
 *
 * Return a NEW block holding a copy of `count` ints from src (malloc + memcpy).
 *  - src == NULL, count == 0 or allocation failure -> return NULL.
 *  - The caller owns the returned block.
 */

int *duplicate_ints(const int *src, size_t count)
{
    if (src == NULL || count == 0)
    {
        return NULL;
    }

    int *arr = malloc(count * sizeof(int));

    if (arr == NULL)
    {
        return NULL;
    }

    memcpy(arr, src, count * sizeof(int));

    return arr;
    (void)src;
    (void)count;
    return NULL;
}

/* ======================================================================
 * PART 2: ByteBuffer - a growing block of bytes
 *
 * Invariants of a valid buffer:
 *   size <= capacity,  capacity >= 1,
 *   every byte of data from index `size` on is 0.
 * ====================================================================== */

/*
 * STEP 6: buffer_create
 *
 * Allocate the struct AND its data block (`capacity` bytes).
 * Fill the data block with zeros (memset), set size = 0.
 *  - capacity == 0 or any allocation failure -> return NULL.
 *  - If the struct was allocated but the data block was not,
 *    FREE THE STRUCT before returning NULL (no leak!).
 */

ByteBuffer *buffer_create(size_t capacity)
{
    if (capacity == 0)
    {
        return NULL;
    }

    ByteBuffer *buf = malloc(sizeof(ByteBuffer));

    if (buf == NULL)
    {
        return NULL;
    }

    buf->data = malloc(capacity);

    if (buf->data == NULL)
    {
        free(buf);
        return NULL;
    }

    memset(buf->data, 0, capacity);

    buf->size = 0;
    buf->capacity = capacity;

    return buf;
    (void)capacity;
    return NULL;
}

/*
 * STEP 7: buffer_reserve
 *
 * Make sure the buffer can hold at least `min_capacity` bytes.
 *  - buf == NULL -> false.
 *  - min_capacity <= capacity -> true, nothing changes.
 *  - Otherwise DOUBLE the capacity until it is >= min_capacity
 *    (4 -> 8 -> 16 -> 32 ...), realloc the data (temporary pointer!),
 *    set the new bytes to zero (memset) and update capacity. Return true.
 *  - If realloc fails -> false, the buffer stays unchanged.
 */

bool buffer_reserve(ByteBuffer *buf, size_t min_capacity)
{
    size_t new_capacity = buf->capacity;
    while (new_capacity < min_capacity)
    {
        new_capacity *= 2;
    }

    unsigned char *temp = realloc(buf->data, new_capacity);

    if (temp == NULL)
    {
        return false;
    }

    memset(temp + buf->capacity, 0, new_capacity - buf->capacity);

    buf->data = temp;
    buf->capacity = new_capacity;

    return true;
    (void)buf;
    (void)min_capacity;
    return false;
}

/*
 * STEP 8: buffer_append
 *
 * Append `n` bytes from `data` after the content (memcpy).
 * Grow with buffer_reserve(buf, size + n) if needed.
 *  - buf == NULL, or data == NULL while n > 0 -> false.
 *  - n == 0 -> true, nothing changes.
 *  - On success size grows by n.
 */

bool buffer_append(ByteBuffer *buf, const void *data, size_t n)
{
    if (buf == NULL)
    {
        return false;
    }

    if (n == 0)
    {
        return true;
    }

    if (data == NULL)
    {
        return false;
    }

    if (!buffer_reserve(buf, buf->size + n))
    {
        return false;
    }

    memcpy(buf->data + buf->size, data, n);

    buf->size += n;

    return true;
    (void)buf;
    (void)data;
    (void)n;
    return false;
}

/*
 * STEP 9: buffer_append_byte
 *
 * Append one byte. Hint: it is a one-liner using buffer_append().
 */

bool buffer_append_byte(ByteBuffer *buf, unsigned char byte)
{
    return buffer_append(buf, &byte, 1);
    (void)buf;
    (void)byte;
    return false;
}

/*
 * STEP 10: buffer_append_string
 *
 * Append the characters of `str` WITHOUT the terminating '\0'
 * (use strlen() and buffer_append()).
 *  - str == NULL -> false.
 *  - empty string -> true, nothing changes.
 */

bool buffer_append_string(ByteBuffer *buf, const char *str)
{
    if (str == NULL)
    {
        return false;
    }

    return buffer_append(buf, (const unsigned char *)str, strlen(str));
    (void)buf;
    (void)str;
    return false;
}

/*
 * STEP 11: buffer_find
 *
 * Return the index of the FIRST occurrence of `byte` in the CONTENT
 * (bytes 0 ... size-1, never the spare room!). Use memchr().
 *  - not found or buf == NULL -> -1.
 *  - Hint: index = (found pointer) - (start of data).
 */

int buffer_find(const ByteBuffer *buf, unsigned char byte)
{
    if (buf == NULL)
    {
        return -1;
    }

    unsigned char *found = memchr(buf->data, byte, buf->size);

    if (found == NULL)
    {
        return -1;
    }

    return found - buf->data;
    (void)buf;
    (void)byte;
    return -1;
}

/*
 * STEP 12: buffer_equals
 *
 * true if both buffers have the same size AND the same content bytes
 * (memcmp). Capacities do not matter. Two empty buffers are equal.
 *  - any argument NULL -> false.
 */

bool buffer_equals(const ByteBuffer *a, const ByteBuffer *b)
{
    if (a == NULL || b == NULL)
    {
        return false;
    }

    if (a->size != b->size)
    {
        return false;
    }

    if (a->size == 0)
    {
        return true;
    }

    return memcmp(a->data, b->data, a->size) == 0;
    (void)a;
    (void)b;
    return false;
}

/*
 * STEP 13: buffer_insert
 *
 * Insert `n` bytes from `data` so that they start at index `pos`.
 * The old bytes from `pos` on move to the right. pos == size means "append".
 *  - buf == NULL, pos > size, or data == NULL while n > 0 -> false
 *    (buffer unchanged).
 *  - n == 0 -> true, nothing changes.
 *  - Grow with buffer_reserve() if needed.
 *  - The tail moves onto ITSELF (overlap!) -> memmove(), then memcpy()
 *    for the new bytes.
 */

bool buffer_insert(ByteBuffer *buf, size_t pos, const void *data, size_t n)
{
    if (buf == NULL || pos > buf->size)
    {
        return false;
    }

    if (n == 0)
    {
        return true;
    }

    if (data == NULL)
    {
        return false;
    }

    if (!buffer_reserve(buf, buf->size + n))
    {
        return false;
    }

    memmove(buf->data + pos + n, buf->data + pos, buf->size - pos);

    memcpy(buf->data + pos, data, n);

    buf->size += n;

    return true;
    (void)buf;
    (void)pos;
    (void)data;
    (void)n;
    return false;
}

/*
 * STEP 14: buffer_remove
 *
 * Remove `n` bytes starting at index `pos`; the following bytes move left.
 *  - buf == NULL, pos > size or pos + n > size -> false (buffer unchanged).
 *    Write the check so that it cannot overflow:  n > size - pos
 *  - n == 0 -> true, nothing changes.
 *  - Use memmove() for the shift. Afterwards the n bytes that became unused
 *    at the end of the content must be set to zero (memset). Then size -= n.
 */

bool buffer_remove(ByteBuffer *buf, size_t pos, size_t n)
{
    if (n > buf->size - pos)
    {
        return false;
    }

    if (n == 0)
    {
        return true;
    }

    memmove(buf->data + pos,
            buf->data + pos + n,
            buf->size - pos - n);

    memset(buf->data + buf->size - n, 0, n);

    buf->size -= n;

    return true;
    (void)buf;
    (void)pos;
    (void)n;
    return false;
}

/*
 * STEP 15: buffer_clear
 *
 * Set size to 0 and zero the WHOLE data block (memset, `capacity` bytes).
 * The capacity stays the same. buf == NULL -> do nothing.
 */

void buffer_clear(ByteBuffer *buf)
{
    if (buf == NULL)
    {
        return;
    }

    memset(buf->data, 0, buf->capacity);

    buf->size = 0;
    (void)buf;
}

/*
 * STEP 16: buffer_destroy
 *
 * Receives the ADDRESS of your pointer (usage:  buffer_destroy(&buf); ).
 * Free the data block, then the struct, then set *buf to NULL.
 *  - buf == NULL or *buf == NULL -> do nothing.
 *  - Calling it twice must be harmless.
 */

void buffer_destroy(ByteBuffer **buf)
{
    if (buf == NULL || *buf == NULL)
    {
        return;
    }

    free((*buf)->data);
    free(*buf);
    *buf = NULL;
    (void)buf;
}

/* ======================================================================
 * PART 3: Deep copies, searching and arrays of strings
 * ====================================================================== */

/*
 * STEP 17: buffer_clone
 *
 * Return a DEEP copy: a new struct with its OWN data block, the same
 * capacity and the same content (memcpy `size` bytes).
 *  - src == NULL or allocation failure -> NULL.
 *  - Changing the clone must never change the source (and vice versa).
 *  - Hint: buffer_create() gives you the new buffer.
 */

ByteBuffer *buffer_clone(const ByteBuffer *src)
{
    if (src == NULL)
    {
        return NULL;
    }

    ByteBuffer *clone = buffer_create(src->capacity);

    if (clone == NULL)
    {
        return NULL;
    }

    memcpy(clone->data, src->data, src->size);

    clone->size = src->size;

    return clone;
    (void)src;
    return NULL;
}

/*
 * STEP 18: buffer_concat
 *
 * Return a NEW buffer holding the content of `a` followed by the content of `b`.
 *  - Its capacity is EXACTLY a->size + b->size (but at least 1).
 *  - Neither source is modified.
 *  - Any NULL argument or allocation failure -> NULL.
 *  - Hint: buffer_create() + buffer_append() twice.
 */

ByteBuffer *buffer_concat(const ByteBuffer *a, const ByteBuffer *b)
{
    if (a == NULL || b == NULL)
    {
        return NULL;
    }

    size_t capacity = a->size + b->size;

    if (capacity == 0)
    {
        capacity = 1;
    }

    ByteBuffer *result = buffer_create(capacity);

    if (result == NULL)
    {
        return NULL;
    }

    if (!buffer_append(result, a->data, a->size))
    {
        buffer_destroy(&result);
        return NULL;
    }

    if (!buffer_append(result, b->data, b->size))
    {
        buffer_destroy(&result);
        return NULL;
    }

    return result;
    (void)a;
    (void)b;
    return NULL;
}

/*
 * STEP 19: buffer_count
 *
 * Return how many times `byte` occurs in the CONTENT (bytes 0 ... size-1).
 * Use memchr() repeatedly, each time continuing right AFTER the last hit.
 *  - buf == NULL -> 0.
 */

size_t buffer_count(const ByteBuffer *buf, unsigned char byte)
{
    if (buf == NULL)
    {
        return 0;
    }

    size_t count = 0;
    unsigned char *current = buf->data;
    size_t remaining = buf->size;

    while (remaining > 0)
    {
        unsigned char *found = memchr(current, byte, remaining);

        if (found == NULL)
        {
            break;
        }

        count++;

        remaining -= (size_t)(found - current) + 1;
        current = found + 1;
    }

    return count;
    (void)buf;
    (void)byte;
    return 0;
}

/*
 * STEP 20: buffer_shrink_to_fit
 *
 * Reduce the capacity to `size` (but at least 1) using realloc through a
 * TEMPORARY pointer. The content stays.
 *  - buf == NULL -> false.
 *  - Success (also when nothing had to change) -> true.
 *  - realloc fails -> false, buffer unchanged.
 */

bool buffer_shrink_to_fit(ByteBuffer *buf)
{
    if (buf == NULL)
    {
        return false;
    }

    size_t new_capacity = buf->size;

    if (new_capacity == 0)
    {
        new_capacity = 1;
    }

    if (new_capacity == buf->capacity)
    {
        return true;
    }

    unsigned char *temp = realloc(buf->data, new_capacity);

    if (temp == NULL)
    {
        return false;
    }

    buf->data = temp;
    buf->capacity = new_capacity;

    return true;
    (void)buf;
    return false;
}

/*
 * STEP 21: buffer_split
 *
 * Split the content at every `delim` byte. Return a heap-allocated ARRAY
 * of heap-allocated STRINGS.
 *  - A buffer with k delimiters gives k + 1 tokens. Empty tokens are kept:
 *        "a,bb,,ccc" split at ','  ->  "a", "bb", "", "ccc"
 *        "a,"                      ->  "a", ""
 *        ""   (empty buffer)       ->  exactly ONE empty token ""
 *  - Every token is a separate malloc'ed, '\0'-terminated copy
 *    (memcpy + terminator). Tokens never point into the buffer;
 *    the buffer is not modified.
 *  - The array of pointers must grow while you discover tokens: start small
 *    (for example 4 pointers) and use realloc (doubling).
 *  - Use memchr() to find the delimiters.
 *  - *out_count receives the number of tokens.
 *  - On ANY failure (also buf == NULL or out_count == NULL): return NULL;
 *    if out_count != NULL set *out_count = 0; and FREE everything you
 *    allocated so far.
 */

char **buffer_split(const ByteBuffer *buf, unsigned char delim, size_t *out_count)
{
    if (out_count != NULL)
    {
        *out_count = 0;
    }

    if (buf == NULL || out_count == NULL)
    {
        return NULL;
    }

    size_t capacity = 4;
    size_t count = 0;

    char **tokens = malloc(capacity * sizeof(char *));

    if (tokens == NULL)
    {
        return NULL;
    }

    const unsigned char *start = buf->data;
    size_t remaining = buf->size;

    while (1)
    {
        unsigned char *found = memchr(start, delim, remaining);

        size_t length;

        if (found == NULL)
        {
            length = remaining;
        }
        else
        {
            length = (size_t)(found - start);
        }

        char *token = malloc(length + 1);

        if (token == NULL)
        {
            for (size_t i = 0; i < count; i++)
            {
                free(tokens[i]);
            }

            free(tokens);
            return NULL;
        }

        memcpy(token, start, length);
        token[length] = '\0';

        if (count == capacity)
        {
            size_t new_capacity = capacity * 2;
            char **temp = realloc(tokens,
                                  new_capacity * sizeof(char *));

            if (temp == NULL)
            {
                free(token);

                for (size_t i = 0; i < count; i++)
                {
                    free(tokens[i]);
                }

                free(tokens);
                return NULL;
            }

            tokens = temp;
            capacity = new_capacity;
        }

        tokens[count] = token;
        count++;

        if (found == NULL)
        {
            break;
        }

        start = found + 1;
        remaining -= length + 1;
    }

    *out_count = count;

    return tokens;
    (void)buf;
    (void)delim;
    (void)out_count;
    return NULL;
}

/*
 * STEP 22: free_tokens
 *
 * Receives the ADDRESS of your tokens pointer (usage: free_tokens(&tokens, n); ).
 * Free every token first, then the array itself, then set *tokens to NULL.
 *  - tokens == NULL or *tokens == NULL -> do nothing.
 *  - Calling it twice must be harmless.
 */

void free_tokens(char ***tokens, size_t count)
{
    if (tokens == NULL || *tokens == NULL)
    {
        return;
    }

    for (size_t i = 0; i < count; i++)
    {
        free((*tokens)[i]);
    }

    free(*tokens);
    *tokens = NULL;
    (void)tokens;
    (void)count;
}
