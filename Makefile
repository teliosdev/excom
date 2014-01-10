
CFILES  :=
OBJS    :=
BINFILES:=
BINOBJS :=
CFLAGS  += -std=c99 -g3 -Wall -Wextra -fPIC -iquote$(CURDIR)/inc -DEXCOM_EPOLL
LDFLAGS += -L$(CURDIR) -pthread

include src/local.mk
OBJS += $(patsubst %.c,%.o, $(CFILES))
BINOJBS += $(patsubst %.c,%.o, $(BINFILES))

.PHONY: default clean

default: excom

libexcom.a: $(OBJS) Makefile
	$(AR) r $@ $(patsubst Makefile,,$^)

excom: libexcom.a $(BINOJBS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(BINOJBS) -lexcom

clean:
	$(RM) $(OBJS) libexcom.a excom

inc/excom.h: inc/excom/protocol/packets.def

inc/excom/protocol/packets.def: scripts/packets.rb scripts/packet_generator.rb
	ruby scripts/packet_generator.rb scripts/packets.rb inc/excom/protocol/packets.def

%.o: %.c inc/*.h inc/*/*.h inc/*/*/*.h Makefile
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<
