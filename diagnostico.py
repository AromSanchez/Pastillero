import os
import django

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'backend.settings')
django.setup()

from pastillas.models import ConfiguracionArduino
import requests

print("=" * 60)
print("DIAGNOSTICO DE SINCRONIZACION")
print("=" * 60)

# 1. Verificar configuración en BD
print("\n1. Configuracion en base de datos:")
config = ConfiguracionArduino.objects.first()

if config:
    print(f"   IP Arduino: {config.ip_arduino}")
    print(f"   Puerto: {config.puerto}")
    print(f"   Activo: {config.activo}")
    arduino_ip = config.ip_arduino
else:
    print("   [!] No hay configuracion de Arduino")
    print("   Creando configuracion por defecto...")
    config = ConfiguracionArduino.objects.create(
        ip_arduino="10.147.1.1",
        puerto=80,
        activo=True
    )
    print(f"   [OK] Configuracion creada: {config.ip_arduino}")
    arduino_ip = "10.147.1.1"

# 2. Probar conexión con Arduino
print(f"\n2. Probando conexion con Arduino en {arduino_ip}...")
try:
    response = requests.post(
        f"http://{arduino_ip}/actualizar",
        json=[],
        timeout=5
    )
    if response.status_code == 200:
        print(f"   [OK] Arduino responde correctamente")
        print(f"   Respuesta: {response.text}")
    else:
        print(f"   [ERROR] Arduino respondio con codigo: {response.status_code}")
except requests.exceptions.Timeout:
    print(f"   [ERROR] Timeout - Arduino no responde")
    print(f"   Verifica que el Arduino este encendido y en la red")
except requests.exceptions.ConnectionError:
    print(f"   [ERROR] No se puede conectar al Arduino")
    print(f"   La IP {arduino_ip} puede ser incorrecta")
    print(f"   Revisa el Serial Monitor para ver la IP real del ESP32")
except Exception as e:
    print(f"   [ERROR] {e}")

print("\n" + "=" * 60)
print("PASOS SIGUIENTES:")
print("=" * 60)
print("1. Abre el Serial Monitor del Arduino (115200 baud)")
print("2. Busca la linea 'IP del ESP32: X.X.X.X'")
print("3. Si la IP es diferente a la configurada, actualiza en:")
print("   http://10.147.1.123:8000/admin/pastillas/configuracionarduino/")
print("=" * 60)
