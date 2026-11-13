# Fontes e limites da reconstrução

## Histórico pessoal consultado

O relato foi reconstruído a partir de conversas pessoais e de uma confirmação do usuário em 10/11/2026. As conversas não constituem documentação pública do fabricante ou da operadora. A recuperação disponibilizou apenas parte de algumas delas, sem continuação acessível.

| Data relevante | Título da conversa | Contribuição |
|---|---|---|
| 12/02/2025 | Diferença GPON TP-Link G3 G7 | Comparação inicial dos equipamentos |
| 03/07/2025 | Trocar ONU por TP-Link | Dúvida sobre substituir o equipamento da operadora |
| 28/08/2025 | Conta banda larga Vivo | MitraStar já em bridge; configuração PPPoE no Huawei |
| 29/08/2025 | Conectar cabo ethernet com gpon | Discussão de reaproveitamento do G7 |
| 03/02/2026 | VLAN ID no GPON | Queda e recuperação do Telnet; propostas de VLAN e MAC |
| 03/02/2026 | Acesso Shell TP-Link G7 | Saída do gccli; identidade; diagnóstico OMCI; hipóteses de cadastro |
| 03/02/2026 | Ajuda gccli Declaração | Discussão de declaração de conformidade |
| 02/03/2026 | Abrir NAT Xbox Huawei | Confirmação posterior de Vivo em bridge com Huawei Mesh 7 |

Outras conversas sobre DNS personalizado e regras NAT no Huawei foram consultadas como contexto posterior. Não demonstram a causa da falha do G7.

## Referências públicas

1. [TP-Link — XZ000-G7: características e especificações](https://service-provider.tp-link.com/tr/gpon/xz000-g7/). Confirma uma porta PON, uma porta Ethernet Gigabit, gerenciamento local Web/Telnet e remoto OMCI. Não comprova compatibilidade com a central específica deste relato.
2. [Linux man-pages — ip-link(8)](https://man7.org/linux/man-pages/man8/ip-link.8.html). Distingue criação de interface virtual de alteração do estado da interface; fundamenta a correção da interpretação dada ao teste com uma interface VLAN inexistente.
3. [TripleOxygen — TP-Link XZ000-G7 (BR) V1.0](https://www.tripleoxygen.net/wiki/ont/tplink/xz000-g7-br-v1). Flash de 4 MB, mapa de partições de fábrica e firmware modificado com shell. Usada para conferir o guia de instalação recebido.
4. [`AliLostInTheDark/openwrt-galachip`, branch `xz000-g7-wip`](https://github.com/AliLostInTheDark/openwrt-galachip/tree/xz000-g7-wip). README, `SECURITY_MODEL.md` e instruções de U-Boot consultados no commit `2d70e38`. Afirmações do projeto, sem teste nesta conexão.

As referências 1 e 2 foram consultadas na consolidação de 10/11/2026; as 3 e 4, em 13/11/2026, para o [levantamento de firmware alternativo](firmware-alternativo.md).

Nenhuma delas foi usada para atribuir ao usuário resultados de testes de terceiros.

## Critérios editoriais

- Relatos diretos e saídas de terminal têm prioridade sobre conclusões de respostas antigas.
- Uma resposta que menciona um log não substitui o log original completo.
- Uma sugestão não comprova execução; execução não comprova persistência; configuração local não comprova aceitação pela rede.
- A ausência de evidência recuperada não prova que um teste nunca aconteceu.
- Não foram publicados senhas, MACs, serial GPON completo, números da conexão ou imagens de etiquetas.
- Termos como “O5 fake”, “bloqueio 100%” e “garantia absoluta” são tratados como linguagem usada no diagnóstico antigo, sem validação independente.
