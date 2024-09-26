#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>
#include <unistd.h>
#include <sys/mman.h>

// unsigned char payload[] = "\x48\x31\xFF\xB8\x3C\x00\x00\x00\x0F\x05";
unsigned char payload[] = "\x50\x57\x56\x52\x55\x48\x89\xE5\x48\x83\xEC\x20\x48\xC7\x04\x24\x2E\x2E\x2E\x2E\x48\xC7\x44\x24\x04\x57\x4F\x4F\x44\x48\xC7\x44\x24\x08\x59\x2E\x2E\x2E\x66\xC7\x44\x24\x0C\x2E\x0A\xB8\x01\x00\x00\x00\xBF\x01\x00\x00\x00\x48\x89\xE6\xBA\x0E\x00\x00\x00\x0F\x05\x48\x89\xEC\x5D\x5A\x5E\x5F\x58";
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

	off_t file_size = get_file_size(argv[1]);
	if (file_size == -1)
		return WOODY_ERR;

	int woody_fd = create_output_file(argv[1], file_size);
	if (woody_fd == -1)
		return WOODY_ERR;


	unsigned char *file = mmap(0, file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, woody_fd, 0);
	if (inject_code_cave(file, file_size, payload, sizeof(payload) - 1) == WOODY_ERR)
		fprintf(stderr, "Error injecting code\n");
	printf("key_value: %s\n", key);

	munmap(file, file_size);
	close(woody_fd);
	return WOODY_OK;
}
