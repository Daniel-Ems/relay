CPPFLAGS+=-Wall -Wextra -Wpedantic -Wextra -Wpedantic -Wwrite-strings 
CPPFLAGS+=-Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

CFLAGS+=-std=c11
LDLIBS+=-lpthread


DISPATCHER_BIN=dispatcher
DISPATCHER_DEPS=dispatcher.o

LISTENER_BIN=listener
LISTENER_DEPS=listener.o

all: $(DISPATCHER_BIN) $(LISTENER_BIN)

$(DISPATCHER_BIN):$(DISPATCHER_DEPS)

$(LISTENER_BIN):$(LISTENER_DEPS)


.PHONY: clean debug profile


debug: CLFAGS+=-g
debug: all

profile: CFLAGS+=-pg
profile: LDFLAGS+=-pg
profile: all


clean: 
		$(RM) $(DISPATCHER_BIN) $(DISPATCHER_DEPS) $(LISTENER_BIN) $(LISTENER_DEPS)

#Makefile support compliments of Jonathan Haubrich
