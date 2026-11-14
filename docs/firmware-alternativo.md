# Firmware alternativo para o XZ000-G7: levantamento e limites

Levantamento feito em 13/11/2026. **Status: pesquisa documental.** Depois deste levantamento, no mesmo dia, o `openwrt-galachip` foi instalado neste G7 sem adaptador serial; ver [OpenWrt sem adaptador serial](openwrt-sem-adaptador-serial.md). O teste com a fibra da Vivo ainda não foi feito.

O histórico não fecha a causa da falha porque faltam os logs OMCI completos (ver [histórico](historico.md), seção 8, e [plano](../plano/plano-2026-11-10.md), etapa 2.1). O firmware de fábrica usa um daemon OMCI fechado (`gc_omcid`), com pouca visibilidade. Este documento registra os firmwares e as ferramentas da comunidade que poderiam ampliar essa visibilidade, o que foi possível verificar em suas fontes e o que continua sem demonstração para a conexão Vivo.

## Projetos encontrados

| Projeto | O que é | Verificado em 13/11/2026 | Relevância para este caso |
|---|---|---|---|
| [TripleOxygen (O3 Labs) — XZ000-G7 (BR) V1.0](https://www.tripleoxygen.net/wiki/ont/tplink/xz000-g7-br-v1) | Página sobre firmware de fábrica modificado para liberar shell | Página acessível; descreve a V1.0 com flash de 4 MB (EN25QH32B), Linux 3.16 e o mapa de partições de fábrica | Provável origem do acesso root já observado; continua usando a pilha OMCI fechada |
| [`AliLostInTheDark/openwrt-galachip`](https://github.com/AliLostInTheDark/openwrt-galachip), branch `xz000-g7-wip` | OpenWrt com Linux 6.18 e pilha GPON/OMCI aberta para o XZ000-G7 de 4 MB | Repositório e branch existem; último commit consultado `2d70e38`, de 08/11/2026 | Oferece rastreamento OMCI e registro do que a OLT provisionou, exatamente a lacuna do histórico |
| [`exsheal/tplink-xz000-g7-v1.2-research`](https://github.com/exsheal/tplink-xz000-g7-v1.2-research) | Engenharia reversa da revisão V1.20 de 8 MB | Repositório existe | Não se aplica à V1.0; útil só se a etiqueta do aparelho indicar V1.20 |
| [`Thaad-QR/fs-gpon-sfp-editable-identity`](https://github.com/Thaad-QR/fs-gpon-sfp-editable-identity) | Mod para um módulo SFP com o mesmo SoC GC1601 | Repositório existe | Referência sobre persistência de identidade via `gccli`; a imagem pronta não é para o G7 |
| [Hack-GPON, issue #424](https://github.com/hack-gpon/hack-gpon.github.io/issues/424) | Discussão sobre chips Faraday/Galachip | Página acessível | Contexto; firmware de outro fabricante não deve ser gravado no G7 |

Também foi citado o pacote GPL do XZ000-G7 publicado pela TP-Link. Ele não foi baixado nem inspecionado neste levantamento.

## O que o `openwrt-galachip` declara

Segundo o README do projeto, consultado no commit acima:

- Suporta apenas a placa com **32 MiB de RAM e 4 MiB de flash SPI NOR**. As variantes BR/TR **V1.20 de 8 MiB não são cobertas**.
- O aparelho funciona como bridge; PPPoE, NAT e DHCP ficam com o roteador. Isso é compatível com a topologia pretendida com o Huawei.
- GPON declarado como funcional: ativação, ranging, criptografia, provisionamento OMCI e recuperação após queda da fibra.
- OLTs testadas: **Nokia ISAM e Airtel, ambas na Índia**. Não há relato de teste na Vivo ou em outra operadora brasileira.
- Clonagem de identidade (serial, vendor ID, versões, equipment ID, SLID/LOID), escolha de perfil de OLT e modos de VLAN transparente, marcado ou controlado pela OLT.
- Diagnóstico: `omcictl status`, estado GPON em sysfs, linha de log `OLT after …` a cada reconexão e captura OMCI sob demanda (`kill -USR1 $(pidof omcid)` seguido de `omci-monitor -r`). O próprio projeto avisa que a captura contém identificadores e deve ficar privada.
- Não implementa download de software por OMCI, VoIP, IGMP snooping nem TR-069.
- Firmware sem assinatura: U-Boot, SPL e `sysupgrade` verificam integridade (tag, tamanho, CRC), não autenticidade (`SECURITY_MODEL.md`).

Essas são afirmações do projeto, não resultados observados nesta conexão. Mesmo com OMCI aberto, a OLT da Vivo pode exigir perfil, versão ou entidades que o projeto ainda não reproduz.

## Revisão de um guia de instalação recebido

Foi recebido um guia, gerado com auxílio de IA, para instalar o `openwrt-galachip` usando um Arduino Nano como adaptador serial. Comparado com o README do projeto e com a página da TripleOxygen, ele contém erros que poderiam danificar a recuperação do aparelho. **As instruções do projeto prevalecem; o guia não deve ser seguido como está.**

| Ponto do guia | Fonte consultada | Consequência |
|---|---|---|
| Backup por Telnet nomeando `mtd0` como U-Boot e `mtd1` como firmware | No firmware de fábrica, `mtd0` é a flash inteira (`whole`), `mtd1` é o U-Boot, depois vêm `kernel`, `rootfs`, `misc` e `hwconfig` | Arquivos rotulados errado; a restauração descrita gravaria o backup do U-Boot na região de firmware |
| Restauração com `mtd -r write mtd1_backup… firmware` | O projeto exige o `firmware.bin` bruto do próprio aparelho, com exatamente 3866624 bytes, e confere com `cmp.b` | Restauração incorreta e sem verificação |
| Boot em RAM com `tftp 0x80008000` e `bootm` | O projeto usa `mw.l 9080000c 0` (desliga o watchdog), `tftpboot 41000000` e `bootz` com a imagem `initramfs-zImage` | Comandos do guia não correspondem a esta plataforma |
| PC em `192.168.1.100` | O projeto usa `192.168.1.10/24` | Diferença simples, mas indica que o guia não deriva da documentação do projeto |
| `cp tplink_xz000-g7.diffconfig .config` na raiz | O arquivo está em `target/linux/galachip/` | O comando falharia |
| `sysupgrade -v` | O projeto valida com `sysupgrade -T` e grava com `sysupgrade -n`, sem `-F` | O guia pula a validação da imagem |
| Exemplos de versão de software e “VLAN 10” como valores da Vivo | Nenhuma fonte; o plano já trata esses valores como desconhecidos | Valores não devem ser copiados |
| Afirma que o aparelho “já está confirmado como V1.0” | `Hardware Version: V1.0` no `gccli` é um campo configurável, e o histórico mostra campos alterados | A revisão física precisa ser conferida na etiqueta |

O uso do Arduino Nano como ponte USB-serial (RESET ligado ao GND e divisor resistivo no sinal de transmissão) também não foi validado aqui. O projeto pede apenas um adaptador USB-UART de 3,3 V; um adaptador dedicado elimina a dúvida sobre níveis de tensão.

## Como isso se encaixa no plano

O plano mantém a instalação de firmware alternativo fora da sequência inicial. Este levantamento não muda essa decisão: trocar o firmware muda ao mesmo tempo a pilha OMCI, o caminho de dados e a identidade anunciada, então um resultado nele não explica a falha do firmware de fábrica.

Na prática, as pré-condições abaixo foram cumpridas com uma exceção: não havia adaptador serial, e a instalação foi feita pelo Telnet com ferramentas próprias, aceitando que uma falha na gravação deixaria o aparelho parado até haver serial. Detalhes em [OpenWrt sem adaptador serial](openwrt-sem-adaptador-serial.md).

O que muda é a disponibilidade de uma opção posterior com diagnóstico melhor. Ela só faz sentido se as etapas com o firmware atual não produzirem os logs necessários, e com estas condições:

1. Revisão física V1.0 de 4 MB confirmada pela etiqueta e pelo mapa de partições, sem publicar o serial.
2. Firmware de fábrica atual guardado com o procedimento de backup do próprio projeto (as quatro partições e a imagem completa de 4194304 bytes, com SHA-256), fora do aparelho e em local privado.
3. Acesso serial de 3,3 V funcionando antes de qualquer gravação, e procedimento de retorno ao firmware de fábrica lido de ponta a ponta.
4. Primeiro boot somente em RAM, com a fibra desconectada.
5. Identidade copiada da MitraStar registrada na etapa 1 do plano, não de exemplos.
6. Mesma sequência A1 → B → A2 e mesmo registro de teste do plano, anotando a build exata do firmware usada.

Resultado positivo nesse caminho demonstraria funcionamento com aquele firmware, não com o firmware TP-Link. Resultado negativo, com o rastreamento OMCI completo, ainda seria útil: identificaria a primeira operação rejeitada pela OLT.

## Fora do escopo

Não foram avaliados: gravação de U-Boot alternativo, programador SPI externo, firmwares de outros fabricantes com o mesmo SoC, gerador de senha de fábrica e chaves de configuração publicados em repositórios de engenharia reversa. Nenhum deles é necessário para coletar os dados que faltam.
