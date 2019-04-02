CC = gcc
CFLAGS = -Wall -g

TARGET_DIR := build

all: server client

$(TARGET_DIR):
	mkdir $(TARGET_DIR)

server: $(TARGET_DIR) server.c
	$(CC) $(CFLAGS) -o $(TARGET_DIR)/$@ $<

client: $(TARGET_DIR) client.c
	$(CC) $(CFLAGS) -o $(TARGET_DIR)/$@ $<

clean:
	-rm -f $(TARGET_DIR)/server $(TARGET_DIR)/client
	-rm -f $(TARGET_DIR)/*.dSYM
