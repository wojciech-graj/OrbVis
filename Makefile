TARGET := orbvis

CC := gcc
WARNINGS := -Wall -Wextra -Wpedantic -Wdouble-promotion -Wstrict-prototypes -Wshadow -Wduplicated-cond -Wduplicated-branches -Wjump-misses-init -Wnull-dereference -Wrestrict -Wlogical-op -Walloc-zero -Wformat-security -Wformat-signedness -Winit-self -Wlogical-op -Wmissing-declarations -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wswitch-enum -Wundef -Wwrite-strings -Wno-discarded-qualifiers
CFLAGS := -O3 `pkg-config --cflags gtk+-3.0 cglm libcurl epoxy` -std=c11
LDFLAGS := -lm
COMMANDS := $(CC) pkg-config xxd sed
PACKAGES := gtk+-3.0 cglm libcurl epoxy

ifeq ($(MAKECMDGOALS),windows)
  CFLAGS += -march=core2 -flto
  LDFLAGS += -lopengl32 `pkg-config --libs gtk+-3.0 cglm libcurl epoxy` -mwindows -flto
else
  LDFLAGS += -ldl `pkg-config --libs gl gtk+-3.0 cglm libcurl epoxy`
  ifeq ($(MAKECMDGOALS),debug)
    CFLAGS += -march=native -fsanitize=address -fsanitize=undefined -g -ftrapv -fno-omit-frame-pointer -lrt -DDEBUG
    LDFLAGS += -fsanitize=address -fsanitize=undefined -g -ftrapv -fno-omit-frame-pointer -lrt
  else
    CFLAGS += -flto
    LDFLAGS += -flto
    ifeq ($(MAKECMDGOALS),release)
      CFLAGS += -march=core2
    else
      CFLAGS += -march=native
    endif
  endif
endif

BUILD_DIR := ./obj
SRC_DIRS := ./src
LIB_DIRS := ./lib
RES_DIRS := ./res

.PHONY: clean

LIB_SRCS := $(shell find $(LIB_DIRS) -name '*.c')
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
RES := $(shell find $(RES_DIRS) -type f)
RES_SRCS := $(RES:%=$(BUILD_DIR)/%.c)

OBJS := $(RES_SRCS:$(BUILD_DIR)/%=$(BUILD_DIR)/%.r.o) $(SRCS:%=$(BUILD_DIR)/%.s.o) $(LIB_SRCS:%=$(BUILD_DIR)/%.l.o)

INC_DIRS := $(shell find $(SRC_DIRS) $(LIB_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

$(TARGET): $(COMMANDS) $(PACKAGES) $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(COMMANDS):
	@if ! [ $$(command -v $@) ]; then \
		echo "Missing command: $@"; \
		false; \
	fi

$(PACKAGES):
	@if ! pkg-config --exists $@; then \
		echo "Missing package: $@"; \
		false; \
	fi

$(BUILD_DIR)/%.c.l.o: %.c
	mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) -I$(<D) -c $<

$(BUILD_DIR)/%.c.s.o: %.c
	mkdir -p $(@D)
	$(CC) $(INC_FLAGS) $(CFLAGS) $(WARNINGS) -c $< -o $@

$(BUILD_DIR)/%.c: %
	mkdir -p $(@D)
	xxd -i $< | sed -e 's/unsigned/const unsigned/' > $@

%.c.r.o: %.c
	mkdir -p $(@D)
	$(CC) $(INC_FLAGS) $(CFLAGS) $(WARNINGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

windows: $(TARGET)
debug: $(TARGET)
release: $(TARGET)
