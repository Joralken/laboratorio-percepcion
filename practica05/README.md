# 🌱 Sistema Difuso para Invernadero Inteligente (Python + ESP32)

Este proyecto implementa un **sistema de lógica difusa** para controlar automáticamente un invernadero utilizando variables ambientales como temperatura, humedad del aire, humedad del suelo y luz. El objetivo es **activar actuadores** como ventilador, bomba de agua, luces y buzzer de manera inteligente, simulando un sistema real que puede integrarse con un **ESP32 u otros microcontroladores**.

---

## 🎯 Objetivo General

Diseñar un **sistema de control difuso** que tome decisiones similares a un ser humano, utilizando reglas basadas en experiencia y condiciones ambientales, para automatizar un invernadero de forma inteligente y eficiente.

---

## ⚙️ ¿Qué Hace el Sistema?

El código:
✔ Define las variables de entrada (sensores) y salida (actuadores)  
✔ Asigna funciones de pertenencia (bajo, medio, alto, seco, húmedo…)  
✔ Aplica **30 reglas difusas** para tomar decisiones lógicas  
✔ Simula la respuesta del sistema con **casos de prueba**  
✔ Puede adaptarse para **controlar sensores reales con ESP32**  

---

## 🧠 Entradas (Sensores)

| Variable             | Rango | Unidad        |
|---------------------|-------|---------------|
| Temperatura         | 0 - 50 | °C            |
| Humedad ambiental   | 0 - 100 | %            |
| Humedad del suelo   | 0 - 100 | %            |
| Luz (LDR ESP32)     | 0 - 1500 | Lux aprox.   |

---

## ⚡ Salidas (Actuadores)

| Actuador     | Valor Difuso (0-1) | Interpretación         |
|--------------|--------------------|-------------------------|
| Ventilador   | bajo / medio / alto | Nivel de activación    |
| Bomba de agua | bajo / medio / alto | Riego automático       |
| Luces        | bajo / medio / alto | Iluminación interna    |
| Buzzer       | bajo / medio / alto | Alarma de emergencia   |

---

## 📌 Estructura del Código

El archivo está organizado en módulos claros:

| Sección | Función |
|--------|---------|
| `define_variables()` | Crea los sensores y actuadores difusos |
| `set_membership_functions()` | Define bajo/medio/alto, seco/óptimo/húmedo |
| `define_rules()` | Crea **30 reglas de decisión difusa** |
| `run_tests()` | Simula escenarios reales |
| `main` | Ejecuta todo el sistema |

---

## 🧪 Ejemplo de Prueba Real

```python
pruebas = [
    {'temperatura': 40, 'humedad_ambiental': 20, 'humedad_suelo': 10, 'luz': 200},   # Emergencia
    {'temperatura': 15, 'humedad_ambiental': 80, 'humedad_suelo': 80, 'luz': 1400},  # Estable
]
Salida en consola (ejemplo):

Ventilador = 0.912  
Luces      = 0.751  
Bomba      = 0.533  
Buzzer     = 0.892  
