# Teste com a fibra: G7 com OpenWrt na Vivo

Executado em 14/11/2026. **Resultado: o XZ000-G7 com o OpenWrt do [`openwrt-galachip`](https://github.com/AliLostInTheDark/openwrt-galachip) substituiu a MitraStar. O Huawei WiFi Mesh 7 discou o PPPoE e a rede da casa passou a ter internet.**

**Status: funcionando; reinício controlado confirmado.** Da etapa 6 do [plano](../plano/plano-2026-11-10.md), ainda falta a janela de observação de estabilidade.

Instalação do firmware: [OpenWrt sem adaptador serial](openwrt-sem-adaptador-serial.md).

## Montagem

```text
Fibra Vivo → G7 (OpenWrt) → switch → WAN do Huawei Mesh 7 (PPPoE)
                                 └─→ PC de diagnóstico (192.168.1.10, gerenciamento do G7)
```

O PC acessou o G7 por SSH. Durante o teste, a conexão do próprio PC à internet foi feita pelo roteador de um celular, já que a rede da casa ficava sem internet. A MitraStar ficou desligada e sem alterações.

## Ensaios

Cada ensaio mudou um fator em relação ao anterior, salvo onde indicado. O reprovisionamento foi forçado silenciando o transmissor do G7 por 20 segundos (`tx_hold`); a OLT respondeu com reset e novo upload da MIB. MIB, capturas OMCI e logs de cada ensaio estão guardados em privado.

| Ensaio | Mudança | O que a OLT fez | Resultado |
|---|---|---|---|
| B1 | Identidade clonada da tentativa anterior (serial, senha, vendor `MSTC`, equipment ID `GPT-2741GNAC-N1`); demais campos no padrão | O5 em cerca de 3,5 min; 280 mensagens, nenhuma recusada; criou bridge, porta LAN, VEIP, VLAN e multicast | **Sem GEM de dados**: nenhum mapeador 802.1p (ME 130) nem GEM interworking TP (ME 266); T-CONTs reservados sem nada ligado |
| B2 | Versões OMCI `GG-GAPL100v02` (hardware) e `GG-11000-C003` (software), da [pista 3 do Adrenaline](adrenaline-casos-semelhantes.md) | Valores confirmados na MIB (ONU-G e Software image); mesma sequência | Igual ao B1 |
| B3 | Versões de volta ao padrão; perfil de OLT forçado para Huawei | Upload com entidades privadas da Huawei (209 em vez de 200); 79 `SET` (filas); recusas em entidades privadas não implementadas (351, 353, 370, 65414) | Sem GEM de dados |
| B4 | Anunciar 4 portas Ethernet (`uni_count=4`) | Configurou as 4 portas | Sem GEM de dados |
| B5 | Responder "ok" a entidades sem suporte (`fake_omci=1`) | Nenhuma recusa | Sem GEM de dados |
| B6 | B5 + versões do B2 | Igual | Sem GEM de dados |
| **B7** | **`traffic_management_option=2`** (o valor do firmware de fábrica do G7; o OpenWrt anuncia 0) | **Criou dois serviços completos**: GEM 392 no T-CONT 1 e GEM 136 no T-CONT 2, com mapeadores e filtros de VLAN | O serviço da VLAN 30 entrou no hardware. O da **VLAN 10** foi recusado pelo G7 (`pon0 service table full`) e a OLT o apagou |
| B8 | B7 + tipo de ONU HGU (`onu_type=2`) | Igual ao B7 | Igual ao B7 |
| B9 | VLAN em "Control by OLT" | Sem reprovisionamento | Tráfego do Huawei marcado com VLAN 30. Uma sondagem do G7 recebeu **DHCP com IP privado** nessa VLAN: é gerenciamento, não internet |
| B10 | 1 porta Ethernet (`uni_count=1`), com reinício do G7 | Dois serviços aceitos; **VLAN 10 ↔ GEM 136, bidirecional** no hardware | Sondagem PPPoE do G7 recebeu PADO na VLAN 10 |
| **B11** | **VLAN Tagged 10** (o G7 marca com VLAN 10 o que vem do Huawei) | — | **Huawei discou; internet na casa** |

A mudança para `uni_count=1` só valeu depois de reiniciar o G7. No B10 sem reinício, a OLT continuou vendo 4 portas e a tabela seguiu cheia.

## O que explica a falha anterior

O sintoma do B1 ao B6 coincide com o histórico de fevereiro de 2026 (`Tcont is null`) e com relatos do Adrenaline de G7 "em O5 sem serviço": a OLT conclui o registro e a configuração do lado da casa, mas não cria o caminho de dados.

Neste teste, a condição que destravou o provisionamento foi o **atributo Traffic management option da ME ONU-G**. Com o valor 0, a OLT não criou nenhuma GEM de dados; com 2, criou os dois serviços. A OLT não informou o motivo em nenhuma resposta. É uma correlação reproduzida (B6 → B7, depois B8 e B10), não uma regra documentada da OLT.

O firmware de fábrica do G7 já anuncia o valor 2. Isso indica que a falha de fevereiro tinha outra causa, ou mais de uma. Pelos dados atuais, não é possível dizer qual. O histórico não tem os logs completos daquela época.

O segundo obstáculo foi do próprio OpenWrt: com 4 portas anunciadas, o número de fluxos excedeu a tabela de serviços do hardware, e o serviço de internet foi o que ficou de fora.

## Configuração em uso

| Campo (`uci`, `omci`) | Valor | Necessidade |
|---|---|---|
| `main.serial`, `main.password` | Da clonagem anterior (omitidos) | Autenticação na OLT |
| `main.vendor_id` / `main.equipment_id` | `MSTC` / `GPT-2741GNAC-N1` | Presentes desde o B1; efeito isolado não testado |
| `main.traffic_management_option` | `2` | **Necessário** neste teste (B6 → B7) |
| `main.uni_count` | `1` | **Necessário** neste teste: com 4, o serviço de internet não coube (B7–B10) |
| `vlan.mode` / `vlan.vid` | `tagged` / `10` | **Necessário** com o Huawei discando sem VLAN (B9–B11) |
| `main.olt_profile`, `main.olt_profile_force` | `huawei` | Ativo; necessidade não comprovada |
| `main.fake_omci` | `1` | Ativo; necessidade não comprovada |
| `main.hardware_version` / `main.software_version_0/1` | `GG-GAPL100v02` / `GG-11000-C003` | Ativo; sem efeito no B2; necessidade junto com o B7 não testada |
| `main.onu_type` | `2` (HGU) | Ativo; sem efeito visível no B8 |

As opções "não comprovadas" continuam ligadas porque a configuração completa é a que funcionou. Removê-las exige novos ensaios, um de cada vez, cada um com a internet da casa fora do ar.

Observado no B11: estado O5, potência óptica de recepção −16,2 dBm e de transmissão +1,8 dBm, PPPoE do Huawei estabelecido, navegação confirmada em dois aparelhos.

## Pendências para validar

1. ~~Reinício controlado do G7~~ **Feito em 14/11/2026:** sem nenhuma intervenção, a OLT reprovisionou (63 criações, nenhuma remoção) e, cerca de 80 s após o boot, o hardware tinha `LAN untagged <-> VLAN 10, GEM 136, T-CONT 2, bidirectional` e o serviço de gerenciamento na VLAN 30. O Huawei voltou a discar e a internet foi confirmada pelo Wi-Fi da casa.
2. Observação de pelo menos 30 minutos, registrando quedas e renegociações.
3. Opcional: remover uma a uma as opções não comprovadas, para chegar à configuração mínima.
4. Manter a MitraStar guardada até a validação.

## Limites

- Uma conexão, uma OLT, um dia de teste. Outra OLT ou outro perfil podem exigir outra combinação.
- A identidade do fabricante da OLT é inferida (entidades privadas da Huawei consultadas por ela); a OLT não preencheu a ME OLT-G.
- O identificador do concentrador PPPoE, IPs e identificadores da conexão foram omitidos.
