#include <woody-woodpacker.h>
#include <elf.h>

Elf64_Phdr *find_code_segment(unsigned char *file, Elf64_Ehdr *file_header)
{
    Elf64_Phdr  *program_header;

    for (Elf64_Half i = 0; i < file_header->e_phnum; ++i)
    {
        program_header = (Elf64_Phdr *)(file + file_header->e_phoff + file_header->e_phentsize * i);
        if (file_header->e_entry >= program_header->p_vaddr && file_header->e_entry < program_header->p_vaddr + program_header->p_memsz)
            return program_header;
    }
    return NULL;
}

Elf64_Phdr *find_next_segment(unsigned char *file, Elf64_Ehdr *file_header, Elf64_Phdr *current_segment)
{
    Elf64_Phdr  *program_header, *next_segment = NULL;

    for (Elf64_Half i = 0; i < file_header->e_phnum; ++i)
    {
        program_header = (Elf64_Phdr *)(file + file_header->e_phoff + file_header->e_phentsize * i);
        if ((current_segment->p_offset < program_header->p_offset && next_segment == NULL) ||
            (next_segment != NULL && current_segment->p_offset < program_header->p_offset && program_header->p_offset < next_segment->p_offset))
            next_segment = program_header;
    }
    return next_segment;
}
