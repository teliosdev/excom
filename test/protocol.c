#include <stdlib.h>
#include <stdio.h>
#include "excom.h"
#define __FILE "protocol.test"
#include "utest.h"

//prefix
static uint16_t id = 0;



static size_t tests  = 0;
static size_t failed = 0;


  
  
  
  
  
    
    
    void _protocol_packet_packing_ok_packet()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " ok packet" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 47 "protocol.test"
#line 3 "protocol.test"
id += 1;

#line 4 "protocol.test"
excom_buffer_t buffer;

#line 5 "protocol.test"
excom_packet_t packet;

#line 6 "protocol.test"
excom_protocol_ok_t* pack_data;

#line 7 "protocol.test"
pack_data = &packet.data.ok;

#line 8 "protocol.test"
(void) pack_data;

#line 9 "protocol.test"
{

#line 10 "protocol.test"
  

#line 11 "protocol.test"
};

#line 12 "protocol.test"
packet.id   = id;

#line 13 "protocol.test"
packet.type = packet(ok);

#line 14 "protocol.test"
uassert_empty(excom_buffer_init(&buffer, 64));

#line 15 "protocol.test"
excom_protocol_write_packet(&packet, &buffer, NULL);

#line 16 "protocol.test"
uassert(buffer.used == sizeof("\0\0\0\0\0\x01\0\x01") - 1);

#line 17 "protocol.test"
uassert_same((char*) buffer.buf,

#line 18 "protocol.test"
  "\0\0\0\0\0\x01\0\x01", sizeof("\0\0\0\0\0\x01\0\x01") - 1);

#line 19 "protocol.test"
excom_buffer_destroy(&buffer);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _protocol_packet_packing_reject_packet()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " reject packet" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 49 "protocol.test"
#line 3 "protocol.test"
id += 1;

#line 4 "protocol.test"
excom_buffer_t buffer;

#line 5 "protocol.test"
excom_packet_t packet;

#line 6 "protocol.test"
excom_protocol_reject_t* pack_data;

#line 7 "protocol.test"
pack_data = &packet.data.reject;

#line 8 "protocol.test"
(void) pack_data;

#line 9 "protocol.test"
{

#line 10 "protocol.test"
  pack_data->reason = 0x40;



#line 11 "protocol.test"
};

#line 12 "protocol.test"
packet.id   = id;

#line 13 "protocol.test"
packet.type = packet(reject);

#line 14 "protocol.test"
uassert_empty(excom_buffer_init(&buffer, 64));

#line 15 "protocol.test"
excom_protocol_write_packet(&packet, &buffer, NULL);

#line 16 "protocol.test"
uassert(buffer.used == sizeof("\0\0\0\x01\0\x02\0\x02\x40") - 1);

#line 17 "protocol.test"
uassert_same((char*) buffer.buf,

#line 18 "protocol.test"
  "\0\0\0\x01\0\x02\0\x02\x40", sizeof("\0\0\0\x01\0\x02\0\x02\x40") - 1);

#line 19 "protocol.test"
excom_buffer_destroy(&buffer);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _protocol_packet_packing_error_packet()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " error packet" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 52 "protocol.test"
#line 3 "protocol.test"
id += 1;

#line 4 "protocol.test"
excom_buffer_t buffer;

#line 5 "protocol.test"
excom_packet_t packet;

#line 6 "protocol.test"
excom_protocol_error_t* pack_data;

#line 7 "protocol.test"
pack_data = &packet.data.error;

#line 8 "protocol.test"
(void) pack_data;

#line 9 "protocol.test"
{

#line 10 "protocol.test"
  pack_data->reason = 56;



#line 11 "protocol.test"
};

#line 12 "protocol.test"
packet.id   = id;

#line 13 "protocol.test"
packet.type = packet(error);

#line 14 "protocol.test"
uassert_empty(excom_buffer_init(&buffer, 64));

#line 15 "protocol.test"
excom_protocol_write_packet(&packet, &buffer, NULL);

#line 16 "protocol.test"
uassert(buffer.used == sizeof("\0\0\0\x01\0\x03\0\x03\x38") - 1);

#line 17 "protocol.test"
uassert_same((char*) buffer.buf,

#line 18 "protocol.test"
  "\0\0\0\x01\0\x03\0\x03\x38", sizeof("\0\0\0\x01\0\x03\0\x03\x38") - 1);

#line 19 "protocol.test"
excom_buffer_destroy(&buffer);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _protocol_packet_packing_protocol_version_packet()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " protocol version packet" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 55 "protocol.test"
#line 3 "protocol.test"
id += 1;

#line 4 "protocol.test"
excom_buffer_t buffer;

#line 5 "protocol.test"
excom_packet_t packet;

#line 6 "protocol.test"
excom_protocol_protocol_version_t* pack_data;

#line 7 "protocol.test"
pack_data = &packet.data.protocol_version;

#line 8 "protocol.test"
(void) pack_data;

#line 9 "protocol.test"
{

#line 10 "protocol.test"
  excom_string_init(&pack_data->version);
excom_string_fill(&pack_data->version, strlen("1.0.0"), "1.0.0");
pack_data->major = 1;
pack_data->minor = 0;
pack_data->patch = 0;



#line 11 "protocol.test"
};

#line 12 "protocol.test"
packet.id   = id;

#line 13 "protocol.test"
packet.type = packet(protocol_version);

#line 14 "protocol.test"
uassert_empty(excom_buffer_init(&buffer, 64));

#line 15 "protocol.test"
excom_protocol_write_packet(&packet, &buffer, NULL);

#line 16 "protocol.test"
uassert(buffer.used == sizeof("\0\0\0\x0c\0\x04\0\x04\0\0\0\0051.0.0\x01\0\0") - 1);

#line 17 "protocol.test"
uassert_same((char*) buffer.buf,

#line 18 "protocol.test"
  "\0\0\0\x0c\0\x04\0\x04\0\0\0\0051.0.0\x01\0\0", sizeof("\0\0\0\x0c\0\x04\0\x04\0\0\0\0051.0.0\x01\0\0") - 1);

#line 19 "protocol.test"
excom_buffer_destroy(&buffer);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _protocol_packet_packing_command()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " command" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 62 "protocol.test"
#line 3 "protocol.test"
id += 1;

#line 4 "protocol.test"
excom_buffer_t buffer;

#line 5 "protocol.test"
excom_packet_t packet;

#line 6 "protocol.test"
excom_protocol_command_t* pack_data;

#line 7 "protocol.test"
pack_data = &packet.data.command;

#line 8 "protocol.test"
(void) pack_data;

#line 9 "protocol.test"
{

#line 10 "protocol.test"
  excom_string_init(&pack_data->command);
excom_string_fill(&pack_data->command, strlen("test"), "test");



#line 11 "protocol.test"
};

#line 12 "protocol.test"
packet.id   = id;

#line 13 "protocol.test"
packet.type = packet(command);

#line 14 "protocol.test"
uassert_empty(excom_buffer_init(&buffer, 64));

#line 15 "protocol.test"
excom_protocol_write_packet(&packet, &buffer, NULL);

#line 16 "protocol.test"
uassert(buffer.used == sizeof("\0\0\0\x08\0\x05\0\x05\0\0\0\004test") - 1);

#line 17 "protocol.test"
uassert_same((char*) buffer.buf,

#line 18 "protocol.test"
  "\0\0\0\x08\0\x05\0\x05\0\0\0\004test", sizeof("\0\0\0\x08\0\x05\0\x05\0\0\0\004test") - 1);

#line 19 "protocol.test"
excom_buffer_destroy(&buffer);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
  void _protocol_packet_packing()
  {
    output(TEXT_COLOR_MAGENTA "\tSET \"" TEXT_COLOR_BOLD_MAGENTA
      " protocol packet packing" TEXT_COLOR_MAGENTA
      "\":\n");
    
    
    
    _protocol_packet_packing_ok_packet();
    
    
    _protocol_packet_packing_reject_packet();
    
    
    _protocol_packet_packing_error_packet();
    
    
    _protocol_packet_packing_protocol_version_packet();
    
    
    _protocol_packet_packing_command();
    
  }

  
  
  
    
    void _protocol_packet_unpacking_ok_packet()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " ok packet" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 67 "protocol.test"
#line 22 "protocol.test"
excom_buffer_t buf;

#line 23 "protocol.test"
excom_packet_t packet;

#line 24 "protocol.test"
memset(&packet, 0xdd, sizeof(excom_packet_t));

#line 25 "protocol.test"
excom_protocol_ok_t pack_data;

#line 26 "protocol.test"
excom_buffer_init(&buf, 16);

#line 27 "protocol.test"
excom_buffer_cappend(&buf, "\0\0\0\0\0\x06\0\x01", sizeof("\0\0\0\0\0\x06\0\x01"));

#line 28 "protocol.test"
uassert_empty(

#line 29 "protocol.test"
  excom_protocol_read_packet(&packet, &buf, NULL)

#line 30 "protocol.test"
);

#line 31 "protocol.test"
id++;

#line 32 "protocol.test"
uassert_equal(packet.type, packet(ok));

#line 33 "protocol.test"
uassert_equal(packet.size, sizeof("\0\0\0\0\0\x06\0\x01") - 9);

#line 34 "protocol.test"
uassert_equal(packet.id, id);

#line 35 "protocol.test"
pack_data = packet.data.ok;

#line 36 "protocol.test"
(void) pack_data;

#line 37 "protocol.test"
{

#line 38 "protocol.test"
  

#line 39 "protocol.test"
};

#line 40 "protocol.test"
excom_buffer_destroy(&buf);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
    
    void _protocol_packet_unpacking_reject_packet()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " reject packet" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 69 "protocol.test"
#line 22 "protocol.test"
excom_buffer_t buf;

#line 23 "protocol.test"
excom_packet_t packet;

#line 24 "protocol.test"
memset(&packet, 0xdd, sizeof(excom_packet_t));

#line 25 "protocol.test"
excom_protocol_reject_t pack_data;

#line 26 "protocol.test"
excom_buffer_init(&buf, 16);

#line 27 "protocol.test"
excom_buffer_cappend(&buf, "\0\0\0\x01\0\x07\0\x02\x40", sizeof("\0\0\0\x01\0\x07\0\x02\x40"));

#line 28 "protocol.test"
uassert_empty(

#line 29 "protocol.test"
  excom_protocol_read_packet(&packet, &buf, NULL)

#line 30 "protocol.test"
);

#line 31 "protocol.test"
id++;

#line 32 "protocol.test"
uassert_equal(packet.type, packet(reject));

#line 33 "protocol.test"
uassert_equal(packet.size, sizeof("\0\0\0\x01\0\x07\0\x02\x40") - 9);

#line 34 "protocol.test"
uassert_equal(packet.id, id);

#line 35 "protocol.test"
pack_data = packet.data.reject;

#line 36 "protocol.test"
(void) pack_data;

#line 37 "protocol.test"
{

#line 38 "protocol.test"
  uassert_equal(pack_data.reason, 0x40);



#line 39 "protocol.test"
};

#line 40 "protocol.test"
excom_buffer_destroy(&buf);


      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
  void _protocol_packet_unpacking()
  {
    output(TEXT_COLOR_MAGENTA "\tSET \"" TEXT_COLOR_BOLD_MAGENTA
      " protocol packet unpacking" TEXT_COLOR_MAGENTA
      "\":\n");
    
    
    _protocol_packet_unpacking_ok_packet();
    
    
    _protocol_packet_unpacking_reject_packet();
    
  }

  
  
  
    
    void _packet_management_prefill()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\t\tTEST \"" TEXT_COLOR_BOLD_MAGENTA
        " prefill" TEXT_COLOR_MAGENTA
        "\": ");
      //before
      
      //body
      #line 73 "protocol.test"
excom_packet_t packet;

#line 74 "protocol.test"
excom_string_t str;

#line 75 "protocol.test"
packet.type = packet(protocol_version);

#line 76 "protocol.test"
excom_string_init(&str);

#line 77 "protocol.test"
excom_string_fill(&str, sizeof("1.0.0"), "1.0.0");

#line 78 "protocol.test"
excom_protocol_prefill(&packet, &str, 1, 0, 0);


#line 80 "protocol.test"
#define pv packet.data.protocol_version


#line 82 "protocol.test"
uassert_same(pv.version.body, "1.0.0", 5);

#line 83 "protocol.test"
uassert_equal(pv.major, 1);

#line 84 "protocol.test"
uassert_equal(pv.minor, 0);

#line 85 "protocol.test"
uassert_equal(pv.patch, 0);

      //after
      
  #line 47 "test_generator.rb"
      if(test_success) {
  #ifdef VERBOSE
        output("\n\t\t\t" TEXT_COLOR_GREEN "OK\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\n");
  #endif
      }
    }
  
  void _packet_management()
  {
    output(TEXT_COLOR_MAGENTA "\tSET \"" TEXT_COLOR_BOLD_MAGENTA
      " packet management" TEXT_COLOR_MAGENTA
      "\":\n");
    
    
    _packet_management_prefill();
    
  }


int main()
{
  output(TEXT_COLOR_MAGENTA "FILE \""  TEXT_COLOR_BOLD_MAGENTA
         __FILE TEXT_COLOR_MAGENTA "\":\n");

  
  
  
  
  _protocol_packet_packing();
  
  
  _protocol_packet_unpacking();
  
  
  _packet_management();
  

  output2("\n\t" TEXT_COLOR_MAGENTA "RESULT:\n\t\t"
    "PASSED: " TEXT_COLOR_BOLD_MAGENTA "%zu"
    TEXT_COLOR_MAGENTA "\n\t\tFAILED: "
    TEXT_COLOR_BOLD_MAGENTA "%zu" TEXT_COLOR_MAGENTA
    "\n", tests - failed, failed);

  return TEST_RETURN;
}

