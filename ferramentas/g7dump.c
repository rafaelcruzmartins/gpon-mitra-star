/* g7dump: serve um arquivo (ex.: /dev/mtd0) por TCP, uma conexao, e sai.
 * Uso: g7dump PORTA ARQUIVO
 * Apenas LEITURA do arquivo. Imprime bytes e CRC32 do que foi enviado. */
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static unsigned long crc_table[256];

static void crc_init(void)
{
	for (unsigned long n = 0; n < 256; n++) {
		unsigned long c = n;
		for (int k = 0; k < 8; k++)
			c = (c & 1) ? 0xedb88320UL ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "uso: %s PORTA ARQUIVO\n", argv[0]);
		return 2;
	}
	crc_init();
	int fd = open(argv[2], O_RDONLY);
	if (fd < 0) { perror("open"); return 1; }

	int s = socket(AF_INET, SOCK_STREAM, 0);
	int one = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
	struct sockaddr_in a;
	memset(&a, 0, sizeof a);
	a.sin_family = AF_INET;
	a.sin_port = htons(atoi(argv[1]));
	a.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (struct sockaddr *)&a, sizeof a) || listen(s, 1)) {
		perror("bind/listen");
		return 1;
	}
	printf("aguardando na porta %s\n", argv[1]);
	fflush(stdout);
	int c = accept(s, 0, 0);
	if (c < 0) { perror("accept"); return 1; }

	static unsigned char buf[16384];
	unsigned long crc = 0xffffffffUL, total = 0;
	for (;;) {
		ssize_t n = read(fd, buf, sizeof buf);
		if (n < 0) { perror("read"); return 1; }
		if (n == 0) break;
		for (ssize_t i = 0; i < n; i++)
			crc = crc_table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
		ssize_t off = 0;
		while (off < n) {
			ssize_t w = write(c, buf + off, n - off);
			if (w <= 0) { perror("write"); return 1; }
			off += w;
		}
		total += n;
	}
	close(c);
	close(fd);
	printf("bytes=%lu crc32=%08lx\n", total, crc ^ 0xffffffffUL);
	return 0;
}
