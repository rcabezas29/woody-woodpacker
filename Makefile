# **************************************************************************** #
#                                   PROGRAM                                    #
# **************************************************************************** #

NAME = $(BUILD_DIR)/woody-woodpacker

# **************************************************************************** #
#                                     VARS                                     #
# **************************************************************************** #

MAKE = make

CP = cp
MV = mv
MKDIR = mkdir -p
RM = rm -rf

# **************************************************************************** #
#                                   COMPILER                                   #
# **************************************************************************** #

CC = gcc
# CFLAGS = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion
CFLAGS = -Wall -Wextra -Werror -Wshadow -g3
NASM = nasm
NASM_FLAGS = -f elf64

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

BUILD_DIR := build
SRC_DIR := src
INC_DIR := includes

SRCS := $(shell find $(SRC_DIR) -name '*.c')
SRCS_ASM := $(shell find $(SRC_DIR) -name '*.s')
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_ASM:%.s=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

# **************************************************************************** #
#                                    FLAGS                                     #
# **************************************************************************** #

CFLAGS += -I./$(INC_DIR)

# **************************************************************************** #
#                                    RULES                                     #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS) $(LDLIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

sanitize:: CFLAGS += -g3 -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=bounds -fsanitize=null
sanitize:: $(NAME)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(NASM) $(NASM_FLAGS) -o $@ $<

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(BUILD_DIR)

re:: clean
re:: all

-include $(DEPS)

.PHONY: all sanitize clean fclean re
