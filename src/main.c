#include <woody-woodpacker.h>

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


void	print_usage(void)
{
	printf("Usage: ./woody-woodpacker [ELF Binary]\n");
	exit(1);
}

int	main(int argc, char **argv)
{
	int	woody_fd, progfd;
	if (argc != 2)
		print_usage();

	unsigned char *key = generate_key();
	if (key == NULL)
		return -1;
	printf("key_value: %s\n", key);

	woody_fd = open("./woody", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	if (woody_fd == -1)
		return 1;
	progfd = open(argv[1], O_RDONLY);
	if (progfd == -1)
	{
		close(woody_fd);
		return 1;
	}
	char	buffer[64];
	int		ret;
	while ((ret = read(progfd, buffer, 64)) > 0)
	{
		for (int i = 0; i < ret; ++i)
			buffer[i] ^= key[i];
		write(woody_fd, buffer, ret);
	}

	free(key);
	close(woody_fd);
	close(progfd);
	return 0;
}
