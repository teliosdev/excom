OBJS    :=
BINOBJS :=
CFLAGS  += -std=c99 -g3 -Wall -Wextra -fPIC -iquote$(CURDIR)/inc -DEXCOM_EPOLL
LDFLAGS += -L$(CURDIR) -pthread

OJBS += src/excom/server.o src/excom/string.o src/excom/thread.o \
  src/excom/factory.o src/excom/event.o src/excom/server/client.o \
  src/excom/client.o src/excom/event/epoll.o src/excom/event/kqueue.o
BINOJBS += src/excom-cli/main.o
CFLAGS += -DEXCOM_INCLUDE_SERVER_CLIENT

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
