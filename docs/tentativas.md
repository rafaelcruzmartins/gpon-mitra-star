# Matriz de tentativas e resultados

Esta tabela separa o que foi executado do que foi apenas sugerido. Uma tentativa proposta não deve ser interpretada como teste concluído.

As próximas etapas estão no [plano de investigação](../plano/plano-2026-11-10.md). Os testes desse plano ainda não foram executados e não alteram os resultados históricos desta tabela.

| Etapa | Objetivo | Evidência recuperada | Resultado / limite |
|---|---|---|---|
| Comparação G3 × G7 | Escolher o terminal | Pergunta em fevereiro de 2025 | Resposta genérica, sem comparação técnica comprovada |
| Substituição direta da ONU | Retirar o equipamento da Vivo | Discussão em julho de 2025 e relato posterior de fracasso | Sem conexão funcional documentada pelo G7 |
| MitraStar em bridge | Usar o Huawei como roteador | Relato direto em agosto de 2025 | Topologia adotada, confirmada novamente em março de 2026 |
| Configuração PPPoE no Huawei | Identificar a conta de banda larga | Pergunta e imagem da etiqueta | Credenciais sugeridas; não há confirmação do par efetivamente utilizado |
| Acesso ao shell do G7 | Inspecionar configuração | Prompt como root e `gccli sys show` | Acesso administrativo confirmado |
| Identidade MitraStar no G7 | Reproduzir identificação do equipamento original | Vendor/Model/EquipmentId preenchidos com MSTC/GPT-2741GNAC-N1 | Configuração local confirmada; aceitação completa pela rede não comprovada |
| GPON SN e senha | Identificação/autenticação óptica | Campos preenchidos na saída | Não prova provisionamento ou internet funcional |
| SN do sistema | Investigar `1234567890` | Usuário distinguiu esse campo do GPON SN | Alteração sugerida; execução, persistência e efeito não confirmados |
| Análise OMCI/TCONT/GEM | Explicar ausência de serviço | Respostas antigas citam `Tcont is null` e `FlowOperation UNI is NULL` | Hipótese principal; logs originais completos indisponíveis |
| Alteração de VLAN | Encontrar configuração funcional | Pergunta seguida da execução de `ifconfig br0 down` | Telnet caiu; não houve solução de VLAN documentada |
| Reconexão ao G7 | Recuperar gerenciamento | Tentativa de Telnet falhou; depois o usuário disse que voltou | Recuperação confirmada; procedimento exato não registrado |
| Criação de interfaces/bridge alternativas | Testar outro caminho de VLAN | Sugestões com `eth0.100`, `pon0.100` e `br1` | Sem confirmação de execução ou sucesso |
| Alteração de MAC | Investigar outra identificação | Pergunta após recuperação | Procedimentos sugeridos, com risco de nova queda; resultado não registrado |
| Declaração de conformidade | Encontrar informação útil ao gccli | Imagem discutida em duas conversas | Identificação de modelo; nenhuma solução operacional demonstrada |
| Cadastro por técnico | Fazer a rede aceitar o G7 | Discussão hipotética | Nenhum atendimento ou cadastro documentado |
| Troca futura de ONU/perfil | Melhorar compatibilidade | Discussão hipotética | Nenhum teste após mudança documentado |
| Reaproveitamento em fibra interna | Usar o G7 em outro papel | Discussão em agosto de 2025 | Explicações contraditórias; nenhuma topologia validada |

## Incidente confirmado: perda de Telnet

1. O G7 estava acessível por Telnet.
2. A orientação de teste mandou desativar `br0`.
3. O usuário executou o comando e informou que perdeu a conexão.
4. A tentativa de reconectar a `192.168.1.1` falhou.
5. Foram sugeridas várias formas de recuperação.
6. O usuário informou que o acesso voltou.

O retorno do gerenciamento não demonstra retorno da internet. Não há evidência de dano permanente no episódio recuperado.

## Questões que o histórico não encerra

- Fabricante, modelo e versão da OLT da conexão.
- Estado GPON completo, sua duração e estabilidade durante cada teste.
- Sequência OMCI completa e a primeira operação que falhou.
- Estado real de TCONT/GEM no momento de cada mensagem citada.
- VLAN e tratamento de tags necessários ao serviço e à interface Ethernet.
- Presença de resposta PPPoE e ponto exato de falha na negociação.
- Efeito e persistência de alterações de SN/MAC, caso tenham sido executadas.
- Procedimento específico usado para recuperar o Telnet.
- Existência de um perfil compatível com o G7 na central da Vivo.

Essas lacunas impedem atribuir o fracasso exclusivamente a bloqueio deliberado da operadora, erro de configuração, firmware ou incompatibilidade de perfil.
