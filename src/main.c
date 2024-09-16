#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>


unsigned char *generate_key(void)
{
	unsigned char *key = malloc(KEY_SIZE);
	if (key == NULL)
		return NULL;

	int urandom_fd = open("/dev/urandom", O_RDONLY);
	if (urandom_fd == -1)
		return NULL;

	if (read(urandom_fd, key, KEY_SIZE) == -1)
	{
		close(urandom_fd);
		return NULL;
	}

	for (int i = 0; i < KEY_SIZE; ++i)
	{
		switch (key[i] % 4)
		{
			case 0:
				key[i] = 'A' + (key[i] % 26);
				break;
			case 1:
				key[i] = 'a' + (key[i] % 26);
				break;
			case 2:
				key[i] = '0' + (key[i] % 10);
				break;
			case 3:
				key[i] = '!' + (key[i] % 14);
				break;
		}
	}

	close(urandom_fd);
	return key;
}

uint64_t find_code_cave(uint8_t *buffer, uint64_t size)
{
	uint64_t current_best = 0;
	uint64_t max_size = 0, i = 0;
	while (i < size)
	{
		if (buffer[i] != 0)
		{
			i++;
			continue;
		}
		uint64_t current_size = 0, j = i;
		while (j < size && buffer[j] == 0x00)
		{
			++current_size;
			++j;
		}
		if (current_size > max_size)
		{
			current_best = i;
			max_size = current_size;
		}
		i = j;
	}
	
	// for (uint64_t i = 0; i < size; ++i)
	// {

	// }
	printf("Cave size = %li\n", max_size);
	return current_best;
}


void	print_usage(void)
{
	printf("Usage: ./woody-woodpacker [ELF Binary]\n");
	exit(1);
}
#include <elf.h>

int	main(int argc, char **argv)
{
	int	woody_fd, progfd;
	if (argc != 2)
		print_usage();

	unsigned char *key = generate_key();
	if (key == NULL)
		return -1;

	woody_fd = open("./woody", O_CREAT |  O_TRUNC | O_RDWR, S_IRWXU);
	if (woody_fd == -1)
		return 1;
	progfd = open(argv[1], O_RDONLY);
	if (progfd == -1)
	{
		close(woody_fd);
		return 1;
	}

	// Detect elf file
	if (!melf_is_elf64(progfd))
		print_usage();

	melf_file_header64 *file = melf_read_header64(progfd); 
	if (file == NULL)
	{
		close(progfd);
		close(woody_fd);
		print_usage();
	}
	printf("Entry point = %lx\n", file->entry_point);


	char	buffer[64];
	int		ret;
	lseek(progfd, 0, SEEK_SET);
	while ((ret = read(progfd, buffer, 64)) > 0)
	{
		// for (int i = 0; i < ret; ++i)
		// 	buffer[i] ^= key[i];
		write(woody_fd, buffer, ret);
	}
	printf("key_value: %s\n", key);


	int size = lseek(woody_fd, 0, SEEK_END);
	uint8_t *cave_buffer = malloc(size);
	lseek(woody_fd, 0, SEEK_SET);
	read(woody_fd, cave_buffer, size);

	uint64_t code_cave_dir = find_code_cave(cave_buffer, size);
	printf("Code cave dir = 0x%lx\n", code_cave_dir);

	free(key);
	free(file);
	free(cave_buffer);
	close(woody_fd);
	close(progfd);
	return 0;
}
