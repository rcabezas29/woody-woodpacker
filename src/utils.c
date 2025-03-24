#include <woody-woodpacker.h>
#include <melf.h>

void	print_usage(void)
{
	printf("Usage: ./woody-woodpacker [ELF Binary]\n");
	exit(WOODY_ERR);
}

off_t	get_file_size(const char *input_file)
{
	struct stat statbuf;

	if (stat(input_file, &statbuf) == -1)
		return -1;
	return statbuf.st_size;
}

int create_output_file(const char *input_file, off_t file_size)
{
	int		input_fd = -1, output_fd = -1;
	uint8_t	*buffer;

	if ((input_fd = open(input_file, O_RDONLY)) == -1)
	{
		perror("Unable to open input file");
		goto error;
	}
	buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, input_fd, 0);
	if (buffer == MAP_FAILED)
	{
		perror("Unable to map input file");
		goto error;
	}
	if (!melf_is_elf64(buffer, file_size))
	{
		fprintf(stderr, "The provided file is not a valid 64 ELF executable\n");
		goto error;
	}
	munmap(buffer, file_size);
	if ((output_fd = open("./woody", O_CREAT |  O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) == -1)
	{
		perror("Unable to create output file");
		goto error;
	}
	if (syscall(SYS_sendfile, output_fd, input_fd, NULL, file_size) == -1)
	{
		perror("Unable to write in output file");
		goto error;
	}
	close(input_fd);
	return output_fd;
error:
	close(input_fd);
	close(output_fd);
	return -1;
}
