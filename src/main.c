#include <woody-woodpacker.h>

const char	*password = "sINCZELWdqp9S1K5yBQsYFhRhadJTXM4cwN+JtAjrmdxXTuM3IwbAVnWZR1ieIP6atq8Ncbarikq6RUsr/79gg==";

void	print_usage(void)
{
	printf("Usage: ./woody-woodpacker [ELF Binary]");
	exit(1);
}

int	main(int argc, char **argv)
{
	int	woody_fd, progfd;
	if (argc != 2)
		print_usage();
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
			buffer[i] ^= password[i];
		write(woody_fd, buffer, ret);
	}
	close(woody_fd);
	close(progfd);
	return 0;
}
