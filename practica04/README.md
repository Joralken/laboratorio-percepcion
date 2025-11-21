# 🌱 Invernadero Inteligente con ESP32 + Red Neuronal Artificial (TinyML)

Este proyecto implementa un **sistema embebido inteligente** basado en un **ESP32** que utiliza una **red neuronal artificial (RNA)** para tomar decisiones automatizadas en un invernadero. Se procesan datos de sensores ambientales y se activan actuadores según la salida inferida por la red neuronal entrenada previamente.

---

## 📌 Objetivo General

Automatizar el control de un invernadero empleando sensores ambientales y una red neuronal que decide qué actuadores activar dependiendo de las condiciones detectadas.

---

## 🧠 ¿Qué hace la red neuronal?

La red neuronal ya está **entrenada previamente** y los **pesos (W1, W2) y biases (b1, b2)** se cargan directamente en el código.  
Esta RNA recibe **4 entradas normalizadas**:

| Entrada | Sensor | Finalidad |
|--------|--------|-----------|
| Temperatura | DHT11 | Control térmico |
| Humedad ambiente | DHT11 | Control ambiental |
| Humedad del suelo | Sensor análogo | Activación de la bomba |
| Luz (LDR) | Sensor análogo | Activación de iluminación |

Salida posible (clases):
| Clase | Acción activada |
|-------|-----------------|
| 0 | No hacer nada |
| 1 | Encender ventilador |
| 2 | Activar bomba de agua |
| 3 | Encender luces y buzzer |

---

## 🧩 Hardware Utilizado

- **ESP32**
- DHT11 – Temperatura y humedad
- Sensor de humedad de suelo (ADC)
- LDR – Sensor de luz (ADC)
- Relevadores para:
  - 💨 Ventilador
  - 💧 Bomba de agua
  - 💡 Tira LED
  - 🔊 Buzzer
- LCD 16x2 con I2C

---

## ⚙️ Funcionamiento General

1. Se leen los sensores.
2. Los valores se **escalan y normalizan** (como en el entrenamiento).
3. Se ejecuta el **forward propagation** de la red neuronal:
   - Capa oculta → función ReLU  
   - Capa de salida → función Softmax
4. Se elige la **clase con mayor probabilidad**.
5. Según la clase, se activan los actuadores.
6. Se muestra la información en **Serial Monitor y LCD I2C**.

---

## 📡 Librerías Requeridas

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <math.h>

