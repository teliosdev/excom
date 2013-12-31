
CFILES :=
OBJS   :=
CFLAGS += -std=c99 -g3 -Wall -Wextra -fPIC -iquote$(CURDIR)/inc
LDFLAGS+= -L$(CURDIR) -ltcmalloc -levent -lpthread

include src/local.mk
OBJS += $(patsubst %.c,%.o, $(CFILES))

.PHONY: default clean

libexcom.a: $(OBJS) Makefile
	$(AR) r $@ $^

default: libexcom.a

clean:
	$(RM) $(OBJS) libexcom.a

%.o: %.c inc/*.h inc/**/*.h Makefile
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<
