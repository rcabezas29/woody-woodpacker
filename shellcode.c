#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>

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

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDWR);

    struct stat statbuf;
	if (syscall(SYS_stat, argv[1], &statbuf))
	{
		perror("Can't get file size");
		return -1;
	}
	unsigned char *file = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);


    Elf64_Phdr *ph = find_code_segment(file, (Elf64_Ehdr *)file);
    printf("Start = 0x%lx Size = 0x%lx\n", ph->p_offset, ph->p_filesz);

    for (uint64_t i = 0; i < ph->p_filesz; ++i)
    {
        printf("\\x%02X", file[ph->p_offset + i]);
    }
    puts("");

    munmap(file, statbuf.st_size);
    close(fd);
    return 0;
}
