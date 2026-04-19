//---- information about the app ----
// project: pekan 4 IoT
// version: 0.1
// created by: M Ibrahim Yusuf
// created at: 13 april 2026
// deskripsi: -

#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// Konfigurasi pin
#define DHTPIN 4
#define DHTTYPE DHT22
#define LED_PIN 35

DHT dht(DHTPIN, DHTTYPE);

// Wifi
const char* ssid = "78";
const char* password = "ibrahimyusuf";

// ThingSpeak
String apiKey = "17JS8RID16PDYEJX";
const char* server = "http://api.thingspeak.com/update";

void scanWiFi() {
  Serial.println("Scanning WiFi...");

  int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("Tidak ada WiFi ditemukan");
  } else {
    Serial.println("WiFi ditemukan:");

    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(". ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm)");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " [Open]" : " [Encrypted]");
      delay(10);
    }
  }

  Serial.println("-----------------------");
}
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  dht.begin();

  scanWiFi();

  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float suhu = random(10,40); //dht.readTemperature();
  float kelembapan = random(30,70); //dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca data dari DHT22");
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.println(" °C");

  Serial.print("Kelembapan: ");
  Serial.print(kelembapan);
  Serial.println(" %");

  // LED menyala jika suhu > 30 derajat
  if (suhu > 30) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Kirim data ke ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(suhu) +
                 "&field2=" + String(kelembapan);

    http.begin(url);
    int httpResponseCode = http.GET();

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  }

  Serial.println("----------------------");
  delay(15000); // ThingSpeak minimal 15 detik
}
