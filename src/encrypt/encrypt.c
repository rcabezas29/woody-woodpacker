#include <woody-woodpacker.h>

woody_status generate_key(uint64_t const size, uint8_t key[size])
{
	int urandom_fd = open("/dev/urandom", O_RDONLY);
	if (urandom_fd == -1)
		return WOODY_ERR;
	if (read(urandom_fd, key, size) == -1)
	{
		close(urandom_fd);
		return WOODY_ERR;
	}
	close(urandom_fd);

	for (uint64_t i = 0; i < size; ++i)
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
	return WOODY_OK;
}

// void	encrypt_xor(uint8_t *buffer, uint64_t const size, uint64_t const key_size, uint8_t const key[key_size])
// {
// 	for (size_t i = 0; i < size; ++i)
// 		buffer[i] ^= key[i % key_size];
// }
