# 📋 Plantillas de Registro de Pruebas
## Pastillero Inteligente - Formatos para Documentación

---

## 🔧 Sesión de Pruebas - Información General

**Fecha**: ________________  
**Hora de inicio**: ________________  
**Hora de fin**: ________________  
**Ejecutor(es)**: ________________  
**Versión Firmware ESP32**: ________________  
**Versión Backend Django**: ________________  
**Versión Frontend React**: ________________  
**Condiciones ambientales**:
- Temperatura: ______°C
- Humedad: ______%
- Ubicación: ________________

---

## 📊 Plantilla 1: Prueba de Servos (H1)

### Datos de Prueba - Compartimento 1
| Intento | Ángulo Apertura (°) | Tiempo Apertura (s) | Ángulo Cierre (°) | Tiempo Cierre (s) | ✓/✗ |
|---------|---------------------|---------------------|-------------------|-------------------|-----|
| 1 | | | | | |
| 2 | | | | | |
| 3 | | | | | |

**Promedio Apertura**: ______° en ______s  
**Promedio Cierre**: ______° en ______s  
**Observaciones**: ________________________________________________

### Datos de Prueba - Compartimento 2
| Intento | Ángulo Apertura (°) | Tiempo Apertura (s) | Ángulo Cierre (°) | Tiempo Cierre (s) | ✓/✗ |
|---------|---------------------|---------------------|-------------------|-------------------|-----|
| 1 | | | | | |
| 2 | | | | | |
| 3 | | | | | |

**Promedio Apertura**: ______° en ______s  
**Promedio Cierre**: ______° en ______s  
**Observaciones**: ________________________________________________

### Datos de Prueba - Compartimento 3
| Intento | Ángulo Apertura (°) | Tiempo Apertura (s) | Ángulo Cierre (°) | Tiempo Cierre (s) | ✓/✗ |
|---------|---------------------|---------------------|-------------------|-------------------|-----|
| 1 | | | | | |
| 2 | | | | | |
| 3 | | | | | |

**Promedio Apertura**: ______° en ______s  
**Promedio Cierre**: ______° en ______s  
**Observaciones**: ________________________________________________

### Datos de Prueba - Compartimento 4
| Intento | Ángulo Apertura (°) | Tiempo Apertura (s) | Ángulo Cierre (°) | Tiempo Cierre (s) | ✓/✗ |
|---------|---------------------|---------------------|-------------------|-------------------|-----|
| 1 | | | | | |
| 2 | | | | | |
| 3 | | | | | |

**Promedio Apertura**: ______° en ______s  
**Promedio Cierre**: ______° en ______s  
**Observaciones**: ________________________________________________

**RESULTADO GENERAL**: ☐ APROBADA ☐ RECHAZADA  
**Criterio de éxito**: Ángulo 85-90°, tiempo <2s

---

## 🔊 Plantilla 2: Prueba de Buzzer (H2)

### Medición de Patrón de Beeps
| Medición | Tiempo ON (ms) | Tiempo OFF (ms) | Ciclos observados | ✓/✗ |
|----------|----------------|-----------------|-------------------|-----|
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |

**Promedio ON**: ______ms (esperado: 300ms)  
**Promedio OFF**: ______ms (esperado: 200ms)

### Medición de Nivel Sonoro
| Distancia | Nivel dB | ¿Audible? | Calidad Sonido (1-5) |
|-----------|----------|-----------|----------------------|
| 1 metro | | ☐ Sí ☐ No | |
| 3 metros | | ☐ Sí ☐ No | |
| 5 metros | | ☐ Sí ☐ No | |

### Encuesta de Percepción (5 usuarios)
| Usuario | ¿Audible a 5m? | ¿Agradable? (1-5) | Comentarios |
|---------|----------------|-------------------|-------------|
| 1 | ☐ Sí ☐ No | | |
| 2 | ☐ Sí ☐ No | | |
| 3 | ☐ Sí ☐ No | | |
| 4 | ☐ Sí ☐ No | | |
| 5 | ☐ Sí ☐ No | | |

**Calificación promedio**: ______/5  
**RESULTADO**: ☐ APROBADA ☐ RECHAZADA  
**Criterio de éxito**: Audible a 5m, calificación ≥3/5

---

## 🔘 Plantilla 3: Prueba de Botón (H3)

### Prueba de Debounce
**Presiones realizadas**: 20  
**Respuestas registradas**: ______  
**Falsos positivos**: ______  
**Falsos negativos**: ______

### Medición de Tiempo de Respuesta
| Presión | Tiempo de respuesta (ms) | ✓/✗ |
|---------|--------------------------|-----|
| 1 | | |
| 2 | | |
| 3 | | |
| 4 | | |
| 5 | | |
| 6 | | |
| 7 | | |
| 8 | | |
| 9 | | |
| 10 | | |

**Tiempo promedio**: ______ms  
**Tiempo mínimo**: ______ms  
**Tiempo máximo**: ______ms

**RESULTADO**: ☐ APROBADA ☐ RECHAZADA  
**Criterio de éxito**: 100% de presiones registradas, 0 falsos positivos

---

## 🌐 Plantilla 4: Carga de Tratamientos (B1)

### Tratamientos Configurados en Backend
| Comp. | ID | Nombre Pastilla | Dosis | Stock | Repetición | Hora Toma | Activo |
|-------|----|-----------------| ------|-------|------------|-----------|--------|
| 1 | | | | | | | ☐ |
| 2 | | | | | | | ☐ |
| 3 | | | | | | | ☐ |
| 4 | | | | | | | ☐ |

### Tratamientos Cargados en ESP32 (desde Serial Monitor)
| Comp. | ID | Nombre Pastilla | Hora Toma | ¿Coincide? |
|-------|----|-----------------|-----------| -----------|
| 1 | | | | ☐ Sí ☐ No |
| 2 | | | | ☐ Sí ☐ No |
| 3 | | | | ☐ Sí ☐ No |
| 4 | | | | ☐ Sí ☐ No |

**Tiempo de carga**: ______s  
**HTTP Response Code**: ______  
**RESULTADO**: ☐ APROBADA ☐ RECHAZADA  
**Criterio de éxito**: 4/4 tratamientos cargados correctamente

---

## ✅ Plantilla 5: Registro de Toma TOMADA (B2)

### Configuración de Prueba
**Tratamiento ID**: ______  
**Compartimento**: ______  
**Nombre Pastilla**: ________________  
**Hora programada**: ______:______  
**Stock inicial**: ______  
**Dosis**: ______

### Registro de Eventos
| Evento | Hora Real | Timestamp |
|--------|-----------|-----------|
| Alarma activada | | |
| Botón presionado | | |
| Servo abierto | | |
| POST a backend | | |
| Respuesta recibida | | |

### Validación en Backend
**Registro creado en HistorialToma**: ☐ Sí ☐ No  
**ID del registro**: ______  
**Estado registrado**: ________________  
**Stock después**: ______  
**Stock reducido correctamente**: ☐ Sí ☐ No (esperado: inicial - dosis)

**Latencia total**: ______ segundos  
**RESULTADO**: ☐ APROBADA ☐ RECHAZADA  
**Criterio de éxito**: Registro creado, stock reducido en `dosis`

---

## ⚠️ Plantilla 6: Registro de Toma OMITIDA (B3)

### Configuración de Prueba
**Tratamiento ID**: ______  
**Compartimento**: ______  
**Nombre Pastilla**: ________________  
**Hora programada**: ______:______  
**Stock inicial**: ______

### Registro de Eventos
| Evento | Hora Real | Timestamp |
|--------|-----------|-----------|
| Alarma activada | | |
| Timeout iniciado | | |
| Timeout completado (5 min) | | |
| POST a backend | | |
| Respuesta recibida | | |

### Validación
**Tiempo transcurrido**: ______ minutos ______ segundos  
**Registro creado**: ☐ Sí ☐ No  
**Estado registrado**: ________________  
**Stock modificado**: ☐ Sí ☐ No  
**Stock después**: ______ (esperado: sin cambios)

**RESULTADO**: ☐ APROBADA ☐ RECHAZADA  
**Criterio de éxito**: Registro OMITIDA después de 5 min, stock sin cambios

---

## 📱 Plantilla 7: Notificaciones Telegram (T1 y T2)

### Prueba T1: Recordatorio de Toma
**Tratamiento ID**: ______  
**Nombre Pastilla**: ________________  
**Compartimento**: ______  
**Hora programada**: ______:______

| Evento | Hora Real | Timestamp |
|--------|-----------|-----------|
| Alarma activada | | |
| POST a /notificar-recordatorio/ | | |
| Mensaje recibido en Telegram | | |

**Latencia**: ______ segundos  
**Contenido del mensaje**:
```
_____________________________________________________
_____________________________________________________
_____________________________________________________
```

**Información correcta**: ☐ Sí ☐ No  
**RESULTADO T1**: ☐ APROBADA ☐ RECHAZADA

### Prueba T2: Confirmación de Toma
**Estado registrado**: ☐ TOMADA ☐ OMITIDA

| Evento | Hora Real | Timestamp |
|--------|-----------|-----------|
| Toma registrada | | |
| Mensaje recibido en Telegram | | |

**Latencia**: ______ segundos  
**Contenido del mensaje**:
```
_____________________________________________________
_____________________________________________________
_____________________________________________________
```

**Emoji correcto**: ☐ ✅ (TOMADA) ☐ ⚠️ (OMITIDA)  
**RESULTADO T2**: ☐ APROBADA ☐ RECHAZADA

---

## 💻 Plantilla 8: Interfaz Web (W1 y W2)

### Prueba W1: Crear Tratamiento
**Inicio**: ______:______  
**Fin**: ______:______  
**Tiempo total**: ______ segundos

**Datos ingresados**:
- Compartimento: ______
- Nombre pastilla: ________________
- Dosis: ______
- Stock: ______
- Repetición: ________________
- Hora toma: ______:______
- Fecha inicio: ______/______/______

**Validaciones**:
- [ ] Tratamiento visible en frontend
- [ ] Tratamiento guardado en backend (Django Admin)
- [ ] ESP32 lo cargó en próximo GET

**RESULTADO W1**: ☐ APROBADA ☐ RECHAZADA

### Prueba W2: Visualizar Historial
**Tomas realizadas previamente**: ______

| # | Estado | Hora Programada | Hora Real | ¿Visible en Frontend? | Datos Correctos |
|---|--------|-----------------|-----------|----------------------|-----------------|
| 1 | | | | ☐ Sí ☐ No | ☐ Sí ☐ No |
| 2 | | | | ☐ Sí ☐ No | ☐ Sí ☐ No |
| 3 | | | | ☐ Sí ☐ No | ☐ Sí ☐ No |

**Filtros funcionales**: ☐ Sí ☐ No  
**Ordenamiento correcto**: ☐ Sí ☐ No  
**RESULTADO W2**: ☐ APROBADA ☐ RECHAZADA

---

## 🚨 Plantilla 9: Casos Extremos

### E1: Múltiples Tratamientos Simultáneos
**Hora programada**: ______:______  
**Tratamientos configurados**: 4

| Compartimento | Nombre Pastilla | ¿Alarma activada? | ¿Servo abierto? | ¿Toma registrada? |
|---------------|-----------------|-------------------|-----------------|-------------------|
| 1 | | ☐ Sí ☐ No | ☐ Sí ☐ No | ☐ Sí ☐ No |
| 2 | | ☐ Sí ☐ No | ☐ Sí ☐ No | ☐ Sí ☐ No |
| 3 | | ☐ Sí ☐ No | ☐ Sí ☐ No | ☐ Sí ☐ No |
| 4 | | ☐ Sí ☐ No | ☐ Sí ☐ No | ☐ Sí ☐ No |

**Tiempo total de proceso**: ______ segundos  
**RESULTADO E1**: ☐ APROBADA ☐ RECHAZADA

### E2: Pérdida de Conexión WiFi
**Hora de desconexión**: ______:______  
**Hora de reconexión**: ______:______  
**Tiempo sin WiFi**: ______ minutos

**Observaciones en Serial Monitor**:
```
_____________________________________________________
_____________________________________________________
```

**Intentos de reconexión observados**: ______  
**Tiempo de recuperación**: ______ segundos  
**Sistema operativo después**: ☐ Sí ☐ No  
**RESULTADO E2**: ☐ APROBADA ☐ RECHAZADA

### E3: Backend No Disponible
**Hora de detención de Django**: ______:______  
**Hora de reinicio**: ______:______

**Error en Serial Monitor**:
```
_____________________________________________________
```

**Sistema bloqueado**: ☐ Sí ☐ No  
**Recuperación automática**: ☐ Sí ☐ No  
**Tiempo de recuperación**: ______ segundos  
**RESULTADO E3**: ☐ APROBADA ☐ RECHAZADA

### E4: Stock Agotado
**Stock inicial**: 1  
**Dosis**: 1

| Evento | Hora | Stock | Observación |
|--------|------|-------|-------------|
| Toma realizada | | | |
| Stock después | | | |
| Siguiente alarma | | | |

**Alarma se activa con stock 0**: ☐ Sí ☐ No  
**Notificación de stock bajo**: ☐ Sí ☐ No  
**RESULTADO E4**: ☐ APROBADA ☐ RECHAZADA

---

## 📈 Resumen Ejecutivo de Sesión

### Pruebas Ejecutadas
| ID | Nombre | Resultado | Observaciones |
|----|--------|-----------|---------------|
| H1 | Servos | ☐ ✅ ☐ ❌ | |
| H2 | Buzzer | ☐ ✅ ☐ ❌ | |
| H3 | Botón | ☐ ✅ ☐ ❌ | |
| B1 | Carga Tratamientos | ☐ ✅ ☐ ❌ | |
| B2 | Registro TOMADA | ☐ ✅ ☐ ❌ | |
| B3 | Registro OMITIDA | ☐ ✅ ☐ ❌ | |
| T1 | Recordatorio Telegram | ☐ ✅ ☐ ❌ | |
| T2 | Confirmación Telegram | ☐ ✅ ☐ ❌ | |
| W1 | Crear Tratamiento Web | ☐ ✅ ☐ ❌ | |
| W2 | Visualizar Historial | ☐ ✅ ☐ ❌ | |
| E1 | Múltiples Simultáneos | ☐ ✅ ☐ ❌ | |
| E2 | Pérdida WiFi | ☐ ✅ ☐ ❌ | |
| E3 | Backend Caído | ☐ ✅ ☐ ❌ | |
| E4 | Stock Agotado | ☐ ✅ ☐ ❌ | |

**Tasa de éxito**: ______/14 = ______%

### Problemas Críticos Encontrados
1. ________________________________________________________
2. ________________________________________________________
3. ________________________________________________________

### Mejoras Prioritarias
1. ________________________________________________________
2. ________________________________________________________
3. ________________________________________________________

### Próximos Pasos
- [ ] ________________________________________________________
- [ ] ________________________________________________________
- [ ] ________________________________________________________

**Firma del ejecutor**: ________________  
**Fecha**: ______/______/______
