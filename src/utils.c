#include <woody-woodpacker.h>
#include <melf.h>

void print_usage(void)
{
    printf("Usage: ./woody-woodpacker FILE [-p KEY] \n");
    exit(WOODY_ERR);
}

off_t get_file_size(const char *input_file)
{
    struct stat statbuf;

    if (stat(input_file, &statbuf) == -1)
        return -1;
    return statbuf.st_size;
}

int create_output_file(const char *input_file, off_t file_size)
{
    int input_fd = -1, output_fd = -1;
    uint8_t *buffer;

    if ((input_fd = open(input_file, O_RDONLY)) == -1) {
        perror("Unable to open input file");
        goto error;
    }
    buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, input_fd, 0);
    if (buffer == MAP_FAILED) {
        perror("Unable to map input file");
        goto error;
    }
    if (!melf_is_elf64(buffer, file_size)) {
        fprintf(stderr, "The provided file is not a valid 64 ELF executable\n");
        goto error;
    }
    munmap(buffer, file_size);
    if ((output_fd =
         open("./woody", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) == -1) {
        perror("Unable to create output file");
        goto error;
    }
    if (syscall(SYS_sendfile, output_fd, input_fd, NULL, file_size) == -1) {
        perror("Unable to write in output file");
        goto error;
    }
    close(input_fd);
    return output_fd;
 error:
    close(input_fd);
    close(output_fd);
    return -1;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
    size_t i;

    i = 0;
    while (i != n) {
        ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
        ++i;
    }
    return (dest);
}

void *ft_memmove(void *dest, const void *src, size_t n)
{
    if (src >= dest)
        return (ft_memcpy(dest, src, n));
    while (n-- != 0)
        ((unsigned char *)dest)[n] = ((unsigned char *)src)[n];
    return (dest);
}

size_t ft_strlen(const char *str)
{
    size_t i = 0;

    while (str[i])
        ++i;
    return i;
}
