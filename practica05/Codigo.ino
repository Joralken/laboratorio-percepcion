import numpy as np
import skfuzzy as fuzz
from skfuzzy import control as ctrl


# -----------------------------
# 1) DEFINICIÓN DE VARIABLES
# -----------------------------
def define_variables():
    """
    Define las variables difusas de entrada y salida.
    Rangos elegidos según discusión (Monterrey / ESP32 LDR ~0-1500).
    """
    # Entradas
    temperatura = ctrl.Antecedent(np.arange(0, 51, 1), 'temperatura')               # 0-50 °C
    humedad_ambiental = ctrl.Antecedent(np.arange(0, 101, 1), 'humedad_ambiental')  # 0-100 %
    humedad_suelo = ctrl.Antecedent(np.arange(0, 101, 1), 'humedad_suelo')          # 0-100 %
    luz = ctrl.Antecedent(np.arange(0, 1501, 1), 'luz')                             # 0-1500 (ESP32)

    # Salidas (0..1 para facilidad; representan nivel de activación)
    ventilador = ctrl.Consequent(np.arange(0, 1.01, 0.01), 'ventilador')
    luces = ctrl.Consequent(np.arange(0, 1.01, 0.01), 'luces')
    bomba = ctrl.Consequent(np.arange(0, 1.01, 0.01), 'bomba')
    buzzer = ctrl.Consequent(np.arange(0, 1.01, 0.01), 'buzzer')

    return temperatura, humedad_ambiental, humedad_suelo, luz, ventilador, luces, bomba, buzzer


# -----------------------------
# 2) FUNCIONES DE PERTENENCIA
# -----------------------------
def set_membership_functions(t, ha, hs, l, vent, luzc, bom, buz):
    """
    Define las funciones de pertenencia para entradas y salidas.
    """
    # Entradas: automf(3) para baja/media/alta
    t.automf(3, names=['baja', 'media', 'alta'])
    ha.automf(3, names=['baja', 'media', 'alta'])

    # Humedad del suelo
    hs['seco']   = fuzz.trimf(hs.universe, [0, 0, 40])
    hs['optimo'] = fuzz.trimf(hs.universe, [25, 50, 75])
    hs['humedo'] = fuzz.trimf(hs.universe, [60, 100, 100])

    # Luz (0-1500)
    l['baja']  = fuzz.trimf(l.universe, [0, 0, 300])
    l['media'] = fuzz.trimf(l.universe, [200, 700, 1300])
    l['alta']  = fuzz.trimf(l.universe, [1000, 1500, 1500])

    # Salidas: bajo/medio/alto (0..1)
    for out in (vent, luzc, bom, buz):
        out['bajo']  = fuzz.trimf(out.universe, [0.0, 0.0, 0.5])   # apagado / bajo
        out['medio'] = fuzz.trimf(out.universe, [0.0, 0.5, 1.0])   # medio
        out['alto']  = fuzz.trimf(out.universe, [0.5, 1.0, 1.0])   # fuerte / encendido


# -----------------------------
# 3) REGLAS (30 reglas ajustadas)
# -----------------------------
def define_rules(t, ha, hs, l, vent, luzc, bom, buz):
    """
    Devuelve la lista de reglas construidas para el sistema.
    Mismo contenido que antes, pero separando múltiples consecuentes en reglas individuales.
    """
    R = []

    # 1-6: reglas básicas temp → ventilador
    R.append(ctrl.Rule(t['alta'] & ha['alta'], vent['alto']))        # 1
    R.append(ctrl.Rule(t['alta'] & ha['media'], vent['medio']))      # 2

    # 3 antes: vent['alto'] & buz['alto']
    R.append(ctrl.Rule(t['alta'] & ha['baja'], vent['alto']))        # 3a
    R.append(ctrl.Rule(t['alta'] & ha['baja'], buz['alto']))         # 3b

    R.append(ctrl.Rule(t['media'] & ha['alta'], vent['medio']))      # 4
    R.append(ctrl.Rule(t['media'] & ha['baja'], vent['bajo']))       # 5
    R.append(ctrl.Rule(t['baja'], vent['bajo']))                     # 6

    # 7-9: humedad suelo → bomba
    R.append(ctrl.Rule(hs['seco'],   bom['alto']))                   # 7
    R.append(ctrl.Rule(hs['optimo'], bom['medio']))                  # 8
    R.append(ctrl.Rule(hs['humedo'], bom['bajo']))                   # 9

    # 10-12: luz → luces
    R.append(ctrl.Rule(l['baja'],  luzc['alto']))                    # 10
    R.append(ctrl.Rule(l['media'], luzc['medio']))                   # 11
    R.append(ctrl.Rule(l['alta'],  luzc['bajo']))                    # 12

    # 13-15: combinadas luz+temp
    # 13 antes: luzc['alto'] & buz['alto']
    R.append(ctrl.Rule(l['baja'] & t['alta'], luzc['alto']))         # 13a
    R.append(ctrl.Rule(l['baja'] & t['alta'], buz['alto']))          # 13b

    R.append(ctrl.Rule(l['baja'] & ha['baja'], luzc['alto']))        # 14

    # 15 antes: bom['alto'] & vent['alto']
    R.append(ctrl.Rule(t['alta'] & hs['seco'], bom['alto']))         # 15a
    R.append(ctrl.Rule(t['alta'] & hs['seco'], vent['alto']))        # 15b

    # 16-18: temp y luz / suelo
    R.append(ctrl.Rule(t['alta']  & l['alta'],  vent['alto']))       # 16
    R.append(ctrl.Rule(t['media'] & hs['seco'], bom['alto']))        # 17
    R.append(ctrl.Rule(t['baja']  & hs['seco'], bom['alto']))        # 18

    # 19-21: condiciones que piden luces o bomba/buzzer
    R.append(ctrl.Rule(t['baja'] & l['baja'], luzc['alto']))         # 19
    R.append(ctrl.Rule(ha['alta'] & l['baja'], luzc['alto']))        # 20

    # 21 antes: bom['alto'] & buz['alto']
    R.append(ctrl.Rule(ha['baja'] & hs['seco'], bom['alto']))        # 21a
    R.append(ctrl.Rule(ha['baja'] & hs['seco'], buz['alto']))        # 21b

    # 22-24: combinaciones humedad/ventilador/luz
    R.append(ctrl.Rule(ha['baja'] & l['alta'], vent['bajo']))        # 22
    R.append(ctrl.Rule(l['alta'] & t['alta'], vent['alto']))         # 23
    R.append(ctrl.Rule(l['alta'] & t['baja'], vent['bajo']))         # 24

    # 25-27: condiciones neutrales
    # 25 antes: vent['bajo'] & bom['bajo']
    R.append(ctrl.Rule(ha['media'] & hs['optimo'], vent['bajo']))    # 25a
    R.append(ctrl.Rule(ha['media'] & hs['optimo'], bom['bajo']))     # 25b

    R.append(ctrl.Rule(ha['alta'] & t['media'], vent['medio']))      # 26
    R.append(ctrl.Rule(ha['baja'] & t['media'], vent['bajo']))       # 27

    # 28-30: reglas compuestas y de seguridad
    R.append(ctrl.Rule(hs['humedo'] & l['baja'], luzc['alto']))      # 28

    # 29 antes: vent['alto'] & luzc['alto']
    R.append(ctrl.Rule(t['alta'] & hs['humedo'] & l['baja'], vent['alto']))   # 29a
    R.append(ctrl.Rule(t['alta'] & hs['humedo'] & l['baja'], luzc['alto']))   # 29b

    # 30 antes: vent['alto'] & buz['alto'] & luzc['alto']
    R.append(ctrl.Rule(t['alta'] & ha['baja'] & l['baja'], vent['alto']))     # 30a
    R.append(ctrl.Rule(t['alta'] & ha['baja'] & l['baja'], buz['alto']))      # 30b
    R.append(ctrl.Rule(t['alta'] & ha['baja'] & l['baja'], luzc['alto']))     # 30c

    return R


# -----------------------------
# 4) UTILIDAD: ejecutar pruebas
# -----------------------------
def run_tests(control_system, tests):
    """
    Ejecuta una lista de pruebas sobre el sistema difuso
    y evita errores cuando alguna salida queda en None.
    """
    for p in tests:
        # nueva simulación por cada prueba
        sim = ctrl.ControlSystemSimulation(control_system)

        # entradas
        sim.input['temperatura'] = p['temperatura']
        sim.input['humedad_ambiental'] = p['humedad_ambiental']
        sim.input['humedad_suelo'] = p['humedad_suelo']
        sim.input['luz'] = p['luz']

        # inferencia
        sim.compute()

        # salidas (pueden ser None)
        ventilador_val = sim.output.get('ventilador', None)
        luces_val      = sim.output.get('luces', None)
        bomba_val      = sim.output.get('bomba', None)
        buzzer_val     = sim.output.get('buzzer', None)

        def fmt(v):
            return "N/A" if v is None else f"{v:.3f}"

        print("--------------------------------------------------------")
        print("Entradas:", p)
        print(f"  Ventilador = {fmt(ventilador_val)}")
        print(f"  Luces      = {fmt(luces_val)}")
        print(f"  Bomba      = {fmt(bomba_val)}")
        print(f"  Buzzer     = {fmt(buzzer_val)}")
        print("--------------------------------------------------------\n")


# -----------------------------
# 5) MAIN
# -----------------------------
if __name__ == "__main__":
    # 1. Variables difusas
    t, ha, hs, l, vent, luzc, bom, buz = define_variables()

    # 2. Funciones de pertenencia
    set_membership_functions(t, ha, hs, l, vent, luzc, bom, buz)

    # 3. Reglas
    reglas = define_rules(t, ha, hs, l, vent, luzc, bom, buz)

    # 4. Sistema de control
    sistema = ctrl.ControlSystem(reglas)

    # 5. Casos de prueba
    pruebas = [
        # Caso extremo ALTO (emergencia)
        {'temperatura': 40, 'humedad_ambiental': 20, 'humedad_suelo': 10, 'luz': 200},

        # Caso extremo BAJO (estabilidad)
        {'temperatura': 15, 'humedad_ambiental': 80, 'humedad_suelo': 80, 'luz': 1400},

        # Casos intermedios
        {'temperatura': 30, 'humedad_ambiental': 50, 'humedad_suelo': 40, 'luz': 500},
        {'temperatura': 25, 'humedad_ambiental': 35, 'humedad_suelo': 20, 'luz': 300},
    ]

    # 6. Ejecutar
    run_tests(sistema, pruebas)
