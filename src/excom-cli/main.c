#include "excom.h"
#include <signal.h>
#include <sqlite3.h>

static excom_server_t server;

void sighandle(int signal)
{
  if(signal == SIGINT)
  {
    printf("[excom-cli] Shutting down...\n");

    excom_event_loop_end(&server.base);
  }
}

int main(int argc, char* argv[])
{
  (void) argc;
  (void) argv;
  excom_backend_t backend;
  excom_backend_value_t v;
  char* err;

  excom_thread_load();
  printf("[excom-cli] init\n");
  excom_server_init(&server);
  printf("[excom-cli] binding\n");
  excom_server_bind(&server);
  printf("[excom-cli] running\n");
  signal(SIGINT, sighandle);
  excom_server_run(&server);
  printf("[excom-cli] destroying server\n");
  excom_server_destroy(&server);

  return 0;

}
