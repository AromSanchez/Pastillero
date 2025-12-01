"""
Script de prueba para verificar la sincronización automática Arduino-Backend
"""
import requests
import json

# Configuración
BACKEND_URL = "http://10.147.1.123:8000/api/tratamientos/"
ARDUINO_IP = "10.147.1.1"  # IP del ESP32

def test_backend_to_arduino():
    """Prueba que el backend notifique correctamente al Arduino"""
    
    print("=" * 60)
    print("TEST: Sincronización Automática Backend -> Arduino")
    print("=" * 60)
    
    # 1. Obtener tratamientos actuales
    print("\n1. Obteniendo tratamientos actuales...")
    response = requests.get(BACKEND_URL)
    if response.status_code == 200:
        tratamientos = response.json()
        print(f"   ✅ {len(tratamientos)} tratamientos encontrados")
        for t in tratamientos:
            print(f"      - Compartimento {t['compartimento']}: {t['nombre_pastilla']} a las {t['hora_toma']}")
    else:
        print(f"   ❌ Error al obtener tratamientos: {response.status_code}")
        return
    
    # 2. Modificar un tratamiento (si existe)
    if tratamientos:
        tratamiento_id = tratamientos[0]['id']
        print(f"\n2. Modificando tratamiento ID {tratamiento_id}...")
        
        # Cambiar la hora de toma
        nuevo_dato = tratamientos[0].copy()
        nuevo_dato['nombre_pastilla'] = "Pastilla Actualizada TEST"
        
        response = requests.put(
            f"{BACKEND_URL}{tratamiento_id}/",
            json=nuevo_dato,
            headers={'Content-Type': 'application/json'}
        )
        
        if response.status_code == 200:
            print("   ✅ Tratamiento actualizado en backend")
            print("   ⏳ El backend debería haber notificado al Arduino automáticamente")
            print(f"   💡 Verifica el Serial Monitor del Arduino para confirmar")
        else:
            print(f"   ❌ Error al actualizar: {response.status_code}")
            print(f"   Response: {response.text}")
    else:
        print("\n2. No hay tratamientos para modificar")
    
    print("\n" + "=" * 60)
    print("INSTRUCCIONES:")
    print("=" * 60)
    print("1. Abre el Serial Monitor del Arduino (115200 baud)")
    print("2. Deberías ver el mensaje: '✅ Tratamientos actualizados desde backend'")
    print("3. Verifica que los datos mostrados coincidan con el backend")
    print("=" * 60)

def test_direct_arduino_post():
    """Prueba directa del endpoint /actualizar del Arduino"""
    
    print("\n" + "=" * 60)
    print("TEST DIRECTO: POST al Arduino")
    print("=" * 60)
    
    # Datos de prueba
    test_data = [
        {
            "id": 999,
            "compartimento": 1,
            "nombre_pastilla": "Test Directo",
            "hora_toma": "14:30:00",
            "repeticion": "DIARIO",
            "activo": True
        }
    ]
    
    try:
        print(f"\n1. Enviando POST a http://{ARDUINO_IP}/actualizar...")
        response = requests.post(
            f"http://{ARDUINO_IP}/actualizar",
            json=test_data,
            timeout=5
        )
        
        if response.status_code == 200:
            print("   ✅ Arduino respondió correctamente")
            print(f"   Response: {response.text}")
        else:
            print(f"   ❌ Error: {response.status_code}")
            print(f"   Response: {response.text}")
    except requests.exceptions.Timeout:
        print("   ⏱️ Timeout - El Arduino no respondió")
        print("   Verifica que:")
        print("      - El Arduino esté encendido")
        print("      - La IP sea correcta")
        print("      - El servidor HTTP esté iniciado")
    except requests.exceptions.ConnectionError:
        print("   🔌 Error de conexión")
        print("   Verifica que:")
        print("      - El Arduino esté en la misma red")
        print(f"      - La IP {ARDUINO_IP} sea correcta")
    except Exception as e:
        print(f"   ❌ Error inesperado: {e}")

if __name__ == "__main__":
    print("\n🧪 SUITE DE PRUEBAS - Sincronización Arduino-Backend\n")
    
    # Prueba 1: Flujo completo backend -> arduino
    test_backend_to_arduino()
    
    # Prueba 2: POST directo al Arduino
    input("\nPresiona ENTER para hacer prueba directa al Arduino...")
    test_direct_arduino_post()
    
    print("\n✅ Pruebas completadas\n")
