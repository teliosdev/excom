CFLAGS  += -std=c99 -g3 -Wall -Wextra -fPIC -iquote$(CURDIR)/inc $(CFLAG)
LDFLAGS += -L$(CURDIR) -pthread
RM ?= rm -f
CURDIR ?= `pwd`

OBJS := src/excom/server.o src/excom/string.o src/excom/thread.o  \
  src/excom/factory.o src/excom/event.o src/excom/server/client.o \
  src/excom/client.o src/excom/buffer.o src/excom/event/epoll.o   \
  src/excom/event/kqueue.o src/excom/protocol.o                   \
  src/excom/protocol/pack.o
TESTOBJS:= test/string.out test/protocol.out test/buffer.out
BINOJBS := src/excom-server/client.c src/excom-cli/main.o

.PHONY: default clean

default: excom.out

libexcom.a: $(OBJS)
	$(AR) r $@ $?

excom.out: libexcom.a $(BINOJBS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(BINOJBS) -lexcom

test: libexcom.a $(TESTOBJS)
	@./test/run_tests ./test

clean:
	$(RM) $(OBJS) $(TESTOBJS) libexcom.a excom

inc/excom.h: inc/excom/protocol/packets.def

inc/excom/protocol/packets.def: scripts/packets.rb scripts/packet_generator.rb
	ruby scripts/packet_generator.rb scripts/packets.rb inc/excom/protocol/packets.def

%.o: %.c inc/*.h inc/*/*.h inc/*/*/*.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<

%.out: %.c %.test inc/*.h inc/*/*.h inc/*/*/*.h test/utest.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -x c $< -lexcom

get-deps:
	sudo apt-get install valgrind
