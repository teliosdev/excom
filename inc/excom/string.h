#ifndef _EXCOM_STRING_H
#define _EXCOM_STRING_H

/*!
 * @file "excom/string.h"
 * Defines types that helps the excom library cope with strings, with
 * strings being defined as any set of characters that can be
 * represented by a series of single bytes, which can be accessed
 * individually.
 */

/*!
 * A representation of a string.  It contains basic information such
 * as the length of the string and the actual content of the string.
 */
typedef struct excom_string
{
  /*!
   * The size of the string.
   */
  uint32_t size;

  /*!
   * The actual content of the string.  This may not be malloc()d, and
   * this may be in the middle of an allocation; if we can free() it
   * is stored in freeable.
   */
  char* body;

  /*!
   * Whether or not this string is freeable, i.e. whether or not we
   * can call free() on body.
   */
  bool freeable;
} excom_string_t;

/*!
 * Initialize a string, giving it its initial values.
 *
 * @param [out] string The string to initialize.
 */
void excom_string_init(excom_string_t* string);

/*!
 * Fills a string with the given size and body.
 *
 * @param [out] string The string to fill.
 * @param [in] size The size of the body.
 * @param [in] body The body to put in the string.
 */
void excom_string_fill(excom_string_t* string, uint32_t size,
  char* body);

/*!
 * Duplicates a given string.  The out parameter should be a
 * valid pointer, however it does not need to be initialized.  Please
 * note that this function does allocate memory in order to copy the
 * body over; the return value should be any error codes it
 * encountered, or `0` if none were.
 *
 * @param [in] string The string with the contents to copy over.
 * @param [out] out The string to copy the contents over to.
 * @returns An error code, or `0` if there was none.
 */
int excom_string_dup(excom_string_t* string,
  excom_string_t* out);

/*!
 * Fills a given string with a duplicate of the data given.  Please
 * note that this function does allocate memory in order to copy the
 * body over; the return value should be any error codes it encounters,
 * or 0.
 *
 * @param [out] string The string to fill.
 * @param [in] size The size of the body.
 * @param [in] body The data to copy over.
 * @returns An error code, or `0` if there was none.
 */
int excom_string_dupfill(excom_string_t* string, uint32_t size,
  char* body);

/*!
 * Marks the string as freeable.
 *
 * @param [in] str The string that should be marked as freeable.  **It
 *   must be a pointer to an excom_string_t!**
 * @returns nothing.
 */
#define excom_string_freeable(str) { (str)->freeable = true; }

/*!
 * Marks the string as unfreeable.
 *
 * @param [in] str The string that should be marked as unfreeable. **It
 *   must be a pointer to an excom_string_t!**
 */
#define excom_string_unfreeable(str) { (str)->freeable = false; }

/*!
 * Returns whether or not the string is freeable.
 *
 * @param [in] str The string that should be checked for freeability.
 *   **It must be a pointer to an excom_string_t!**
 * @returns A bool value.
 */
#define excom_string_isfreeable(str) ((str)->freeable)

#endif
