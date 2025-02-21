#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

#define OFFSET_TO_TEXT_SEGMENT_SIZE 11
unsigned char payload[] = "\x52\x54\x48\x83\xEC\x52\x49\x89\xE7\x49\xBE\x42\x42\x42\x42\x42\x42\x42\x42\x49\xC7\x07\x2E\x2E\x2E\x2E\x49\xC7\x47\x04\x57\x4F\x4F\x44\x49\xC7\x47\x08\x59\x2E\x2E\x2E\x66\x41\xC7\x47\x0C\x2E\x0A\xB8\x01\x00\x00\x00\xBF\x01\x00\x00\x00\x4C\x89\xFE\xBA\x0E\x00\x00\x00\x0F\x05\x49\xC7\x07\x50\x61\x73\x73\x49\xC7\x47\x04\x77\x6F\x72\x64\x66\x41\xC7\x47\x08\x3A\x20\xB8\x01\x00\x00\x00\xBF\x01\x00\x00\x00\x4C\x89\xFE\xBA\x0A\x00\x00\x00\x0F\x05\xB8\x00\x00\x00\x00\xBF\x00\x00\x00\x00\x49\x8D\x77\x12\xBA\x40\x00\x00\x00\x0F\x05\xE8\x00\x00\x00\x00\x5D\x48\x81\xED\x89\x10\x40\x00\x4C\x89\xF0\x48\x8D\xB5\x00\x10\x40\x00\x48\x29\xC6\x4C\x89\xF2\x49\x8D\x7F\x12\x44\x8A\x06\x44\x8A\x0F\x45\x30\xC8\x44\x88\x06\x48\xFF\xC6\x48\xFF\xC7\x48\xFF\xCA\x48\x85\xD2\x74\x0C\x49\x3B\x7F\x52\x75\xE0\x49\x8D\x7F\x12\xEB\xDA\x48\x83\xC4\x52\x5C\x5A";

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
	Elf64_Phdr *text_segment = find_code_segment(file, (Elf64_Ehdr *)file);
	if (!text_segment)
	{
		fprintf(stderr, "Error finding .text section\n");
		munmap(file, file_size);
		close(woody_fd);
		return WOODY_ERR;
	}
	memcpy(payload + OFFSET_TO_TEXT_SEGMENT_SIZE, &text_segment->p_memsz, sizeof(text_segment->p_memsz));
	if (inject_code_cave(file, file_size, payload, sizeof(payload) - 1) == WOODY_ERR)
		fprintf(stderr, "Error injecting code\n");

	encrypt_xor(file + text_segment->p_offset, text_segment->p_memsz, 64, key);

	printf("key_value: %s\n", key);

	munmap(file, file_size);
	close(woody_fd);
	return WOODY_OK;
}
