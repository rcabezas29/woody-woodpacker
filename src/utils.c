#include <woody-woodpacker.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <melf.h>

void	print_usage(void)
{
	printf("Usage: ./woody-woodpacker [ELF Binary]\n");
	exit(WOODY_ERR);
}

int create_output_file(const char *input_file, off_t file_size)
{
	
	int prog_fd = open(input_file, O_RDONLY);
	if (prog_fd == -1)
	{
		perror("Can't open input file");
		goto error;
	}
	if (!melf_is_elf64(prog_fd))
	{
		fprintf(stderr, "File isn't elf64\n");
		goto error;
	}

	int woody_fd = open("./woody", O_CREAT |  O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (woody_fd == -1)
	{
		perror("Can't create output file");
		goto error;
	}

	if (syscall(SYS_sendfile, woody_fd, prog_fd, NULL, file_size) == -1)
	{
		close(woody_fd);
		perror("Can't write in output file");
		goto error;
	}
	close(prog_fd);
	return woody_fd;

error:
	close(prog_fd);
	return -1;
}

off_t get_file_size(const char *input_file)
{
	struct stat statbuf;
	// if (syscall(SYS_fstat, input_file, &statbuf))
	if (stat(input_file, &statbuf) == -1)
	{
		perror("Can't get file size");
		return -1;
	}
	return statbuf.st_size;
}

Elf64_Shdr *get_text_section(unsigned char *file)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
	Elf64_Shdr *shdr = (Elf64_Shdr *)(file + ehdr->e_shoff);
	const char *shstrtab = (const char *)(file + shdr[ehdr->e_shstrndx].sh_offset);

	for (int i = 0; i < ehdr->e_shnum; ++i)
	{
		if (strcmp(shstrtab + shdr[i].sh_name, ".text") == 0)
			return &shdr[i];
	}
	return NULL;
}
