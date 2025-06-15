# === CONFIG ===
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Iinclude -Itests/unity -pthread -fsanitize=thread -g
SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin
UNITY_DIR = $(TEST_DIR)/unity

# === CORE SOURCES ===
COMMON = $(SRC_DIR)/common.c
MESSAGE = $(SRC_DIR)/message.c
NET = $(SRC_DIR)/net.c
PROTOCOL = $(SRC_DIR)/protocol.c
CLIENTS = $(SRC_DIR)/clients.c
CLIENT_HANDLER = $(SRC_DIR)/client_handler.c 
REQUEST_HANDLER = $(SRC_DIR)/request_handler.c 

# === APP SOURCES ===
SERVER_SRC = $(SRC_DIR)/chat_server.c $(COMMON) $(MESSAGE) $(NET) $(PROTOCOL) $(CLIENTS)
CLIENT_SRC = $(SRC_DIR)/chat_client.c $(COMMON) $(MESSAGE) $(NET) $(PROTOCOL)

# === TEST SOURCES ===
TEST_MESSAGE = $(TEST_DIR)/test_message.c
TEST_NET = $(TEST_DIR)/test_net.c
TEST_PROTOCOL = $(TEST_DIR)/test_protocol.c
TEST_CLIENTS = $(TEST_DIR)/test_clients.c
TEST_REQUEST_HANDLER = $(TEST_DIR)/test_request_handler.c 
UNITY_SRC = $(UNITY_DIR)/unity.c

# === BINARIES ===
TEST_MESSAGE_BIN = $(BIN_DIR)/test_message
TEST_NET_BIN = $(BIN_DIR)/test_net
TEST_PROTOCOL_BIN = $(BIN_DIR)/test_protocol
TEST_CLIENTS_BIN = $(BIN_DIR)/test_clients
TEST_REQUEST_HANDLER_BIN = $(BIN_DIR)/test_request_handler 
SERVER_BIN = $(BIN_DIR)/chat_server
CLIENT_BIN = $(BIN_DIR)/chat_client

# === TARGETS ===
.PHONY: all clean test build run-server run-client test_message test_net test_protocol test_request_handler

all: test build

# === Internal: Ensure bin directory exists ===
bin_dir:
	mkdir -p $(BIN_DIR)

# === Test binaries ===
$(TEST_MESSAGE_BIN): $(TEST_MESSAGE) $(MESSAGE) $(COMMON) $(PROTOCOL) $(UNITY_SRC) | bin_dir
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_NET_BIN): $(TEST_NET) $(NET) $(COMMON) $(UNITY_SRC) | bin_dir
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_PROTOCOL_BIN): $(TEST_PROTOCOL) $(PROTOCOL) $(COMMON) $(UNITY_SRC) | bin_dir
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_CLIENTS_BIN): $(TEST_CLIENTS) $(CLIENTS) $(NET) $(COMMON) $(UNITY_SRC) | bin_dir 
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_REQUEST_HANDLER_BIN): $(TEST_REQUEST_HANDLER) $(REQUEST_HANDLER) $(CLIENTS) $(MESSAGE) $(COMMON) $(NET) $(PROTOCOL) $(UNITY_SRC) | bin_dir
	$(CC) $(CFLAGS) $^ -o $@

# === Individual test targets ===
test_message: $(TEST_MESSAGE_BIN)
	@$(TEST_MESSAGE_BIN)

test_net: $(TEST_NET_BIN)
	@$(TEST_NET_BIN)

test_protocol: $(TEST_PROTOCOL_BIN)
	@$(TEST_PROTOCOL_BIN)

test_clients: $(TEST_CLIENTS_BIN)
	@$(TEST_CLIENTS_BIN)

test_request_handler: $(TEST_REQUEST_HANDLER_BIN)
	@$(TEST_REQUEST_HANDLER_BIN)

# === Run all tests ===
test: test_message test_net test_protocol test_clients test_request_handler

# === Build only app binaries ===
$(SERVER_BIN): $(SERVER_SRC) | bin_dir
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_BIN): $(CLIENT_SRC) | bin_dir
	$(CC) $(CFLAGS) $^ -o $@

build: $(SERVER_BIN) $(CLIENT_BIN)

# === Clean binaries ===
clean:
	rm -rf $(BIN_DIR)

# === Optional: run the server and client ===
run-server: $(SERVER_BIN)
	@$(SERVER_BIN)

run-client: $(CLIENT_BIN)
	@$(CLIENT_BIN)
