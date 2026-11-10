# Histórico detalhado da tentativa de substituir a MitraStar

## 1. Objetivo e resultado

O objetivo era substituir o equipamento óptico da Vivo, uma MitraStar GPT-2741GNAC/GPT-2741GNAC-N1, por um TP-Link XZ000-G7 e manter um Huawei WiFi Mesh 7 como roteador doméstico.

A substituição não produziu uma conexão de internet funcional. O G7 pôde ser acessado e configurado pelo terminal, mas não há sucesso documentado de serviço de internet por ele. A solução operacional adotada foi manter a MitraStar em bridge, ligada ao Huawei.

O histórico tem duas fases: a MitraStar já estava em bridge em agosto de 2025; a investigação aprofundada pelo terminal do G7 foi retomada em fevereiro de 2026. Em março de 2026, a topologia com bridge foi novamente confirmada.

## 2. Comparação inicial entre G3 e G7 — fevereiro de 2025

Em 12/02/2025, a pergunta foi sobre a diferença entre os modelos GPON TP-Link G3 e G7. A resposta recebida descreveu genericamente o G7 como mais robusto, com possíveis interfaces adicionais e recursos como Wi-Fi ou VoIP, sem demonstrar as especificações exatas.

Essa resposta não era uma comparação técnica confiável. A ficha oficial do XZ000-G7 descreve uma porta PON e uma porta Ethernet Gigabit. A decisão de uso precisava considerar compatibilidade com a rede, e não uma suposta superioridade geral deduzida do nome G7.

## 3. Substituir a ONU da operadora — julho de 2025

Em 03/07/2025, o usuário perguntou por que a TP-Link vendia o terminal se não recomendava substituir diretamente a ONU da provedora.

A discussão tratou de identificação por serial/credenciais, aceitação pela OLT, firmware e uso do equipamento original em bridge. O ponto útil era que suportar o padrão GPON não demonstrava funcionamento naquela instalação específica.

Entretanto, a resposta generalizou as limitações de grandes operadoras e apresentou algumas condições como proibições técnicas quase universais, sem identificar a central, seu perfil ou seu comportamento real. Não há nessa conversa um teste conclusivo da conexão.

## 4. MitraStar em bridge e conta de banda larga — agosto de 2025

Em 28/08/2025, o usuário informou que havia conectado o roteador próprio ao equipamento da Vivo, já em bridge, e perguntou onde encontrar a conta de banda larga solicitada na configuração.

A dificuldade era distinguir a conta PPPoE de outras informações: senha do Wi-Fi, senha administrativa, número de série e credenciais GPON. Uma imagem da etiqueta foi enviada. A resposta identificou dados de administração e Wi-Fi, sem demonstrar a conta PPPoE.

Foram sugeridas credenciais genéricas e consulta à configuração WAN anterior ou ao suporte. Não há confirmação recuperada de quais credenciais foram utilizadas com sucesso. Houve inconsistência nas orientações: inicialmente a etiqueta foi indicada como possível fonte; depois ficou claro que a etiqueta enviada não continha aquela conta.

O relato confirma a topologia em bridge, mas não registra o resultado imediato de cada tentativa de autenticação no Huawei.

## 5. Reaproveitamento do G7 — agosto de 2025

Em 29/08/2025, foi discutido transportar uma rede doméstica por fibra entre dois pontos, usando equipamentos nas extremidades.

As respostas confundiram ONU GPON com conversor de mídia, alternando entre aceitar e negar o arranjo. Também criaram uma distinção inadequadamente explicada entre transportar rede local e transportar internet. O usuário apontou a contradição entre a proposta inicial e as respostas seguintes.

Não há topologia validada ou teste bem-sucedido documentado para esse reaproveitamento. Essa discussão não resolveu a substituição nem demonstrou um uso alternativo funcional do aparelho.

## 6. Acesso ao terminal e identidade local — fevereiro de 2026

Em 03/02/2026, uma saída de `gccli sys show` confirma acesso como `root` ao XZ000-G7. Os seguintes valores estavam presentes:

| Campo | Valor recuperado |
|---|---|
| Model Name | GPT-2741GNAC-N1 |
| EquipmentId | GPT-2741GNAC-N1 |
| Vendor / Vendor Id | MSTC |
| Model Id | MSTC |
| OUI, como rotulado pelo programa | MSTC |
| Hardware Version | V1.0 |
| Software Version | V0.1-3.0.0 |
| Boot Version | V1.2.3(09/27/2023-15:11:04) |
| Area / Province | BR / SP |
| Sys Pon Mode | AUTO |
| Sys Pon Type | HGU |
| SN do sistema | 1234567890 |
| GPON SN | Preenchido, com prefixo MSTC; identificador completo omitido |
| GPON Password | Preenchida; valor omitido |
| LOID Name / Password | Vazios |
| Mfg Mode | 1 |

Os campos de identidade local já reproduziam MitraStar/MSTC. Isso mostra que a tentativa avançou além de conectar fisicamente o equipamento. Porém, os valores locais não demonstram que a rede aceitou todos os atributos, que o firmware reproduzia o comportamento da MitraStar ou que o serviço foi provisionado.

## 7. SN do sistema versus GPON SN

O usuário precisou esclarecer que sua pergunta era sobre `SN: 1234567890`, não sobre o GPON SN. Os campos eram distintos na saída.

A resposta sugeriu alterar o serial do sistema, salvar e reiniciar, mas não há registro recuperado de aceitação do comando, persistência da alteração ou efeito sobre a conexão.

Também foi afirmado que a Vivo ignorava esse campo e que ele não influenciava OMCI. Os trechos disponíveis não sustentam essa exclusão absoluta. O que ficou documentado foi a investigação de uma possível identidade incompleta; sua relevância não foi estabelecida.

## 8. Diagnóstico discutido: provisionamento OMCI

As respostas antigas fizeram referências repetidas a estado O5, OMCI respondendo parcialmente, ausência de TCONT/GEM e informações WAN vazias. Citaram duas mensagens:

```text
Tcont is null
FlowOperation UNI is NULL
```

Essas linhas foram recuperadas nas respostas antigas, não em um log original completo. A hipótese central era que o equipamento avançava no registro GPON, mas o serviço não era montado corretamente.

A documentação preserva essa hipótese sem transformá-la em causa comprovada. Não há evidência integral para decidir entre incompatibilidade do firmware/perfil, configuração incompleta ou outro problema de provisionamento.

As respostas extrapolaram ao declarar “bloqueio 100% de rede/controle”, excluir SN/MAC/modelo/vendor de modo absoluto e afirmar que o setup estava tecnicamente correto. Não foi apresentado um conjunto de testes suficiente para encerrar essas alternativas.

O termo “O5 fake” também apareceu. Aqui ele é apenas o rótulo usado nas conversas para descrever registro sem internet funcional; não prova que a operadora simulava deliberadamente um estado de conexão.

## 9. Teste de VLAN e perda de gerenciamento

Em 03/02/2026, o usuário pediu ajuda para alterar a VLAN pelo terminal. A resposta descreveu `br0` contendo `eth0.0` e `pon0.0`, afirmou que a VLAN era zero e que a bridge era criada por OMCI conforme o perfil esperado pelo provedor.

Não foi recuperada uma verificação independente que comprovasse a VLAN de serviço ou a aceitação dessa configuração. A interpretação foi apresentada como certeza cedo demais.

A orientação iniciou o teste com a desativação de `br0`. O usuário informou que, após executar `ifconfig br0 down`, a conexão caiu. Em seguida, a tentativa de Telnet a `192.168.1.1` falhou.

**Esse comando é citado apenas para documentar o incidente, não como procedimento recomendado.**

A orientação desativou o caminho de gerenciamento usado pela própria sessão, sem assegurar previamente outra forma de acesso ou recuperação. A resposta posterior descreveu a queda como esperada; esse efeito deveria ter sido considerado antes do teste.

## 10. Recuperação do acesso

Foram sugeridos reinício, conexão direta ao computador, IPv4 manual, inicialização sem fibra e novos testes de ping/Telnet. Reset físico e acesso serial foram citados como alternativas.

O usuário depois confirmou que o acesso voltou. Não há indicação de qual medida resolveu. Também não há confirmação de internet funcional pelo G7 nesse momento.

A resposta tratou a recuperação como se tudo estivesse funcionando corretamente. Isso excedia a evidência: o retorno confirmado era do gerenciamento. Garantias de recuperação total, preservação de serial após reset ou funcionamento absoluto de acesso serial não foram demonstradas.

## 11. Outras limitações das propostas de VLAN

Foram propostas interfaces como `eth0.100` e `pon0.100`, além de uma bridge alternativa `br1`. Não há resultado recuperado que confirme criação ou funcionamento dessas interfaces.

Uma orientação sugeriu que falhar ao ativar `eth0.100` com “No such device” demonstraria falta de suporte do kernel a VLAN. Essa interpretação era inadequada: criar uma interface VLAN é diferente de ativar uma interface já existente. A ausência de um dispositivo com aquele nome não demonstra ausência de suporte. A referência do Linux em [fontes](fontes.md) documenta essa distinção.

As respostas alternavam entre declarar a configuração atual correta, dizer que mudanças seriam sobrescritas pela OLT e sugerir alterações locais. O resultado concreto foi a queda temporária de gerenciamento; não a identificação de uma VLAN funcional.

## 12. Investigação de MAC e repetição do risco

Após recuperar o acesso, o usuário perguntou sobre alterar o MAC pelo terminal. A resposta propôs desativar `eth0.0` e garantiu que o Telnet não cairia porque a sessão já estava aberta.

Essa garantia era inadequada: estar conectado não assegura continuidade se o caminho de rede for desativado. A resposta ainda voltou a propor desativar `br0`, apesar de ter acabado de estabelecer que isso não deveria ser repetido.

Não há confirmação de execução dessa sequência, de nova queda ou de sucesso por alteração de MAC. O problema documentado é a contradição entre as orientações e a ausência de resultado validado.

## 13. Declaração de conformidade

Uma imagem de declaração de conformidade foi discutida em duas conversas. As respostas identificaram MitraStar, GPT-2741GNAC e o firmware de referência `ES_g5.9_110WVI0b25_1`.

O documento não forneceu uma solução operacional: não demonstrou comandos utilizáveis, firmware compatível para instalar no G7 nem correção do provisionamento.

Em uma resposta, o foco se perdeu e passou a ser habilitar gccli na própria MitraStar, embora a investigação envolvesse o TP-Link. Outra resposta afirmou que o documento confirmava exatamente a identidade esperada pela OLT; isso não foi demonstrado. Identificar um modelo não revela o perfil efetivamente aplicado à conexão.

## 14. Cadastro por técnico e mudança futura de equipamento

Foram discutidas três perguntas: por que um novo equipamento instalado pela Vivo seria aceito; se um técnico poderia aprovar o G7; e se uma mudança do modelo fornecido pela operadora poderia tornar o G7 compatível.

As respostas falaram em cadastro de serial, associação de perfil e comportamento OMCI. Entretanto, garantiram que um técnico poderia fazer o G7 funcionar e atribuíram o impedimento essencialmente a política e auditoria.

Não há atendimento, cadastro, acesso à OLT ou teste após troca documentado. A existência de um perfil compatível e a possibilidade de um técnico aplicá-lo permaneceram em aberto. Mudanças futuras da rede foram apenas hipóteses, sem resultado observado.

## 15. Desfecho e dificuldades posteriores

Em 02/03/2026, ao perguntar sobre NAT do Xbox, o usuário confirmou Huawei Mesh 7 com o equipamento da Vivo em bridge. Em 10/11/2026, confirmou novamente que a substituição não deu certo e que precisou manter a MitraStar nesse papel.

Conversas posteriores sobre DNS personalizado e limite de regras NAT descrevem dificuldades da rede com o Huawei. Não estabelecem que a MitraStar em bridge estivesse defeituosa nem explicam o fracasso da substituição pelo G7.

## 16. Conclusão técnica documentável

Houve acesso administrativo e configuração de identidade no G7, mas não serviço de internet funcional documentado. A hipótese principal foi dificuldade de provisionamento/compatibilidade OMCI; a causa precisa ficou sem fechamento por falta dos registros completos.

Além do obstáculo técnico, o processo de diagnóstico teve falhas: hipóteses tratadas como certezas, comandos propostos sem validar disponibilidade, confusão entre camadas de identificação e uma alteração que interrompeu o próprio acesso remoto. A recuperação desse acesso não resolveu a substituição.

A MitraStar em bridge foi a solução operacional adotada. O histórico não sustenta atribuir todo o fracasso exclusivamente à operadora nem afirmar que bastaria alterar um campo específico para o G7 funcionar.
