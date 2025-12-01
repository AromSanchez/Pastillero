# 🧪 Protocolo de Pruebas de Laboratorio
## Pastillero Inteligente - Sistema de Recordatorio de Medicamentos

---

## 📋 Índice
1. [Preparación del Prototipo](#1-preparación-del-prototipo)
2. [Definición de Criterios de Prueba](#2-definición-de-criterios-de-prueba)
3. [Ejecución de Pruebas](#3-ejecución-de-pruebas)
4. [Análisis de Datos](#4-análisis-de-datos)
5. [Registro de Mejoras](#5-registro-de-mejoras)

---

## 1. Preparación del Prototipo

### 1.1 Verificación de Hardware

#### Componentes a Verificar
| Componente | Pin | Estado Esperado | ✓ |
|------------|-----|-----------------|---|
| LED Indicador | GPIO 2 | Enciende/Apaga | ☐ |
| Botón de Control | GPIO 4 | Pull-up activo | ☐ |
| Buzzer | GPIO 15 | Emite sonido | ☐ |
| Servo Compartimento 1 | GPIO 5 | Rango 0-90° | ☐ |
| Servo Compartimento 2 | GPIO 18 | Rango 0-90° | ☐ |
| Servo Compartimento 3 | GPIO 19 | Rango 0-90° | ☐ |
| Servo Compartimento 4 | GPIO 21 | Rango 0-90° | ☐ |

#### Checklist de Preparación
- [ ] **Alimentación**: Verificar voltaje estable (5V para servos, 3.3V para ESP32)
- [ ] **Conexiones**: Revisar soldaduras y cables
- [ ] **Servos**: Probar movimiento completo sin carga
- [ ] **Compartimentos**: Verificar que abren/cierran correctamente
- [ ] **Buzzer**: Probar diferentes frecuencias
- [ ] **LED**: Verificar visibilidad

### 1.2 Configuración de Software

#### Configuración WiFi
```cpp
SSID: "Alicia"
Password: "lizzy1506"
```

**Pruebas de Conectividad:**
- [ ] Conexión exitosa a WiFi (LED integrado parpadea)
- [ ] Obtención de IP local
- [ ] Ping al servidor backend (10.147.1.123:8000)

#### Sincronización de Tiempo
- [ ] Conexión a servidor NTP (`pool.ntp.org`)
- [ ] Zona horaria correcta (GMT-5, Perú)
- [ ] Hora sincronizada con margen ±2 segundos

#### Comunicación con Backend
**Endpoints a Verificar:**
| Endpoint | Método | Propósito | Estado |
|----------|--------|-----------|--------|
| `/api/tratamientos/` | GET | Obtener tratamientos | ☐ |
| `/api/registrar-toma/` | POST | Registrar toma | ☐ |
| `/api/notificar-recordatorio/` | POST | Enviar recordatorio | ☐ |

---

## 2. Definición de Criterios de Prueba

### 2.1 Funcionalidad

#### F1: Gestión de Tratamientos
- **Criterio**: El sistema debe cargar correctamente tratamientos desde el backend
- **Métrica**: 100% de tratamientos activos cargados
- **Método**: Comparar datos en backend vs. Serial Monitor

#### F2: Detección de Hora de Toma
- **Criterio**: Activar alarma exactamente a la hora programada
- **Métrica**: Precisión ±30 segundos
- **Método**: Programar toma y medir tiempo de activación

#### F3: Apertura de Compartimentos
- **Criterio**: Servo abre compartimento al presionar botón durante alarma
- **Métrica**: Apertura completa (90°) en <2 segundos
- **Método**: Medir ángulo con goniómetro y cronómetro

#### F4: Registro de Tomas
- **Criterio**: Estado (TOMADA/OMITIDA) se registra en backend
- **Métrica**: 100% de registros exitosos
- **Método**: Verificar en base de datos Django

#### F5: Notificaciones Telegram
- **Criterio**: Enviar mensajes de recordatorio y confirmación
- **Métrica**: Latencia <5 segundos
- **Método**: Medir tiempo entre evento y recepción de mensaje

### 2.2 Rendimiento

#### P1: Tiempo de Respuesta
| Acción | Tiempo Máximo Aceptable |
|--------|-------------------------|
| Conexión WiFi | 10 segundos |
| Carga de tratamientos | 3 segundos |
| Activación de alarma | 1 segundo |
| Apertura de servo | 2 segundos |
| Registro en backend | 2 segundos |

#### P2: Consumo Energético
- **Medición**: Corriente en reposo vs. operación
- **Criterio**: <500mA en reposo, <2A con servos activos

#### P3: Estabilidad de Conexión
- **Criterio**: Mantener conexión WiFi >99% del tiempo
- **Método**: Monitoreo continuo durante 24 horas

### 2.3 Resistencia

#### R1: Ciclos de Operación
- **Prueba**: 100 ciclos de apertura/cierre por compartimento
- **Criterio**: Sin degradación mecánica visible
- **Medición**: Inspección visual y prueba de torque

#### R2: Condiciones Ambientales
| Condición | Rango de Prueba | Criterio de Éxito |
|-----------|-----------------|-------------------|
| Temperatura | 15°C - 35°C | Operación normal |
| Humedad | 30% - 70% | Sin corrosión |
| Vibración | Simulación de transporte | Sin desconexiones |

#### R3: Recuperación ante Fallos
- **Pérdida de WiFi**: Reconexión automática en <30 segundos
- **Pérdida de energía**: Retomar operación al restaurar
- **Backend caído**: Reintentos cada 60 segundos

### 2.4 Ergonomía y Usabilidad

#### E1: Interfaz de Usuario (Botón)
- **Criterio**: Respuesta táctil clara (debounce 200ms)
- **Prueba**: 20 usuarios presionan botón, 100% de éxito

#### E2: Retroalimentación Sensorial
- **Visual**: LED visible a 3 metros
- **Auditiva**: Buzzer audible a 5 metros (patrón agradable)
- **Criterio**: 90% de usuarios identifican alarma correctamente

#### E3: Accesibilidad de Compartimentos
- **Criterio**: Apertura suficiente para extraer pastilla con una mano
- **Prueba**: 10 usuarios de diferentes edades

#### E4: Interfaz Web/Móvil
- **Criterio**: Configurar tratamiento en <2 minutos
- **Método**: Prueba con 5 usuarios sin experiencia técnica

---

## 3. Ejecución de Pruebas

### 3.1 Pruebas de Funcionalidad del Hardware

#### Prueba H1: Servos - Apertura Individual
**Objetivo**: Verificar que cada servo abre su compartimento correctamente

**Procedimiento**:
1. Cargar código de prueba en ESP32
2. Enviar comando para abrir Compartimento 1
3. Medir ángulo de apertura con goniómetro
4. Repetir para compartimentos 2, 3 y 4
5. Verificar cierre completo (0°)

**Datos a Registrar**:
| Compartimento | Ángulo Apertura | Tiempo Apertura | Ángulo Cierre | Observaciones |
|---------------|-----------------|-----------------|---------------|---------------|
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |
| 4 | | | | |

**Criterio de Éxito**: Ángulo 85-90°, tiempo <2s

---

#### Prueba H2: Buzzer - Patrón de Alarma
**Objetivo**: Validar que el buzzer emite patrón agradable y audible

**Procedimiento**:
1. Activar alarma manualmente
2. Medir duración de beeps (esperado: 300ms ON, 200ms OFF)
3. Medir nivel de sonido con sonómetro a 1m, 3m y 5m
4. Encuesta de percepción a 5 usuarios

**Datos a Registrar**:
| Distancia | Nivel dB | ¿Audible? | ¿Agradable? (1-5) |
|-----------|----------|-----------|-------------------|
| 1m | | | |
| 3m | | | |
| 5m | | | |

**Criterio de Éxito**: Audible a 5m, calificación promedio ≥3/5

---

#### Prueba H3: Botón - Debounce y Respuesta
**Objetivo**: Verificar que el botón responde correctamente sin rebotes

**Procedimiento**:
1. Presionar botón 20 veces rápidamente
2. Contar respuestas registradas en Serial Monitor
3. Medir tiempo de respuesta desde presión hasta acción

**Datos a Registrar**:
- Presiones realizadas: 20
- Respuestas registradas: ____
- Tiempo promedio de respuesta: ____ ms
- Falsos positivos: ____

**Criterio de Éxito**: 100% de presiones registradas, 0 falsos positivos

---

### 3.2 Pruebas de Comunicación Backend-Arduino

#### Prueba B1: Carga de Tratamientos
**Objetivo**: Verificar que el ESP32 obtiene tratamientos correctamente

**Procedimiento**:
1. Crear 4 tratamientos en el backend (uno por compartimento)
2. Reiniciar ESP32
3. Verificar en Serial Monitor que se cargaron todos
4. Comparar datos cargados vs. base de datos

**Datos a Registrar**:
| Compartimento | Nombre Pastilla | Hora Toma | Repetición | ¿Cargado? |
|---------------|-----------------|-----------|------------|-----------|
| 1 | | | | ☐ |
| 2 | | | | ☐ |
| 3 | | | | ☐ |
| 4 | | | | ☐ |

**Criterio de Éxito**: 4/4 tratamientos cargados correctamente

---

#### Prueba B2: Registro de Toma (TOMADA)
**Objetivo**: Validar que el estado TOMADA se registra en backend

**Procedimiento**:
1. Programar tratamiento para dentro de 2 minutos
2. Esperar alarma
3. Presionar botón (simular toma)
4. Verificar en Django Admin que se creó `HistorialToma` con estado TOMADA
5. Verificar reducción de stock

**Datos a Registrar**:
- Hora programada: ____
- Hora de alarma: ____
- Hora de registro: ____
- Stock antes: ____
- Stock después: ____
- Estado en DB: ____

**Criterio de Éxito**: Registro creado, stock reducido en `dosis`

---

#### Prueba B3: Registro de Toma (OMITIDA)
**Objetivo**: Validar timeout de 5 minutos y registro automático como OMITIDA

**Procedimiento**:
1. Programar tratamiento
2. Esperar alarma
3. NO presionar botón
4. Esperar 5 minutos
5. Verificar registro en backend con estado OMITIDA

**Datos a Registrar**:
- Hora de alarma: ____
- Hora de timeout: ____
- Tiempo transcurrido: ____ min
- Estado en DB: ____
- Stock modificado: ☐ Sí ☐ No

**Criterio de Éxito**: Registro OMITIDA después de 5 min, stock sin cambios

---

### 3.3 Pruebas de Notificaciones Telegram

#### Prueba T1: Recordatorio de Toma
**Objetivo**: Verificar envío de recordatorio al activarse alarma

**Procedimiento**:
1. Configurar `TELEGRAM_BOT_TOKEN` y `TELEGRAM_CHAT_ID` en `settings.py`
2. Programar tratamiento
3. Esperar activación de alarma
4. Medir tiempo entre alarma y recepción de mensaje Telegram

**Datos a Registrar**:
- Hora de alarma: ____
- Hora de mensaje Telegram: ____
- Latencia: ____ segundos
- Contenido del mensaje: ____

**Criterio de Éxito**: Mensaje recibido en <5 segundos con información correcta

---

#### Prueba T2: Confirmación de Toma
**Objetivo**: Verificar notificación cuando se registra toma

**Procedimiento**:
1. Completar ciclo de toma (presionar botón)
2. Verificar recepción de mensaje de confirmación
3. Validar contenido del mensaje

**Datos a Registrar**:
- Estado registrado: ____
- Mensaje recibido: ☐ Sí ☐ No
- Contenido correcto: ☐ Sí ☐ No

**Criterio de Éxito**: Mensaje con ✅ para TOMADA, ⚠️ para OMITIDA

---

### 3.4 Pruebas de Interfaz Web/Móvil

#### Prueba W1: Crear Tratamiento desde Web
**Objetivo**: Validar flujo completo de creación de tratamiento

**Procedimiento**:
1. Acceder a frontend React (http://localhost:5173)
2. Crear nuevo tratamiento con datos:
   - Compartimento: 1
   - Pastilla: "Ibuprofeno"
   - Dosis: 1
   - Stock: 10
   - Repetición: DIARIO
   - Hora: 14:00
3. Guardar y verificar en backend
4. Verificar que ESP32 lo carga en próximo GET

**Datos a Registrar**:
- Tiempo de creación: ____ segundos
- Tratamiento visible en backend: ☐ Sí ☐ No
- ESP32 lo cargó: ☐ Sí ☐ No

**Criterio de Éxito**: Tratamiento creado y sincronizado en <30 segundos

---

#### Prueba W2: Visualizar Historial
**Objetivo**: Verificar que el historial de tomas se muestra correctamente

**Procedimiento**:
1. Realizar 3 tomas (2 TOMADAS, 1 OMITIDA)
2. Acceder a sección de historial en frontend
3. Verificar que se muestran las 3 tomas con información correcta

**Datos a Registrar**:
| Toma | Estado | Hora Programada | Hora Real | ¿Visible? |
|------|--------|-----------------|-----------|-----------|
| 1 | TOMADA | | | ☐ |
| 2 | TOMADA | | | ☐ |
| 3 | OMITIDA | | | ☐ |

**Criterio de Éxito**: 100% de tomas visibles con datos correctos

---

### 3.5 Pruebas de Casos Extremos

#### Prueba E1: Múltiples Tratamientos Simultáneos
**Objetivo**: Validar comportamiento con 4 tratamientos a la misma hora

**Procedimiento**:
1. Programar 4 tratamientos para la misma hora
2. Esperar alarma
3. Presionar botón
4. Verificar que se abren los 4 compartimentos
5. Verificar que se registran las 4 tomas

**Datos a Registrar**:
- Compartimentos abiertos: ____
- Tomas registradas: ____
- Tiempo total de proceso: ____ segundos

**Criterio de Éxito**: 4 compartimentos abiertos, 4 registros creados

---

#### Prueba E2: Pérdida de Conexión WiFi
**Objetivo**: Verificar recuperación automática

**Procedimiento**:
1. Iniciar sistema normalmente
2. Desconectar WiFi (apagar router)
3. Observar intentos de reconexión en Serial Monitor
4. Reconectar WiFi después de 2 minutos
5. Verificar que el sistema se recupera

**Datos a Registrar**:
- Tiempo sin WiFi: ____ minutos
- Intentos de reconexión: ____
- Tiempo de recuperación: ____ segundos
- Sistema operativo después: ☐ Sí ☐ No

**Criterio de Éxito**: Reconexión automática en <30 segundos

---

#### Prueba E3: Backend No Disponible
**Objetivo**: Validar comportamiento cuando backend está caído

**Procedimiento**:
1. Detener servidor Django
2. Reiniciar ESP32
3. Observar comportamiento
4. Reiniciar backend
5. Verificar recuperación

**Datos a Registrar**:
- Error mostrado en Serial: ____
- Sistema bloqueado: ☐ Sí ☐ No
- Recuperación automática: ☐ Sí ☐ No

**Criterio de Éxito**: Sistema no se bloquea, reintenta conexión

---

#### Prueba E4: Stock Agotado
**Objetivo**: Verificar comportamiento cuando stock llega a 0

**Procedimiento**:
1. Crear tratamiento con stock = 1, dosis = 1
2. Realizar toma
3. Verificar que stock = 0
4. Programar siguiente toma
5. Observar comportamiento

**Datos a Registrar**:
- Stock inicial: 1
- Stock después de toma: ____
- Alarma se activa con stock 0: ☐ Sí ☐ No
- Notificación de stock bajo: ☐ Sí ☐ No

**Criterio de Éxito**: Sistema alerta sobre stock agotado

---

## 4. Análisis de Datos

### 4.1 Compilación de Resultados

#### Tabla Resumen de Funcionalidad
| ID Prueba | Descripción | Resultado | Cumple Criterio | Observaciones |
|-----------|-------------|-----------|-----------------|---------------|
| H1 | Servos - Apertura | | ☐ Sí ☐ No | |
| H2 | Buzzer - Patrón | | ☐ Sí ☐ No | |
| H3 | Botón - Debounce | | ☐ Sí ☐ No | |
| B1 | Carga Tratamientos | | ☐ Sí ☐ No | |
| B2 | Registro TOMADA | | ☐ Sí ☐ No | |
| B3 | Registro OMITIDA | | ☐ Sí ☐ No | |
| T1 | Recordatorio Telegram | | ☐ Sí ☐ No | |
| T2 | Confirmación Telegram | | ☐ Sí ☐ No | |
| W1 | Crear Tratamiento Web | | ☐ Sí ☐ No | |
| W2 | Visualizar Historial | | ☐ Sí ☐ No | |
| E1 | Múltiples Simultáneos | | ☐ Sí ☐ No | |
| E2 | Pérdida WiFi | | ☐ Sí ☐ No | |
| E3 | Backend Caído | | ☐ Sí ☐ No | |
| E4 | Stock Agotado | | ☐ Sí ☐ No | |

**Tasa de Éxito Global**: _____ / 14 pruebas = _____ %

### 4.2 Comparación con Requisitos

#### Requisitos Funcionales
| Requisito | Estado | Evidencia |
|-----------|--------|-----------|
| RF1: Recordar tomas a hora exacta | ☐ ✅ ☐ ⚠️ ☐ ❌ | Prueba H2, B1 |
| RF2: Abrir compartimento al presionar botón | ☐ ✅ ☐ ⚠️ ☐ ❌ | Prueba H1, H3 |
| RF3: Registrar tomas en backend | ☐ ✅ ☐ ⚠️ ☐ ❌ | Prueba B2, B3 |
| RF4: Notificar vía Telegram | ☐ ✅ ☐ ⚠️ ☐ ❌ | Prueba T1, T2 |
| RF5: Gestionar tratamientos desde web | ☐ ✅ ☐ ⚠️ ☐ ❌ | Prueba W1 |
| RF6: Visualizar historial | ☐ ✅ ☐ ⚠️ ☐ ❌ | Prueba W2 |

**Leyenda**: ✅ Cumple | ⚠️ Cumple parcialmente | ❌ No cumple

#### Requisitos No Funcionales
| Requisito | Métrica Esperada | Métrica Obtenida | Estado |
|-----------|------------------|------------------|--------|
| RNF1: Tiempo de respuesta | <2s | ____ s | ☐ ✅ ☐ ❌ |
| RNF2: Disponibilidad | >95% | ____ % | ☐ ✅ ☐ ❌ |
| RNF3: Precisión temporal | ±30s | ±____ s | ☐ ✅ ☐ ❌ |
| RNF4: Consumo energético | <2A | ____ A | ☐ ✅ ☐ ❌ |
| RNF5: Ciclos de vida | >100 | ____ | ☐ ✅ ☐ ❌ |

### 4.3 Identificación de Desviaciones

#### Desviaciones Críticas (Bloquean funcionalidad principal)
1. **Descripción**: _______________________________________________
   - **Impacto**: _______________________________________________
   - **Causa raíz**: _______________________________________________

2. **Descripción**: _______________________________________________
   - **Impacto**: _______________________________________________
   - **Causa raíz**: _______________________________________________

#### Desviaciones Moderadas (Afectan experiencia de usuario)
1. **Descripción**: _______________________________________________
   - **Impacto**: _______________________________________________
   - **Causa raíz**: _______________________________________________

#### Desviaciones Menores (Mejoras deseables)
1. **Descripción**: _______________________________________________
   - **Impacto**: _______________________________________________
   - **Causa raíz**: _______________________________________________

### 4.4 Análisis de Fallos

#### Matriz de Modos de Fallo
| Componente | Modo de Fallo | Frecuencia | Severidad | Detección | RPN* |
|------------|---------------|------------|-----------|-----------|------|
| Servo | No abre completamente | | | | |
| WiFi | Desconexión intermitente | | | | |
| Backend | Timeout en requests | | | | |
| Buzzer | Volumen insuficiente | | | | |

*RPN = Risk Priority Number (Frecuencia × Severidad × Detección)

**Escala**:
- Frecuencia: 1 (raro) - 10 (frecuente)
- Severidad: 1 (menor) - 10 (crítico)
- Detección: 1 (fácil) - 10 (difícil)

---

## 5. Registro de Mejoras

### 5.1 Lista de Ajustes Necesarios

#### Prioridad ALTA (Crítico para funcionamiento)
| ID | Componente | Problema | Solución Propuesta | Esfuerzo | Responsable |
|----|------------|----------|-------------------|----------|-------------|
| A1 | | | | | |
| A2 | | | | | |
| A3 | | | | | |

#### Prioridad MEDIA (Mejora experiencia de usuario)
| ID | Componente | Problema | Solución Propuesta | Esfuerzo | Responsable |
|----|------------|----------|-------------------|----------|-------------|
| M1 | | | | | |
| M2 | | | | | |
| M3 | | | | | |

#### Prioridad BAJA (Optimizaciones)
| ID | Componente | Problema | Solución Propuesta | Esfuerzo | Responsable |
|----|------------|----------|-------------------|----------|-------------|
| B1 | | | | | |
| B2 | | | | | |

**Escala de Esfuerzo**: 🟢 Bajo (1-4h) | 🟡 Medio (4-16h) | 🔴 Alto (>16h)

### 5.2 Plan de Optimización

#### Fase 1: Correcciones Críticas (Semana 1)
- [ ] Implementar mejora A1
- [ ] Implementar mejora A2
- [ ] Implementar mejora A3
- [ ] Realizar pruebas de regresión

#### Fase 2: Mejoras de Experiencia (Semana 2)
- [ ] Implementar mejora M1
- [ ] Implementar mejora M2
- [ ] Implementar mejora M3
- [ ] Pruebas de usabilidad con usuarios

#### Fase 3: Optimizaciones (Semana 3)
- [ ] Implementar mejora B1
- [ ] Implementar mejora B2
- [ ] Optimización de código
- [ ] Documentación final

### 5.3 Mejoras Sugeridas por Categoría

#### Hardware
- [ ] **Servo más robusto**: Considerar servos con mayor torque (ej: MG996R)
- [ ] **Buzzer piezo**: Reemplazar por buzzer con mejor calidad de sonido
- [ ] **LED RGB**: Usar LED multicolor para diferentes estados
- [ ] **Sensor de presencia**: Detectar si se extrajo la pastilla
- [ ] **Batería de respaldo**: UPS para mantener operación durante cortes

#### Software - Arduino
- [ ] **Watchdog timer**: Reinicio automático si el sistema se cuelga
- [ ] **Logs en SD**: Guardar eventos localmente para debugging
- [ ] **OTA updates**: Actualización de firmware sin cable
- [ ] **Modo offline**: Funcionar con tratamientos en caché si backend cae
- [ ] **Calibración de servos**: Ajuste fino de ángulos por compartimento

#### Software - Backend
- [ ] **Notificaciones push**: Además de Telegram, usar FCM para móvil
- [ ] **Alertas de stock bajo**: Notificar cuando quedan <3 dosis
- [ ] **Estadísticas**: Dashboard con adherencia al tratamiento
- [ ] **Múltiples usuarios**: Soporte para varios pacientes
- [ ] **API de salud**: Integración con Google Fit / Apple Health

#### Software - Frontend
- [ ] **Modo oscuro**: Tema dark para mejor experiencia nocturna
- [ ] **Calendario visual**: Vista mensual de tomas programadas
- [ ] **Recordatorios web**: Notificaciones del navegador
- [ ] **Exportar historial**: PDF/CSV para médicos
- [ ] **Modo offline**: PWA con sincronización posterior

#### Ergonomía
- [ ] **Botón más grande**: Facilitar uso para personas mayores
- [ ] **Etiquetas Braille**: Accesibilidad para personas con discapacidad visual
- [ ] **Compartimentos transparentes**: Ver nivel de pastillas sin abrir
- [ ] **Base antideslizante**: Evitar movimiento al presionar botón
- [ ] **Asa de transporte**: Facilitar movilidad del dispositivo

#### Seguridad
- [ ] **Autenticación WiFi**: WPA3 en lugar de WPA2
- [ ] **HTTPS**: Comunicación encriptada con backend
- [ ] **Bloqueo de compartimentos**: Evitar apertura no autorizada
- [ ] **Logs de auditoría**: Registro de quién modificó tratamientos
- [ ] **Backup automático**: Respaldo diario de base de datos

### 5.4 Métricas de Éxito Post-Mejoras

#### KPIs a Medir
| Métrica | Valor Actual | Objetivo | Plazo |
|---------|--------------|----------|-------|
| Tasa de éxito de tomas | ___% | 95% | 1 mes |
| Tiempo de respuesta promedio | ___s | <2s | 2 semanas |
| Disponibilidad del sistema | ___% | 99% | 1 mes |
| Satisfacción de usuario (1-10) | ___ | 8+ | 1 mes |
| Fallos por semana | ___ | <1 | 1 mes |

#### Criterios de Aceptación Final
- [ ] Todas las pruebas de funcionalidad pasan al 100%
- [ ] No hay desviaciones críticas pendientes
- [ ] Tiempo de respuesta <2s en el 95% de operaciones
- [ ] Sistema opera 7 días continuos sin fallos
- [ ] 5 usuarios validan usabilidad con calificación ≥8/10

---

## 📊 Anexos

### Anexo A: Formato de Reporte de Prueba Individual

```markdown
# Reporte de Prueba: [ID - Nombre]

**Fecha**: _______________
**Ejecutor**: _______________
**Versión de Firmware**: _______________
**Versión de Backend**: _______________

## Objetivo
[Descripción del objetivo de la prueba]

## Procedimiento Ejecutado
1. [Paso 1]
2. [Paso 2]
...

## Resultados Obtenidos
[Descripción detallada de resultados]

## Datos Medidos
[Tabla o lista con datos cuantitativos]

## Observaciones
[Cualquier comportamiento inesperado o nota relevante]

## Conclusión
☐ APROBADA
☐ APROBADA CON OBSERVACIONES
☐ RECHAZADA

**Justificación**: _______________
```

### Anexo B: Configuración de Entorno de Pruebas

#### Hardware Necesario
- ESP32 DevKit v1
- 4× Servos SG90 (o equivalente)
- Buzzer pasivo 5V
- LED 5mm
- Pulsador normalmente abierto
- Resistencia 10kΩ (pull-up)
- Fuente de alimentación 5V 3A
- Cables jumper
- Protoboard o PCB

#### Software Necesario
- Arduino IDE 2.x
- Librerías:
  - `WiFi.h`
  - `HTTPClient.h`
  - `ArduinoJson` v6.x
  - `ESP32Servo.h`
- Python 3.10+
- Django 4.x
- Node.js 18+
- React 18+

#### Herramientas de Medición
- Multímetro digital
- Goniómetro (medición de ángulos)
- Cronómetro
- Sonómetro (app móvil aceptable)
- Cámara para documentación

### Anexo C: Checklist de Seguridad

Antes de cada sesión de pruebas, verificar:
- [ ] Voltajes correctos en todos los componentes
- [ ] No hay cables sueltos o mal conectados
- [ ] Servos no están bloqueados mecánicamente
- [ ] Área de trabajo libre de líquidos
- [ ] Extintor disponible (precaución con baterías)
- [ ] Ventilación adecuada
- [ ] Equipo de protección personal (si aplica)

---

## 📝 Notas Finales

### Recomendaciones Generales
1. **Documentar todo**: Tomar fotos/videos de cada prueba
2. **Ser sistemático**: Seguir el orden establecido
3. **Repetir pruebas fallidas**: Confirmar que no fue error humano
4. **Ambiente controlado**: Temperatura y humedad estables
5. **Versionado**: Registrar versiones exactas de software/firmware

### Próximos Pasos
1. Completar todas las pruebas de este protocolo
2. Analizar resultados y priorizar mejoras
3. Implementar correcciones críticas
4. Re-ejecutar pruebas afectadas
5. Preparar demostración final del prototipo

---

**Elaborado por**: _______________
**Fecha de creación**: 30/11/2025
**Versión del documento**: 1.0
**Última actualización**: _______________

---

> [!IMPORTANT]
> Este protocolo debe ser ejecutado en su totalidad antes de considerar el prototipo listo para producción o despliegue con usuarios reales. La seguridad y confiabilidad del sistema son críticas dado que maneja medicamentos.
