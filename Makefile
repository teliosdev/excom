CFLAGS  += -std=c99 -fno-builtin -g3 -Wall -Wextra -fPIC -I$(CURDIR)/inc $(CFLAG)
LDFLAGS += -L$(CURDIR) -pthread -lexcom -ltoml -lsodium
RM ?= rm -f
CURDIR ?= `pwd`
SODIUM_VERSION = 0.5.0

OBJS := src/excom/server.o src/excom/string.o src/excom/thread.o  \
	src/excom/factory.o src/excom/event.o src/excom/server/client.o \
	src/excom/client.o src/excom/buffer.o src/excom/event/epoll.o   \
	src/excom/event/kqueue.o src/excom/event/poll.o                 \
	src/excom/protocol.o src/excom/protocol/pack.o                  \
	src/excom/encryption.o
TESTOBJS:= test/string.out test/buffer.out test/protocol.out
BINOJBS := src/excom-server/client.c src/excom-client/client.c    \
	src/excom-cli/main.o

CFLAGS  += -I$(CURDIR)/lib/toml

.PHONY: default clean test get-deps cdt

default: excom.out

cdt: clean default test

libexcom.a: $(OBJS)
	$(AR) r $@ $?

libsodium.a: libsodium-$(SODIUM_VERSION)
	cd libsodium-$(SODIUM_VERSION) && autoreconf -i && ./configure --prefix=$(CURDIR)/lib/sodium
	make -C libsodium-$(SODIUM_VERSION)
	cp -R libsodium-$(SODIUM_VERSION)/src/libsodium/include/sodium* inc
	cp libsodium-$(SODIUM_VERSION)/src/libsodium/.libs/libsodium.a .

libsodium-$(SODIUM_VERSION):
	git clone git@github.com:jedisct1/libsodium.git
	mv libsodium libsodium-$(SODIUM_VERSION)

#libsodium-$(SODIUM_VERSION): libsodium-$(SODIUM_VERSION).tar.gz
#	tar xvf $<

#libsodium-$(SODIUM_VERSION).tar.gz:
#	wget "https://download.libsodium.org/libsodium/releases/libsodium-$(SODIUM_VERSION).tar.gz"

libtoml.so:
	cd $(CURDIR)/lib/toml && autoconf && ./configure CFLAGS=-fPIC && make
	cp lib/toml/libtoml.so libtoml.so

excom.out: libsodium.a libexcom.a libtoml.so $(BINOJBS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(BINOJBS) -lexcom -ltoml -lsodium

test: libexcom.a $(TESTOBJS)
	@./test/run_tests ./test

clean:
	$(RM) -r $(OBJS) $(TESTOBJS) libexcom.a excom

clean-sodium:
	$(RM) -r libsodium-$(SODIUM_VERSION) libsodium-$(SODIUM_VERSION).tar.gz libsodium.a

inc/excom.h: inc/excom/protocol/packets.def

inc/excom/protocol/packets.def: scripts/packets.rb scripts/packet_generator.rb
	ruby scripts/packet_generator.rb scripts/packets.rb inc/excom/protocol/packets.def

%.o: %.c inc/*.h inc/*/*.h inc/*/*/*.h Makefile
	$(CC) -o $@ $(CFLAGS) -c $< $(LDFLAGS)

test/%.out: test/%.test inc/*.h inc/*/*.h inc/*/*/*.h test/utest.h Makefile
	ruby scripts/test_generator.rb
	$(CC) -o $@ $(CFLAGS) -x c $(patsubst %.out,%.c,$@) $(LDFLAGS)

get-deps:
	sudo apt-get install valgrind
