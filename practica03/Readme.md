🧠 Red Neuronal para Invernadero con Exportación a C / MicroPython

Este proyecto entrena una red neuronal artificial (RNA) con datos de un invernadero, para clasificar o predecir condiciones ambientales, y posteriormente exportar el modelo entrenado para ser usado en ESP32 (C) o MicroPython.

📌 Objetivo del Proyecto

Entrenar una red neuronal con datos reales de ambiente (temperatura, humedad, iluminación, etc.) y generar código listo para usar en dispositivos embebidos como ESP32, lo que permite implementar predicción automática sin conexión a internet.

🧾 Estructura del Código
📁 1. Librerías Importadas
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout
from tensorflow.keras.regularizers import l2
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.utils import to_categorical
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.model_selection import train_test_split

Se utilizan librerías para:

Procesamiento de datos (pandas, numpy)

Creación de la red neuronal (TensorFlow-Keras)

Preprocesamiento (sklearn)

Graficación de resultados (matplotlib)

⚙️ 2. CONFIGURACIÓN INICIAL (Modificar antes de ejecutar)

Esta sección define el tipo de problema, épocas, tasa de aprendizaje, capas ocultas, rutas y formatos de exportación.
DATA_FILE_PATH = 'C:/ruta/datos.xlsx'
NUM_ENTRADAS   = 4
NUM_SALIDAS    = 4
TIPO_PROBLEMA  = 3        # '1'=Regresión, '2'=Binaria, '3'=Múltiple
EPOCHS         = 170
LEARNING_RATE  = 0.001
HIDDEN_LAYERS  = [16]
EXPORT_FORMAT  = 'c'      # También puede ser 'micropython'

📥 3. Carga y Preparación de Datosdef cargar_datos(): ...
def preparar_entradas_salidas(df): ...
def dividir_datos(X, y): ...

* Lee el archivo Excel
* Separa X = entradas y y = salidas
* Divide en train / validation

🧠 4. Codificación y Escalado
codificar_etiquetas()
escalar_datos()

🏗️ 5. Construcción del Modelo
model = Sequential()
for i, n in enumerate(HIDDEN_LAYERS):
    model.add(Dense(n, activation='relu', input_shape=(NUM_ENTRADAS,)))

Compilación:
model.compile(optimizer=Adam(learning_rate=LEARNING_RATE),
              loss=loss, metrics=mets)

🚀 6. Entrenamiento
history = model.fit(X_tr, y_tr, validation_data=(X_val, y_val),
                    epochs=EPOCHS, verbose=1)
                    
📊 7. Graficar Pérdida
graficar_perdida(history)

🔮 8. Predicción de un Vector
EJEMPLO_VECTOR = [29, 75, 20, 300]
resultado = predecir(model, EJEMPLO_VECTOR, sx, sy)
print("Predicción:", resultado)

📤 9. Exportación a C / MicroPython
✔️ Exportar a C para ESP32:
export_model_weights_to_c()
export_preprocessing_to_c()
generate_forward_propagation_code()

✔️ Exportar a Micropython:
export_model_weights_to_micropython()
export_preprocessing_to_micropython()
generate_forward_propagation_function()

🧾 10. Ejemplo Real de Salida en C
double W1[16][4] = {
  {0.029, -0.695, 0.517, -0.689},
  {-0.813, -0.376, 0.395, 0.708},
  ...
};

👉 Este código se puede copiar directamente a PlatformIO o Arduino IDE para usar en ESP32.

📌 Conclusión

Este script permite:
✔ Entrenar una RNA con datos reales
✔ Evaluarla y probar predicciones
✔ Exportarla para hardware embebido
✔ Minimizar consumo computacional usando C o MicroPython

Ideal para IoT agrícola, automatización, robótica e invernaderos inteligentes 🌱🤖

📩 ¿Dudas o deseas integrar el código en ESP32?

Puedo ayudarte con:

Plataforma IoT (Blynk, Thingspeak, MQTT…)

Conversión a Arduino-C completo

Integración con sensores reales

💬 Solo dime: “Quiero implementarlo en ESP32 con sensores reales”

