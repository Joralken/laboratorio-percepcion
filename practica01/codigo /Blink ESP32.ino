#define LED_BUILTIN 2  // Define el pin del LED incorporado en la mayoría de las placas ESP32

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Configura el pin como salida
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // Enciende el LED
  delay(1000);                      // Espera 1 segundo
  digitalWrite(LED_BUILTIN, LOW);   // Apaga el LED
  delay(1000);                      // Espera 1 segundo
}
