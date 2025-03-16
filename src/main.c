#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <errno.h>

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

	unsigned char *original_payload = (unsigned char *)payload_address;
	uint64_t payload_size = 0;
	while (original_payload[payload_size] != NOP_64) // Search for nop in assembly code
		++payload_size;

	unsigned char *payload = malloc(payload_size * sizeof(char));
	if (payload == NULL)
	{
		perror("Can't clone payload");
		close(woody_fd);
		return WOODY_ERR;
	}
	memcpy(payload, original_payload, payload_size);

	unsigned char *file = mmap(0, file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, woody_fd, 0);
	Elf64_Phdr *text_segment = find_code_segment(file, (Elf64_Ehdr *)file);
	if (!text_segment)
	{
		fprintf(stderr, "Error finding .text section\n");
		munmap(file, file_size);
		free(payload);
		close(woody_fd);
		return WOODY_ERR;
	}
    //memcpy(payload + OFFSET_TO_TEXT_SEGMENT_SIZE, &text_segment->p_memsz, sizeof(text_segment->p_memsz));
	if (inject_code_cave(file, file_size, payload, payload_size) == WOODY_ERR)
		fprintf(stderr, "Error injecting code\n");
	encrypt_xor_temp(file + text_segment->p_offset, text_segment->p_filesz, 0x20);
	//encrypt_xor(file + text_segment->p_offset, text_segment->p_memsz, 64, key);
	printf("key_value: %s\n", key);

	free(payload);
	munmap(file, file_size);
	close(woody_fd);
	return WOODY_OK;
}
