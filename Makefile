TARGET := orbvis

CC := gcc
WARNINGS := -Wall -Wextra -Wpedantic -Wdouble-promotion -Wstrict-prototypes -Wshadow -Wduplicated-cond -Wduplicated-branches -Wjump-misses-init -Wnull-dereference -Wrestrict -Wlogical-op -Walloc-zero -Wformat-security -Wformat-signedness -Winit-self -Wlogical-op -Wmissing-declarations -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wswitch-enum -Wundef -Wwrite-strings -Wno-discarded-qualifiers
CFLAGS := -march=native -O3
LDFLAGS := -lm

ifeq ($(MAKECMDGOALS),windows)
  CFLAGS += `pkg-config --cflags gtk+-3.0 cglm libcurl epoxy` -flto
  LDFLAGS += -lopengl32 `pkg-config --libs gtk+-3.0 cglm libcurl epoxy` -mwindows -flto
else
  CFLAGS += `pkg-config --cflags gl gtk+-3.0 cglm libcurl epoxy`
  LDFLAGS += -ldl `pkg-config --libs gl gtk+-3.0 cglm libcurl epoxy`
  ifeq ($(MAKECMDGOALS),debug)
    CFLAGS += -fsanitize=address -fsanitize=undefined -g -ftrapv -fno-omit-frame-pointer -lrt
  else
    CFLAGS += -flto
    LDFLAGS += -flto
  endif
endif

BUILD_DIR := ./obj
SRC_DIRS := ./src
LIB_DIRS := ./lib

.PHONY: clean

SRC_HASHMAP := lib/hashmap_c/hashmap.c
SRC_STB_IMAGE := lib/stb_image/stb_image.c
SRCS_SGP4 := $(shell find lib/sgp4 -name '*.c')
SRCS_SATCAT := $(shell find lib/satcat -name '*.c')

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

OBJ_HASHMAP = $(BUILD_DIR)/lib/hashmap_c/hashmap.c.o
OBJ_STB_IMAGE = $(BUILD_DIR)/lib/stb_image/stb_image.c.o
OBJS_SGP4 = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(SRCS_SGP4)))
OBJS_SATCAT = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(SRCS_SATCAT)))
OBJS := $(OBJ_HASHMAP) $(OBJ_STB_IMAGE) $(OBJS_SGP4) $(OBJS_SATCAT) $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) $(LIB_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_HASHMAP):
	mkdir -p $(BUILD_DIR)/lib/hashmap_c
	$(CC) -o $(OBJ_HASHMAP) -std=c99 $(CFLAGS) -Ilib/hashmap_c -c $(SRC_HASHMAP)

$(OBJ_STB_IMAGE):
	mkdir -p $(BUILD_DIR)/lib/stb_image
	$(CC) -o $(OBJ_STB_IMAGE) -std=c99 $(CFLAGS) -Ilib/stb_image -c $(SRC_STB_IMAGE)

$(OBJS_SGP4):
	mkdir -p $(BUILD_DIR)/lib/sgp4
	$(foreach src,$(SRCS_SGP4),$(CC) -o $(BUILD_DIR)/$(src).o -std=c99 $(CFLAGS) -Ilib/sgp4 -c $(src) ;)

$(OBJS_SATCAT):
	mkdir -p $(BUILD_DIR)/lib/satcat
	$(foreach src,$(SRCS_SATCAT),$(CC) -o $(BUILD_DIR)/$(src).o -std=c99 $(CFLAGS) -Ilib/satcat -c $(src) ;)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(INC_FLAGS) -std=c11 $(CFLAGS) $(WARNINGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

windows: $(TARGET)
debug: $(TARGET)

-include $(DEPS)
