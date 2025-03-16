#include <string.h>
#include <elf.h>
#include <woody-woodpacker.h>

// void find_code_cave(uint8_t *buffer, uint64_t size, code_cave *cave)
// {
// 	uint64_t best_position = 0, max_size = 0;

// 	for (uint64_t i = 0; i < size; ++i)
// 	{
// 		if (buffer[i] != 0)
// 			continue;
		
// 		uint64_t j = i;
// 		uint64_t current_size = 0;
// 		while (j < size && buffer[j] == 0)
// 		{
// 			++j;
// 			++current_size;
// 		}
// 		if (current_size > max_size)
// 		{
// 			max_size = current_size;
// 			best_position = i;
// 		}
// 		i = j - 1;
// 	}
// 	cave->size = max_size;
// 	cave->start = best_position;
// }


// This function cause segfault if you search for a value that is not in haystack
unsigned char *find_value(unsigned char *haystack, size_t size, uint64_t needle)
{
	(void)size;
	while (*(uint64_t *)haystack != needle)
		++haystack;
	return haystack;
}

woody_status	inject_code_cave(unsigned char *file, uint64_t file_size, unsigned char *payload, uint64_t payload_size)
{
	Elf64_Ehdr *file_header = (Elf64_Ehdr *)file;
	(void)file_size;
	(void)payload;
	(void)payload_size;
	Elf64_Phdr *code_segment, *next_segment;
	// Elf64_Half program_entry_number = file_header->e_phnum;

	code_segment = find_code_segment(file, file_header);
	if (code_segment == NULL)
		return WOODY_ERR;
	next_segment = find_next_segment(file, file_header, code_segment);
	if (next_segment == NULL)
		return WOODY_ERR;
	//printf("Code segment offset %lx, code segment size = %lx virtual address = %lx\n", next_segment->p_offset, next_segment->p_filesz, next_segment->p_vaddr);

	//uint64_t code_cave_size = next_segment->p_offset - (code_segment->p_offset + code_segment->p_filesz);
	//printf("Code cave size = 0x%lx\n", code_cave_size);

	// Patch jmp to the original code
	// for (uint64_t i = 0; i < payload_size; ++i)
	// {
	// 	if (payload[i] != 0x42)
	// 		continue;
	// 	uint64_t j = i;
	// 	while (payload[j] == 0x42)
	// 		++j;
	// 	if (j - i != 8)
	// 		continue;
	// 	uint64_t *pointer = (uint64_t *)(payload + i);
	// 	*pointer = file_header->e_entry;
	// }
	*((uint64_t *)find_value(payload, payload_size, OEP)) = file_header->e_entry;
	*((uint64_t *)find_value(payload, payload_size, OVA)) = code_segment->p_vaddr;
	*((uint64_t *)find_value(payload, payload_size, NEP)) = code_segment->p_vaddr + code_segment->p_filesz;	
	*((uint64_t *)find_value(payload, payload_size, CSZ)) = code_segment->p_memsz;	

	uint32_t jump_position = code_segment->p_filesz - (file_header->e_entry - code_segment->p_vaddr) + payload_size + 5;
	jump_position *= -1;

	file_header->e_entry = code_segment->p_vaddr + code_segment->p_filesz;
	memmove(file + code_segment->p_offset + code_segment->p_filesz, payload, payload_size);
	// file[code_segment->p_offset + code_segment->p_filesz + payload_size] = '\xe9';
	// uint32_t *pointer = (uint32_t *)(file + code_segment->p_offset + code_segment->p_filesz + payload_size + 1);
	// *pointer = jump_position;

	return WOODY_OK;
}
