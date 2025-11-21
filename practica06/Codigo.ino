#include <Fuzzy.h>
#include <DHT.h>

// ================== PINES SENSORES (ESP32) ==================
#define DHTPIN   4
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);

const int PIN_SOIL = 34;   // Humedad de suelo (ADC)
const int PIN_LDR  = 35;   // LDR (ADC)

// ================== PINES ACTUADORES ==================
const int PIN_VENT  = 19;  // Relé ventilador
const int PIN_BOMBA = 18;  // Relé bomba 5V
const int PIN_LEDS  = 23;  // Relé tiras LED
const int PIN_BUZZ  = 5;   // Transistor / buzzer

// Si tus relés son activos en LOW, cambia ACTIVO/INACTIVO
const int ACTIVO   = HIGH;
const int INACTIVO = LOW;

// ================== SISTEMA DIFUSO ==================
Fuzzy *fuzzy = new Fuzzy();

// ------- Entradas: temperatura, humedad suelo, luz, humedad ambiental -------

// Temperatura (0–50 °C aprox.)
FuzzySet *tempBaja  = new FuzzySet(0, 0, 10, 25);
FuzzySet *tempMedia = new FuzzySet(15, 25, 25, 35);
FuzzySet *tempAlta  = new FuzzySet(30, 40, 50, 50);

// Humedad de suelo (0–100 %)
FuzzySet *humedadBaja  = new FuzzySet(0, 0, 20, 40);
FuzzySet *humedadMedia = new FuzzySet(30, 50, 50, 70);
FuzzySet *humedadAlta  = new FuzzySet(60, 80, 100, 100);

// Luz (0–1500 aprox. mapeo desde ADC)
FuzzySet *luzBaja  = new FuzzySet(0, 0, 200, 400);
FuzzySet *luzMedia = new FuzzySet(300, 700, 700, 1100);
FuzzySet *luzAlta  = new FuzzySet(1000, 1300, 1500, 1500);

// Humedad ambiental (0–100 %)
FuzzySet *humAmbBaja  = new FuzzySet(0, 0, 30, 45);
FuzzySet *humAmbMedia = new FuzzySet(35, 50, 50, 70);
FuzzySet *humAmbAlta  = new FuzzySet(60, 80, 100, 100);

// ------- Salidas: 0–1 (nivel de activación) -------

FuzzySet *ventBajo  = new FuzzySet(0.0, 0.0, 0.2, 0.4);
FuzzySet *ventMedio = new FuzzySet(0.3, 0.5, 0.5, 0.7);
FuzzySet *ventAlto  = new FuzzySet(0.6, 0.8, 1.0, 1.0);

FuzzySet *luzcBajo  = new FuzzySet(0.0, 0.0, 0.2, 0.4);
FuzzySet *luzcMedio = new FuzzySet(0.3, 0.5, 0.5, 0.7);
FuzzySet *luzcAlto  = new FuzzySet(0.6, 0.8, 1.0, 1.0);

FuzzySet *bomBajo   = new FuzzySet(0.0, 0.0, 0.2, 0.4);
FuzzySet *bomMedio  = new FuzzySet(0.3, 0.5, 0.5, 0.7);
FuzzySet *bomAlto   = new FuzzySet(0.6, 0.8, 1.0, 1.0);

FuzzySet *buzBajo   = new FuzzySet(0.0, 0.0, 0.2, 0.4);
FuzzySet *buzMedio  = new FuzzySet(0.3, 0.5, 0.5, 0.7);
FuzzySet *buzAlto   = new FuzzySet(0.6, 0.8, 1.0, 1.0);

void setup() {
  Serial.begin(115200);
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

  // ====== Entradas difusas ======
  // ID 1: temperatura
  FuzzyInput *temperatura = new FuzzyInput(1);
  temperatura->addFuzzySet(tempBaja);
  temperatura->addFuzzySet(tempMedia);
  temperatura->addFuzzySet(tempAlta);
  fuzzy->addFuzzyInput(temperatura);

  // ID 2: humedad de suelo
  FuzzyInput *humedadSuelo = new FuzzyInput(2);
  humedadSuelo->addFuzzySet(humedadBaja);
  humedadSuelo->addFuzzySet(humedadMedia);
  humedadSuelo->addFuzzySet(humedadAlta);
  fuzzy->addFuzzyInput(humedadSuelo);

  // ID 3: luz
  FuzzyInput *luzIn = new FuzzyInput(3);
  luzIn->addFuzzySet(luzBaja);
  luzIn->addFuzzySet(luzMedia);
  luzIn->addFuzzySet(luzAlta);
  fuzzy->addFuzzyInput(luzIn);

  // ID 4: humedad ambiental
  FuzzyInput *humAmbIn = new FuzzyInput(4);
  humAmbIn->addFuzzySet(humAmbBaja);
  humAmbIn->addFuzzySet(humAmbMedia);
  humAmbIn->addFuzzySet(humAmbAlta);
  fuzzy->addFuzzyInput(humAmbIn);

  // ====== Salidas difusas ======
  FuzzyOutput *ventilador = new FuzzyOutput(1);
  ventilador->addFuzzySet(ventBajo);
  ventilador->addFuzzySet(ventMedio);
  ventilador->addFuzzySet(ventAlto);
  fuzzy->addFuzzyOutput(ventilador);

  FuzzyOutput *luces = new FuzzyOutput(2);
  luces->addFuzzySet(luzcBajo);
  luces->addFuzzySet(luzcMedio);
  luces->addFuzzySet(luzcAlto);
  fuzzy->addFuzzyOutput(luces);

  FuzzyOutput *bomba = new FuzzyOutput(3);
  bomba->addFuzzySet(bomBajo);
  bomba->addFuzzySet(bomMedio);
  bomba->addFuzzySet(bomAlto);
  fuzzy->addFuzzyOutput(bomba);

  FuzzyOutput *buzzer = new FuzzyOutput(4);
  buzzer->addFuzzySet(buzBajo);
  buzzer->addFuzzySet(buzMedio);
  buzzer->addFuzzySet(buzAlto);
  fuzzy->addFuzzyOutput(buzzer);

  // ====== REGLAS (ejemplo, se pueden extender) ======

  // 1) Temp alta y suelo seco -> bomba alta + ventilador alto
  FuzzyRuleAntecedent *tempAltaYsueloSeco = new FuzzyRuleAntecedent();
  tempAltaYsueloSeco->joinWithAND(tempAlta, humedadBaja);
  FuzzyRuleConsequent *cons1 = new FuzzyRuleConsequent();
  cons1->addOutput(bomAlto);
  cons1->addOutput(ventAlto);
  FuzzyRule *rule1 = new FuzzyRule(1, tempAltaYsueloSeco, cons1);
  fuzzy->addFuzzyRule(rule1);

  // 2) Suelo muy húmedo -> bomba baja
  FuzzyRuleAntecedent *sueloHumedo = new FuzzyRuleAntecedent();
  sueloHumedo->joinSingle(humedadAlta);
  FuzzyRuleConsequent *cons2 = new FuzzyRuleConsequent();
  cons2->addOutput(bomBajo);
  FuzzyRule *rule2 = new FuzzyRule(2, sueloHumedo, cons2);
  fuzzy->addFuzzyRule(rule2);

  // 3) Luz baja -> luces altas
  FuzzyRuleAntecedent *luzPoca = new FuzzyRuleAntecedent();
  luzPoca->joinSingle(luzBaja);
  FuzzyRuleConsequent *cons3 = new FuzzyRuleConsequent();
  cons3->addOutput(luzcAlto);
  FuzzyRule *rule3 = new FuzzyRule(3, luzPoca, cons3);
  fuzzy->addFuzzyRule(rule3);

  // 4) Temp muy alta y luz baja -> ventilador alto + buzzer alto
  FuzzyRuleAntecedent *tempAltaYluzBaja = new FuzzyRuleAntecedent();
  tempAltaYluzBaja->joinWithAND(tempAlta, luzBaja);
  FuzzyRuleConsequent *cons4 = new FuzzyRuleConsequent();
  cons4->addOutput(ventAlto);
  cons4->addOutput(buzAlto);
  FuzzyRule *rule4 = new FuzzyRule(4, tempAltaYluzBaja, cons4);
  fuzzy->addFuzzyRule(rule4);

  // 5) Humedad ambiental alta -> ventilador medio (mover aire)
  FuzzyRuleAntecedent *humAmbAltaAnte = new FuzzyRuleAntecedent();
  humAmbAltaAnte->joinSingle(humAmbAlta);
  FuzzyRuleConsequent *cons5 = new FuzzyRuleConsequent();
  cons5->addOutput(ventMedio);
  FuzzyRule *rule5 = new FuzzyRule(5, humAmbAltaAnte, cons5);
  fuzzy->addFuzzyRule(rule5);

  // 6) Temp alta y hum. ambiental baja -> buzzer alto (estrés por sequedad)
  FuzzyRuleAntecedent *tempAltaYhumAmbBaja = new FuzzyRuleAntecedent();
  tempAltaYhumAmbBaja->joinWithAND(tempAlta, humAmbBaja);
  FuzzyRuleConsequent *cons6 = new FuzzyRuleConsequent();
  cons6->addOutput(buzAlto);
  FuzzyRule *rule6 = new FuzzyRule(6, tempAltaYhumAmbBaja, cons6);
  fuzzy->addFuzzyRule(rule6);
}

void loop() {
  // ====== LECTURA SENSORES REALES ======
  float temp = dht.readTemperature();      // °C
  float humAmb = dht.readHumidity();       // % humedad aire

  if (isnan(temp))   temp   = 25.0;
  if (isnan(humAmb)) humAmb = 50.0;

  int soilRaw = analogRead(PIN_SOIL);      // 0–4095
  int ldrRaw  = analogRead(PIN_LDR);       // 0–4095

  // Humedad suelo: 0 = seco, 100 = muy húmedo (ajusta si tu sensor es al revés)
  float humSuelo = 100.0 - ((float)soilRaw / 4095.0 * 100.0);
  if (humSuelo < 0)   humSuelo = 0;
  if (humSuelo > 100) humSuelo = 100;

  // Luz: recortamos a 0–1500
  float luzVal = (float)ldrRaw;
  if (luzVal > 1500.0) luzVal = 1500.0;

  // ====== ENTRADAS AL SISTEMA DIFUSO ======
  fuzzy->setInput(1, temp);      // temperatura (0–50)
  fuzzy->setInput(2, humSuelo);  // humedad suelo (0–100)
  fuzzy->setInput(3, luzVal);    // luz (0–1500)
  fuzzy->setInput(4, humAmb);    // humedad ambiental (0–100)

  // ====== INFERENCIA ======
  fuzzy->fuzzify();

  float outVent   = fuzzy->defuzzify(1);   // 0–1
  float outLuces  = fuzzy->defuzzify(2);   // 0–1
  float outBomba  = fuzzy->defuzzify(3);   // 0–1
  float outBuzzer = fuzzy->defuzzify(4);   // 0–1

  // ====== UMBRALES PARA ACTUADORES ======
  bool ventOn  = (outVent   > 0.5);
  bool lucesOn = (outLuces  > 0.5);
  bool bomOn   = (outBomba  > 0.5);
  bool buzOn   = (outBuzzer > 0.5);

  digitalWrite(PIN_VENT,  ventOn  ? ACTIVO : INACTIVO);
  digitalWrite(PIN_LEDS,  lucesOn ? ACTIVO : INACTIVO);
  digitalWrite(PIN_BOMBA, bomOn   ? ACTIVO : INACTIVO);
  digitalWrite(PIN_BUZZ,  buzOn   ? ACTIVO : INACTIVO);

  // ====== MONITOR SERIAL ======
  Serial.println("--------------------------------------------------------");
  Serial.print("Temp = ");      Serial.print(temp);
  Serial.print("  HumAmb% = "); Serial.print(humAmb);
  Serial.print("  HumSuelo% = "); Serial.print(humSuelo);
  Serial.print("  Luz = ");     Serial.println(luzVal);

  Serial.print("  Ventilador = "); Serial.println(outVent,   3);
  Serial.print("  Luces      = "); Serial.println(outLuces,  3);
  Serial.print("  Bomba      = "); Serial.println(outBomba,  3);
  Serial.print("  Buzzer     = "); Serial.println(outBuzzer, 3);
  Serial.println("--------------------------------------------------------\n");

  delay(2000);
}
