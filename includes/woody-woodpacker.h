#ifndef WOODY_WOODPACKER_H
#define WOOD_WOODPACKER_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <elf.h>

typedef uint8_t woody_status;
#define WOODY_OK 0
#define WOODY_ERR 1

#define KEY_SIZE 64

// inject/code_cave.c

Elf64_Phdr		*find_code_segment(unsigned char *file, Elf64_Ehdr *file_header);
Elf64_Phdr		*find_next_segment(unsigned char *file, Elf64_Ehdr *file_header, Elf64_Phdr *current_segment);
woody_status	inject_code_cave(unsigned char *file, uint64_t file_size, unsigned char *payload, uint64_t payload_size);

// encrypt.c
void			encrypt_xor(unsigned char *buffer, uint64_t size, uint64_t key_size, unsigned char key[key_size]);
woody_status	generate_key(uint64_t size, unsigned char key[size]);

// utils.c
void	print_usage(void);
int     create_output_file(const char *input_file, off_t file_size);
off_t   get_file_size(const char *input_file);

#endif
