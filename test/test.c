#include <stdlib.h>
#include <stdio.h>
#include "excom.h"
#define __FILE "test.test2"
#include "utest.h"

//prefix


static size_t sets   = 1;
static size_t tests  = 0;
static size_t failed = 0;


  
  
  
    
    void _v1_something()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " something" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 4 "test.test2" 
testing

      //body
      #line 5 "test.test2" 
watermelons();

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
  
  void _v1() 
  {
    output(TEXT_COLOR_MAGENTA "\tSET \"" TEXT_COLOR_BOLD_MAGENTA
      " v1" TEXT_COLOR_MAGENTA
      "\":\n");
    
    
    _v1_something();
    
  }


int main()
{
  
  
  
  _v1();
  
  
  output2("\n\t" TEXT_COLOR_MAGENTA "RESULT:\n\t\t"
    "PASSED TESTS: " TEXT_COLOR_BOLD_MAGENTA "%zu"
    TEXT_COLOR_MAGENTA "\n\t\tFAILED TESTS: "
    TEXT_COLOR_BOLD_MAGENTA "%zu" TEXT_COLOR_MAGENTA
    "\n", tests - failed, failed);
  
  return TEST_RETURN;
}

