# Práctica 02 - Sistema de Invernadero Automatizado 🌱💧

Este proyecto corresponde a la **Práctica 2** del laboratorio de percepción.  
Implementa un **sistema de invernadero automatizado** utilizando un ESP32, sensores de temperatura, humedad ambiental y humedad de suelo, además de una bomba de riego controlada mediante un relé.

---

## 📂 Estructura del proyecto

- **Codigo.ino** → Código principal del sistema de invernadero.  
- **Esquematico Proteus.zip** → Archivo con el diagrama del circuito en Proteus.  
- **README.md** → Explicación de la práctica, instalación y uso.  

---

## ⚙️ Descripción del sistema

El sistema automatiza el monitoreo y riego en un invernadero pequeño:

- **Sensor DHT11/DHT22**: mide temperatura y humedad ambiental.  
- **Sensor de humedad de suelo (LM393, salida digital)**: detecta si el suelo está seco o húmedo.  
- **Pantalla LCD 16x2 (I2C)**: muestra temperatura, humedad y estado del suelo/bomba.  
- **Bomba de agua + relé**: activa automáticamente cuando el suelo está seco.  

---

## 📌 Funcionamiento del código

1. **Inicialización (`setup`)**
   - Configura la comunicación I2C para la pantalla LCD.
   - Inicializa el sensor DHT.
   - Define los pines para el sensor de suelo y la bomba.
   - Muestra el mensaje *“Iniciando…”* en la pantalla.

2. **Bucle principal (`loop`)**
   - Lee **temperatura y humedad ambiental** con el DHT.
   - Lee el estado del **sensor de suelo (seco/húmedo)**.
   - Muestra en LCD y en el monitor serial:
     - Temperatura (°C).
     - Humedad (%).
     - Estado del suelo (SECO/HÚMEDO).
   - **Control de riego**:
     - Si el suelo está **seco**, enciende la bomba durante 8 segundos.
     - Luego, la apaga y espera un breve tiempo de seguridad.
     - Si el suelo está **húmedo**, la bomba permanece apagada.  

---

## 📚 Librerías necesarias

Antes de compilar el código, asegúrate de instalar las siguientes librerías en el **Arduino IDE** o **PlatformIO**:

- [**DHT sensor library**](https://github.com/adafruit/DHT-sensor-library)  
- [**Adafruit Unified Sensor**](https://github.com/adafruit/Adafruit_Sensor)  
- [**LiquidCrystal I2C**](https://github.com/johnrickman/LiquidCrystal_I2C)  

### Instalación en Arduino IDE
1. Ve a **Herramientas > Administrar bibliotecas**.  
2. Busca cada librería por su nombre.  
3. Haz clic en **Instalar**.  

---

## 🖥️ Esquemático

En la carpeta se incluye un archivo **Proteus** con el diagrama de conexiones:  
- ESP32 ↔ DHT11/DHT22  
- ESP32 ↔ Sensor de suelo LM393  
- ESP32 ↔ Relé + Bomba  
- ESP32 ↔ LCD 16x2 (I2C)  

---

## 🚀 Ejecución

1. Conecta el ESP32 al PC.  
2. Carga el archivo `Codigo.ino` desde el **Arduino IDE**.  
3. Abre el **Monitor Serial** a `115200 baudios` para ver las lecturas.  
4. Observa en la **pantalla LCD** el estado del invernadero.  

---

## 📊 Salida esperada

Ejemplo de salida en el monitor serial:


Temperatura: 25.3 °C | Humedad: 60.5 % | Suelo: SECO
Suelo seco -> Encendiendo bomba...
Bomba apagada.

En la pantalla LCD:
Temp:25.3°C
Hum:60.5% Seco B:ON
