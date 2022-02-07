SOURCE_DIR := src
SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.c)

TARGET_DIR := target
OBJECT_FILES := $(patsubst $(SOURCE_DIR)/%.c,$(TARGET_DIR)/%.o,$(SOURCE_FILES))

LIBUSBIP_DIR := external/libusbip

CC := gcc
CFLAGS := -Wall -I.

LDLIBS := -ludev -lusb-1.0

usairb: $(OBJECT_FILES) $(LIBUSBIP_DIR)/libusbip/libusbip.a
	$(CC) $(CFLAGS) $(LDLIBS) -o $(TARGET_DIR)/$@ $^

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET_DIR)/%.o: $(SOURCE_DIR)/%.c | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

external/libusbip/libusbip/libusbip.a:
	$(MAKE) -C $(LIBUSBIP_DIR)
