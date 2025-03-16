#ifndef MELF_H
#define MELF_H

#define MELF_MAGIC_NUMBER  0x464C457F
#define MELF_CLASS64 2

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	uint32_t magic_number;
	uint8_t class;
	uint8_t encoding;
	uint8_t version;
	uint8_t os_abi;
	uint64_t padding;
} melf_identifier;

typedef struct {    
	melf_identifier identifier;

	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint64_t entry_point;
	uint64_t program_header_offset;
	uint64_t section_header_offset;
	uint32_t flags;
	uint16_t header_size;
	uint16_t program_entry_size;
	uint16_t program_entry_number;
	uint16_t section_entry_size;
	uint16_t section_entry_number;
	uint16_t section_header_string_index;
} melf_file_header64;

typedef struct { 
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t virtual_address;
	uint64_t physical_address;
	uint64_t file_size;
	uint64_t memory_size;
	uint64_t align;
} melf_program_header64;

bool melf_is_elf(uint8_t const *file, uint64_t const file_size);
bool melf_is_elf64(uint8_t const *file, uint64_t const file_size);

melf_identifier 	*melf_read_identifier(int fd);
melf_file_header64	*melf_read_header64(int fd);


#ifdef MELF_IMPLEMENTATION

bool melf_is_elf(uint8_t const *file, uint64_t const file_size)
{
	melf_identifier *identifier = (melf_identifier *)file;
	if (file_size < sizeof(melf_identifier) || identifier->magic_number != MELF_MAGIC_NUMBER)
		return false;
	return true;
}

bool melf_is_elf64(uint8_t const *file, uint64_t const file_size)
{
	melf_identifier *identifier = (melf_identifier *)file;
	if (file_size < sizeof(melf_identifier) || identifier->magic_number != MELF_MAGIC_NUMBER)
		return false;
	if (identifier->class != MELF_CLASS64)
		return false;
	return true;
}

melf_identifier *melf_read_identifier(int fd)
{
	melf_identifier *identifier = malloc(sizeof(melf_identifier));
	if (identifier == NULL)
		return NULL;
	
	int ret = read(fd, identifier, sizeof(melf_identifier));
	if (ret != (int) sizeof(melf_identifier))
	{
		free(identifier);
		return NULL;
	}

	return identifier;
}

melf_file_header64 *melf_read_header64(int fd)
{
	melf_file_header64 *file = malloc(sizeof(melf_file_header64));
	if (file == NULL)
		return NULL;
	
	int ret = read(fd, file, sizeof(melf_file_header64));
	if (ret != (int) sizeof(melf_file_header64))
	{
		free(file);
		return NULL;
	}
	return file;
}

#endif // MELF_IMPLEMENTATION 	

#endif // MELF_H
