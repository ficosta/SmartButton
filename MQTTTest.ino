#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

const char* SSID = "LIVE TIM"; //Seu SSID da Rede WIFI
const char* PASSWORD = "36923190"; // A Senha da Rede WIFI
const char* MQTT_SERVER = "felipeiasi.ddns.net"; //Broker do Mosquitto.org
const char* MQTT_USER = "ficosta";
const char* MQTT_PASSWORD = "369231";
 

int value = 0; //Contador de é incrementado de -20 a 50
long lastMsg = 0;
char msg[50];
WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D4, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
    Serial.println("LOW");
  } else {
    digitalWrite(D4, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("HIGH");
  }

}

//CONFIGURAÇÃO DA INTERFACE DE REDE
void setupWIFI() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando na rede: ");
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
  }
}
void setup(void) {
  Serial.begin(115200);
  setupWIFI(); 
  MQTT.setServer(MQTT_SERVER, 1883);
  MQTT.setCallback(callback);
  pinMode(D4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(D4, LOW);
}
void reconectar() {
  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT.");
    if (MQTT.connect("ESP8266",MQTT_USER,MQTT_PASSWORD)) {
      MQTT.subscribe("inTopic");
      Serial.println("Conectado com Sucesso ao Broker");
    } else {
      Serial.print("Falha ao Conectador, rc=");
      Serial.print(MQTT.state());
      Serial.println(" tentando se reconectar...");
      delay(3000);
    }
  }
}
void loop(void) {
  if (!MQTT.connected()) {
    reconectar();
  }
  MQTT.loop();
  long now = millis();
  // Nessa função é onde a brincadeira acontece. A cada segundo ele PUBLICA aquele CONTADOR de -20 a 50 para o Broker MQTT(mosquitto.org)
  if (now - lastMsg > 1000) {
    lastMsg = now;
    value++;
    if(value >=50) value = -20;
    snprintf (msg, 75, "%ld", value);
    Serial.print("Mensagem a ser Puplicada: ");
    Serial.println(msg);
    MQTT.publish("temp/random", msg);
  }
}
