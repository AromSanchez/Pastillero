# Solución Final: Arduino Autónomo con Polling

## El Escenario Real

Tu Arduino estará:
- ✅ Conectado solo a alimentación 5V (sin laptop)
- ✅ Conectado a WiFi
- ✅ Funcionando de forma autónoma

El backend estará en Render (internet público).

## 🎯 Solución: Polling (Consulta Periódica)

El Arduino **consultará al backend cada 60 segundos** para ver si hay cambios en los tratamientos.

### Ventajas:
- ✅ No necesita ngrok ni servicios externos
- ✅ Funciona 24/7 sin laptop
- ✅ Simple y confiable
- ✅ El Arduino siempre está actualizado (máximo 1 minuto de retraso)

### Cómo Funciona:

```
Arduino (cada 60s) → GET → Backend Render → Respuesta con tratamientos
```

Si editaste un tratamiento en el admin de Django, el Arduino lo detectará en la próxima consulta (máximo 60 segundos después).

## 📝 Cambios Realizados

### 1. Arduino (`pastillero.ino`)

Se agregó:
- Variable `ultimaConsultaBackend` - Guarda cuándo fue la última consulta
- Variable `INTERVALO_CONSULTA` - Cada cuánto consultar (60 segundos)
- En `loop()`: Consulta automática cada 60 segundos

```cpp
// En loop()
if (millis() - ultimaConsultaBackend >= INTERVALO_CONSULTA) {
    Serial.println("[POLLING] Consultando backend...");
    obtenerTratamientos();
    ultimaConsultaBackend = millis();
}
```

### 2. URLs del Arduino

Actualiza las URLs en `pastillero.ino` para apuntar a Render:

```cpp
// Cambiar de local a Render
const char* backendURL = "https://TU-APP.onrender.com/api/tratamientos/";
const char* registrarTomaURL = "https://TU-APP.onrender.com/api/registrar-toma/";
const char* notificarRecordatorioURL = "https://TU-APP.onrender.com/api/notificar-recordatorio/";
```

Reemplaza `TU-APP` con el nombre de tu servicio en Render.

## 🔧 Configuración Completa

### Paso 1: Desplegar Backend en Render

Sigue `DEPLOY_SIMPLE.md`:
1. Sube a GitHub
2. Crea Web Service en Render
3. Espera el deploy
4. Tu backend estará en: `https://tu-app.onrender.com`

### Paso 2: Actualizar Arduino

1. Abre `pastillero.ino`
2. Cambia las 3 URLs al dominio de Render
3. Carga el código al ESP32

### Paso 3: Probar

1. Enciende el Arduino (solo con alimentación 5V)
2. Abre Serial Monitor para ver los logs
3. Verás cada 60 segundos: `[POLLING] Consultando backend...`
4. Edita un tratamiento en Django admin
5. En máximo 60 segundos, el Arduino lo detectará

## ⏱️ Ajustar Frecuencia de Consulta

Si quieres que consulte más o menos frecuente, cambia `INTERVALO_CONSULTA`:

```cpp
// Opciones:
const unsigned long INTERVALO_CONSULTA = 30000;   // 30 segundos (más rápido)
const unsigned long INTERVALO_CONSULTA = 60000;   // 60 segundos (recomendado)
const unsigned long INTERVALO_CONSULTA = 120000;  // 2 minutos (ahorra batería)
const unsigned long INTERVALO_CONSULTA = 300000;  // 5 minutos (muy espaciado)
```

**Recomendación:** 60 segundos es un buen balance entre actualización rápida y no saturar el backend.

## 📊 Comparación con Sincronización Automática

| Característica | Polling (Actual) | Push (ngrok) |
|----------------|------------------|--------------|
| **Funciona sin laptop** | ✅ Sí | ❌ No (necesita PC con ngrok) |
| **Tiempo real** | ⚠️ Hasta 60s de retraso | ✅ Instantáneo |
| **Complejidad** | ✅ Simple | ⚠️ Requiere ngrok corriendo |
| **Confiabilidad** | ✅ Alta | ⚠️ Depende de ngrok |
| **Consumo de recursos** | ⚠️ Consultas periódicas | ✅ Solo cuando hay cambios |

Para tu caso (Arduino autónomo), **Polling es la mejor opción**.

## 🎉 Resultado Final

Tu sistema funcionará así:

1. **Arduino encendido** (solo alimentación 5V)
2. **Conectado a WiFi**
3. **Consulta backend cada 60s** para ver cambios
4. **Registra tomas** en el backend
5. **Envía notificaciones** a Telegram
6. **Funciona 24/7** sin necesidad de laptop

## 🔍 Logs del Arduino

En el Serial Monitor verás:

```
Conectando a WiFi...
Conectado a WiFi!
IP del ESP32: 10.147.1.1
Servidor HTTP iniciado en puerto 80

[POLLING] Consultando backend por actualizaciones...
Tratamiento en compartimento 1 (ID: 1): Aspirina a las 08:00:00
Tratamiento en compartimento 2 (ID: 2): Ibuprofeno a las 14:00:00

[POLLING] Consultando backend por actualizaciones...
(Sin cambios)

[POLLING] Consultando backend por actualizaciones...
✅ Tratamientos actualizados desde backend
Tratamiento en compartimento 1 (ID: 1): Paracetamol a las 09:00:00  ← CAMBIO DETECTADO
```

## 💡 Notas Importantes

1. **El servidor HTTP del Arduino sigue activo** - Si en el futuro quieres usar ngrok (con laptop), funcionará sin cambios
2. **Consumo de datos** - Cada consulta es ~1-2KB. En 24 horas: ~2-3MB (insignificante)
3. **Render Free Tier** - Se duerme después de 15 min de inactividad. El Arduino lo "despertará" con cada consulta
4. **Primer arranque** - El Arduino consulta inmediatamente al iniciar, luego cada 60s

## ✅ Checklist

- [ ] Backend desplegado en Render
- [ ] URLs actualizadas en `pastillero.ino`
- [ ] Código cargado al ESP32
- [ ] Arduino funcionando solo con alimentación 5V
- [ ] Serial Monitor muestra consultas cada 60s
- [ ] Prueba editando un tratamiento en Django
- [ ] Arduino detecta el cambio en máximo 60s

¡Listo! Tu pastillero inteligente funcionará completamente autónomo.
