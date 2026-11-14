# Adrenaline: casos semelhantes e contornos relatados

Consulta em 13/11/2026. **Pesquisa documental; nenhum teste novo executado no equipamento deste projeto.**

## Cobertura e critério

Foi feita uma varredura textual de todas as **377 páginas**, totalizando **9.423 mensagens únicas**, desde 24/12/2022 até a mensagem de 13/11/2026 às 01:50:34 (-03:00). A última página não oferecia próxima página. Também foi consultada a [planilha incorporada ao primeiro post](https://docs.google.com/spreadsheets/d/1fcOsBXCNeWBBw46egOoshNc9m1pTuKZjpqREstnDuL8/preview).

A seleção cruzou modelo original, XZ000-G7, O5 sem PPPoE, comandos gccli/gc_omcicli, OMCI, UNI/VEIP, TCONT, VLAN e respostas posteriores dos autores. Citações de mensagens antigas foram diferenciadas do texto escrito por quem respondeu. Imagens, vídeos e todos os links externos não foram analisados integralmente; a cobertura de todas as páginas se refere ao texto das mensagens.

“Sucesso relatado” significa que o autor afirmou ter conectado/navegado, não validação independente nem garantia de compatibilidade. Uma linha na planilha sem procedimento tem força menor. A ausência de desfecho no tópico não prova que o autor nunca resolveu fora dele.

Referência deste projeto: Vivo, MitraStar GPT-2741GNAC-N1 em bridge, Huawei WiFi Mesh 7 e tentativa com TP-Link XZ000-G7. As mensagens históricas sobre TCONT/UNI ainda precisam ser confrontadas com logs completos.

## Mesmo MitraStar e falha anterior com G7

| Autor e páginas | Situação comparável | Desfecho relatado |
|---|---|---|
| GibaDek — 212, 214 e 220 | Mesmo GPT-2741GNAC-N1, Vivo 1, Huawei AX3; G3 e G7 falharam, inclusive com VLAN obtida na MitraStar | VSOL V2802RH. Primeiro discou pelo PC; depois o firmware 1.9V, com ajustes de encaminhamento FwdOp no boot, permitiu PPPoE pelo AX3 em bridge. [Equipamento original](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077183657), [falha G3/G7](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077192597), [sucesso](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077206612). |
| Feliperas — 253, 258 e 259 | Mesmo GPT-2741GNAC-N1, Vivo 1 no interior paulista; G7 permanecia em O5 sem serviço | Devolveu o G7 e conectou com VSOL V2802RH em bridge + UCG Ultra. Destacou a VLAN encontrada por OMCI. [Configuração funcional](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077320389), [confirmação da troca após falha](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077324284). |
| luiz_pennywise — 284 e 286 | Mesmo GPT-2741GNAC-N1, Vivo 1 na Mooca; G7 com firmware modificado e consulta da MIB 84 vazia | VSOL V2801RD; após permitir edição da versão de hardware, apareceram VLAN e serviço. Usou firmware de outro modelo: é parte do relato, **não uma recomendação de instalação**. [Falha com shell](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077406973), [sucesso](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077409390). |
| Ninjitisu — 321 | Mesmo GPT-2741GNAC-N1; G7 em O5 sem PPPoE tanto com Archer AX50 quanto com UCG Ultra | Reaproveitou um Askey RTF7000 e conectou com o UCG. Pretendia voltar a testar o G7, mas não há nova mensagem dele no corpus consultado. [Relato completo](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077493832). |

Os quatro contornos acima retiraram o G7 da conexão. Não demonstram uma correção do firmware do G7.

## Outros contornos confirmados pelo autor

- **Chipz, páginas 280–281:** Vivo 1, MitraStar GPT-2742GX4X5; G7 com firmware TripleOxygen, gccli, VLAN real e MIBs vazias, sem PPPoE. Conectou depois com **Intelbras R1** e VLAN 3470. [Tentativa](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077400372) e [resultado](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077401699). Ao mudar posteriormente de endereço/operadora, encontrou outro problema; o sucesso anterior não se transfere à nova conexão.
- **LucasHunger, páginas 205–210:** G7, G6 e R1 mostravam VLAN 600, mas sem PPPoE. Investigou VEIP e associações sem obter correção. Depois informou sucesso com **ODI**, firmware V1.0-220923, SN e VLAN transparente. [Falha](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077156132), [hipótese VEIP](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077161702), [desfecho](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077172835).
- **brunocfp, página 172:** mesmo GPT-2741GNAC-N1 na Vivo 1, sem PPPoE apenas com SN/SLID. Relatou sucesso com **Intelbras R1**, incluindo identidade OMCI e configuração de VLAN. Não afirmou nesse post ter tentado G7. [Procedimento e resultado](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077061171).

## Houve solução mantendo o próprio G7?

**Sim, para um sintoma parecido cuja falha estava no roteador.** FaReWeLL, página 247, tinha G7 em O5 na Vivo 2, mas o ER605 não discava. A conexão direta pelo computador funcionava. Após restaurar e reconfigurar o ER605, manteve o G7 com firmware 230927 e conexão funcional, incluindo IPv6. [Relato](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077280390).

Esse caso sustenta comparar clientes PPPoE antes de responsabilizar a ONU. Não sustenta restaurar o Huawei deste projeto nem demonstra correção de TCONT/UNI/VEIP ausentes.

A planilha contém duas declarações de **G7 substituindo exatamente GPT-2741GNAC-N1 na Vivo 1**: maestro.wagner (15/03/2024) e Gamer0129 (09/01/2025). Ambas sem um procedimento verificável ligado ao registro. Portanto, o modelo original não permite declarar incompatibilidade universal, mas esses registros não fornecem uma receita para este caso.

Não foi localizada demonstração textual de correção, **no próprio G7**, da combinação de falha OMCI sugerida pelo histórico deste projeto.

## Pistas técnicas úteis ao plano

### 1. Identidade no gccli não basta para comprovar identidade OMCI

Rodrigo Magna, página 196, mostrou um G7 com identificação Nokia em gccli, enquanto a ME 257 ainda continha bytes iniciais correspondentes a XZ000-. A tentativa de alterar OMCC para 0x80 não mudou o valor lido, que permaneceu a0. As strings de software/boot são semelhantes às do nosso histórico. Era outro provedor e outra ONT original. [Saídas publicadas](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077137619).

**Inferência para este projeto:** comparar configuração, MIB local e, se disponível, mensagem OMCI; não assumir que mudar MSTC/MitraStar no shell mudou o anúncio à OLT. A leitura da MIB também não substitui captura da transação enviada.

### 2. Regra salva pode perder efeito depois do provisionamento

stargazer1, página 376, relatou sucesso na Vivo com **FS GPON-SFP-ONTMACI, Galachip GC1601, firmware V1.2.4**. Após reinício, PADI não avançava para a GEM. Alternar o modo VLAN da porta e retornar ao transparente, depois do provisionamento, restabelecia PPPoE; clicar Apply sem mudança não bastava. Apresentou diferenças de regra e contadores. [Relato de 28/10/2026](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077705929).

**Hipótese para teste:** verificar no G7 regras efetivas e contadores após O5. É outro produto/firmware; não presume os mesmos menus, arquivos ou comandos, nem autoriza usar sua imagem no G7.

### 3. Versões de identidade podem mudar o provisionamento

andersonviudes, página 32, substituiu o mesmo GPT-2741GNAC-N1 por ODI. Copiar as versões exibidas pelo equipamento original não trouxe VLANs; mudar as versões OMCI para GG-11000-C003 e o hardware para GG-GAPL100v02 trouxe a VLAN e permitiu internet. [Relato](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1076577432).

noobs415, página 274, reproduziu a sequência de MIB vazia → alteração desses campos → VLAN disponível em VSOL, saindo de um Askey. Recolocar essa VLAN no G7 continuou sem resolver. [Relato](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077386253).

Esses valores são identidades usadas por terceiros, não builds de firmware nem valores confirmados da MitraStar deste projeto. Sua transferência ao G7 continua não verificada.

### 4. Logs precisam ser interpretados pelo fluxo afetado

maurimojr, página 333, continuou sem serviço no G7 mesmo com shell liberado. Publicou uma mensagem sobre VEIP/IGMP não suportado e atribuiu a ela a falha. [Log e interpretação do autor](https://forum.adrenaline.com.br/threads/eliminar-ou-trocar-onu-ont-hgu-da-operadora-provedor.699243/post-1077564953).

O trecho trata de multicast/IGMP; isoladamente não demonstra a causa da falha de internet unicast. Não foi encontrada no texto consultado a dupla exata de erros Tcont is null e FlowOperation UNI is NULL do histórico. A semelhança não substitui a captura local completa.

## Consequências para este projeto

1. Conferir identidade na MIB antes de novas alterações de strings.
2. Distinguir ausência de objetos de serviço de objetos presentes com encaminhamento ineficaz.
3. Comparar regras/contadores após O5, após reaplicação controlada de configuração suportada e após reinício, sem acumular mudanças.
4. Manter a comparação funcional MitraStar → G7 → MitraStar e validar o cliente PPPoE.
5. Tratar VSOL, R1, ODI e Askey como alternativas com relatos documentados; nenhuma possui garantia para a OLT/perfil desta conexão.
6. Não registrar resultados dos outros como testes próprios em tentativas.md.

As alterações propostas estão incorporadas ao [plano de investigação](../plano/plano-2026-11-10.md). A pesquisa de [firmware alternativo](firmware-alternativo.md) permanece um caminho separado.
