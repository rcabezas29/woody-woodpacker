#ifndef MELF_H
#define MELF_H

#define MELF_MAGIC_NUMBER  0x464C457F
#define MELF_CLASS64 2

#define MELF_PT_NOTE 4

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

melf_program_header64 *melf_get_text_segment(uint8_t const *file);
melf_program_header64 *melf_get_next_segment(uint8_t const *file, melf_program_header64 * current_segment);
melf_program_header64 *melf_get_note_segment(uint8_t const *file);
melf_program_header64 *melf_get_max_virtual_address(uint8_t const *file);

#ifdef MELF_IMPLEMENTATION

bool melf_is_elf(uint8_t const *file, uint64_t const file_size)
{
    melf_identifier *identifier = (melf_identifier *) file;
    if (file_size < sizeof(melf_identifier) || identifier->magic_number != MELF_MAGIC_NUMBER)
        return false;
    return true;
}

bool melf_is_elf64(uint8_t const *file, uint64_t const file_size)
{
    melf_identifier *identifier = (melf_identifier *) file;
    if (file_size < sizeof(melf_identifier) || identifier->magic_number != MELF_MAGIC_NUMBER)
        return false;
    if (identifier->class != MELF_CLASS64)
        return false;
    return true;
}

melf_program_header64 *melf_get_text_segment(uint8_t const *file)
{
    melf_file_header64 *file_header = (melf_file_header64 *) file;
    melf_program_header64 *program_header;

    for (Elf64_Half i = 0; i < file_header->program_entry_number; ++i) {
        program_header =
            (melf_program_header64 *) (file + file_header->program_header_offset + file_header->program_entry_size * i);
        if (file_header->entry_point >= program_header->virtual_address
            && file_header->entry_point < program_header->virtual_address + program_header->memory_size)
            return program_header;
    }
    return NULL;
}

melf_program_header64 *melf_get_next_segment(uint8_t const *file, melf_program_header64 * current_segment)
{
    melf_file_header64 *file_header = (melf_file_header64 *) file;
    melf_program_header64 *program_header, *next_segment = NULL;

    for (Elf64_Half i = 0; i < file_header->program_entry_number; ++i) {
        program_header =
            (melf_program_header64 *) (file + file_header->program_header_offset + file_header->program_entry_size * i);
        if ((current_segment->offset < program_header->offset && next_segment == NULL)
            || (next_segment != NULL && current_segment->offset < program_header->offset
                && program_header->offset < next_segment->offset))
            next_segment = program_header;
    }
    return next_segment;
}

#include <elf.h>

melf_program_header64 *melf_get_note_segment(uint8_t const *file)
{
    melf_file_header64 *file_header = (melf_file_header64 *) file;
    melf_program_header64 *program_header;

    for (Elf64_Half i = 0; i < file_header->program_entry_number; ++i) {
        program_header =
            (melf_program_header64 *) (file + file_header->program_header_offset + file_header->program_entry_size * i);
        if (program_header->type == PT_NOTE)
            return program_header;
    }
    return NULL;
}

melf_program_header64 *melf_get_max_virtual_address(uint8_t const *file)
{
    melf_file_header64 *file_header = (melf_file_header64 *) file;
    melf_program_header64 *program_header, *max_virtual_address = NULL;

    for (Elf64_Half i = 0; i < file_header->program_entry_number; ++i) {
        program_header =
            (melf_program_header64 *) (file + file_header->program_header_offset + file_header->program_entry_size * i);
        if (max_virtual_address == NULL || program_header->virtual_address > max_virtual_address->virtual_address)
            max_virtual_address = program_header;
    }
    return max_virtual_address;
}

#endif                          // MELF_IMPLEMENTATION

#endif                          // MELF_H
