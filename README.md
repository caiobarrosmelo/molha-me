---

**Sistema simples, eficiente e confiável para monitoramento IoT de plantas!**


````markdown
# Molha.me - Sistema IoT Simplificado

Sistema IoT simples para monitoramento automático de plantas com regras locais no ESP32 e dashboard web.

## Instalação e Uso

### **1. Instalar Dependências**
- Certifique-se de que o Node.js está instalado antes de rodar este comando.
Dentro da pasta raiz do projeto (`molha-me`), execute:
```bash
npm install
````

### **2. Configurar ESP32**

Abra `molha-me/main/main.ino` no Arduino IDE e configure:

```cpp
// Seu WiFi
const char* ssid = "SEU_WIFI_AQUI";
const char* password = "SUA_SENHA_AQUI";

// IP da sua máquina (descubra com: ipconfig)
const char* serverUrl = "http://SEU_IP:3000";
```

### **3. Iniciar Sistema**

```bash
# Terminal 1: iniciar API
cd molha-me/molha-me
node api_simples.js

# Terminal 2: abrir dashboard
# Abra dashboard.html no navegador
# Ou acesse: http://localhost:3000
```

## Como Funciona

### **Regras Locais (ESP32)**

* **Temperatura > 30°C** → Acionar LED/relé
* **Umidade < 40%** → Acionar LED/relé
* **Luminosidade < 200 lux** → Acionar LED/relé
* **Distância fora do range** → Acionar LED/relé

### **Integração API**

* ESP32 envia dados dos sensores (POST)
* ESP32 busca thresholds atualizados (GET)
* Dashboard mostra dados em tempo real

### **Fail-safe**

* Se API não responder → usa valores padrão
* Sistema continua funcionando offline
* Reconexão automática

## Dashboard

* **Visualização**: dados dos sensores em tempo real
* **Configuração**: ajuste thresholds via interface
* **Histórico**: últimos 10 registros
* **Status**: Online/Offline do ESP32

## Configuração dos Sensores

### **Pinos ESP32**

```cpp
PIN_TEMPERATURA = 34    // DHT22
PIN_UMIDADE = 35        // DHT22
PIN_LUMINOSIDADE = 36   // LDR
PIN_DISTANCIA = 37      // HC-SR04
PIN_LED = 2             // LED indicador
PIN_RELE = 4            // Relé para bomba
```

### **Thresholds Padrão**

```cpp
temperatura_max = 30.0°C
umidade_min = 40.0%
luminosidade_min = 200 lux
distancia_min = 5.0 cm
distancia_max = 50.0 cm
```

## Solução de Problemas

### **ESP32 não conecta**

* Verifique SSID e senha WiFi
* Confirme IP da API
* Teste: `ping IP_DA_API`

### **API não responde**

* Verifique se Node.js está instalado
* Confirme se porta 3000 está livre
* Teste: `curl http://localhost:3000/api/get-thresholds`

### **Dashboard não carrega**

* Abra console do navegador (F12)
* Verifique erros de CORS
* Confirme se API está rodando

## Estrutura do Projeto

```
molha-me/
├── molha-me/
│   ├── main/
│   │   └── main.ino         # Código ESP32
│   ├── api_simples.js       # API REST
│   ├── dashboard.html       # Dashboard web
│   ├── molha_me_db.sql      # Script do banco
│   └── README.md            # Esta documentação
├── package.json             # Dependências
└── package-lock.json
```
---
