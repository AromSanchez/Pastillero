# Soluciones para Conectar Backend (Render) con Arduino Local

## El Problema

Tu backend está en Render (internet público), pero tu Arduino está en tu red local (10.147.1.1). Render **NO puede** enviar POST directamente a una IP privada.

## 🎯 Soluciones (de más simple a más completa)

---

## Solución 1: ngrok (Recomendada para Desarrollo/Pruebas) ⭐

**Ventajas:**
- ✅ Gratis
- ✅ Muy fácil de configurar (5 minutos)
- ✅ No requiere cambios en tu red
- ✅ Perfecto para pruebas

**Desventajas:**
- ⚠️ La URL cambia cada vez que reinicias ngrok (en plan gratuito)
- ⚠️ Debes mantener ngrok corriendo en tu PC

### Paso a Paso:

#### 1. Descargar ngrok

1. Ve a [ngrok.com](https://ngrok.com)
2. Regístrate gratis
3. Descarga ngrok para Windows
4. Descomprime el archivo

#### 2. Configurar ngrok

Abre PowerShell en la carpeta de ngrok y ejecuta:

```powershell
# Autenticar (usa tu token de ngrok.com)
.\ngrok.exe authtoken TU_TOKEN_DE_NGROK

# Exponer el puerto 80 del Arduino
.\ngrok.exe http 10.147.1.1:80
```

Verás algo como:
```
Forwarding   https://abc123.ngrok.io -> http://10.147.1.1:80
```

#### 3. Actualizar Django

En el admin de Django (`https://pastillero-backend.onrender.com/admin/`):

1. Ve a **Pastillas → Configuración Arduino**
2. Cambia la IP a: `abc123.ngrok.io` (sin http://)
3. Puerto: `443` (ngrok usa HTTPS)
4. Guarda

#### 4. Actualizar código Django

Modifica `pastillas/views.py`, función `notificar_arduino_actualizacion()`:

```python
def notificar_arduino_actualizacion():
    try:
        config = ConfiguracionArduino.objects.first()
        if not config or not config.activo:
            return False
        
        tratamientos = Tratamiento.objects.filter(activo=True)
        serializer = TratamientoSerializer(tratamientos, many=True)
        
        # Usar HTTPS si el puerto es 443 (ngrok)
        protocol = "https" if config.puerto == 443 else "http"
        url = f"{protocol}://{config.ip_arduino}/actualizar"
        
        response = requests.post(url, json=serializer.data, timeout=10)
        
        if response.status_code == 200:
            print(f"✅ Arduino actualizado en {config.ip_arduino}")
            return True
        else:
            print(f"❌ Error: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ Error: {e}")
        return False
```

#### 5. ¡Listo!

Ahora cuando edites un tratamiento en Django, el backend enviará POST a ngrok, que lo redirige a tu Arduino local.

**Mantén ngrok corriendo** mientras quieras que funcione la sincronización.

---

## Solución 2: Polling desde Arduino (Sin ngrok)

**Ventajas:**
- ✅ No necesitas ngrok
- ✅ Funciona siempre
- ✅ Más simple

**Desventajas:**
- ⚠️ No es "tiempo real" (el Arduino consulta cada X segundos)
- ⚠️ Consume más batería/recursos

### Implementación:

Modifica `pastillero.ino` para que el Arduino consulte periódicamente:

```cpp
unsigned long ultimaConsulta = 0;
const unsigned long INTERVALO_CONSULTA = 30000; // 30 segundos

void loop() {
  server.handleClient();
  
  // Consultar backend cada 30 segundos
  if (millis() - ultimaConsulta >= INTERVALO_CONSULTA) {
    obtenerTratamientos();
    ultimaConsulta = millis();
  }
  
  // ... resto del código ...
}
```

**Elimina** la sincronización automática del backend (ya no la necesitas).

---

## Solución 3: Arduino Cloud IoT (Más Profesional)

**Ventajas:**
- ✅ Solución profesional
- ✅ Arduino siempre accesible
- ✅ No depende de tu PC

**Desventajas:**
- ⚠️ Más complejo de configurar
- ⚠️ Requiere Arduino compatible (ESP32 sí lo es)
- ⚠️ Plan gratuito limitado

### Pasos:

1. Ve a [create.arduino.cc/iot](https://create.arduino.cc/iot)
2. Crea una "Thing"
3. Configura variables cloud
4. Usa la librería `ArduinoIoTCloud`
5. El backend se comunica vía API de Arduino Cloud

**Nota:** Esto requiere reescribir bastante código. Solo recomendado si quieres una solución permanente.

---

## Solución 4: VPN o Túnel SSH (Avanzado)

Configurar una VPN o túnel SSH permanente entre Render y tu red local.

**No recomendado** para este proyecto por complejidad.

---

## 🎯 Recomendación

Para tu caso, te recomiendo:

### Para Pruebas/Desarrollo:
**Usa ngrok** (Solución 1) - Es gratis, fácil y funciona perfecto.

### Para Producción Real:
**Usa Polling** (Solución 2) - El Arduino consulta cada 30 segundos. Es simple y confiable.

---

## 📝 Script para ngrok (Windows)

Crea un archivo `start_ngrok.bat`:

```batch
@echo off
echo Iniciando ngrok para Arduino...
cd C:\ruta\a\ngrok
ngrok.exe http 10.147.1.1:80
pause
```

Doble click para iniciar ngrok fácilmente.

---

## 🔧 Troubleshooting

### ngrok dice "connection refused"
- Verifica que el Arduino esté encendido
- Verifica que la IP sea correcta (10.147.1.1)
- Asegúrate de que el servidor HTTP del Arduino esté corriendo

### Backend no puede conectar a ngrok
- Verifica que la URL de ngrok esté correcta en ConfiguracionArduino
- Usa HTTPS (puerto 443) para ngrok
- Revisa los logs de Render

### ngrok se desconecta
- En plan gratuito, ngrok se desconecta después de 2 horas
- Reinicia ngrok cuando sea necesario
- Considera ngrok Pro si necesitas conexión permanente

---

## 💡 Consejo Final

Para este proyecto de pruebas, **usa ngrok**. Es la forma más rápida de hacer que funcione todo.

Si quieres algo más permanente sin complicaciones, **usa polling** (el Arduino consulta cada 30s).
