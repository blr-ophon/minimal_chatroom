CC := gcc
OPT := -O0
CFLAGS = -std=gnu99 -fPIC -g -Wall -Wextra -pedantic $(OPT)

INCLUDES := -I./client/include -I./server/include
HEADERS := $(shell find ./ -name '*.h')
LIBS := -lncurses

BUILD_DIR := ./build

CLIENT_SRC := ./client/src
CLIENT_BUILD := ${BUILD_DIR}/client

SERVER_SRC := ./server/src
SERVER_BUILD:= ${BUILD_DIR}/server

#client/src/tcpclient.c   --->   build/client/tcpclient.o

CLIENT_C := $(shell find ./client -name '*.c')
CLIENT_O := $(CLIENT_C:${CLIENT_SRC}/%.c=$(CLIENT_BUILD)/%.o)

SERVER_C := $(shell find ./server -name '*.c')
SERVER_O := $(SERVER_C:${SERVER_SRC}/%.c=$(SERVER_BUILD)/%.o)

CLIENT_EXEC := ./mncr_client
SERVER_EXEC := ./mncr_server


all: ${CLIENT_EXEC} ${SERVER_EXEC}

${CLIENT_EXEC}: ${CLIENT_O}
	${CC} ${CFLAGS} ${INCLUDES} $^ -o $@ ${LIBS}

${SERVER_EXEC}: ${SERVER_O}
	${CC} ${CFLAGS} ${INCLUDES} $^ -o $@

${BUILD_DIR}/client/%.o: ${CLIENT_SRC}/%.c ${HEADERS}
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} -c $< -o $@

${BUILD_DIR}/server/%.o: ${SERVER_SRC}/%.c ${HEADERS}
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} -c $< -o $@


clean:
	rm -rf ${BUILD_DIR}

run-client: ${CLIENT_EXEC}
	./$^ 127.0.0.1 8080

run-server: ${SERVER_EXEC}
	./$^ 
