CFLAGS = -O2 -g -I include -Wall -Wextra -Werror
LDFLAGS = -lm

ifeq ($(OS),Windows_NT)
	LDFLAGS += -lws2_32
endif

OUT = bin
SRV_EXEC = $(OUT)/echo_server
CLT_EXEC = $(OUT)/echo_client

SRV_OBJS = \
	src/server.o \
	src/server_main.o

CLT_OBJS = \
	src/client.o \
	src/client_main.o

srv_deps := $(SRV_OBJS:%.o=%.o.d)
clt_deps := $(CLT_OBJS:%.o=%.o.d)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF $@.d -c $<

$(SRV_EXEC): $(SRV_OBJS)
	mkdir -p $(OUT)
	$(CC) $(SRV_OBJS) -o $@ $(LDFLAGS)

$(CLT_EXEC): $(CLT_OBJS)
	mkdir -p $(OUT)
	$(CC) $(CLT_OBJS) -o $@ $(LDFLAGS)

all: $(SRV_EXEC) $(CLT_EXEC)
run: $(SRV_EXEC)
	@echo "Starting Echo service..."
	@./$(SRV_EXEC) $(port)

clean:
	$(RM) $(SRV_OBJS) $(SRV_EXEC) $(srv_deps) \
		  $(CLT_EXEC) $(CLT_OBJS) $(clt_deps)
		  
distclean: clean

build_docker:
	docker build -t echo_demo .

-include $(srv_deps) $(clt_deps)