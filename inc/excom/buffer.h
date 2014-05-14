#ifndef _EXCOM_BUFFER_H
#define _EXCOM_BUFFER_H

/*!
 * @file "excom/buffer.h"
 * Defines a buffer that is used across the excom project.
 * Very handy, I must say.
 */

 /*!
  * @defgroup append Buffer Append Functions
  *   Functions to append to a given buffer.
  */

/*!
 * A buffer.  Buffers have a definite size to start with, but
 * grow in order to accomidate their contents.  The starting
 * size is recommended to be a power of 2 (since the buffer
 * grows by multiplying the size by two).  Buffers can be
 * unmutable and/or freeable.  An unmutable bufer will reject
 * any modifications to it with an EACCES error code.  A
 * freeable buffer means that the contents can be freed.  A
 * buffer, by default, is mutable and freeable.
 */
typedef struct excom_buffer
{
  /*!
   * For internal use only.  The original starting size of the
   * buffer.
   */
  size_t _init;

  /*!
   * The maximum number of bytes the buffer can handle.  This
   * is used to resize the buffer if needed.
   */
  size_t max;

  /*!
   * The number of bytes that have been used in the buffer.  This
   * will **never** exceed `max`.
   */
  size_t used;

  /*!
   * The current position of the pointer in the buffer.  This will
   * always be somewhere between `buf` and `buf + max - 1`.
   */
  uint8_t* pos;

  /*!
   * The start of the buffer.
   */
  uint8_t* buf;

  /*!
   * Whether or not the buffer is unmutable.  By default, it is
   * mutable.
   */
  bool unmutable;

  /*!
   * Whether or not the buffer is freeable.  This means that the
   * contents of the buffer may be freed.  Defaults to true.
   * If, for example, a pointer is stored to the content of the
   * buffer, then this should be changed to false, so that the
   * pointer isn't invalidated.
   */
  bool freeable;

  /*!
   * This is locked when a buffer is being read or written.
   * This is to ensure nothing bad happens to buffers.
   */
  excom_mutex_t mutex;
} excom_buffer_t;

/*!
 * Initializes the buffer.
 *
 * @param [inout] buffer The buffer to initialize.
 * @param [in] start The starting size of the buffer.  This is
 *   recommended to be a power of two.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_init(excom_buffer_t* buffer, size_t start);

/*!
 * Resets the buffer to its original size.  This completely wipes
 * the contents of the buffer.
 *
 * @param [inout] buffer The buffer to reset.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_reset(excom_buffer_t* buffer);

/*!
 * Resizes the buffer to fit the given number of bytes.  The
 * function only resizes up.  If the number of bytes can be
 * sufficiently held by the current buffer, it makes no change.
 * This function takes no consideration for the remaining
 * number of bytes, however.
 *
 * @param [inout] buffer The buffer to resize.
 * @param [in] to_fit The number of bytes the buffer should fit.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_resize(excom_buffer_t* buffer, size_t to_fit);

/*!
 * Appends a C string to the buffer.  The function will not
 * guess the size of the string, so it must be supplied.
 *
 * @ingroup append
 * @param [inout] buffer The buffer to append to.
 * @param [in] str The C string to append.
 * @param [in] size The size of the C string.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_cappend(excom_buffer_t* buffer, const char* str,
  size_t size);

/*!
 * Appends an excom string to the buffer.
 *
 * @ingroup append
 * @param [inout] buffer The buffer to append to.
 * @param [in] str The string to append to the buffer.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_sappend(excom_buffer_t* buffer,
  excom_string_t* str);

/*!
 * Appends a buffer to the buffer.  It appends the entirety
 * of the used buffer, from the start to the end.
 * basically, from `src->buf` to `src->buf + src->used`.
 *
 * @ingroup append
 * @param [inout] buffer The buffer to append to.
 * @param [in] src The buffer which is appended.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_bappend(excom_buffer_t* buffer,
  excom_buffer_t* src);

/*!
 * Appends a buffer to the buffer.  This is different from
 * excom_buffer_bappend in that it does it up until a given
 * point, and then stops.  From `src->buf` to `src->buf + size`.
 *
 * @ingroup append
 * @param [inout] buffer The buffer to append to.
 * @param [in] src The buffer which is appended.
 * @param [in] size The amount to append.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_bappend2(excom_buffer_t* buffer,
  excom_buffer_t* src, size_t size);

/*!
 * Appends a buffer to the buffer.  This is different from
 * excom_buffer_bappend in that it appends from the current
 * position until the end.  From `src->pos` to
 * `src->buf + src->used`.
 *
 * @ingroup append
 * @param [inout] buffer The buffer to append to.
 * @param [in] src The buffer which is appended.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_bappend_remaining(excom_buffer_t* buffer,
                                   excom_buffer_t* src);

/*!
 * Appends a buffer to the buffer.  This is different from
 * excom_buffer_bappend_remaining in that it appends until
 * a certain point, and then stops.  From `src->pos` to
 * `src->pos + size`.
 *
 * @ingroup append
 * @param [inout] buffer The buffer to append to.
 * @param [in] src The buffer which is appended.
 * @param [in] size The amount to append.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_bappend_remaining2(excom_buffer_t* buffer,
                                    excom_buffer_t* src,
                                    size_t size);

/*!
 * Formats, and then appends to a buffer.  Works in  a
 * similar manner to snprintf.  Some of the options
 * accepted are:
 *
 * @li `%%s`  - A C string, which is terminated by a NUL
 *   byte.
 * @li `%%S`  - A excom_string_t
 * @li `%%b`  - A C string, whose size is given before the
 *   string in the arguments (which would be a uint32_t).
 * @li `%%` - An actual percent sign.
 *
 * @param [inout] out The buffer to append the formatted
 *   string to.
 * @param [in] format The format for the buffer.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_format(excom_buffer_t* out,
  const char* format, ...);

/*!
 * Destroys a given buffer.  If the freeable flag is set
 * to true, it frees the body, as well.  Do not use any
 * part of the buffer after this; the content of the
 * buffer is undefined.
 *
 * @param [inout] buffer The buffer to destroy.
 */
void excom_buffer_destroy(excom_buffer_t* buffer);

/*!
 * Writes the buffer to the given socket.  It increments
 * the pointer as much as it can.  Writes from the pointer
 * to the end of the buffer (defined as
 * `buffer->buf + buffer->used`).
 *
 * @param [inout] buffer The buffer to write to the socket.
 * @param [in] sock The socket to write to.
 * @return 0 on success, an error code on failure.
 */
int excom_buffer_write(excom_buffer_t* buffer, int sock);

/*!
 * Returns the number of bytes remaining in the buffer -
 * i.e., the number of used bytes after the `buffer->pos`
 * pointer.
 *
 * @param [in] buffer
 * @return The number of remaining used bytes.
 */
static inline uintptr_t excom_buffer_remaining(excom_buffer_t* buffer) {
  return buffer->used - (buffer->pos - buffer->buf);
}


#endif
