CFILES  :=
OBJS    :=
BINFILES:=
BINOBJS :=
CFLAGS  += -std=c99 -g3 -Wall -Wextra -fPIC -iquote$(CURDIR)/inc -DEXCOM_EPOLL
LDFLAGS += -L$(CURDIR) -pthread

CFILES += src/excom/server.c src/excom/string.c src/excom/thread.c \
  src/excom/factory.c src/excom/event.c src/excom/server/client.c \
  src/excom/client.c src/excom/event/epoll.c src/excom/event/kqueue.c
BINFILES += src/excom-cli/main.c
CFLAGS += -DEXCOM_INCLUDE_SERVER_CLIENT

OBJS += $(patsubst %.c,%.o, $(CFILES))
BINOJBS += $(patsubst %.c,%.o, $(BINFILES))

.PHONY: default clean

default: excom.out

libexcom.a: $(OBJS) Makefile
	$(AR) r $@ $(patsubst Makefile,,$^)

excom.out: libexcom.a $(BINOJBS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(BINOJBS) -lexcom

clean:
	$(RM) $(OBJS) libexcom.a excom

inc/excom.h: inc/excom/protocol/packets.def

inc/excom/protocol/packets.def: scripts/packets.rb scripts/packet_generator.rb
	ruby scripts/packet_generator.rb scripts/packets.rb inc/excom/protocol/packets.def

%.o: %.c inc/*.h inc/*/*.h inc/*/*/*.h inc/excom/protocol/packets.def Makefile
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<
