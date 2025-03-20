#include <woody-woodpacker.h>
#include <melf.h>
#include <string.h>

static unsigned char *find_value(unsigned char *haystack, size_t size, uint64_t needle)
{
	(void)size;
	while (*(uint64_t *)haystack != needle)
		++haystack;
	return haystack;
}

woody_status	inject_payload(unsigned char *file, uint64_t file_size, unsigned char *payload, uint64_t payload_size)
{
	melf_file_header64 *file_header = (melf_file_header64 *)file;
	melf_program_header64 *text_segment = melf_get_text_segment(file);
	melf_program_header64 *payload_segment = melf_get_note_segment(file);

	if (text_segment == NULL || payload_segment == NULL)
		return WOODY_ERR;

	payload_segment->type = PT_LOAD;
	payload_segment->flags = PF_R | PF_X;
	payload_segment->offset = file_size;
	payload_segment->virtual_address = 0xF000000 + file_size;
	payload_segment->file_size = payload_size;
	payload_segment->memory_size = payload_size;
	
	//printf("Code segment offset %lx, code segment size = %lx virtual address = %lx\n", next_segment->p_offset, next_segment->p_filesz, next_segment->p_vaddr);

	*((uint64_t *)find_value(payload, payload_size, OEP)) = file_header->entry_point;
	*((uint64_t *)find_value(payload, payload_size, OVA)) = text_segment->virtual_address;
	*((uint64_t *)find_value(payload, payload_size, NEP)) = payload_segment->virtual_address;
	*((uint64_t *)find_value(payload, payload_size, CSZ)) = text_segment->memory_size;

	// uint32_t jump_position = code_segment->file_size - (file_header->entry_point - code_segment->virtual_address) + payload_size + 5;
	// jump_position *= -1;

	file_header->entry_point = payload_segment->virtual_address;
	memmove(file + file_size, payload, payload_size);
	return WOODY_OK;
}
