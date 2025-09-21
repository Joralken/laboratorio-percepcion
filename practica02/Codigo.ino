#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ---------- Configura aquí ----------
#define PIN_DHT    2          // GPIO del DHT (DHT11/DHT22). Nota: en ESP32 el GPIO2 es "strapping"; si da lata, usa 15.
#define TIPO_DHT   DHT11      // Cambia a DHT22 si ese es tu sensor

#define SDA_PIN    21         // I2C ESP32
#define SCL_PIN    22
#define I2C_ADDR   0x27       // Si no ves nada, prueba 0x3F

#define SOIL_DIG   25         // D0 del módulo de humedad de suelo (LM393)
#define PUMP_PIN    4         // IN del relé/bomba
#define RELAY_ACTIVE_LOW 1    // 1 si tu relé se activa en LOW (común), 0 si en HIGH

// Tiempos de riego cuando está seco
const unsigned long TIEMPO_ON_MS  = 8000;
const unsigned long TIEMPO_OFF_MS = 1000;  // pausa tras riego

// ---------- Objetos ----------
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);
DHT dht(PIN_DHT, TIPO_DHT);

// ---------- Utilidad ----------
inline void releWrite(bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(PUMP_PIN, on ? LOW : HIGH);
#else
  digitalWrite(PUMP_PIN, on ? HIGH : LOW);
#endif
}

void setup() {
  Serial.begin(115200);

  // I2C y LCD
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Iniciando...");
  delay(1200);
  lcd.clear();

  // DHT
  dht.begin();

  // Suelo y bomba
  pinMode(SOIL_DIG, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  releWrite(false);  // bomba apagada
}

void loop() {
  // ----- Lectura DHT -----
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // °C

  // ----- Lectura suelo (digital) -----
  int estadoSuelo = digitalRead(SOIL_DIG); // HIGH=seco, LOW=húmedo (típico LM393)
  bool sueloSeco = (estadoSuelo == HIGH);

  // ----- Mostrar en LCD / Serial -----
  if (isnan(h) || isnan(t)) {
    lcd.setCursor(0,0); lcd.print("Error sensor     ");
    lcd.setCursor(0,1); lcd.print("DHT invalido     ");
    Serial.println("Lectura DHT invalida");
  } else {
    lcd.setCursor(0,0);
    lcd.print("Temp:");
    lcd.print(t, 1);
    lcd.print((char)223);
    lcd.print("C   ");

    lcd.setCursor(0,1);
    lcd.print("Hum:");
    lcd.print(h, 1);
    lcd.print("% ");
    lcd.print(sueloSeco ? "Seco " : "Hum. ");

    Serial.print("Temperatura: ");
    Serial.print(t, 1);
    Serial.print(" °C | Humedad: ");
    Serial.print(h, 1);
    Serial.print(" % | Suelo: ");
    Serial.println(sueloSeco ? "SECO" : "HUMEDO");
  }

  // ----- Lógica bomba -----
  if (sueloSeco) {
    Serial.println("Suelo seco -> Encendiendo bomba...");
    lcd.setCursor(10,1); lcd.print("B:ON");
    releWrite(true);
    delay(TIEMPO_ON_MS);

    releWrite(false);
    lcd.setCursor(10,1); lcd.print("B:OF");
    Serial.println("Bomba apagada.");
    delay(TIEMPO_OFF_MS);
  } else {
    // Suelo húmedo: mantener apagada
    releWrite(false);
    lcd.setCursor(10,1); lcd.print("B:OF");
    delay(2000); // refresco normal
  }
}
