# 🧠 Red Neuronal para Invernadero — Exportación a C / MicroPython

Este proyecto implementa una **red neuronal artificial (RNA)** entrenada con datos reales de un invernadero (temperatura, humedad, iluminación, etc.), con el objetivo de **clasificar o predecir condiciones ambientales** y exportar el modelo entrenado para ser usado en sistemas embebidos como el **ESP32**, ya sea en **C** o **MicroPython**. Esto permite ejecutar inteligencia artificial **sin conexión a internet** y directamente en hardware de bajo consumo ⚡.

---

## 🎯 Objetivo General

Desarrollar un sistema inteligente capaz de **analizar datos ambientales mediante una red neuronal** y generar un modelo optimizado para **implementación embebida**, ideal para aplicaciones IoT agrícolas, automatización y robótica.

---

## 📌 Características Principales

✔ Entrenamiento con datos reales del invernadero  
✔ Preprocesamiento automático de entradas/salidas  
✔ Predicciones de nuevas condiciones ambientales  
✔ Exportación del modelo a **C o MicroPython**  
✔ Compatible con **ESP32 / Arduino / PlatformIO**  
✔ IA completamente **offline** después del entrenamiento  

---

## 🧠 Flujo General del Proyecto

1. Carga y procesamiento de datos del invernadero  
2. Construcción y entrenamiento de la RNA con TensorFlow/Keras  
3. Evaluación y predicción de pruebas  
4. Exportación del modelo para sistemas embebidos:  
   - 🧾 **C para ESP32 (Arduino / PlatformIO)**  
   - 🐍 **MicroPython para ESP32 / Raspberry Pi Pico**  
5. Generación de código listo para copiar y pegar en el microcontrolador  

---

## 📤 Exportación a Sistemas Embebidos

### 🚀 C (Arduino / PlatformIO)
```c
double W1[16][4] = {
  { 0.029, -0.695, 0.517, -0.689 },
  { -0.813, -0.376, 0.395, 0.708 },
  ...
};
