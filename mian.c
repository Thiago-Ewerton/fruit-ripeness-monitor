#define BLYNK_TEMPLATE_ID "TMPL2A0ClPMmR"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "TaB8kOG4E85pcvjrGUWR_GkiyG2MnF5x"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Configurações do Wi-Fi
char ssid[] = "A54 de Tamires";
char pass[] = "87155087";

// Configurações da Tela OLED (0.96" I2C)
#define LARGURA_TELA 128
#define ALTURA_TELA 64
#define ENDERECO_OLED 0x3C

#define PIN_SDA 21
#define PIN_SCL 22

Adafruit_SSD1306 display(LARGURA_TELA, ALTURA_TELA, &Wire, -1);

// Configurações do Sensor DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
#define LED_PIN 2

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer; // Timer para substituir o delay()

// Função que será executada a cada 2 segundos pelo BlynkTimer
void enviarDadosSensor() {
  // Lê a umidade (%) e a temperatura (°C)
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Verifica se a leitura falhou
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println(F("Falha ao ler o sensor DHT11!"));
    digitalWrite(LED_PIN, HIGH); // LED aceso fixo indica erro
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(15, 25);
    display.print("Erro no Sensor!");
    display.display();
    return;
  }

  // Pisca rápido o LED indicando leitura OK
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);

  // Envia os dados para o Blynk nos Pinos Virtuais V1 e V2
  Blynk.virtualWrite(V1, temperatura);
  Blynk.virtualWrite(V2, umidade);

  // Imprime no Monitor Serial
  Serial.print("Umidade: ");
  Serial.print(umidade, 0);
  Serial.print("%  |  Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println("°C");

  // --- Atualiza a Tela OLED ---
  display.clearDisplay();

  // Título
  display.setTextSize(1);             
  display.setCursor(20, 0);           
  display.print("PAINEL DE CLIMA");
  display.drawFastHLine(0, 12, 128, SSD1306_WHITE);

  // Temperatura
  display.setTextSize(1);
  display.setCursor(0, 24);
  display.print("TEMP:");
  display.setTextSize(2);             
  display.setCursor(40, 18);          
  display.print(temperatura, 1);      
  display.setTextSize(1);
  display.print(" ");
  display.write(247);                 
  display.print("C");                

  // Umidade
  display.setCursor(0, 48);
  display.setTextSize(1);
  display.print("UMID:");
  display.setTextSize(2);             
  display.setCursor(40, 42);          
  display.print(umidade, 0);          
  display.setTextSize(1);
  display.print(" %");

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(PIN_SDA, PIN_SCL);
  dht.begin();

  // Inicializa o OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO_OLED)) {
    Serial.println(F("Falha ao inicializar a tela OLED."));
    while(true) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Inicializa a conexão com o Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Configura o timer para rodar a função 'enviarDadosSensor' a cada 2000ms (2 segundos)
  timer.setInterval(2000L, enviarDadosSensor);
}

void loop() {
  Blynk.run(); // Mantém a comunicação com o Blynk ativa
  timer.run(); // Controla o tempo de leitura do sensor
}
