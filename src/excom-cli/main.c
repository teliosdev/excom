#include "excom.h"
#include <signal.h>
#include <assert.h>

static excom_server_t server;
static excom_client_t client;
static int srv;

void sighandle(int signal)
{
  if(signal == SIGINT)
  {
    if(srv)
    {
      printf("[excom-cli/server] Shutting down...\n");

      excom_event_loop_end(&server.base);
    }
    else
    {
      printf("[excom-cli/client] Shutting down...\n");

      excom_client_destroy(&client);
    }
  }
}

static void pack(excom_packet_t* packet, void* cl)
{
# define PACKET(name, __, ___) case packet(name): \
    printf("Received %s packet\n", #name);        \
    break;                                        \

  switch(packet->type)
  {
#   include "excom/protocol/packets.def"
    default:
    case packet(INVALID):
      printf("Received invalid packet!\n");
      break;
  }
}

int main(int argc, char* argv[])
{
  (void) argc;
  (void) argv;
  srv = 0;

  if(argc != 2)
  {
    srv = 1;
  }
  else if(strcmp("server", argv[1]) == 0)
  {
    srv = 1;
  }

  excom_thread_load();

  if(srv)
  {
    printf("[excom-cli/server] init\n");
    excom_server_init(&server);
    printf("[excom-cli/server] binding\n");
    excom_server_bind(&server);
    printf("[excom-cli/server] running\n");
    signal(SIGINT, sighandle);
    excom_server_run(&server);
    printf("[excom-cli/server] destroying server\n");
    excom_server_destroy(&server);
  }
  else
  {
    printf("[excom-cli/client] init\n");
    excom_client_init(&client);
    printf("[excom-cli/client] connect\n");
    assert(excom_client_connect(&client) == 0);
    printf("[excom-cli/client] handling packets\n");
    excom_client_handle_packets(&client, pack);
    excom_thread_join(&client.thread, NULL);
  }

  return 0;

}
