# -- coding: utf-8 --
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

# -----------------------------
# CONFIGURACIÓN (¡ajusta aquí antes de ejecutar!)
# -----------------------------
DATA_FILE_PATH   = 'C:/Users/shark/Downloads/datos_nn_invernadero_normalizado.xlsx'  # ← Ruta y nombre de tu archivo de datos
NUM_ENTRADAS     = 4                      # ← Número de columnas de características
NUM_SALIDAS      = 4                      # ← Número de columnas objetivo
TIPO_PROBLEMA    = 3                    # '1'=Regresión, '2'=Binaria, '3'=Múltiple
EPOCHS           = 170                    # ← Épocas de entrenamiento
LEARNING_RATE    = 0.001                   # ← Tasa de aprendizaje para el optimizador Adam

HIDDEN_LAYERS    = [16]                    # ← Neuronas en cada capa oculta, ej [8,6]
USAR_L2          = False                  # ← True para activar regularización L2
L2_FACTOR        = 0.0036                # ← Coeficiente L2
USAR_DROPOUT     = False                  # ← True para activar Dropout
DROPOUT_RATE     = 0.3                    # ← Tasa de Dropout (0.0–1.0)

# Define aquí tu vector de entrada de prueba (longitud = NUM_ENTRADAS)
EJEMPLO_VECTOR   = [29, 75, 20, 300]
# -----------------------------

# 'c'           → genera arrays en C para ESP32
# 'micropython' → genera listas y función en Python para MicroPython
EXPORT_FORMAT = 'c'

# Semillas para reproducibilidad
np.random.seed(42)
tf.random.set_seed(42)


def paso_titulo(n, texto):
    print("\n" + "="*10 + f" Paso {n}: {texto} " + "="*10 + "\n")


def cargar_datos():
    """Paso 1: Carga de datos desde Excel usando DATA_FILE_PATH"""
    try:
        df = pd.read_excel(DATA_FILE_PATH)
        print(f"�� Datos cargados de '{DATA_FILE_PATH}': {df.shape[0]} filas × {df.shape[1]} columnas")
        print(df.head(3), "\n")
        if df.shape[1] < NUM_ENTRADAS + NUM_SALIDAS:
            raise ValueError(f"Se esperaban ≥{NUM_ENTRADAS + NUM_SALIDAS} columnas.")
        return df
    except Exception as e:
        print(f"❌ Error leyendo '{DATA_FILE_PATH}': {e}")
        exit(1)


def preparar_entradas_salidas(df):
    """Paso 2: Separar X e y"""
    X = df.iloc[:, :NUM_ENTRADAS].values
    y = df.iloc[:, -NUM_SALIDAS:].values
    print(f"✔︎ X shape: {X.shape}, y shape: {y.shape}\n")
    return X, y


def dividir_datos(X, y, test_size=0.2):
    """Paso 3: Dividir en train/val"""
    X_tr, X_val, y_tr, y_val = train_test_split(
        X, y, test_size=test_size, random_state=42
    )
    print(f"✔︎ Train: {X_tr.shape}, Val: {X_val.shape}\n")
    return X_tr, X_val, y_tr, y_val


def codificar_etiquetas(y_tr, y_val):
    """Paso 4: Codificar etiquetas para clasificación"""
    if TIPO_PROBLEMA == '2':  # binaria
        le = LabelEncoder()
        y_tr = le.fit_transform(y_tr.ravel()).reshape(-1, 1)
        y_val = le.transform(y_val.ravel()).reshape(-1, 1)
        print("✔︎ Etiquetas binarias codificadas.\n")
    elif TIPO_PROBLEMA == '3':  # múltiple
        le = LabelEncoder()
        tr_enc = le.fit_transform(y_tr.ravel())
        val_enc = le.transform(y_val.ravel())
        y_tr = to_categorical(tr_enc)
        y_val = to_categorical(val_enc)
        print("✔︎ Etiquetas múltiples codificadas.\n")
    return y_tr, y_val


def escalar_datos(X_tr, X_val, y_tr, y_val):
    """Paso 5: Escalar X e y (solo regresión)"""
    sx = StandardScaler().fit(X_tr)
    X_tr_s = sx.transform(X_tr)
    X_val_s = sx.transform(X_val)
    print("✔︎ Entradas escaladas.\n")

    if TIPO_PROBLEMA == '1':
        sy = StandardScaler().fit(y_tr)
        y_tr_s = sy.transform(y_tr)
        y_val_s = sy.transform(y_val)
        print("✔︎ Salidas escaladas (regresión).\n")
    else:
        sy = None
        y_tr_s, y_val_s = y_tr, y_val

    return X_tr_s, X_val_s, y_tr_s, y_val_s, sx, sy


def construir_modelo():
    """Paso 6: Construir y compilar el modelo"""
    if TIPO_PROBLEMA == '1':
        act_out, loss, mets = 'linear', 'mse', ['mae']
    elif TIPO_PROBLEMA == '2':
        act_out, loss, mets = 'sigmoid', 'binary_crossentropy', ['accuracy']
    else:
        act_out, loss, mets = 'softmax', 'categorical_crossentropy', ['accuracy']

    reg = l2(L2_FACTOR) if USAR_L2 else None

    model = Sequential()
    for i, n in enumerate(HIDDEN_LAYERS):
        if i == 0:
            model.add(Dense(n, activation='relu',
                            input_shape=(NUM_ENTRADAS,),
                            kernel_regularizer=reg))
        else:
            model.add(Dense(n, activation='relu', kernel_regularizer=reg))
        if USAR_DROPOUT:
            model.add(Dropout(DROPOUT_RATE))

    model.add(Dense(NUM_SALIDAS, activation=act_out))
    model.compile(optimizer=Adam(learning_rate=LEARNING_RATE),
                  loss=loss, metrics=mets)

    print("Resumen del modelo:")
    model.summary()
    print()
    return model


def entrenar_modelo(model, X_tr, y_tr, X_val, y_val):
    """ Paso 7: Entrenamiento"""
    history = model.fit(
        X_tr, y_tr,
        validation_data=(X_val, y_val),
        epochs=EPOCHS,
        verbose=1
    )
    return history


def graficar_perdida(hist):
    """ Paso 8: Graficar curva de pérdida"""
    plt.plot(hist.history['loss'], label='Train')
    if 'val_loss' in hist.history:
        plt.plot(hist.history['val_loss'], label='Val')
    plt.xlabel('Épocas')
    plt.ylabel('Pérdida')
    plt.legend()
    plt.grid(True)
    plt.show()


def predecir(model, X_muestra, sx, sy):
    """
     Paso 9: Predicción de una muestra
    - Regresión: devuelve valor(es) continuo(s) desescalado(s)
    - Binaria: devuelve 0 o 1
    - Múltiple: devuelve etiqueta entera 0,1,2,...
    """
    X_muestra = np.atleast_2d(X_muestra)
    Xe = sx.transform(X_muestra)
    yhat = model.predict(Xe)

    if TIPO_PROBLEMA == '1':
        # regresión: deshace el escalado y devuelve array
        return sy.inverse_transform(yhat)
    elif TIPO_PROBLEMA == '2':
        # clasificación binaria: umbral 0.5 → clase 1, sino 0
        return int(yhat[0,0] >= 0.5)
    else:
        # clasificación múltiple: argmax sobre softmax
        return int(np.argmax(yhat, axis=1)[0])


# -----------------------------
# FUNCIONES DE EXPORTACIÓN C 
# -----------------------------
def export_model_weights_to_c(layers, n=3):
    W = "\n\n///////////////////////////////// Variables Red Neuronal /////////////////////////////////\n"
    b = "\n"
    for k, layer in enumerate(layers):
        weights = layer.get_weights()
        if not weights: continue
        w, biases = weights[0].T, weights[1]
        rows, cols = w.shape
        W += f"double a{k}[{cols}];\n"
        W += f"double W{k+1}[{rows}][{cols}] = {{\n"
        for i in range(rows):
            row = ", ".join(f"{w[i,j]:.{n}f}" for j in range(cols))
            sep = "," if i < rows-1 else ""
            W += f"  {{{row}}}{sep}\n"
        W += "};\n"
        b_vals = ", ".join(f"{biases[i]:.{n}f}" for i in range(biases.shape[0]))
        b += f"double b{k+1}[{biases.shape[0]}] = {{{b_vals}}};\n"
    W += f"double a{len(layers)}[{biases.shape[0]}];\n"
    b += "double aux = 0.0;\n//////////////////////////////////////////////////////////////////\n\n\n"
    print(W + b)

def export_preprocessing_to_c(sx, sy, n=3):
    mx, sx_var = sx.mean_, np.sqrt(sx.var_)
    print("///////////////////////////////// Preprocesamiento Entradas /////////////////////////////////")
    print(f"double mean_x[{len(mx)}] = {{"+",".join(f"{v:.{n}f}" for v in mx)+"}};")
    print(f"double std_x[{len(sx_var)}]  = {{"+",".join(f"{v:.{n}f}" for v in sx_var)+"}};")
    if sy:
        my, sy_var = sy.mean_, np.sqrt(sy.var_)
        print("///////////////////////////////// Preprocesamiento Salidas /////////////////////////////////")
        print(f"double mean_y[{len(my)}] = {{"+",".join(f"{v:.{n}f}" for v in my)+"}};")
        print(f"double std_y[{len(sy_var)}]  = {{"+",".join(f"{v:.{n}f}" for v in sy_var)+"}};")
    print("///////////////////////////////////////////////////////////////////////////////////////////////////////\n\n")

def generate_forward_propagation_code(layers):
    print("///////////////////////////////// Código Forward Propagation /////////////////////////////////")
    for k, layer in enumerate(layers):
        weights = layer.get_weights()
        if not weights: continue
        w, b = weights[0].T, weights[1]
        rows, cols = w.shape
        act = layer.get_config()['activation']
        print(f"// Capa {k+1} ({act})")
        print(f"for(int i=0; i<{rows}; i++) {{")
        print("  double sum = 0.0;")
        print(f"  for(int j=0; j<{cols}; j++) sum += W{k+1}[i][j] * a{k}[j];")
        if act=='linear':
            print(f"  a{k+1}[i] = sum + b{k+1}[i];")
        else:
            print(f"  a{k+1}[i] = {act}(sum + b{k+1}[i]);")
        print("}\n")
    print("//////////////////////////////////////////////////////////////////////////////////////////\n\n")

# -----------------------------
# FUNCIONES DE EXPORTACIÓN MICROPYTHON
# -----------------------------
def export_model_weights_to_micropython(layers, n=3):
    print("\n# Pesos y sesgos de la red neuronal\n")
    for k, layer in enumerate(layers):
        layer_weights = layer.get_weights()
        if not layer_weights: continue
        w, biases = layer_weights
        w_ = np.round(w, n).tolist()
        b_ = np.round(biases, n).tolist()
        print(f"weights_{k} = {w_}")
        print(f"biases_{k} = {b_}\n")

def export_preprocessing_to_micropython(sx, sy, n=3):
    mean_x = np.round(sx.mean_, n).tolist()
    std_x  = np.round(np.sqrt(sx.var_), n).tolist()
    print("# Parámetros de preprocesamiento")
    print(f"mean_x = {mean_x}")
    print(f"std_x = {std_x}")
    if sy:
        mean_y = np.round(sy.mean_, n).tolist()
        std_y  = np.round(np.sqrt(sy.var_), n).tolist()
        print(f"mean_y = {mean_y}")
        print(f"std_y = {std_y}")
    print()

def generate_forward_propagation_function(layers):
    code = "\n# Función de forward de la red neuronal (MicroPython)\n"
    code += "def neural_network_forward(inputs):\n"
    code += "    import math\n"
    code += "    activations = inputs\n"
    for k, layer in enumerate(layers):
        weights = layer.get_weights()
        if not weights: continue
        w, b = weights
        act = layer.get_config()['activation']
        code += f"    # Capa {k+1} ({act})\n"
        code += f"    weights = weights_{k}\n"
        code += f"    biases = biases_{k}\n"
        code += "    z = []\n"
        code += "    for j in range(len(biases)):\n"
        code += "        s = sum(activations[i]*weights[i][j] for i in range(len(activations))) + biases[j]\n"
        code += "        z.append(s)\n"
        if act=='relu':
            code += "    activations = [max(0, v) for v in z]\n"
        elif act=='sigmoid':
            code += "    activations = [1/(1+math.exp(-v)) for v in z]\n"
        elif act=='softmax':
            code += "    exp_z = [math.exp(v) for v in z]\n"
            code += "    s = sum(exp_z)\n"
            code += "    activations = [v/s for v in exp_z]\n"
        else:
            code += "    activations = z\n"
        code += "\n"
    code += "    return activations\n"
    print(code)


if _name_ == "_main_":
    paso_titulo(1, "Carga de datos")
    df = cargar_datos()

    paso_titulo(2, "Preparar entradas y salidas")
    X, y = preparar_entradas_salidas(df)

    paso_titulo(3, "Dividir dataset")
    X_tr, X_val, y_tr, y_val = dividir_datos(X, y)

    paso_titulo(4, "Codificar etiquetas")
    y_tr, y_val = codificar_etiquetas(y_tr, y_val)

    paso_titulo(5, "Escalar datos")
    X_tr_s, X_val_s, y_tr_s, y_val_s, sx, sy = escalar_datos(X_tr, X_val, y_tr, y_val)

    paso_titulo(6, "Construir modelo")
    model = construir_modelo()

    paso_titulo(7, "Entrenar modelo")
    history = entrenar_modelo(model, X_tr_s, y_tr_s, X_val_s, y_val_s)

    paso_titulo(8, "Graficar pérdida")
    graficar_perdida(history)

    paso_titulo(9, "Predicción de ejemplo con vector")
    print("Vector de ejemplo:", EJEMPLO_VECTOR)
    resultado = predecir(model, EJEMPLO_VECTOR, sx, sy)
    print("Predicción:", resultado)

    if EXPORT_FORMAT == 'c':
        paso_titulo(10, "Exportar a C")
        export_model_weights_to_c(model.layers)
        export_preprocessing_to_c(sx, sy)
        generate_forward_propagation_code(model.layers)
    elif EXPORT_FORMAT == 'micropython':
        paso_titulo(10, "Exportar a MicroPython")
        export_model_weights_to_micropython(model.layers, n=3)
        export_preprocessing_to_micropython(sx, sy, n=3)
        generate_forward_propagation_function(model.layers)
    else:
        print(f"⚠️ Formato de exportación '{EXPORT_FORMAT}' no reconocido.")

    print("¡Proceso completo!")

========== Paso 10: Exportar a C ==========



///////////////////////////////// Variables Red Neuronal /////////////////////////////////
double a0[4];
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
double a1[16];
double W2[4][16] = {
  {-0.672, -0.819, 0.607, -0.287, -0.463, 0.450, -0.538, 0.945, 0.740, -0.574, -0.431, 0.622, 0.145, -0.430, -0.770, -0.813},
  {0.480, -0.779, 0.275, -0.217, -0.529, 0.956, -0.396, 0.621, 1.003, -0.140, -0.278, -0.352, -0.609, -0.950, -0.416, -0.615},
  {-0.717, -0.856, 0.673, -0.274, -0.443, 0.610, 0.026, -0.047, 0.987, 0.396, -0.521, -0.023, -0.257, -0.238, -0.511, -0.970},
  {0.386, -0.380, -0.486, -0.635, -0.845, -0.190, -0.583, -0.020, -0.172, -0.120, -0.616, -0.570, -0.222, -0.611, -0.321, -1.060}
};
double a2[4];

double b1[16] = {0.478, 0.569, 0.642, 0.288, 0.361, 0.585, 0.459, 0.227, 0.587, 0.102, 0.345, 0.062, -0.038, 0.408, 0.416, 0.606};
double b2[4] = {-0.215, 0.008, 0.116, -0.235};
double aux = 0.0;
//////////////////////////////////////////////////////////////////



///////////////////////////////// Preprocesamiento Entradas /////////////////////////////////
double mean_x[4] = {0.545,0.506,0.456,0.477}};
double std_x[4]  = {0.272,0.286,0.250,0.293}};
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////// Código Forward Propagation /////////////////////////////////
// Capa 1 (relu)
for(int i=0; i<16; i++) {
  double sum = 0.0;
  for(int j=0; j<4; j++) sum += W1[i][j] * a0[j];
  a1[i] = relu(sum + b1[i]);
}

// Capa 2 (softmax)
for(int i=0; i<4; i++) {
  double sum = 0.0;
  for(int j=0; j<16; j++) sum += W2[i][j] * a1[j];
  a2[i] = softmax(sum + b2[i]);
}
