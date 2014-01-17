CFILES += src/excom/server.c src/excom/string.c src/excom/thread.c \
  src/excom/factory.c src/excom/event.c src/excom/server/client.c \
  src/excom/client.c

CFLAGS += -DEXCOM_INCLUDE_SERVER_CLIENT

include src/excom/event/local.mk
