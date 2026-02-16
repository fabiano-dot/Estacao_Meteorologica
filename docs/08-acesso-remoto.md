# 🌐 Acesso Remoto (Domínio + Cloudflare Tunnel)

Este documento descreve como foi implementado o acesso remoto seguro aos serviços hospedados na Raspberry Pi (ex.: ThingsBoard e ChirpStack) utilizando **Cloudflare Zero Trust (Tunnel)**, evitando abertura de portas na rede institucional.

A abordagem adotada permite acessar os serviços via HTTPS por subdomínios, mesmo sob restrições de firewall/NAT, com o túnel rodando como serviço na Raspberry.

---

## 🎯 Motivação

No ambiente institucional, abrir portas de entrada e manter regras de firewall pode ser inviável ou indesejável. Para permitir acesso remoto:

- Evita-se expor portas diretamente na rede
- Evita-se configuração de NAT/port-forward
- Mantém-se um canal de acesso controlado e centralizado
- Reduz-se o acoplamento com a infraestrutura de rede local

A solução adotada foi **Cloudflare Tunnel**, que estabelece uma conexão de saída (outbound) da Raspberry para a Cloudflare, publicando os serviços via domínio.

---

## 🧾 Domínio (Porkbun) e DNS

Para utilizar Cloudflare com subdomínios públicos, é necessário possuir um domínio sob controle para editar DNS.

### Domínio
- Registrar um domínio em provedor externo (ex.: Porkbun)
- Custo anual pode variar conforme TLD (alguns domínios promocionais custam pouco no primeiro ano e aumentam na renovação)

### Delegação DNS para Cloudflare (Nameservers)
O domínio foi adicionado na Cloudflare e, em seguida, no painel do provedor (Porkbun), os **Nameservers (NS)** do domínio foram alterados para os NS fornecidos pela Cloudflare.

Após a propagação, a Cloudflare passa a gerenciar completamente as entradas DNS do domínio.

---

## 🧩 Cloudflare Zero Trust — Tunnel

Após o domínio estar sob gestão da Cloudflare, foi criado um **Tunnel** no painel do Cloudflare Zero Trust.

O túnel funciona como um conector seguro que:
- roda na Raspberry (`cloudflared`)
- abre conexão de saída para a Cloudflare
- encaminha requisições recebidas nos subdomínios para serviços locais

---

## 🧭 Publicação de Serviços (Published Application Routes)

A Cloudflare permite publicar múltiplas aplicações no mesmo túnel, separando por **hostname** (subdomínios).

Exemplo de rotas publicadas:

| Hostname (subdomínio)            | Service (destino local)      |
|----------------------------------|------------------------------|
| `iot.bitdoglablsm.space`         | `http://localhost:8080`      |
| `cs.bitdoglablsm.space`          | `http://localhost:8081`      |

Observações:
- `iot...` aponta para o ThingsBoard (porta 8080)
- `cs...` aponta para o ChirpStack (porta 8081)
- Um único túnel pode servir diversas rotas/aplicações

---

## 🖥️ Execução do Tunnel como serviço (auto-start)

Para garantir disponibilidade contínua e inicialização automática após reboot, o `cloudflared` foi configurado para rodar como serviço na Raspberry.

A ideia operacional é:

- Containers Docker sobem automaticamente (policy de restart / compose)
- O Tunnel sobe automaticamente (serviço)
- O acesso remoto permanece disponível sem intervenção manual

---

## 🔐 Considerações de Segurança

A exposição via Cloudflare Tunnel reduz risco de acesso direto à rede, porém exige boas práticas:

- Evitar versionar credenciais (tokens, senhas)
- Controlar quem tem acesso ao painel Cloudflare Zero Trust
- Utilizar autenticação adequada nas aplicações
- Manter o sistema atualizado
- Revisar permissões e logs periodicamente

---

## ✅ Resultado

Com essa configuração:

- ThingsBoard fica acessível remotamente via:
  - `https://iot.bitdoglablsm.space`
- ChirpStack fica acessível remotamente via:
  - `https://cs.bitdoglablsm.space`
- Não é necessário abrir portas na rede institucional
- O túnel mantém o roteamento de forma persistente

---

## 📚 Referências

- Cloudflare Zero Trust / Tunnel
- Porkbun (registro de domínio)
