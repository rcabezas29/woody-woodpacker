#include <woody-woodpacker.h>

#define MELF_IMPLEMENTATION
#include <melf.h>

int main(int argc, char **argv)
{
    payload_t payload = {.value = NULL,.size = 0 };
    off_t file_size = 0;
    int output_fd = -1;
    uint8_t key[KEY_SIZE + 1];
    uint8_t *file = NULL;
    woody_status return_value = WOODY_ERR;
    uint64_t key_size;

    if (argc == 2) {
        key_size = KEY_SIZE;
        if (generate_key(key_size, key) == WOODY_ERR) {
            perror("Unable to generate encryption key");
            return WOODY_ERR;
        }
    } else if (argc == 4 && *argv[2] && *((uint16_t *) argv[2]) == *((uint16_t *) "-p")) {
        key_size = ft_strlen(argv[3]);
        if (key_size > 64) {
            fprintf(stderr, "Key must be as much as 64 bytes long\n");
            goto end;
        } else if (key_size == 0) {
            fprintf(stderr, "Key must be at least 1 byte long\n");
            goto end;
        }
        ft_memcpy(key, argv[3], key_size);
    } else
        print_usage();
    key[key_size] = '\0';

    if ((file_size = get_file_size(argv[1])) == -1) {
        perror("Unable to get file size");
        return WOODY_ERR;
    }

    if ((output_fd = create_output_file(argv[1], file_size)) == -1)
        return WOODY_ERR;

    if ((generate_payload(&payload)) == WOODY_ERR) {
        perror("Unable to generate payload");
        goto end;
    }
    if (syscall(SYS_ftruncate, output_fd, file_size + payload.size) == -1) {
        perror("Unable to truncate file");
        goto end;
    }
    if ((file =
         mmap(0, file_size + payload.size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, output_fd, 0)) == NULL) {
        perror("Unable to map file in memory");
        goto end;
    }

    melf_program_header64 *text_segment = melf_get_text_segment(file);
    if (!text_segment) {
        fprintf(stderr, "Unable to find .text section\n");
        goto end;
    }
    if (inject_payload(file, file_size, payload, key_size) == WOODY_ERR) {
        fprintf(stderr, "Error injecting code\n");
        goto end;
    }
    encrypt_xor(file + text_segment->offset, text_segment->file_size, key_size, key);
    printf("key_value: %s\n", key);

    return_value = WOODY_OK;
 end:
    close(output_fd);
    if (payload.value != NULL)
        free(payload.value);
    munmap(file, file_size);
    return return_value;
}
