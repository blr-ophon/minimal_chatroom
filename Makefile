CC := gcc
CFLAGS := 
INCLUDES := -I./client/include -I./server/include
HEADERS := $(shell find ./ -name '*.h')

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



all: ${CLIENT_O} ${SERVER_O}


${BUILD_DIR}/client/%.o: ${CLIENT_SRC}/%.c ${HEADERS}
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} -c $< -o $@

${BUILD_DIR}/server/%.o: ${SERVER_SRC}/%.c ${HEADERS}
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} -c $< -o $@
