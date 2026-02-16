# 📦 Exportação e Versionamento de Dados

Este documento descreve o mecanismo implementado para exportação automática da telemetria armazenada no **ThingsBoard CE** e seu versionamento diário no **GitLab**.

O objetivo é garantir:

- Backup histórico independente do banco PostgreSQL
- Rastreabilidade temporal da telemetria
- Redução do risco de perda de dados
- Reprodutibilidade da infraestrutura

---

# 🎯 Motivação

Embora o ThingsBoard armazene os dados em PostgreSQL, essa abordagem apresenta limitações:

- Dependência de um único banco local
- Risco de corrupção ou falha de hardware
- Dificuldade de versionamento histórico estruturado
- Crescimento contínuo do banco ao longo do tempo

Para mitigar esses fatores, foi implementado um pipeline complementar de exportação diária para arquivos CSV versionados em repositório Git.

Essa abordagem permite:

- Independência parcial do backend
- Versionamento distribuído
- Possibilidade futura de limpeza do banco sem perda histórica

---
## 🗄️ PostgreSQL (ThingsBoard) — papel e estratégia de retenção

O ThingsBoard CE utiliza PostgreSQL como banco de dados principal para:
- configuração de entidades (devices, dashboards, rule chains)
- telemetria (timeseries e “latest telemetry”)

Na Raspberry Pi, o PostgreSQL é tratado como armazenamento **operacional** (curto/médio prazo), pois a telemetria tende a crescer continuamente ao longo do tempo.

### Estratégia adotada neste projeto
- **PostgreSQL (ThingsBoard):** dados operacionais para visualização e processamento contínuo.
- **GitLab (CSV diário):** histórico versionado de longo prazo, independente do banco.
- **Objetivo:** possibilitar limpeza futura do banco do ThingsBoard sem perda do histórico.

### Observação sobre migração (ChirpStack)
Em uma futura arquitetura com ChirpStack autogerenciado, o uso de banco de dados também se mantém (ex.: PostgreSQL), reforçando a importância de uma estratégia explícita de retenção e backup.


# 🔄 Fluxo Implementado

O fluxo atual é o seguinte:

    ThingsBoard  
    ↓  
    Script Bash (consulta API REST)  
    ↓  
    Geração de CSV diário  
    ↓  
    Commit automático  
    ↓  
    Push para GitLab  

A Raspberry Pi atua como executor automático desse processo.

---

# 📁 Estrutura de Diretórios

Os dados exportados são organizados no seguinte formato:

data/
YYYY/
MM/
DD/
em01.csv
em02.csv
raw_json/
raw_em01.json
raw_em02.json

OBS: Caso haja dados apenas em em01 e em02, a configuração atual, que contém o ID dos 10 dispositivos, exporta apenas os dispositivos com telemetria ativa

## Organização adotada

- Estrutura hierárquica por ano/mês/dia
- Separação entre CSV tratado e JSON bruto
- Arquivo individual por dispositivo

Essa organização facilita:

- Auditoria histórica
- Reprocessamento futuro
- Análises externas com Python ou R
- Controle de alterações por dia

---

# 🖥️ Scripts Implementados

Os scripts encontram-se no diretório:

scripts/


## 1️⃣ export_for_date.sh

Função:

- Consulta a API REST do ThingsBoard
- Filtra por intervalo de tempo (startTs / endTs)
- Gera CSV estruturado
- Salva JSON bruto para auditoria

Principais características:

- Ajuste de timezone para America/Sao_Paulo
- Offset de correção temporal
- Geração automática de cabeçalho
- Indexação sequencial das linhas

---

## 2️⃣ export_yesterday.sh

Função:

- Calcula automaticamente a data do dia anterior
- Invoca export_for_date.sh

Permite execução automática diária sem intervenção manual.

---

## 3️⃣ run_daily.sh

Função:

- Executa exportação do dia anterior
- Adiciona arquivos modificados ao Git
- Cria commit automático
- Realiza push para o GitLab

Fluxo lógico:

    git add data
    git commit -m "Daily export YYYY-MM-DD"
    git push origin main


A execução só ocorre se houver alteração nos arquivos.

---

# ⏱️ Automatização com Cron (Raspberry Pi)

O script pode ser agendado no sistema Linux via `crontab`.

Exemplo:

    0 5 * * * /home/pi/projeto/scripts/run_daily.sh


Isso executa diariamente às 05:00.

## Estratégia adotada

- Apenas a Raspberry realiza push automático
- A máquina de desenvolvimento pode realizar commits normalmente
- Antes de editar localmente, recomenda-se:

    git pull origin main


Essa abordagem reduz a possibilidade de conflitos.

---

# 🔐 Autenticação Git

A autenticação é feita por meio de:

- Personal Access Token do GitLab
- Armazenado via Git Credential Manager

O token não é versionado no repositório.

---

# 📊 Benefícios Arquiteturais

A solução implementada oferece:

- Backup distribuído
- Histórico versionado
- Independência parcial do banco PostgreSQL
- Possibilidade de auditoria retroativa
- Base estruturada para análise estatística e IA

Além disso, permite:

- Limpeza futura do banco do ThingsBoard
- Migração para outro backend sem perda de dados históricos
- Integração com pipelines de ciência de dados

---

# ⚠️ Limitações

Esta abordagem:

- Não substitui um Data Warehouse
- Não possui compressão otimizada
- Depende de disciplina operacional
- Pode gerar crescimento significativo do repositório ao longo dos anos

Trata-se de uma solução acadêmica robusta, porém simplificada.

---

# 🤖 Integração com Análise de Dados

Os arquivos CSV exportados permitem:

- Processamento com Python (Pandas)
- Aplicação de modelos estatísticos
- Treinamento de modelos de Machine Learning
- Detecção de anomalias
- Estudo de estabilidade de enlace LoRaWAN
- Análise psicrométrica (Dew Point, VPD, etc.)

Essa estrutura prepara o projeto para análises avançadas futuras.

---

# 📌 Escopo deste documento

Este documento cobre:

- Exportação de dados do ThingsBoard
- Organização estrutural dos arquivos
- Automação diária
- Versionamento no GitLab

Não cobre:

- Processamento interno do ThingsBoard
- Configuração do gateway
- Configuração da TTN
- Migração para ChirpStack

Esses tópicos são tratados em documentos específicos.

---

# 📚 Referências

- https://thingsboard.io/docs/
- https://git-scm.com/docs
- https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html


