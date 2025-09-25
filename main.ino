// --- Inclusão das bibliotecas necessárias ---
#include <WiFi.h>          // Biblioteca para conectar o ESP32 ao WiFi
#include <HTTPClient.h>    // Biblioteca para realizar requisições HTTP (GET, POST, etc.)
#include <ArduinoJson.h>   // Biblioteca para manipular JSON de forma simples e segura

// --- Configurações da rede WiFi ---
const char* ssid     = "Wokwi-GUEST";   // Nome da rede WiFi (SSID)
const char* password = "";  // Senha da rede WiFi

// --- URL da API do backend ---
// O backend deve expor um endpoint que retorne as configurações em JSON
const char* serverUrl = "http://SEU_BACKEND:3000/config";

// --- Valores padrão (sobrescritos pelo backend) ---
// Caso não haja resposta do backend, o ESP32 usará esses valores
float UMIDADE_SOLO_IDEAL = 50.0;
float UMIDADE_AR_IDEAL   = 60.0;

// --- Função para atualizar os limiares ---
// Recebe o payload JSON do backend e atualiza os valores no ESP32
void atualizarLimiares(String payload) {
  StaticJsonDocument<200> doc;  // Cria um documento JSON com capacidade para 200 bytes

  // Faz o parse do JSON recebido
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    // Caso haja erro no parse, mostra mensagem no Serial
    Serial.print("Erro ao parsear JSON: ");
    Serial.println(error.c_str());
    return; // Sai da função sem atualizar nada
  }

  // Atualiza apenas se o JSON tiver as chaves esperadas
  if (doc.containsKey("umidade_solo")) {
    UMIDADE_SOLO_IDEAL = doc["umidade_solo"].as<float>();
  }
  if (doc.containsKey("umidade_ar")) {
    UMIDADE_AR_IDEAL = doc["umidade_ar"].as<float>();
  }

  // Mostra os valores atualizados no Serial Monitor
  Serial.println("Limiares atualizados pelo backend:");
  Serial.print(" - Umidade Solo: "); Serial.println(UMIDADE_SOLO_IDEAL);
  Serial.print(" - Umidade Ar: ");   Serial.println(UMIDADE_AR_IDEAL);
}

// --- Setup ---
// Executado apenas uma vez, na inicialização do ESP32
void setup() {
  Serial.begin(115200); // Inicia comunicação Serial para debug

  // Conectar ao WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);        // Aguarda meio segundo
    Serial.print("."); // Mostra progresso da conexão
  }
  Serial.println("conectado!");
}

// --- Loop principal ---
// Executado em loop infinito após o setup()
void loop() {
  if (WiFi.status() == WL_CONNECTED) {  // Verifica se está conectado ao WiFi
    HTTPClient http;                     // Cria cliente HTTP

    http.begin(serverUrl);               // Define a URL do backend
    int httpResponseCode = http.GET();   // Faz requisição GET

    if (httpResponseCode > 0) {          // Se recebeu resposta válida
      String payload = http.getString(); // Pega o conteúdo da resposta
      Serial.println("Resposta do backend:");
      Serial.println(payload);

      // Atualiza os limiares com base no JSON recebido
      atualizarLimiares(payload);
    } else {
      // Caso haja erro (ex: backend offline ou rota incorreta)
      Serial.print("Erro na requisição: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Libera os recursos da conexão HTTP
  } else {
    // Se o WiFi cair, mostra aviso no Serial
    Serial.println("WiFi desconectado.");
  }

  delay(10000); // Aguarda 10 segundos antes de fazer nova requisição
}
