#ifndef _EXCOM_STRING_H
#define _EXCOM_STRING_H

typedef struct excom_string
{
  uint32_t size;
  char* body;
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
 * encountered, or 0 if none were.
 *
 * @param [in] string The string with the contents to copy over.
 * @param [out] out The string to copy the contents over to.
 * @returns The error code, or 0.
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
 * @returns The error code, or 0.
 */
int excom_string_dupfill(excom_string_t* string, uint32_t size,
  char* body);

#endif
