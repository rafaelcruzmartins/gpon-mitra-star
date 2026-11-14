# OpenWrt no G7 sem adaptador serial

Executado em 13/11/2026 neste aparelho (XZ000-G7 BR V1.0, 4 MB). **Resultado: o OpenWrt do [`openwrt-galachip`](https://github.com/AliLostInTheDark/openwrt-galachip) foi gravado pelo Telnet do firmware TripleOxygen e iniciou da flash.** O serviço GPON ainda não foi testado com a fibra.

O caminho oficial do projeto exige UART: boot em RAM pelo U-Boot, backup e `sysupgrade`. Aqui não havia adaptador serial, então o backup e a gravação foram feitos a partir do sistema de fábrica em execução, usando ferramentas próprias que estão em [`ferramentas/`](../ferramentas/README.md).

**Isto não é o procedimento recomendado pelo projeto.** Funcionou uma vez, nesta unidade. Se algo falha no meio da gravação, a recuperação exige a serial (ou um programador SPI), e o aparelho fica parado até lá. O método foi escolhido aceitando esse risco porque o G7 não estava em uso.

## Pré-requisitos verificados

| Item | Como foi confirmado |
|---|---|
| Revisão V1.0 de 4 MB | `/proc/mtd` com `whole` 4 MiB, `uboot`, `kernel` 0x170000, `rootfs` 0x240000, `misc`, `hwconfig`; flash com setores de 4 KiB |
| Shell root | Firmware TripleOxygen v1.0 (build 230927), Telnet `root`/`root` documentado pela [wiki da TripleOxygen](https://www.tripleoxygen.net/wiki/ont/tplink/xz000-g7-br-v1) |
| Imagem compatível com o U-Boot de fábrica | A região de firmware (0x20000) começa com o mesmo cabeçalho `03000003` que a imagem `sysupgrade` do projeto |
| Imagem do OpenWrt | Compilada localmente, commit `2d70e38` do branch `xz000-g7-wip`; ver [compilação](#compilação) |

## Obstáculos do firmware de fábrica

- O BusyBox do G7 não tem `dd`, `nc`, `od`, `base64`, `md5sum`, `mtd` nem `flash_erase`. Tem `cat`, `printf`, `tftp`, `wget` e `devmem`.
- O PC usa a Ethernet como rede "Pública" no Windows, que bloqueia conexões de entrada. Assim, `tftp` e `wget` do G7 para o PC não funcionam sem mexer no firewall. Toda conexão foi aberta pelo PC em direção ao G7.
- O sistema em execução lê o próprio `rootfs` (squashfs) da flash sob demanda. Sobrescrever essa partição com ele rodando pode derrubar qualquer processo que precise de uma página que não esteja em memória.
- O watchdog de hardware está ativo (`WDCR=0x00000007` em `0x9080000c`). Se o processo que o alimenta cair durante a gravação, o aparelho reinicia com o firmware pela metade.

## Como foi feito

1. **Ponte Telnet.** O usuário fez o login uma vez em uma ponte local (`g7bridge.py`); os comandos seguintes passaram por ela, com a saída delimitada por marcador e código de retorno.
2. **Envio de binários.** Programas estáticos para ARMv5TE, compilados com o toolchain do OpenWrt, foram enviados para `/var/tmp` (RAM) em blocos de `printf` com escapes octais (`upload.py`). Nada foi gravado na flash nesta etapa.
3. **Backup.** O `g7dump` abre uma partição só para leitura e a serve em uma porta TCP. O PC conecta, baixa e compara o CRC32 calculado no G7 com o local (`fetch.py`). Resultado:
   - `mtd0` (flash inteira) lido duas vezes, cópias idênticas;
   - `mtd1` a `mtd5` lidos separadamente, cada um igual ao trecho correspondente de `mtd0`;
   - `firmware.bin` montado com `kernel` + `rootfs` (3.866.624 bytes), o arquivo que o procedimento de recuperação do projeto espera.

   Os arquivos ficam em local privado: contêm calibração óptica e identificadores.
4. **Ensaio.** O `g7flash` recebe a imagem por TCP direto na memória e confere tamanho, CRC32 informado pelo PC, cabeçalho `03000003`, nomes e tamanhos de `mtd2`/`mtd3` em `/proc/mtd`, permissão de escrita e estado do watchdog. Nada é gravado sem `--write`.
5. **Gravação.** Com `--write`, o `g7flash`:
   - vira daemon, ignora `SIGHUP`/`SIGTERM` e escreve o log em `/var/tmp`;
   - trava a própria memória (`mlockall`); como é estático e roda da RAM, não depende do `rootfs`;
   - desliga o watchdog escrevendo 0 em `0x9080000c`, o mesmo que o U-Boot do projeto faz em `board_init`;
   - apaga, grava e confere cada bloco de 64 KiB de `mtd2` e depois de `mtd3`, com até três tentativas por bloco, preenchendo com `0xff` o espaço após a imagem;
   - reinicia o aparelho.

   Só `/dev/mtd2` e `/dev/mtd3` são abertos para escrita. Juntos, eles cobrem exatamente a partição `firmware` do OpenWrt (0x20000–0x3d0000). `mtd0`, `uboot`, `misc` e `hwconfig` nunca são abertos para escrita. Isso equivale ao que o `sysupgrade` do projeto faz com a partição `firmware`.

Neste aparelho, a sessão Telnet sobreviveu até o fim da gravação, que levou menos de um minuto. Cerca de 70 segundos depois do reinício, o G7 respondeu com SSH e LuCI. O OpenWrt reconheceu o layout `u-boot`, `firmware` (`kernel`, `rootfs`, `rootfs_data`), `misc` e `hwconfig`, e formatou a área de configuração no primeiro boot.

## Estado após a instalação

| Item | Observado |
|---|---|
| Sistema | `OpenWrt SNAPSHOT r37800-2d70e38661`, Linux 6.18.55 |
| Gerenciamento | 192.168.1.1, LuCI e SSH; sem DHCP; PC em IP fixo na mesma rede |
| GPON sem fibra | `omcid` em execução, estado `O1`, `los=1`; esperado com a fibra desconectada |
| Identidade | Serial e senha GPON, vendor `MSTC` e equipment ID `GPT-2741GNAC-N1` aplicados por `uci`; versões de hardware/software e tipo de ONU mantidos no padrão do projeto (`XZ000-G7 v1.0`, SFU) |

O G7 continua sendo uma bridge: PPPoE, NAT e DHCP ficam no Huawei. O LuCI pede uma senha de root, porque a interface de gerenciamento fica acessível pela porta LAN.

## Compilação

Ubuntu 24.04 no WSL, seguindo o README do projeto (`feeds update/install`, `tplink_xz000-g7.diffconfig`, `make`). Uma diferença foi necessária: o projeto não fixa a versão do feed LuCI, e um commit do LuCI de 10/11/2026 alterou `interfaces.js`, o que fez o patch `link-display.patch` falhar na montagem da imagem. O feed foi fixado em `4a3d16e` (31/10/2026), anterior ao commit do projeto usado:

```text
src-git luci https://git.openwrt.org/project/luci.git^4a3d16e914564ccf239802ad8dd57026c3860dd0
```

Imagem gravada: `openwrt-galachip-gc1601-tplink_xz000-g7-squashfs-sysupgrade.bin`, 3.408.146 bytes, CRC32 `24e017d1`. A build é local e não foi publicada.

## Recuperação

- **OpenWrt iniciando:** voltar ao firmware de fábrica gravando o `firmware.bin` do backup na partição `firmware` pelo próprio OpenWrt. Não precisa de serial.
- **OpenWrt não inicia:** UART 3,3 V e o procedimento de recuperação do README do projeto, com o mesmo `firmware.bin` (3.866.624 bytes) via TFTP no U-Boot de fábrica.
- **`misc` ou `hwconfig` danificados:** só o backup da própria unidade e um programador SPI. O método acima não abre essas partições para escrita.

## Limites

- Uma execução, em uma unidade. Outro firmware de fábrica, outra build do OpenWrt ou outra carga de memória podem se comportar diferente durante a gravação.
- Não houve boot em RAM antes de gravar. Se a imagem não iniciasse, o aparelho precisaria de serial.
- Só para a V1.0 de 4 MB com o layout acima. A V1.20 de 8 MB tem outro layout e não é suportada pelo projeto.
- Instalar o OpenWrt muda a pilha OMCI, o caminho de dados e a identidade anunciada ao mesmo tempo. Resultados com ele não explicam a falha do firmware TP-Link; ver [firmware alternativo](firmware-alternativo.md).
