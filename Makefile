
CFILES :=
OBJS   :=
CFLAGS += -std=c99 -g3 -Wall -Wextra -fPIC -iquote$(CURDIR)/inc
LDFLAGS+= -L$(CURDIR) -pthread

include src/local.mk
OBJS += $(patsubst %.c,%.o, $(CFILES))

.PHONY: default clean
.DEFAULT: default

libexcom.a: $(OBJS) Makefile
	$(AR) r $@ $(patsubst Makefile,,$^)

default: libexcom.a

clean:
	$(RM) $(OBJS) libexcom.a

inc/excom.h: inc/excom/protocol/packets.def

inc/excom/protocol/packets.def: scripts/packets.rb scripts/packet_generator.rb
	scripts/packet_generator.rb scripts/packets.rb inc/excom/protocol/packets.def

%.o: %.c inc/*.h inc/*/*.h inc/*/*/*.h Makefile
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<
