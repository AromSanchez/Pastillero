"""
Script de diagnóstico para verificar la sincronización Arduino-Backend
"""
import requests
import json

# Configuración
BACKEND_URL = "http://10.147.1.123:8000/api/tratamientos/"
ARDUINO_IP = "10.147.1.1"  # IP del ESP32

print("=" * 60)
print("DIAGNÓSTICO: Sincronización Arduino-Backend")
print("=" * 60)

# 1. Verificar que el backend responde
print("\n1. Verificando backend...")
try:
    response = requests.get(BACKEND_URL, timeout=5)
    if response.status_code == 200:
        tratamientos = response.json()
        print(f"   ✅ Backend OK - {len(tratamientos)} tratamientos")
    else:
        print(f"   ❌ Backend error: {response.status_code}")
except Exception as e:
    print(f"   ❌ No se puede conectar al backend: {e}")

# 2. Verificar que el Arduino responde
print("\n2. Verificando Arduino...")
try:
    # Intentar hacer un POST de prueba
    test_data = []
    response = requests.post(
        f"http://{ARDUINO_IP}/actualizar",
        json=test_data,
        timeout=5
    )
    if response.status_code == 200:
        print(f"   ✅ Arduino responde en {ARDUINO_IP}")
        print(f"   Respuesta: {response.text}")
    else:
        print(f"   ⚠️ Arduino respondió con código: {response.status_code}")
except requests.exceptions.Timeout:
    print(f"   ❌ Timeout - Arduino no responde en {ARDUINO_IP}")
    print(f"   Verifica:")
    print(f"      - El Arduino está encendido")
    print(f"      - La IP {ARDUINO_IP} es correcta")
    print(f"      - El servidor HTTP está iniciado")
except requests.exceptions.ConnectionError:
    print(f"   ❌ No se puede conectar al Arduino en {ARDUINO_IP}")
    print(f"   Verifica:")
    print(f"      - El Arduino está en la misma red")
    print(f"      - La IP es correcta (revisa Serial Monitor)")
except Exception as e:
    print(f"   ❌ Error: {e}")

# 3. Verificar configuración en Django
print("\n3. Verificando configuración Django...")
try:
    config_url = "http://10.147.1.123:8000/admin/pastillas/configuracionarduino/"
    print(f"   Ve a: {config_url}")
    print(f"   Asegúrate de que:")
    print(f"      - IP Arduino: {ARDUINO_IP}")
    print(f"      - Puerto: 80")
    print(f"      - Activo: ✅ Marcado")
except Exception as e:
    print(f"   ❌ Error: {e}")

print("\n" + "=" * 60)
print("INSTRUCCIONES:")
print("=" * 60)
print("1. Abre el Serial Monitor del Arduino (115200 baud)")
print("2. Busca la línea 'IP del ESP32: X.X.X.X'")
print("3. Usa esa IP en la configuración de Django")
print("4. Intenta editar un tratamiento de nuevo")
print("=" * 60)
