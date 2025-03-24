#include <woody-woodpacker.h>
#include <melf.h>


static unsigned char *find_value(unsigned char *haystack, uint64_t needle)
{
	while (*(uint64_t *)haystack != needle)
		++haystack;
	return haystack;
}

woody_status	inject_payload(unsigned char *file, uint64_t file_size, payload_t payload, uint64_t key_size)
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
	payload_segment->file_size = payload.size;
	payload_segment->memory_size = payload.size;

	*((uint64_t *)find_value(payload.value, OEP)) = file_header->entry_point;
	*((uint64_t *)find_value(payload.value, OVA)) = text_segment->virtual_address;
	*((uint64_t *)find_value(payload.value, NEP)) = payload_segment->virtual_address;
	*((uint64_t *)find_value(payload.value, CSZ)) = text_segment->memory_size;
	*((uint64_t *)find_value(payload.value, KSZ)) = key_size;

	file_header->entry_point = payload_segment->virtual_address;
	ft_memmove(file + file_size, payload.value, payload.size);
	return WOODY_OK;
}
