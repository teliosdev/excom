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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 12 "buffer.test"
uassert_equal(buffer.max, 32);

#line 13 "buffer.test"
uassert_equal(buffer.used, 0);

#line 14 "buffer.test"
uassert(buffer.buf);

#line 15 "buffer.test"
uassert_equal(buffer.buf, buffer.pos);


      //after
      #line 9 "buffer.test"
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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 18 "buffer.test"
memset(buffer.buf, 0xdd, 31);

#line 19 "buffer.test"
uassert_empty(

#line 20 "buffer.test"
  excom_buffer_cappend(&buffer, "only this gets appended", 4)

#line 21 "buffer.test"
);

#line 22 "buffer.test"
uassert_equal(buffer.used, 4);

#line 23 "buffer.test"
uassert_same(buffer.buf, "only\xdd", 5);


      //after
      #line 9 "buffer.test"
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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 26 "buffer.test"
excom_string_t str;

#line 27 "buffer.test"
excom_string_init(&str);

#line 28 "buffer.test"
excom_string_fill(&str, sizeof("hello"), "hello");


#line 30 "buffer.test"
uassert_empty(excom_buffer_sappend(&buffer, &str));


#line 32 "buffer.test"
uassert_equal(buffer.used, sizeof("hello"));

#line 33 "buffer.test"
uassert_same(buffer.buf, "hello", sizeof("hello") - 1);

#line 34 "buffer.test"
buffer.used = 0;


      //after
      #line 9 "buffer.test"
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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 37 "buffer.test"
excom_buffer_t buf;

#line 38 "buffer.test"
uassert_empty(excom_buffer_init(&buf, 16));

#line 39 "buffer.test"
uassert_empty(

#line 40 "buffer.test"
  excom_buffer_cappend(&buf, "hello world", 11)

#line 41 "buffer.test"
);


#line 43 "buffer.test"
uassert_empty(excom_buffer_bappend(&buffer, &buf));


#line 45 "buffer.test"
uassert_equal(buffer.used, 11);

#line 46 "buffer.test"
uassert_same(buffer.buf, "hello world", 11);


#line 48 "buffer.test"
excom_buffer_destroy(&buf);

#line 49 "buffer.test"
uassert_equal(buf.max, 0);

#line 50 "buffer.test"
uassert_equal(buf.used, 0);

#line 51 "buffer.test"
buffer.used = 0;


      //after
      #line 9 "buffer.test"
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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 54 "buffer.test"
uassert_empty(

#line 55 "buffer.test"
  excom_buffer_format(&buffer, "hello %s", "world")

#line 56 "buffer.test"
);

#line 57 "buffer.test"
uassert_equal(buffer.used, 11);

#line 58 "buffer.test"
uassert_same(buffer.buf, "hello world", 11);

#line 59 "buffer.test"
buffer.used = 0;


      //after
      #line 9 "buffer.test"
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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 62 "buffer.test"
char str[] = "FohtnIBgPs5leGO20d24VvP8HvPStP1NWFopdUQuZyJHnosyh6SPUEHb9KFp4hxk9VpWITOb7BhxIkV6";


#line 64 "buffer.test"
uassert_empty(

#line 65 "buffer.test"
  excom_buffer_cappend(&buffer, str, strlen(str) - 1)

#line 66 "buffer.test"
);

#line 67 "buffer.test"
uassert_equal(buffer.max, 128);

#line 68 "buffer.test"
uassert_equal(buffer.used, strlen(str) - 1);

#line 69 "buffer.test"
uassert_same(buffer.buf, str, strlen(str) - 1);

#line 70 "buffer.test"
buffer.used = 0;


      //after
      #line 9 "buffer.test"
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
      #line 6 "buffer.test"
excom_buffer_t buffer;

#line 7 "buffer.test"
excom_buffer_init(&buffer, 32);

      //body
      #line 73 "buffer.test"
int fds[2];

#line 74 "buffer.test"
ssize_t bytes;

#line 75 "buffer.test"
char buff[32];


#line 77 "buffer.test"
uassert_empty(excom_buffer_reset(&buffer));


#line 79 "buffer.test"
uassert_empty(pipe(fds));

#line 80 "buffer.test"
excom_buffer_cappend(&buffer, "hello world", 11);


#line 82 "buffer.test"
excom_buffer_write(&buffer, fds[1]);


#line 84 "buffer.test"
bytes = read(fds[0], buff, 11);


#line 86 "buffer.test"
uassert(bytes == 11);

#line 87 "buffer.test"
uassert_same(buff, "hello world", 11);

      //after
      #line 9 "buffer.test"
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

