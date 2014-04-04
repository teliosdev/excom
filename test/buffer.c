#include <stdlib.h>
#include <stdio.h>
#include "excom.h"
#define __FILE "buffer.test"
#include "utest.h"

//prefix
int excom_buffer_write(excom_buffer_t*, int);


static size_t tests  = 0;
static size_t failed = 0;


  
  
  
    
    
    
    
    void _buffer_management_initialization()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " initialization" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 11 "buffer.test" 
uassert_equal(buffer.max, 32);

#line 12 "buffer.test" 
uassert_equal(buffer.used, 0);

#line 13 "buffer.test" 
uassert(buffer.buf);

#line 14 "buffer.test" 
uassert_equal(buffer.buf, buffer.pos);


      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _buffer_management_c_string_appending()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " C String appending" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 17 "buffer.test" 
memset(buffer.buf, 0xdd, 31);

#line 18 "buffer.test" 
uassert_empty(

#line 19 "buffer.test" 
  excom_buffer_cappend(&buffer, "only this gets appended", 4)

#line 20 "buffer.test" 
);

#line 21 "buffer.test" 
uassert_equal(buffer.used, 4);

#line 22 "buffer.test" 
uassert_same(buffer.buf, "only\xdd", 5);


      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _buffer_management_excom_string_appended()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " excom string appended" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 25 "buffer.test" 
excom_string_t str;

#line 26 "buffer.test" 
excom_string_init(&str);

#line 27 "buffer.test" 
excom_string_fill(&str, sizeof("hello"), "hello");


#line 29 "buffer.test" 
uassert_empty(excom_buffer_sappend(&buffer, &str));


#line 31 "buffer.test" 
uassert_equal(buffer.used, sizeof("hello"));

#line 32 "buffer.test" 
uassert_same(buffer.buf, "hello", sizeof("hello") - 1);

#line 33 "buffer.test" 
buffer.used = 0;


      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _buffer_management_buffer_appending()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " buffer appending" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 36 "buffer.test" 
excom_buffer_t buf;

#line 37 "buffer.test" 
uassert_empty(excom_buffer_init(&buf, 16));

#line 38 "buffer.test" 
uassert_empty(

#line 39 "buffer.test" 
  excom_buffer_cappend(&buf, "hello world", 11)

#line 40 "buffer.test" 
);


#line 42 "buffer.test" 
uassert_empty(excom_buffer_bappend(&buffer, &buf));


#line 44 "buffer.test" 
uassert_equal(buffer.used, 11);

#line 45 "buffer.test" 
uassert_same(buffer.buf, "hello world", 11);


#line 47 "buffer.test" 
excom_buffer_destroy(&buf);

#line 48 "buffer.test" 
uassert_equal(buf.max, 0);

#line 49 "buffer.test" 
uassert_equal(buf.used, 0);

#line 50 "buffer.test" 
buffer.used = 0;


      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _buffer_management_formatting()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " formatting" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 53 "buffer.test" 
uassert_empty(

#line 54 "buffer.test" 
  excom_buffer_format(&buffer, "hello %s", "world")

#line 55 "buffer.test" 
);

#line 56 "buffer.test" 
uassert_equal(buffer.used, 11);

#line 57 "buffer.test" 
uassert_same(buffer.buf, "hello world", 11);

#line 58 "buffer.test" 
buffer.used = 0;


      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _buffer_management_force_resize()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " force resize" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 61 "buffer.test" 
char str[] = "FohtnIBgPs5leGO20d24VvP8HvPStP1NWFopdUQuZyJHnosyh6SPUEHb9KFp4hxk9VpWITOb7BhxIkV6";


#line 63 "buffer.test" 
uassert_empty(

#line 64 "buffer.test" 
  excom_buffer_cappend(&buffer, str, strlen(str) - 1)

#line 65 "buffer.test" 
);

#line 66 "buffer.test" 
uassert_equal(buffer.max, 128);

#line 67 "buffer.test" 
uassert_equal(buffer.used, strlen(str) - 1);

#line 68 "buffer.test" 
uassert_same(buffer.buf, str, strlen(str) - 1);

#line 69 "buffer.test" 
buffer.used = 0;


      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _buffer_management_writing_buffers()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " writing buffers" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      #line 5 "buffer.test" 
excom_buffer_t buffer;

#line 6 "buffer.test" 
excom_buffer_init(&buffer, 32);

      //body
      #line 72 "buffer.test" 
int fds[2];

#line 73 "buffer.test" 
ssize_t bytes;

#line 74 "buffer.test" 
char buff[32];


#line 76 "buffer.test" 
uassert_empty(excom_buffer_reset(&buffer));


#line 78 "buffer.test" 
uassert_empty(pipe(fds));

#line 79 "buffer.test" 
excom_buffer_cappend(&buffer, "hello world", 11);


#line 81 "buffer.test" 
excom_buffer_write(&buffer, fds[1]);


#line 83 "buffer.test" 
bytes = read(fds[0], buff, 11);


#line 85 "buffer.test" 
uassert(bytes == 11);

#line 86 "buffer.test" 
uassert_same(buff, "hello world", 11);

      //after
      #line 8 "buffer.test" 
excom_buffer_destroy(&buffer);


  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
  void _buffer_management()
  {
    output(TEXT_COLOR_MAGENTA "\tSET \"" TEXT_COLOR_BOLD_MAGENTA
      " buffer management" TEXT_COLOR_MAGENTA
      "\":\n");
    
    
    
    
    
    _buffer_management_initialization();
    
    
    _buffer_management_c_string_appending();
    
    
    _buffer_management_excom_string_appended();
    
    
    _buffer_management_buffer_appending();
    
    
    _buffer_management_formatting();
    
    
    _buffer_management_force_resize();
    
    
    _buffer_management_writing_buffers();
    
  }


int main()
{
  output(TEXT_COLOR_MAGENTA "FILE \""  TEXT_COLOR_BOLD_MAGENTA
         __FILE TEXT_COLOR_MAGENTA "\":\n");

  
  
  _buffer_management();
  

  output2("\n\t" TEXT_COLOR_MAGENTA "RESULT:\n\t\t"
    "PASSED: " TEXT_COLOR_BOLD_MAGENTA "%zu"
    TEXT_COLOR_MAGENTA "\n\t\tFAILED: "
    TEXT_COLOR_BOLD_MAGENTA "%zu" TEXT_COLOR_MAGENTA
    "\n", tests - failed, failed);

  return TEST_RETURN;
}

