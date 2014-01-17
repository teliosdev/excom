#include "excom.h"

int main(int argc, char* argv[])
{
  (void) argc;
  (void) argv;
  excom_server_t server;

  excom_server_init(&server);
  excom_server_bind(&server);
  excom_server_run(&server);
}
