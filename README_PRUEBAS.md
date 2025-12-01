# 🧪 Guía de Pruebas de Laboratorio - Pastillero Inteligente

## 📚 Documentos Creados

Este proyecto ahora incluye un protocolo completo de pruebas de laboratorio con los siguientes archivos:

### 1. **PROTOCOLO_PRUEBAS_LABORATORIO.md**
Documento principal con 5 fases de pruebas:
- ✅ Fase 1: Preparación del Prototipo
- ✅ Fase 2: Definición de Criterios de Prueba
- ✅ Fase 3: Ejecución de Pruebas (14 pruebas detalladas)
- ✅ Fase 4: Análisis de Datos
- ✅ Fase 5: Registro de Mejoras

### 2. **PLANTILLAS_REGISTRO_PRUEBAS.md**
Plantillas listas para imprimir y completar durante las pruebas con tablas para registrar datos.

### 3. **Scripts de Prueba Automatizados** (`scripts_prueba/`)
- `test_servos.ino` - Prueba individual de servos
- `test_buzzer_led.ino` - Prueba de buzzer y LED
- `test_backend.py` - Prueba de endpoints del backend

## 🚀 Cómo Usar Este Protocolo

### Paso 1: Preparación
1. Leer completamente `PROTOCOLO_PRUEBAS_LABORATORIO.md`
2. Imprimir `PLANTILLAS_REGISTRO_PRUEBAS.md` para documentar
3. Reunir herramientas necesarias (ver Anexo B del protocolo)

### Paso 2: Pruebas de Hardware
```bash
# Cargar test_servos.ino en Arduino IDE
# Abrir Serial Monitor (115200 baud)
# Comandos disponibles:
#   1A - Abrir compartimento 1
#   TEST - Ciclo completo
```

### Paso 3: Pruebas de Backend
```bash
# Activar entorno virtual
cd Pastillero
python -m venv venv
venv\Scripts\activate

# Instalar dependencias
pip install requests colorama

# Ejecutar pruebas
python scripts_prueba/test_backend.py
```

### Paso 4: Documentar Resultados
Completar las plantillas de registro con los datos obtenidos.

## 📊 Estructura de Pruebas

| ID | Prueba | Archivo | Duración Estimada |
|----|--------|---------|-------------------|
| H1 | Servos | test_servos.ino | 10 min |
| H2 | Buzzer/LED | test_buzzer_led.ino | 10 min |
| H3 | Botón | pastillero.ino | 5 min |
| B1-B3 | Backend | test_backend.py | 5 min |
| T1-T2 | Telegram | Manual | 10 min |
| W1-W2 | Frontend | Manual | 15 min |
| E1-E4 | Casos extremos | Manual | 30 min |

**Tiempo total estimado**: 2-3 horas

## ✅ Checklist Rápido

- [ ] Hardware armado y operativo
- [ ] WiFi configurado
- [ ] Backend Django corriendo
- [ ] Frontend React corriendo
- [ ] Telegram configurado (opcional)
- [ ] Plantillas impresas
- [ ] Herramientas de medición listas

## 📝 Criterios de Éxito

El prototipo está listo para producción si:
- ✅ Tasa de éxito de pruebas ≥ 95%
- ✅ No hay desviaciones críticas
- ✅ Tiempo de respuesta < 2s
- ✅ Sistema opera 7 días sin fallos

## 🔧 Solución de Problemas

### Backend no responde
```bash
cd Pastillero
python manage.py runserver 10.147.1.123:8000
```

### ESP32 no conecta a WiFi
- Verificar SSID y contraseña en líneas 7-8 de `pastillero.ino`
- Verificar que el router esté encendido

### Servos no se mueven
- Verificar alimentación 5V
- Verificar pines GPIO correctos

## 📞 Contacto y Soporte

Para dudas sobre el protocolo de pruebas, consultar:
- Protocolo principal: `PROTOCOLO_PRUEBAS_LABORATORIO.md`
- Plantillas: `PLANTILLAS_REGISTRO_PRUEBAS.md`

---

**Versión**: 1.0  
**Fecha**: 30/11/2025  
**Autor**: Equipo Pastillero Inteligente
