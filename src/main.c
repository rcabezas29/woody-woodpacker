#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>

unsigned char payload[] = "\x48\x31\xFF\xB8\x3C\x00\x00\x00\x0F\x05";
// unsigned char payload[] = "\x50\x57\x48\xbf\x42\x42\x42\x42\x42\x42\x42\x42\x48\xb8\x00\x00\x00\x00\x00\x00\x00\x00\x48\x29\xf8\x5f\x58\xff\xe0";
// unsigned char payload[] = "\x48\xb8\x42\x42\x42\x42\x42\x42\x42\x42\xff\xe0";

#include <elf.h>

int	main(int argc, char **argv)
{
	if (argc != 2)
		print_usage();

	unsigned char key[KEY_SIZE + 1];
	key[KEY_SIZE] = '\0';
	if (generate_key(KEY_SIZE, key) == WOODY_ERR)
	{
		perror("Can't generate encryption key");
		return WOODY_ERR;
	}

	int prog_fd = open(argv[1], O_RDONLY);
	if (prog_fd == -1)
	{
		perror("Can't open input file");
		return WOODY_ERR;
	}
	if (!melf_is_elf64(prog_fd))
	{
		close(prog_fd);
		print_usage();
	}

	int woody_fd = open("./woody", O_CREAT |  O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (woody_fd == -1)
	{
		perror("Can't create output file");
		close(prog_fd);
		return WOODY_ERR;
	}
	off_t file_size = lseek(prog_fd, 0, SEEK_END);
	if (file_size == -1 || lseek(prog_fd, 0, SEEK_SET) == -1)
	{
		close(prog_fd);
		close(woody_fd);
		perror("Can't get file size");
		return WOODY_ERR;
	}
	if (syscall(SYS_sendfile, woody_fd, prog_fd, NULL, file_size) == -1)
	{
		close(prog_fd);
		close(woody_fd);
		perror("Can't write in output file");
		return WOODY_ERR;
	}
	close(prog_fd);

	unsigned char *file = mmap(0, file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, woody_fd, 0);
	if (inject_code_cave(file, file_size, payload, sizeof(payload) - 1) == WOODY_ERR)
		fprintf(stderr, "Error injecting code\n");
	printf("key_value: %s\n", key);

	munmap(file, file_size);
	close(woody_fd);
	return WOODY_OK;
}
