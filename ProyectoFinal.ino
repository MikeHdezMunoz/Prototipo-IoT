#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <DFRobot_DHT11.h>
#include <LCD_I2C.h>

// Definiciones para DHT11
const int DHT_PIN = 15;  // Cambiar según el pin de conexión
DFRobot_DHT11 dht;
// Definiciones para LCD
LCD_I2C lcd(0x27, 20, 4);
// Definición para el sensor de gas MQ135
const int MQ135_PIN = 4;  // Cambiar según el pin de conexión del MQ135

// Internet connection credentials for TEC
const char *ssid = "Tec-Contigencia";
const char *password = "";

// Link to your API
const char *apiEndpoint = "https://sturdy-engine-r4g6p596p9p72559w-5000.app.github.dev/sensor_data";

// Declarar e inicializar la variable concentracionGas
float concentracionGas = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to connect to Wi-Fi using the provided credentials
void setupWifi()
{
  Serial.begin(115200);
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.print(" Connected: ");
  Serial.println(WiFi.localIP());
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  lcd.clear();
  lcd.setCursor(0, 0);
  setupWifi();
}

void loop()
{
  mostrarTemperatura();
  delay(2000);
  mostrarGas();
  delay(2000);
  // Send data to API endpoint
  sendData();
  delay(2000);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendData()
{
  float sensorTemperature = dht.temperature;  // Obtener la temperatura de la función mostrarTemperatura
  float sensorGas = concentracionGas;  // Obtener la concentración de gas de la variable global concentracionGas

  Serial.print("Sending data to API: ");

  // Set up HTTP connection with the API endpoint
  HTTPClient http;
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  // Create a JSON document using the ArduinoJson library
  DynamicJsonDocument doc(2000);

  // Add data to JSON
  doc["temperature"] = sensorTemperature;
  doc["gas"] = sensorGas;
  // Add date/time to JSON
//  String dateTime = String(year()) + "-" + month() + "-" + day() + " " + hour() + ":" + minute() + ":" + second();
 // doc["date_time"] = dateTime;
  // Serialize the JSON to string
  String json;
  serializeJson(doc, json);

  // Request of POST method to the server
  int httpResponseCode = http.POST(json);
  if (httpResponseCode > 0)
  {
    // Server response (serial monitor)
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String responseString = http.getString();
    Serial.println("Received response: " + responseString);
  }
  else
  {
    // Error code
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mostrarTemperatura() {
  int intentos = 0;
  dht.read(DHT_PIN);
  while (dht.temperature == 255 || dht.humidity == 255) {
    delay(500);  // Esperar medio segundo antes de intentar nuevamente
    dht.read(DHT_PIN);
    intentos++;

    // Si no se obtienen lecturas válidas después de varios intentos, salir del bucle
    if (intentos > 10) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error en DHT11");
      return;
    }
  }

  float temperatura = dht.temperature;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatura);
  lcd.print(" C");
}

void mostrarGas() {
  int valorGas = analogRead(MQ135_PIN);
  
  // Ajustar el valor leído según la calibración del sensor y tu entorno
  // Puedes necesitar experimentar con diferentes valores de calibración
  // y ajustarlos según las especificaciones de tu sensor y el entorno.
  concentracionGas = map(valorGas, 0, 1023, 0, 100);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas: ");
  lcd.print(concentracionGas);
  lcd.print("%");
  delay(1000);
}
