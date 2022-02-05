SOURCE_DIR := src
SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.c)

TARGET_DIR := target
OBJECT_FILES := $(patsubst $(SOURCE_DIR)/%.c,$(TARGET_DIR)/%.o,$(SOURCE_FILES))

CC := gcc
CFLAGS := -Wall

LDLIBS := -ludev

usairb: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(TARGET_DIR)/$@ $^

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET_DIR)/%.o: $(SOURCE_DIR)/%.c | $(TARGET_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

