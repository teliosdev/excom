#ifndef _EXCOM_TEST_HELPER_H
# define _EXCOM_TEST_HELPER_H

# ifndef UTEST_NOCOLOR
#  define TEXT_COLOR_BLACK "\e[0;30m"
#  define TEXT_COLOR_BOLD_BLACK "\e[0;1;30m"
#  define TEXT_COLOR_RED "\e[0;31m"
#  define TEXT_COLOR_BOLD_RED "\e[0;1;31m"
#  define TEXT_COLOR_GREEN "\e[0;32m"
#  define TEXT_COLOR_BOLD_GREEN "\e[0;1;32m"
#  define TEXT_COLOR_YELLOW "\e[0;33m"
#  define TEXT_COLOR_BOLD_YELLOW "\e[0;1;33m"
#  define TEXT_COLOR_BLUE "\e[0;34m"
#  define TEXT_COLOR_BOLD_BLUE "\e[0;1;34m"
#  define TEXT_COLOR_MAGENTA "\e[0;35m"
#  define TEXT_COLOR_BOLD_MAGENTA "\e[0;1;35m"
#  define TEXT_COLOR_CYAN "\e[0;36m"
#  define TEXT_COLOR_BOLD_CYAN "\e[0;1;36m"
#  define TEXT_COLOR_WHITE "\e[0;37m"
#  define TEXT_COLOR_BOLD_WHITE "\e[0;1;37m"
#  define TEXT_COLOR_RESET "\e[0m"
# else
#  define TEXT_COLOR_BLACK ""
#  define TEXT_COLOR_BOLD_BLACK ""
#  define TEXT_COLOR_RED ""
#  define TEXT_COLOR_BOLD_RED ""
#  define TEXT_COLOR_GREEN ""
#  define TEXT_COLOR_BOLD_GREEN ""
#  define TEXT_COLOR_YELLOW ""
#  define TEXT_COLOR_BOLD_YELLOW ""
#  define TEXT_COLOR_BLUE ""
#  define TEXT_COLOR_BOLD_BLUE ""
#  define TEXT_COLOR_MAGENTA ""
#  define TEXT_COLOR_BOLD_MAGENTA ""
#  define TEXT_COLOR_CYAN ""
#  define TEXT_COLOR_BOLD_CYAN ""
#  define TEXT_COLOR_WHITE ""
#  define TEXT_COLOR_BOLD_WHITE ""
#  define TEXT_COLOR_RESET ""
# endif

# define output(format) printf(format TEXT_COLOR_RESET)
# define output2(format, ...) printf(format TEXT_COLOR_RESET, __VA_ARGS__)
# define error(format) fprintf(stderr, format TEXT_COLOR_RESET)
# define error2(format, ...) fprintf(stderr, format TEXT_COLOR_RESET, __VA_ARGS__)

# define TEST_RETURN (failed > 0)

#endif

#ifdef SET
# undef SET
#endif

#ifdef TEST
# undef TEST
#endif

#ifdef BEFORE
# undef BEFORE
#endif

#ifdef AFTER
# undef AFTER
#endif

#ifdef PREFIX
# undef PREFIX
#endif

#ifdef TEST_PREFIX
# define SET(_)
# define TEST(_, __)
# define BEFORE(_)
# define AFTER(_)
# define PREFIX(body) body
# include <stdlib.h>
# include <stdio.h>
#endif

#ifdef TEST_BEFORE

# define TEST_BEFORE_HOOK1 \
  size_t sets   = 0;       \
  size_t tests  = 0;       \
  size_t failed = 0;
# define SET(_) sets++;
# define TEST(_, __) tests++;
# define BEFORE(body) body;
# define AFTER(_)
# define PREFIX(_)

# define TEST_BEFORE_HOOK2                               \
  output(TEXT_COLOR_MAGENTA "BEGIN TESTING FROM FILE \"" \
    TEXT_COLOR_BOLD_MAGENTA FILE TEXT_COLOR_MAGENTA      \
    "\":\n");

#endif

#ifdef TEST_DURING

# define SET(name)                     \
  output(TEXT_COLOR_MAGENTA "\tSET \"" \
    TEXT_COLOR_BOLD_MAGENTA name       \
    TEXT_COLOR_MAGENTA "\":\n");
# define BEFORE(_)
# define AFTER(_)
# define PREFIX(_)

# define uassert_empty(value) \
  uassert(((intmax_t) (value)) == 0)

# define uassert_equal(v1, v2) \
  uassert(v1 == v2)

# define uassert_nequal(v1, v2) \
  uassert(v1 != v2)

# define uassert_same(str1, str2, len) \
  uassert_empty(strncmp((char*) str1, (char*) str2, len))


# ifndef VERBOSE
#   define TEST(name, body) do                \
    {                                         \
      int test_success = 1;                   \
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" \
        TEXT_COLOR_BOLD_MAGENTA name          \
        TEXT_COLOR_MAGENTA "\": ");           \
      do body while(0);                       \
      if(test_success) {                      \
        output(TEXT_COLOR_BOLD_GREEN "OK\n"); \
      }                                       \
    } while(0);

#   define uassert(body) do                             \
    {                                                   \
      if(!(body)) {                                     \
        test_success = 0;                               \
        failed++;                                       \
        output2("\n" TEXT_COLOR_RED                     \
          "\t\t\tASSERT FAILED "                        \
          "\"" TEXT_COLOR_BOLD_RED "%s" TEXT_COLOR_RED  \
          "\"\n\t\t\t" TEXT_COLOR_RED "LINE "           \
          TEXT_COLOR_BOLD_RED "%d"  "\n\t\t\t"          \
          TEXT_COLOR_RED "FILE \"" TEXT_COLOR_BOLD_RED  \
          "%s" TEXT_COLOR_RED  "\"\n", #body, __LINE__, \
          FILE);                                        \
      }                                                 \
  } while(0)
# else

#   define TEST(name, body) do                   \
    {                                            \
      int test_success = 1;                      \
      output(TEXT_COLOR_MAGENTA "\t\tTEST \""    \
        TEXT_COLOR_BOLD_MAGENTA name             \
        TEXT_COLOR_MAGENTA "\": ");              \
      do body while(0);                          \
      if(test_success) {                         \
        output("\n\t\t\t" TEXT_COLOR_GREEN       \
          "OK\n");                               \
      }                                          \
    } while(0);

#   define uassert(body) do                             \
    {                                                   \
      output(TEXT_COLOR_MAGENTA "\n\t\t\tASSERT \""     \
        TEXT_COLOR_BOLD_MAGENTA #body TEXT_COLOR_MAGENTA\
        "\"");                                          \
      if(!(body)) {                                     \
        test_success = 0;                               \
        failed++;                                       \
        output2("\n" TEXT_COLOR_RED                     \
          "\t\t\tASSERT FAILED "                        \
          "\"" TEXT_COLOR_BOLD_RED "%s" TEXT_COLOR_RED  \
          "\"\n\t\t\t" TEXT_COLOR_RED "LINE "           \
          TEXT_COLOR_BOLD_RED  "%d"  "\n\t\t\t"         \
          TEXT_COLOR_RED "FILE \""  TEXT_COLOR_BOLD_RED \
          "%s" TEXT_COLOR_RED  "\"", #body, __LINE__,   \
          FILE);                                        \
      }                                                 \
    } while(0)
# endif

#endif

#ifdef TEST_AFTER

# define SET(_)
# define TEST(_, __)
# define BEFORE(_)
# define AFTER(body) body;
# define PREFIX(_)

# define TEST_AFTER_HOOK1 do                            \
  {                                                     \
    output2("\n\t" TEXT_COLOR_MAGENTA "RESULT:\n\t\t"   \
      TEXT_COLOR_MAGENTA "PASSED TESTS: "               \
      TEXT_COLOR_BOLD_MAGENTA "%zu" TEXT_COLOR_MAGENTA  \
      "\n\t\t" TEXT_COLOR_MAGENTA "FAILED TESTS: "      \
      TEXT_COLOR_BOLD_MAGENTA "%zu"                     \
      TEXT_COLOR_MAGENTA "\n", tests - failed, failed); \
  } while(0)

# define TEST_AFTER_HOOK2
#endif
