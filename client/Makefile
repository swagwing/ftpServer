SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=client
CC:=gcc
$(ELF):$(OBJS)
	gcc $^ -o $@ -lcrypt -lcurses 
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS)
