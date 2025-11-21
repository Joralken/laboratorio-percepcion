#include <math.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ==================== PINES DE SENSORES ====================

// DHT11 (temperatura y humedad ambiental)
#define DHTPIN   4
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensores analógicos (ESP32 ADC)
const int PIN_SOIL = 34;   // Humedad de suelo
const int PIN_LDR  = 35;   // LDR

// ==================== PINES DE ACTUADORES ====================
// Estos pines van a las entradas de los relevadores o transistores

const int PIN_VENT  = 19;  // Relé ventilador 12V
const int PIN_BOMBA = 18;  // Relé bomba 12V
const int PIN_LEDS  = 23;  // Tira LED
const int PIN_BUZZ  = 5;   // Buzzer

// Cambia a LOW si tus relevadores son activos en LOW
const int ACTIVO = HIGH;
const int INACTIVO = LOW;

// ==================== LCD I2C ====================
// Dirección típica: 0x27 (si no, prueba 0x3F)

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==================== MUESTREO ====================

unsigned long lastTime = 0;
const unsigned long sampleTime = 1000;  // ms

// ==================== RED NEURONAL ====================

double a0[4];   // entradas normalizadas
double a1[16];  // capa oculta
double a2[4];   // salidas (logits / luego softmax)

// W1 (16 x 4)
double W1[16][4] = {
  {0.029, -0.695, 0.517, -0.689},
  {-0.813, -0.376, 0.395, 0.708},
  {0.881, 0.429, -0.682, -0.134},
  {-0.853, -0.497, 0.109, 0.238},
  {-0.913, 0.080, 0.177, 0.574},
  {0.193, 0.895, -1.059, -0.839},
  {-0.595, -0.085, -0.295, 0.658},
  {0.909, 0.713, 0.189, -0.144},
  {0.191, 0.764, -0.345, -0.921},
  {0.236, -0.434, -0.292, -0.010},
  {-0.006, 0.114, 0.527, 0.738},
  {0.719, 0.726, 0.144, 0.481},
  {-0.487, -0.067, -0.214, 0.682},
  {-0.883, -0.535, -0.072, 0.563},
  {-1.050, -0.342, 0.193, 0.170},
  {-0.942, -0.734, 0.495, 0.544}
};

// W2 (4 x 16)
double W2[4][16] = {
  {-0.672, -0.819, 0.607, -0.287, -0.463, 0.450, -0.538, 0.945, 0.740, -0.574, -0.431, 0.622, 0.145, -0.430, -0.770, -0.813},
  {0.480, -0.779, 0.275, -0.217, -0.529, 0.956, -0.396, 0.621, 1.003, -0.140, -0.278, -0.352, -0.609, -0.950, -0.416, -0.615},
  {-0.717, -0.856, 0.673, -0.274, -0.443, 0.610, 0.026, -0.047, 0.987, 0.396, -0.521, -0.023, -0.257, -0.238, -0.511, -0.970},
  {0.386, -0.380, -0.486, -0.635, -0.845, -0.190, -0.583, -0.020, -0.172, -0.120, -0.616, -0.570, -0.222, -0.611, -0.321, -1.060}
};

// Biases
double b1[16] = {
  0.478, 0.569, 0.642, 0.288,
  0.361, 0.585, 0.459, 0.227,
  0.587, 0.102, 0.345, 0.062,
  -0.038, 0.408, 0.416, 0.606
};

double b2[4] = { -0.215, 0.008, 0.116, -0.235 };

// Normalización (mismos valores del entrenamiento)
double mean_x[4] = {0.545, 0.506, 0.456, 0.477};
double std_x[4]  = {0.272, 0.286, 0.250, 0.293};

// ==================== PROTOTIPOS ====================

double relu(double n);
void   softmax(double *inputs, int length);
double dataNormalized(double inputData, double mean, double desv);

// ==================== SETUP ====================

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(PIN_SOIL, INPUT);
  pinMode(PIN_LDR,  INPUT);

  pinMode(PIN_VENT,  OUTPUT);
  pinMode(PIN_BOMBA, OUTPUT);
  pinMode(PIN_LEDS,  OUTPUT);
  pinMode(PIN_BUZZ,  OUTPUT);

  digitalWrite(PIN_VENT,  INACTIVO);
  digitalWrite(PIN_BOMBA, INACTIVO);
  digitalWrite(PIN_LEDS,  INACTIVO);
  digitalWrite(PIN_BUZZ,  INACTIVO);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Invernadero NN");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");
}

// ==================== LOOP ====================

void loop() {
  if (millis() - lastTime >= sampleTime) {
    lastTime = millis();

    // ---------- 1) Lectura de sensores ----------
    float temp = dht.readTemperature();  // °C
    float humA = dht.readHumidity();     // % humedad ambiente
    int   soilR = analogRead(PIN_SOIL);  // 0–4095
    int   ldrR  = analogRead(PIN_LDR);   // 0–4095

    if (isnan(temp) || isnan(humA)) {
      temp = 25.0;
      humA = 50.0;
    }

    // ---------- 2) Escalado a 0–1 y normalización ----------
    double x_temp = temp  / 50.0;    // rango 0–50 °C
    double x_humA = humA  / 100.0;   // 0–100 %
    double x_soil = soilR / 4095.0;
    double x_ldr  = ldrR  / 4095.0;

    a0[0] = dataNormalized(x_temp, mean_x[0], std_x[0]);
    a0[1] = dataNormalized(x_humA, mean_x[1], std_x[1]);
    a0[2] = dataNormalized(x_soil, mean_x[2], std_x[2]);
    a0[3] = dataNormalized(x_ldr,  mean_x[3], std_x[3]);

    // ---------- 3) Forward propagation ----------

    // Capa oculta (ReLU)
    for (int i = 0; i < 16; i++) {
      double sum = 0.0;
      for (int j = 0; j < 4; j++) {
        sum += W1[i][j] * a0[j];
      }
      a1[i] = relu(sum + b1[i]);
    }

    // Capa de salida (logits)
    for (int i = 0; i < 4; i++) {
      double sum = 0.0;
      for (int j = 0; j < 16; j++) {
        sum += W2[i][j] * a1[j];
      }
      a2[i] = sum + b2[i];
    }

    // Softmax → probabilidades
    softmax(a2, 4);

    // ---------- 4) Clase ganadora ----------
    int clase = 0;
    double maxVal = a2[0];
    for (int i = 1; i < 4; i++) {
      if (a2[i] > maxVal) {
        maxVal = a2[i];
        clase = i;
      }
    }

    // ---------- 5) Mapear clase → actuadores ----------
    bool ventOn  = false;
    bool bombaOn = false;
    bool ledsOn  = false;
    bool buzzOn  = false;

    // Ajusta esto a cómo entrenaste tu red
    // 0: nada
    // 1: ventilador
    // 2: bomba
    // 3: luces + buzzer
    if (clase == 1) {
      ventOn = true;
    } else if (clase == 2) {
      bombaOn = true;
    } else if (clase == 3) {
      ledsOn = true;
      buzzOn = true;
    }

    digitalWrite(PIN_VENT,  ventOn  ? ACTIVO : INACTIVO);
    digitalWrite(PIN_BOMBA, bombaOn ? ACTIVO : INACTIVO);
    digitalWrite(PIN_LEDS,  ledsOn  ? ACTIVO : INACTIVO);
    digitalWrite(PIN_BUZZ,  buzzOn  ? ACTIVO : INACTIVO);

    // ---------- 6) Debug por Serial ----------
    Serial.print("Temp: ");  Serial.print(temp);
    Serial.print("  HumA: "); Serial.print(humA);
    Serial.print("  SoilADC: "); Serial.print(soilR);
    Serial.print("  LDRADC: ");  Serial.print(ldrR);
    Serial.print("  | Clase: ");
    Serial.print(clase);
    Serial.print("  Prob: ");
    Serial.println(maxVal, 4);

    // ---------- 7) Mostrar en LCD ----------
    lcd.clear();

    // Línea 1: temperatura y humedad
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print("C H:");
    lcd.print(humA, 0);
    lcd.print("%");

    // Línea 2: estados (BOMB ON, FAN ON)
    lcd.setCursor(0, 1);
    if (bombaOn) {
      lcd.print("BOMB ON ");
    } else {
      lcd.print("BOMB OFF");
    }

    lcd.setCursor(10, 1);
    if (ventOn) {
      lcd.print("FAN");
    } else {
      lcd.print("   ");
    }
  }
}

// ==================== FUNCIONES AUXILIARES ====================

double relu(double n) {
  return (n >= 0.0) ? n : 0.0;
}

void softmax(double *inputs, int length) {
  double maxVal = inputs[0];
  for (int i = 1; i < length; i++) {
    if (inputs[i] > maxVal) maxVal = inputs[i];
  }

  double sum = 0.0;
  for (int i = 0; i < length; i++) {
    inputs[i] = exp(inputs[i] - maxVal);
    sum += inputs[i];
  }
  for (int i = 0; i < length; i++) {
    inputs[i] /= sum;
  }
}

double dataNormalized(double inputData, double mean, double desv) {
  return (inputData - mean) / desv;
}
