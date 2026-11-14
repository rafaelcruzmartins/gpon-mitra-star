/* g7flash: grava a imagem OpenWrt (sysupgrade) na regiao de firmware do G7
 * (0x20000-0x3d0000) a partir do firmware de fabrica em execucao.
 *
 * Uso: g7flash PORTA TAMANHO CRC32_HEX [--write]
 *  - recebe exatamente TAMANHO bytes por TCP na PORTA, direto para a RAM;
 *  - confere CRC32, cabecalho 03000003 e tamanho maximo;
 *  - confere /proc/mtd: mtd2 = "kernel" 0x170000, mtd3 = "rootfs" 0x240000;
 *  - sem --write: apenas relata (ensaio) e sai;
 *  - com --write: vira daemon (log em /var/tmp/g7flash.log), trava a memoria,
 *    desliga o watchdog, apaga/grava/confere cada bloco de mtd2 e depois mtd3,
 *    e reinicia. Nunca abre uboot, misc, hwconfig ou mtd0. */
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <mtd/mtd-user.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/reboot.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define FW_MAX     0x3b0000u
#define KERN_SIZE  0x170000u
#define ROOT_SIZE  0x240000u
#define WDT_BASE   0x90800000u
#define WDT_CR     0x0c

static unsigned long crc_table[256];
static unsigned char vbuf[0x10000];

static void crc_init(void)
{
	for (unsigned long n = 0; n < 256; n++) {
		unsigned long c = n;
		for (int k = 0; k < 8; k++)
			c = (c & 1) ? 0xedb88320UL ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}
}

static unsigned long crc32(const unsigned char *p, size_t n)
{
	unsigned long c = 0xffffffffUL;
	while (n--)
		c = crc_table[(c ^ *p++) & 0xff] ^ (c >> 8);
	return c ^ 0xffffffffUL;
}

static void logf_(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void logf_(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	fflush(stdout);
	fsync(1);
}

/* confere nome e tamanho de uma particao em /proc/mtd */
static int check_proc_mtd(const char *dev, unsigned size, const char *name)
{
	FILE *f = fopen("/proc/mtd", "r");
	char line[256], want[64];
	int ok = 0;
	if (!f)
		return 0;
	snprintf(want, sizeof want, "%s: %08x 00001000 \"%s\"", dev, size, name);
	while (fgets(line, sizeof line, f))
		if (strncmp(line, want, strlen(want)) == 0)
			ok = 1;
	fclose(f);
	return ok;
}

static int open_part(const char *path, unsigned size, int rw)
{
	int fd = open(path, rw ? O_RDWR | O_SYNC : O_RDONLY);
	struct mtd_info_user mi;
	if (fd < 0) {
		logf_("ERRO abrir %s: %s\n", path, strerror(errno));
		return -1;
	}
	if (ioctl(fd, MEMGETINFO, &mi)) {
		logf_("ERRO MEMGETINFO %s: %s\n", path, strerror(errno));
		return -1;
	}
	logf_("%s: size=0x%x erasesize=0x%x writesize=%u flags=0x%x %s\n", path,
	      mi.size, mi.erasesize, mi.writesize, mi.flags,
	      (mi.flags & MTD_WRITEABLE) ? "gravavel" : "SOMENTE-LEITURA");
	if (mi.size != size || mi.erasesize != 0x1000 || !(mi.flags & MTD_WRITEABLE)) {
		logf_("ERRO: %s nao corresponde ao esperado\n", path);
		return -1;
	}
	return fd;
}

static volatile uint32_t *wdt_map(void)
{
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
		return NULL;
	void *p = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, WDT_BASE);
	close(fd);
	return p == MAP_FAILED ? NULL : (volatile uint32_t *)p;
}

/* apaga, grava e confere bloco a bloco (blocos de 64 KiB = 16 setores) */
static int write_part(int fd, const char *name, const unsigned char *data,
		      unsigned len, unsigned size)
{
	const unsigned BLK = 0x10000;
	static unsigned char blk[0x10000];
	for (unsigned off = 0; off < size; off += BLK) {
		unsigned n = size - off < BLK ? size - off : BLK;
		memset(blk, 0xff, n);
		if (off < len)
			memcpy(blk, data + off, (len - off) < n ? (len - off) : n);
		int tries;
		for (tries = 0; tries < 3; tries++) {
			struct erase_info_user ei = { .start = off, .length = n };
			if (ioctl(fd, MEMERASE, &ei)) {
				logf_("  %s 0x%06x erase falhou: %s\n", name, off, strerror(errno));
				continue;
			}
			if (pwrite(fd, blk, n, off) != (ssize_t)n) {
				logf_("  %s 0x%06x write falhou: %s\n", name, off, strerror(errno));
				continue;
			}
			if (pread(fd, vbuf, n, off) != (ssize_t)n || memcmp(vbuf, blk, n)) {
				logf_("  %s 0x%06x verificacao falhou\n", name, off);
				continue;
			}
			break;
		}
		if (tries == 3) {
			logf_("ERRO FATAL em %s 0x%06x\n", name, off);
			return -1;
		}
		logf_("  %s 0x%06x ok\n", name, off);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 4) {
		fprintf(stderr, "uso: %s PORTA TAMANHO CRC32 [--write]\n", argv[0]);
		return 2;
	}
	int do_write = argc > 4 && strcmp(argv[4], "--write") == 0;
	unsigned long size = strtoul(argv[2], 0, 0);
	unsigned long want_crc = strtoul(argv[3], 0, 16);
	crc_init();

	if (size < 0x200 || size > FW_MAX) {
		logf_("ERRO: tamanho %lu fora do limite (max %u)\n", size, FW_MAX);
		return 1;
	}
	if (!check_proc_mtd("mtd2", KERN_SIZE, "kernel") ||
	    !check_proc_mtd("mtd3", ROOT_SIZE, "rootfs")) {
		logf_("ERRO: /proc/mtd nao tem o layout esperado\n");
		return 1;
	}
	unsigned char *img = malloc(size);
	if (!img) {
		logf_("ERRO: sem memoria\n");
		return 1;
	}

	/* recebe a imagem */
	int s = socket(AF_INET, SOCK_STREAM, 0), one = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
	struct sockaddr_in a = { .sin_family = AF_INET,
				 .sin_port = htons(atoi(argv[1])) };
	if (bind(s, (struct sockaddr *)&a, sizeof a) || listen(s, 1)) {
		logf_("ERRO bind: %s\n", strerror(errno));
		return 1;
	}
	logf_("aguardando imagem na porta %s (%lu bytes)\n", argv[1], size);
	int c = accept(s, 0, 0);
	unsigned long got = 0;
	while (got < size) {
		ssize_t n = read(c, img + got, size - got);
		if (n <= 0)
			break;
		got += n;
	}
	unsigned long crc = crc32(img, got);
	char reply[128];
	int rl = snprintf(reply, sizeof reply, "bytes=%lu crc32=%08lx\n", got, crc);
	write(c, reply, rl);
	close(c);
	close(s);
	logf_("%s", reply);
	if (got != size || crc != want_crc) {
		logf_("ERRO: imagem recebida nao confere (esperado crc %08lx)\n", want_crc);
		return 1;
	}
	if (img[0] != 0x03 || img[1] || img[2] || img[3] != 0x03) {
		logf_("ERRO: cabecalho nao e 03000003\n");
		return 1;
	}
	logf_("imagem OK: %lu bytes, crc32 %08lx, cabecalho 03000003\n", got, crc);

	int fk = open_part("/dev/mtd2", KERN_SIZE, do_write);
	int fr = open_part("/dev/mtd3", ROOT_SIZE, do_write);
	if (fk < 0 || fr < 0)
		return 1;
	volatile uint32_t *wdt = wdt_map();
	logf_("watchdog WDCR=0x%08x\n", wdt ? wdt[WDT_CR / 4] : 0xffffffffu);

	if (!do_write) {
		logf_("ENSAIO OK: nada foi gravado\n");
		return 0;
	}

	/* modo gravacao: independente da sessao telnet */
	pid_t pid = fork();
	if (pid > 0) {
		logf_("gravacao iniciada em segundo plano, pid %d, log /var/tmp/g7flash.log\n", pid);
		return 0;
	}
	setsid();
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	int lf = open("/var/tmp/g7flash.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	dup2(lf, 1);
	dup2(lf, 2);
	int nf = open("/dev/null", O_RDONLY);
	dup2(nf, 0);
	if (mlockall(MCL_CURRENT | MCL_FUTURE))
		logf_("aviso: mlockall: %s\n", strerror(errno));

	if (wdt) {
		wdt[WDT_CR / 4] = 0;
		logf_("watchdog desligado: WDCR=0x%08x\n", wdt[WDT_CR / 4]);
	}
	unsigned klen = got < KERN_SIZE ? got : KERN_SIZE;
	logf_("gravando kernel (mtd2)...\n");
	if (write_part(fk, "mtd2", img, klen, KERN_SIZE))
		return 1;
	logf_("gravando rootfs (mtd3)...\n");
	if (write_part(fr, "mtd3", img + KERN_SIZE, got > KERN_SIZE ? got - KERN_SIZE : 0, ROOT_SIZE))
		return 1;
	logf_("GRAVACAO CONCLUIDA, reiniciando\n");
	sync();
	sleep(2);
	reboot(RB_AUTOBOOT);
	return 0;
}
