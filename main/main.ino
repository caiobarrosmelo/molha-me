/*
 * MOLHA.ME - Sistema IoT Simplificado
 * 
 * FUNCIONALIDADES:
 * 1. Regras locais: temperatura, umidade, luminosidade, distância
 * 2. Integração API: enviar leituras (POST) e buscar thresholds (GET)
 * 3. Fail-safe: valores padrão se API não responder
 * 4. Controle de LED/relé baseado em regras locais
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ===========================================
// CONFIGURAÇÕES WIFI
// ===========================================
const char* ssid = "Seu wifi";
const char* password = "A senha dele";

// ===========================================
// CONFIGURAÇÕES API
// ===========================================
const char* serverUrl = "http://SEUIP:3000"; // Substitua pelo seu IP
const char* apiSendData = "/api/send-data";
const char* apiGetThresholds = "/api/get-thresholds";

// ===========================================
// PINOS DOS SENSORES E ATUADORES
// ===========================================
const int PIN_TEMPERATURA = 34;    // Sensor de temperatura (DHT22)
const int PIN_UMIDADE = 35;        // Sensor de umidade (DHT22)
const int PIN_LUMINOSIDADE = 36;   // Sensor de luminosidade (LDR)
const int PIN_DISTANCIA = 37;      // Sensor de distância (HC-SR04)
const int PIN_LED = 2;             // LED indicador
const int PIN_RELE = 4;            // Relé para bomba/irrigação

// ===========================================
// THRESHOLDS PADRÃO (FALLBACK)
// ===========================================
struct Thresholds {
  float temperatura_max = 30.0;     // °C
  float umidade_min = 40.0;        // %
  float luminosidade_min = 200.0;  // lux
  float distancia_min = 5.0;       // cm
  float distancia_max = 50.0;     // cm
};

Thresholds thresholds;
bool apiOnline = false;
unsigned long lastApiCheck = 0;
const unsigned long API_CHECK_INTERVAL = 30000; // 30 segundos

// ===========================================
// DADOS DOS SENSORES
// ===========================================
struct SensorData {
  float temperatura = 0.0;
  float umidade = 0.0;
  float luminosidade = 0.0;
  float distancia = 0.0;
  unsigned long timestamp = 0;
};

SensorData sensorData;

// ===========================================
// FUNÇÕES DE LEITURA DOS SENSORES
// ===========================================

float lerTemperatura() {
  // Simulação - substitua pela leitura real do DHT22
  return 20.0 + random(0, 200) / 10.0; // 20-40°C
}

float lerUmidade() {
  // Simulação - substitua pela leitura real do DHT22
  return 30.0 + random(0, 500) / 10.0; // 30-80%
}

float lerLuminosidade() {
  // Simulação - substitua pela leitura real do LDR
  return random(0, 1000); // 0-1000 lux
}

float lerDistancia() {
  // Simulação - substitua pela leitura real do HC-SR04
  return random(0, 100); // 0-100 cm
}

// ===========================================
// FUNÇÕES DE CONTROLE
// ===========================================

void acionarLED(bool ligar) {
  digitalWrite(PIN_LED, ligar ? HIGH : LOW);
  Serial.println(ligar ? "LED LIGADO" : "LED DESLIGADO");
}

void acionarRele(bool ligar) {
  digitalWrite(PIN_RELE, ligar ? HIGH : LOW);
  Serial.println(ligar ? "RELE LIGADO (Irrigação)" : "RELE DESLIGADO");
}

// ===========================================
// LÓGICA DE DECISÃO LOCAL
// ===========================================

void executarRegrasLocais() {
  bool acionarSistema = false;
  String motivo = "";
  
  // Regra 1: Temperatura muito alta
  if (sensorData.temperatura > thresholds.temperatura_max) {
    acionarSistema = true;
    motivo += "Temperatura alta (" + String(sensorData.temperatura) + "°C) ";
  }
  
  // Regra 2: Umidade muito baixa
  if (sensorData.umidade < thresholds.umidade_min) {
    acionarSistema = true;
    motivo += "Umidade baixa (" + String(sensorData.umidade) + "%) ";
  }
  
  // Regra 3: Luminosidade muito baixa
  if (sensorData.luminosidade < thresholds.luminosidade_min) {
    acionarSistema = true;
    motivo += "Luminosidade baixa (" + String(sensorData.luminosidade) + " lux) ";
  }
  
  // Regra 4: Distância fora do range
  if (sensorData.distancia < thresholds.distancia_min || 
      sensorData.distancia > thresholds.distancia_max) {
    acionarSistema = true;
    motivo += "Distância fora do range (" + String(sensorData.distancia) + " cm) ";
  }
  
  // Executar ação
  if (acionarSistema) {
    acionarLED(true);
    acionarRele(true);
    Serial.println("SISTEMA ACIONADO: " + motivo);
  } else {
    acionarLED(false);
    acionarRele(false);
    Serial.println("Sistema normal - todas as condições OK");
  }
}

// ===========================================
// COMUNICAÇÃO COM API
// ===========================================

bool buscarThresholdsDaAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  
  HTTPClient http;
  http.begin(serverUrl + String(apiGetThresholds));
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Resposta da API: " + response);
    
    // Parse do JSON
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      thresholds.temperatura_max = doc["temperatura_max"] | thresholds.temperatura_max;
      thresholds.umidade_min = doc["umidade_min"] | thresholds.umidade_min;
      thresholds.luminosidade_min = doc["luminosidade_min"] | thresholds.luminosidade_min;
      thresholds.distancia_min = doc["distancia_min"] | thresholds.distancia_min;
      thresholds.distancia_max = doc["distancia_max"] | thresholds.distancia_max;
      
      Serial.println("Thresholds atualizados da API");
      return true;
    }
  }
  
  http.end();
  return false;
}

bool enviarDadosParaAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  
  HTTPClient http;
  http.begin(serverUrl + String(apiSendData));
  http.addHeader("Content-Type", "application/json");
  
  // Criar JSON com dados dos sensores
  StaticJsonDocument<300> doc;
  doc["temperatura"] = sensorData.temperatura;
  doc["umidade"] = sensorData.umidade;
  doc["luminosidade"] = sensorData.luminosidade;
  doc["distancia"] = sensorData.distancia;
  doc["timestamp"] = sensorData.timestamp;
  doc["api_online"] = apiOnline;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    Serial.println("Dados enviados para API com sucesso");
    return true;
  } else {
    Serial.println("Erro ao enviar dados para API: " + String(httpResponseCode));
    return false;
  }
  
  http.end();
  return false;
}

// ===========================================
// SETUP E LOOP PRINCIPAIS
// ===========================================

void setup() {
  Serial.begin(115200);
  
  // Configurar pinos
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);
  
  // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nFalha ao conectar WiFi - usando modo offline");
  }
  
  // Inicializar sensores
  Serial.println("Inicializando sensores...");
  
  // Primeira leitura dos sensores
  lerSensores();
  
  Serial.println("Sistema iniciado!");
  Serial.println("Modo: " + String(apiOnline ? "Online" : "Offline"));
}

void lerSensores() {
  sensorData.temperatura = lerTemperatura();
  sensorData.umidade = lerUmidade();
  sensorData.luminosidade = lerLuminosidade();
  sensorData.distancia = lerDistancia();
  sensorData.timestamp = millis();
  
  Serial.println("=== LEITURA DOS SENSORES ===");
  Serial.println("Temperatura: " + String(sensorData.temperatura) + "°C");
  Serial.println("Umidade: " + String(sensorData.umidade) + "%");
  Serial.println("Luminosidade: " + String(sensorData.luminosidade) + " lux");
  Serial.println("Distância: " + String(sensorData.distancia) + " cm");
  Serial.println("=============================");
}

void loop() {
  unsigned long agora = millis();
  
  // Ler sensores a cada 5 segundos
  static unsigned long ultimaLeitura = 0;
  if (agora - ultimaLeitura >= 5000) {
    lerSensores();
    ultimaLeitura = agora;
  }
  
  // Verificar API a cada 30 segundos
  if (agora - lastApiCheck >= API_CHECK_INTERVAL) {
    Serial.println("Verificando API...");
    apiOnline = buscarThresholdsDaAPI();
    lastApiCheck = agora;
    
    Serial.println("API Status: " + String(apiOnline ? "Online" : "Offline"));
    if (apiOnline) {
      Serial.println("Usando thresholds da API");
    } else {
      Serial.println("Usando thresholds padrão (fail-safe)");
    }
  }
  
  // Enviar dados para API a cada 10 segundos
  static unsigned long ultimoEnvio = 0;
  if (agora - ultimoEnvio >= 10000) {
    if (apiOnline) {
      enviarDadosParaAPI();
    }
    ultimoEnvio = agora;
  }
  
  // Executar regras locais a cada 2 segundos
  static unsigned long ultimaRegra = 0;
  if (agora - ultimaRegra >= 2000) {
    executarRegrasLocais();
    ultimaRegra = agora;
  }
  
  delay(100); // Pequena pausa para estabilidade
}