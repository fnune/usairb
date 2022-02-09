SOURCE_DIR := src
SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.c)

TARGET_DIR := target
OBJECT_FILES := $(patsubst $(SOURCE_DIR)/%.c,$(TARGET_DIR)/%.o,$(SOURCE_FILES))

LIBUSBIP_DIR := external/libusbip
LIBUSBIP_TARGET := $(LIBUSBIP_DIR)/libusbip/libusbip.a

CC := gcc
CFLAGS := -Wall -I.

LDLIBS := -ludev -lusb-1.0

# usairb

$(TARGET_DIR)/usairb: $(OBJECT_FILES) $(LIBUSBIP_TARGET) | $(TARGET_DIR)
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^

$(TARGET_DIR)/%.o: $(SOURCE_DIR)/%.c | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# dependencies

$(LIBUSBIP_TARGET):
	$(MAKE) -C $(LIBUSBIP_DIR)

# utilities

.PHONY: clean
clean:
	rm -rf $(TARGET_DIR)
	$(MAKE) clean -C $(LIBUSBIP_DIR)

compile_commands.json: clean
	bear -- $(MAKE)
