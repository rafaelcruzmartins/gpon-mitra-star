# MitraStar, TP-Link G7 e Vivo Fibra: histórico da substituição

Registro da tentativa de substituir a MitraStar GPT-2741GNAC-N1 pelo TP-Link XZ000-G7, mantendo um Huawei WiFi Mesh 7 como roteador doméstico.

**Resultado: a substituição não produziu uma conexão de internet funcional. A solução adotada foi manter a MitraStar em bridge.**

Este repositório documenta o que foi tentado, os resultados observados, os problemas do diagnóstico e as dúvidas ainda abertas. Não é um tutorial validado de substituição de ONU.

## Topologias

Pretendida:

```text
Fibra da Vivo → TP-Link XZ000-G7 → Huawei WiFi Mesh 7 → dispositivos
```

Adotada:

```text
Fibra da Vivo → MitraStar em bridge → Huawei WiFi Mesh 7 → dispositivos
```

## Documentação

- [Plano de investigação e testes controlados](plano/plano-2026-11-10.md)
- [Histórico detalhado e análise das tentativas](docs/historico.md)
- [Matriz de tentativas e resultados](docs/tentativas.md)
- [Fontes e limites da reconstrução](docs/fontes.md)
- [Firmware alternativo: levantamento e limites](docs/firmware-alternativo.md)
- [OpenWrt no G7 sem adaptador serial](docs/openwrt-sem-adaptador-serial.md) e as [ferramentas usadas](ferramentas/README.md)

## Principais achados

- O acesso administrativo ao G7 funcionava: há saída de `gccli sys show` executada como `root`.
- Vários campos do G7 estavam configurados com a identidade MitraStar/MSTC.
- As respostas da época apontavam para dificuldades de provisionamento OMCI, citando TCONT/GEM ausentes. Os logs completos dessas falhas não foram recuperados.
- Durante um teste de VLAN, desativar `br0` derrubou o Telnet. A perda e a posterior recuperação do acesso foram confirmadas pelo usuário.
- Não há sucesso documentado com alteração de VLAN, MAC, serial do sistema ou cadastro por técnico.
- Algumas orientações antigas eram contraditórias ou apresentavam hipóteses como certezas. Elas são registradas como parte do problema, não como instruções a seguir.
- Existe um port de OpenWrt com pilha OMCI aberta para o G7 V1.0 de 4 MB, com rastreamento OMCI. Em 13/11/2026 ele foi instalado neste G7 sem adaptador serial, pelo Telnet do firmware TripleOxygen, depois de um backup completo e conferido da flash. O aparelho inicia e é gerenciável; o serviço GPON na Vivo ainda não foi testado.
- A MitraStar já estava em bridge em agosto de 2025; os testes pelo terminal foram retomados em fevereiro de 2026. Em março de 2026, o uso em bridge foi novamente confirmado.

## Como interpretar as evidências

| Classificação | Significado |
|---|---|
| Confirmado | Relato direto do usuário ou saída de terminal recuperada |
| Diagnóstico discutido | Interpretação presente nas respostas antigas, sem evidência integral disponível |
| Proposto, não confirmado | Procedimento sugerido sem registro recuperado de execução ou resultado |
| Em aberto | Informação insuficiente para concluir |

A documentação foi consolidada em 10/11/2026, com datas no fuso de São Paulo. Não afirma abranger todas as mensagens originais: algumas conversas só puderam ser recuperadas parcialmente.

Senhas, MACs e identificadores completos da conexão foram omitidos. Os links privados das conversas também não são necessários para compreender o relato público.
