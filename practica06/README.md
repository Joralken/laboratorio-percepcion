# 🌱 Sistema de Riego Automatizado con Lógica Difusa (ESP32)

Este proyecto implementa un sistema inteligente de control ambiental para agricultura utilizando un ESP32 y lógica difusa (Fuzzy Logic). El sistema analiza las condiciones del entorno mediante sensores y controla automáticamente ventilación, riego, iluminación y alertas sonoras.

---

## 🧰 Características principales

- Lectura de **4 sensores ambientales**:
  - 🌡️ DHT11 → temperatura y humedad del aire  
  - 🌱 Sensor de humedad de suelo (ADC)  
  - 💡 Sensor LDR para intensidad lumínica (ADC)

- Control de **4 actuadores mediante relés**:
  - 🌀 Ventilador  
  - 🚰 Bomba de agua  
  - 💡 Luces LED  
  - 🔔 Buzzer (alertas)

- Uso de **lógica difusa** para decisiones realistas y suaves, evitando el uso de comparaciones simples con `if`.

---

## 🧠 ¿Qué hace la lógica difusa?

Se definen rangos de comportamiento (bajo, medio, alto) para cada variable y se crean reglas como:

> 🔥 Si la temperatura es alta y la humedad del suelo es baja → activar bomba y ventilador al máximo.

Esto permite respuestas más naturales y adaptables a condiciones reales del entorno.

---

## 📌 Pines utilizados

| Dispositivo         | Pin ESP32 |
|--------------------|------------|
| 🌡️ DHT11              | 4          |
| 🌱 Sensor suelo (ADC) | 34         |
| 💡 LDR (ADC)          | 35         |
| 🌀 Ventilador         | 19         |
| 🚰 Bomba              | 18         |
| 💡 Luces LED          | 23         |
| 🔔 Buzzer             | 5          |

---

## 📦 Librerías necesarias

```cpp
#include <Fuzzy.h>
#include <DHT.h>
