#ifndef WOODY_WOODPACKER_H
#define WOOD_WOODPACKER_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

typedef uint8_t woody_status;
#define WOODY_OK 0
#define WOODY_ERR 1

#define KEY_SIZE 64

#define OEP		0xEAAEEAAEEAAEEAAE // Original Entry Point
#define NEP		0xABBAABBAABBAABBA // New Entry Point
#define OVA 	0xDEEDDEEDDEEDDEED // Original virtual address
#define CSZ		0xCEECCEECCEECCEEC // Code Size

#define NOP_64	0x90

typedef struct {
	uint8_t *value;
	uint64_t size;
} payload_t;

// inject_payload.c
woody_status	inject_payload(unsigned char *file, uint64_t file_size, unsigned char *payload, uint64_t payload_size);

// encrypt.c
woody_status	generate_key(uint64_t const size, uint8_t key[size]);

//encrypt_xor.s
void			encrypt_xor(uint8_t *buffer, uint64_t const size, uint64_t const key_size, uint8_t const key[key_size]);

// utils.c
void			print_usage(void);
off_t			get_file_size(const char *input_file);
int				create_output_file(const char *input_file, off_t file_size);
void			*ft_memcpy(void *dest, const void *src, size_t n);
void			*ft_memmove(void *dest, const void *src, size_t n);


// generate_payload.c
woody_status	generate_payload(payload_t *payload);

// payload.s
void			payload_address(void);

#endif
