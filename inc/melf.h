#ifndef MELF_H
#define MELF_H

#define MELF_MAGIC_NUMBER "\x7f""ELF"

#include <stdint.h>
#include <string.h>

typedef struct {
    uint32_t magic_number;
    uint8_t class;
    uint8_t encoding;
    uint8_t version;
    uint8_t os_abi;
    uint64_t padding;
} melf_identifier;

melf_identifier *melf_read_identifier(int fd);


#ifdef MELF_IMPLEMENTATION

melf_identifier *melf_read_identifier(int fd)
{
    melf_identifier *identifier = malloc(sizeof(melf_identifier));
    if (identifier == NULL)
        return NULL;
    
    int ret = read(fd, identifier, sizeof(melf_identifier));
    if (ret == -1 || ret < (int) sizeof(melf_identifier))
    {
        free(identifier);
        return NULL;
    }
    if (memcmp(&identifier->magic_number, MELF_MAGIC_NUMBER, sizeof(uint32_t)) != 0)
    {
        free(identifier);
        return NULL;
    }

    return identifier;
}

#endif // MELF_IMPLEMENTATION

#endif // MELF_H
