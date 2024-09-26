#include <woody-woodpacker.h>
#include <sys/syscall.h>
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
	if (syscall(SYS_stat, input_file, &statbuf))
	{
		perror("Can't get file size");
		return -1;
	}
	return statbuf.st_size;
}
