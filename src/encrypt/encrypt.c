#include <woody-woodpacker.h>

woody_status generate_key(uint64_t const size, uint8_t key[size])
{
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd == -1)
        return WOODY_ERR;
    if (read(urandom_fd, key, size) == -1) {
        close(urandom_fd);
        return WOODY_ERR;
    }
    close(urandom_fd);

    for (uint64_t i = 0; i < size; ++i) {
        switch (key[i] % 4) {
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
    return WOODY_OK;
}
