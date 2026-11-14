# Ferramentas usadas na instalação sem serial

Código usado em 13/11/2026 para fazer backup e gravar o OpenWrt no G7 pelo Telnet do firmware TripleOxygen. O contexto, os resultados e os limites estão em [OpenWrt sem adaptador serial](../docs/openwrt-sem-adaptador-serial.md).

**Uso por sua conta e risco.** Os programas foram escritos para o XZ000-G7 V1.0 de 4 MB com o layout de fábrica conferido naquele documento e foram executados uma vez. Antes de qualquer gravação, faça e confira o backup.

| Arquivo | Onde roda | Função |
|---|---|---|
| `g7bridge.py` | PC (Windows) | Abre o Telnet do G7 em um console. O usuário faz o login; depois a ponte aceita comandos em `127.0.0.1:2323` (`PEEK`, `RUN <cmd>`, `RAW <texto>`). A senha não é ecoada nem gravada |
| `g7.py` | PC | Cliente da ponte: `python g7.py RUN "cat /proc/mtd"` |
| `upload.py` | PC | Envia um arquivo para o G7 em blocos de `printf` com escapes octais: `python upload.py g7dump /var/tmp/g7dump` |
| `g7dump.c` | G7 | Serve um arquivo ou partição, só leitura, em uma porta TCP e informa bytes e CRC32 |
| `fetch.py` | PC | Inicia o `g7dump`, baixa e confere o CRC32: `python fetch.py /dev/mtd0 mtd0.bin 2401` |
| `g7flash.c` | G7 | Recebe a imagem `sysupgrade` por TCP, confere e, com `--write`, grava `mtd2`+`mtd3` com o watchdog desligado |
| `flashrun.py` | PC | Envia a imagem ao `g7flash`: ensaio sem argumento extra, gravação com `--write` |

No Git Bash, use `MSYS_NO_PATHCONV=1` antes dos comandos que recebem caminhos do G7 (`/var/tmp/...`, `/dev/mtd...`), para que não sejam convertidos em caminhos do Windows.

## Compilação dos programas do G7

Com o toolchain gerado pela compilação do `openwrt-galachip`:

```sh
export STAGING_DIR=~/openwrt-galachip/staging_dir
CC=$(ls $STAGING_DIR/toolchain-arm_*/bin/arm-openwrt-linux-*gcc | head -1)
$CC -static -Os -Wall -Wextra -march=armv5te -mtune=arm926ej-s -marm -s -o g7dump g7dump.c
$CC -static -Os -Wall -Wextra -march=armv5te -mtune=arm926ej-s -marm -s -o g7flash g7flash.c
```

Os binários são estáticos porque o firmware de fábrica usa uClibc e não tem as bibliotecas do musl.

## Ordem usada

1. `g7bridge.py` em um console; login `root` no Telnet.
2. `upload.py` do `g7dump`; teste servindo o próprio binário e comparando com o original.
3. `fetch.py` de `mtd0` duas vezes e de `mtd1`–`mtd5`; conferir que cada partição é igual ao trecho correspondente de `mtd0`; montar `firmware.bin` = `mtd2` + `mtd3`.
4. `upload.py` do `g7flash`, com a mesma verificação.
5. `flashrun.py IMAGEM PORTA` (ensaio). Só seguir se tudo conferir.
6. `flashrun.py IMAGEM PORTA --write` e aguardar o OpenWrt em 192.168.1.1.
