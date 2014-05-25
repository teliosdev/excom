CURDIR != pwd
RM ?= rm -f
SODIUM_VERSION = 0.4.5
CFLAGS  += -std=c99 -fno-builtin -g3 -Wall -Wextra -fPIC -I$(CURDIR)/inc $(CFLAG)
LDFLAGS += -L$(CURDIR) -pthread -lexcom -ltoml -lsodium -lm

OBJS := src/excom/server.o src/excom/string.o src/excom/thread.o  \
	src/excom/factory.o src/excom/event.o src/excom/server/client.o \
	src/excom/client.o src/excom/buffer.o src/excom/event/epoll.o   \
	src/excom/event/kqueue.o src/excom/event/poll.o                 \
	src/excom/protocol.o src/excom/protocol/pack.o                  \
	src/excom/encryption.o src/excom/event/select.o
TESTOBJS:= test/string.out test/buffer.out test/protocol.out
BINOJBS := src/excom-server/client.c src/excom-client/client.c    \
	src/excom-cli/main.o

CFLAGS  += -I$(CURDIR)/lib/toml

DYLIB = so

ifeq ($(OS),Windows_NT)
	DYLIB = dll
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		DYLIB = dylib
	endif
endif

.PHONY: default clean test get-deps cdt
.SUFFIXES: .o .c .out

default: excom.out

cdt: clean default test

libexcom.a: $(OBJS)
	$(AR) r $@ $?

libsodium.a: libsodium-$(SODIUM_VERSION)
	cd libsodium-$(SODIUM_VERSION) && ./configure --prefix=$(CURDIR)/lib/sodium
	make -C libsodium-$(SODIUM_VERSION)
	cp -R libsodium-$(SODIUM_VERSION)/src/libsodium/include/sodium* inc
	cp libsodium-$(SODIUM_VERSION)/src/libsodium/.libs/libsodium.a .

libsodium-$(SODIUM_VERSION): libsodium-$(SODIUM_VERSION).tar.gz
	tar xvf $<

libsodium-$(SODIUM_VERSION).tar.gz:
	curl "https://download.libsodium.org/libsodium/releases/libsodium-$(SODIUM_VERSION).tar.gz" -o libsodium-$(SODIUM_VERSION).tar.gz

libtoml.$(DYLIB):
	cd $(CURDIR)/lib/toml && autoconf && ./configure CFLAGS=-fPIC && make
	cp lib/toml/libtoml.$(DYLIB) libtoml.$(DYLIB)

excom.out: libsodium.a libexcom.a libtoml.$(DYLIB) $(BINOJBS)
	$(CC) -o $@ $(CFLAGS) $(BINOJBS) $(LDFLAGS)

test: libexcom.a libtoml.$(DYLIB) libsodium.a $(TESTOBJS)
	@./test/run_tests ./test

clean:
	$(RM) -r $(OBJS) $(TESTOBJS) libexcom.a excom excom.out

clean-sodium:
	$(RM) -r libsodium-$(SODIUM_VERSION) libsodium-$(SODIUM_VERSION).tar.gz libsodium.a inc/sodium{,.h}

inc/excom.h: inc/excom/protocol/packets.def

inc/excom/protocol/packets.def: scripts/packets.rb scripts/packet_generator.rb
	ruby scripts/packet_generator.rb scripts/packets.rb inc/excom/protocol/packets.def

%.o: %.c inc/*.h inc/*/*.h inc/*/*/*.h inc/excom/config.h Makefile
	$(CC) -o $@ $(CFLAGS) -c $< $(LDFLAGS)

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $< $(LDFLAGS)

test/%.out: test/%.test inc/*.h inc/*/*.h inc/*/*/*.h test/utest.h Makefile
	ruby scripts/test_generator.rb
	$(CC) -o $@ $(CFLAGS) -x c $(patsubst %.out,%.c,$@) $(LDFLAGS)

.c.out:
	ruby scripts/test_generator.rb
	$(CC) -o $@ $(CFLAGS) -x c $< $(LDFLAGS)

get-deps: libsodium.a
	sudo apt-get install valgrind

inc/excom/config.h: ./configure
	@./configure

src/excom/excom-client/client.c: src/excom/excom-client/handle_packets.ci
src/excom/excom-server/client.c: src/excom/excom-server/handle_packets.ci
