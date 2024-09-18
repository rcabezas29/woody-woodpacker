#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>

const char *instructions = "\x48\x31\xFF\xB8\x3C\x00\x00\x00\x0F\x05";

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

uint64_t find_code_cave(uint8_t *buffer, uint64_t size, uint64_t *code_cave_size)
{
	uint64_t current_best = 0;
	uint64_t max_size = 0, i = 0;
	while (i < size)
	{
		if (buffer[i] != 0)
		{
			++i;
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
	// printf("First char (0x%X) = 0x%x\n", current_best, buffer[current_best]);
	// printf("First char = (0x%X) = 0x%x\n", current_best + max_size, buffer[current_best + max_size - 1]);
	*code_cave_size = max_size; 
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

	woody_fd = open("./woody", O_CREAT |  O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
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
	sync();
	printf("key_value: %s\n", key);


	int size = lseek(progfd, 0, SEEK_END);
	uint8_t *cave_buffer = malloc(size);
	lseek(progfd, 0, SEEK_SET);
	read(progfd, cave_buffer, size);

	uint64_t code_cave_size;
	uint64_t code_cave_dir = find_code_cave(cave_buffer, size, &code_cave_size);
	printf("Code cave dir = 0x%lx\n", code_cave_dir);
	printf("Cave size = %li\n", code_cave_size);

	melf_program_header64 program_header;
	uint16_t program_entry_number = file->program_entry_number;
	lseek(progfd, file->program_header_offset, SEEK_SET);

	for (uint16_t i = 0; i < program_entry_number; ++i)
	{
		read(progfd, &program_header, sizeof(program_header));
		printf("Type = %i File Address = 0x%lx File size = 0x%lx\n\n", program_header.type, program_header.offset, program_header.file_size);
		if (code_cave_dir >= program_header.offset && code_cave_dir + code_cave_size <= program_header.offset + program_header.align)
		{
			printf("Encontrado\n");
			printf("Lugar en memoria = 0x%lx Tamaño = %lx\n", program_header.virtual_address, program_header.memory_size);


			uint64_t new_entry_point = code_cave_dir + program_header.virtual_address - program_header.offset;
			printf("New entry point = 0x%lx\n", new_entry_point);

			program_header.flags |= PF_X;
			lseek(woody_fd, file->program_header_offset + i * sizeof(program_entry_number), SEEK_SET);
			write(woody_fd, &program_header, sizeof(program_header));

			lseek(woody_fd, code_cave_dir, SEEK_SET);
			write(woody_fd, instructions, 10);

			file->entry_point = new_entry_point;
			lseek(woody_fd, 0, SEEK_SET);
			write(woody_fd, file, sizeof(melf_file_header64));
			break;
		}
	}

	// for (uint16_t i = 0; i < program_entry_number; ++i)
	// {
	// 	read(progfd, &program_header, sizeof(program_header));
	// 	printf("Type = %i Virtual Address = %lx Mem size = %lx\n\n", program_header.type, program_header.virtual_address, program_header.memory_size);
	// 	if (file->entry_point >= program_header.virtual_address && file->entry_point < program_header.virtual_address + program_header.memory_size)
	// 	{
	// 		printf("Encontrado\n");
	// 		printf("Lugar en el fichero = 0x%lx Tama\n", program_header.offset);
	// 	}
	// }

	free(key);
	free(file);
	free(cave_buffer);
	close(woody_fd);
	close(progfd);
	return 0;
}
