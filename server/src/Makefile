SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=thread_pool_server
CC:=gcc
$(ELF):$(OBJS)
	gcc $^ -o $@ -pthread -lmysqlclient
clean:
	rm -rf $(OBJS) $(ELF)
