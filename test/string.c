#include <stdlib.h>
#include <stdio.h>
#include "excom.h"
#define __FILE "string.test"
#include "utest.h"

//prefix


static size_t tests  = 0;
static size_t failed = 0;


  
  
  
    
    
    void _string_management_initialization()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " initialization" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 4 "string.test"
excom_string_t string;

#line 5 "string.test"
excom_string_init(&string);


      //body
      #line 8 "string.test"
uassert_empty(string.size);

#line 9 "string.test"
uassert_empty(string.body);

#line 10 "string.test"
uassert_empty(string.freeable);


      //after
      
  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _string_management_filling()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " filling" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 4 "string.test"
excom_string_t string;

#line 5 "string.test"
excom_string_init(&string);


#line 14 "string.test"
char str[] = "hello world";

#line 15 "string.test"
excom_string_fill(&string, strlen(str), str);

      //body
      #line 16 "string.test"
uassert_equal(string.body, str);

#line 17 "string.test"
uassert_equal(string.size, strlen(str));

#line 18 "string.test"
uassert_empty(string.freeable);


      //after
      
  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _string_management_free_mark()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " free mark" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 4 "string.test"
excom_string_t string;

#line 5 "string.test"
excom_string_init(&string);


      //body
      #line 21 "string.test"
uassert_empty(string.freeable);

#line 22 "string.test"
excom_string_freeable(&string);

#line 23 "string.test"
uassert(string.freeable);

#line 24 "string.test"
uassert(excom_string_isfreeable(&string));

#line 25 "string.test"
excom_string_unfreeable(&string);

#line 26 "string.test"
uassert_empty(string.freeable);


      //after
      
  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _string_management_duplication()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " duplication" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 4 "string.test"
excom_string_t string;

#line 5 "string.test"
excom_string_init(&string);


#line 30 "string.test"
char str[] = "hello world";

#line 31 "string.test"
excom_string_fill(&string, strlen(str), str);

      //body
      #line 32 "string.test"
excom_string_t out;

#line 33 "string.test"
excom_string_dup(&string, &out);

#line 34 "string.test"
uassert_equal(string.size, out.size);

#line 35 "string.test"
uassert_same(string.body, out.body, out.size);

#line 36 "string.test"
uassert(out.freeable);

#line 37 "string.test"
uassert_empty(out.body[out.size]);

#line 38 "string.test"
free(out.body);

      //after
      
  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
  void _string_management()
  {
    output(TEXT_COLOR_MAGENTA "\tSET \"" TEXT_COLOR_BOLD_MAGENTA
      " string management" TEXT_COLOR_MAGENTA
      "\":\n");
    
    
    
    _string_management_initialization();
    
    
    _string_management_filling();
    
    
    _string_management_free_mark();
    
    
    _string_management_duplication();
    
  }


int main()
{
  output(TEXT_COLOR_MAGENTA "FILE \""  TEXT_COLOR_BOLD_MAGENTA
         __FILE TEXT_COLOR_MAGENTA "\":\n");

  
  
  _string_management();
  

  output2("\n\t" TEXT_COLOR_MAGENTA "RESULT:\n\t\t"
    "PASSED: " TEXT_COLOR_BOLD_MAGENTA "%zu"
    TEXT_COLOR_MAGENTA "\n\t\tFAILED: "
    TEXT_COLOR_BOLD_MAGENTA "%zu" TEXT_COLOR_MAGENTA
    "\n", tests - failed, failed);

  return TEST_RETURN;
}

