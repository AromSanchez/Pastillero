# Guía Completa: Desplegar Django en Render con SQLite

## 📋 Tabla de Contenidos

1. [Preparación del Proyecto](#1-preparación-del-proyecto)
2. [Configuración de Archivos](#2-configuración-de-archivos)
3. [Subir a GitHub](#3-subir-a-github)
4. [Desplegar en Render](#4-desplegar-en-render)
5. [Configuración Post-Despliegue](#5-configuración-post-despliegue)
6. [Actualizar el Arduino](#6-actualizar-el-arduino)
7. [Solución de Problemas](#7-solución-de-problemas)

---

## 1. Preparación del Proyecto

### ✅ Archivos ya creados

Los siguientes archivos ya están listos en tu proyecto:

- ✅ `requirements.txt` - Dependencias de Python
- ✅ `Procfile` - Comando para iniciar el servidor
- ✅ `runtime.txt` - Versión de Python
- ✅ `build.sh` - Script de construcción
- ✅ `backend/settings.py` - Configuración actualizada para producción

### Verificar estructura del proyecto

Tu proyecto debería verse así:

```
Pastillero/
├── backend/
│   ├── __init__.py
│   ├── settings.py  ✅ Actualizado
│   ├── urls.py
│   └── wsgi.py
├── pastillas/
│   ├── migrations/
│   ├── models.py
│   ├── views.py
│   ├── serializers.py
│   └── ...
├── db.sqlite3  ⚠️ Se copiará a Render
├── manage.py
├── requirements.txt  ✅ Creado
├── Procfile  ✅ Creado
├── runtime.txt  ✅ Creado
├── build.sh  ✅ Creado
└── .gitignore
```

---

## 2. Configuración de Archivos

### 2.1 Verificar `.gitignore`

Asegúrate de que tu `.gitignore` incluya:

```
# Python
__pycache__/
*.py[cod]
*$py.class
*.so
.Python
venv/
env/
ENV/

# Django
*.log
local_settings.py
staticfiles/

# IDE
.vscode/
.idea/

# Otros
.DS_Store
*.swp
diagnostico*.py
test_sincronizacion.py
```

> [!IMPORTANT]
> **NO ignores `db.sqlite3`** - Lo necesitamos para que Render tenga los datos iniciales

### 2.2 Generar SECRET_KEY para producción

Ejecuta en Python:

```python
python -c "from django.core.management.utils import get_random_secret_key; print(get_random_secret_key())"
```

Guarda esta clave, la usarás en Render.

---

## 3. Subir a GitHub

### 3.1 Inicializar repositorio (si no lo has hecho)

```bash
git init
git add .
git commit -m "Preparar proyecto para despliegue en Render"
```

### 3.2 Crear repositorio en GitHub

1. Ve a [github.com](https://github.com)
2. Click en "New repository"
3. Nombre: `Pastillero` (o el que prefieras)
4. **NO marques** "Initialize with README"
5. Click "Create repository"

### 3.3 Conectar y subir

```bash
git remote add origin https://github.com/TU_USUARIO/Pastillero.git
git branch -M main
git push -u origin main
```

---

## 4. Desplegar en Render

### 4.1 Crear cuenta en Render

1. Ve a [render.com](https://render.com)
2. Regístrate con tu cuenta de GitHub
3. Autoriza a Render para acceder a tus repositorios

### 4.2 Crear nuevo Web Service

1. Click en "New +" → "Web Service"
2. Conecta tu repositorio `Pastillero`
3. Configura los siguientes campos:

| Campo | Valor |
|-------|-------|
| **Name** | `pastillero-backend` |
| **Region** | `Oregon (US West)` o el más cercano |
| **Branch** | `main` |
| **Root Directory** | (dejar vacío) |
| **Runtime** | `Python 3` |
| **Build Command** | `bash build.sh` |
| **Start Command** | `gunicorn backend.wsgi:application` |
| **Plan** | `Free` |

### 4.3 Configurar Variables de Entorno

En la sección "Environment Variables", agrega:

| Key | Value |
|-----|-------|
| `SECRET_KEY` | (la clave generada en paso 2.2) |
| `DEBUG` | `False` |
| `ALLOWED_HOSTS` | `pastillero-backend.onrender.com` |
| `TELEGRAM_BOT_TOKEN` | `8551131249:AAEfry7Q9v-eVNXVq2atPW3WaS5y4vAtrIY` |
| `TELEGRAM_CHAT_ID` | `5936427853` |
| `CORS_ALLOW_ALL_ORIGINS` | `True` |

> [!NOTE]
> Reemplaza `pastillero-backend` con el nombre que elegiste

### 4.4 Desplegar

1. Click en "Create Web Service"
2. Espera 5-10 minutos mientras Render:
   - Instala dependencias
   - Ejecuta migraciones
   - Recolecta archivos estáticos
   - Inicia el servidor

---

## 5. Configuración Post-Despliegue

### 5.1 Verificar que funciona

Tu backend estará disponible en:
```
https://pastillero-backend.onrender.com
```

Prueba los endpoints:
- `https://pastillero-backend.onrender.com/admin/` - Panel de administración
- `https://pastillero-backend.onrender.com/api/tratamientos/` - API de tratamientos

### 5.2 Crear superusuario

Render no permite ejecutar comandos interactivos directamente, así que:

**Opción 1: Usar Django Shell desde Render Dashboard**

1. Ve a tu servicio en Render
2. Click en "Shell" en el menú lateral
3. Ejecuta:

```python
from django.contrib.auth import get_user_model
User = get_user_model()
User.objects.create_superuser('admin', 'admin@example.com', 'tu_password_seguro')
```

**Opción 2: Crear desde código (temporal)**

Agrega esto temporalmente a `pastillas/views.py`:

```python
from django.contrib.auth import get_user_model
from rest_framework.decorators import api_view
from rest_framework.response import Response

@api_view(['GET'])
def create_admin(request):
    User = get_user_model()
    if not User.objects.filter(username='admin').exists():
        User.objects.create_superuser('admin', 'admin@example.com', 'AdminPass123!')
        return Response({"message": "Admin creado"})
    return Response({"message": "Admin ya existe"})
```

Agrega la ruta en `pastillas/urls.py` y visita `/api/create-admin/`

> [!CAUTION]
> **Elimina este endpoint después de crear el admin**

### 5.3 Configurar IP del Arduino en producción

1. Accede al admin: `https://pastillero-backend.onrender.com/admin/`
2. Login con el superusuario creado
3. Ve a **Pastillas → Configuración Arduino**
4. Actualiza la IP del Arduino (la IP pública o usa un servicio como ngrok si el Arduino está en red local)

---

## 6. Actualizar el Arduino

### 6.1 Cambiar URLs en `pastillero.ino`

Reemplaza las URLs locales por las de producción:

```cpp
// ANTES (local)
const char* backendURL = "http://10.147.1.123:8000/api/tratamientos/";
const char* registrarTomaURL = "http://10.147.1.123:8000/api/registrar-toma/";
const char* notificarRecordatorioURL = "http://10.147.1.123:8000/api/notificar-recordatorio/";

// DESPUÉS (producción)
const char* backendURL = "https://pastillero-backend.onrender.com/api/tratamientos/";
const char* registrarTomaURL = "https://pastillero-backend.onrender.com/api/registrar-toma/";
const char* notificarRecordatorioURL = "https://pastillero-backend.onrender.com/api/notificar-recordatorio/";
```

### 6.2 Problema: Arduino en red local

> [!WARNING]
> **El backend en Render NO puede enviar POST al Arduino** si el Arduino está en tu red local (10.147.1.1)

**Soluciones:**

1. **Usar polling (recomendado para pruebas):**
   - El Arduino hace GET periódicamente al backend
   - Elimina la sincronización automática por ahora

2. **Usar ngrok (para desarrollo):**
   - Expone tu Arduino a internet temporalmente
   - Actualiza la IP en ConfiguracionArduino

3. **VPN o túnel SSH (producción):**
   - Configura un túnel permanente
   - Más complejo pero más seguro

---

## 7. Solución de Problemas

### Error: "Application failed to respond"

**Causa:** El servidor no inicia correctamente

**Solución:**
1. Revisa los logs en Render Dashboard
2. Verifica que `gunicorn` esté en `requirements.txt`
3. Asegúrate de que `ALLOWED_HOSTS` incluya tu dominio de Render

### Error: "Static files not found"

**Causa:** WhiteNoise no configurado correctamente

**Solución:**
1. Verifica que `whitenoise` esté en `requirements.txt`
2. Asegúrate de que `STATIC_ROOT` esté configurado en `settings.py`
3. Ejecuta `python manage.py collectstatic` localmente para probar

### Base de datos vacía

**Causa:** `db.sqlite3` no se subió a GitHub

**Solución:**
1. Asegúrate de que `db.sqlite3` NO esté en `.gitignore`
2. Haz commit y push:
   ```bash
   git add db.sqlite3
   git commit -m "Agregar base de datos inicial"
   git push
   ```
3. Redeploy en Render

### Arduino no se conecta

**Causa:** URLs incorrectas o CORS

**Solución:**
1. Verifica que las URLs en Arduino sean HTTPS
2. Asegúrate de que `CORS_ALLOW_ALL_ORIGINS = True`
3. Revisa los logs del Arduino en Serial Monitor

---

## 📝 Checklist Final

Antes de considerar el despliegue completo:

- [ ] Proyecto subido a GitHub
- [ ] Web Service creado en Render
- [ ] Variables de entorno configuradas
- [ ] Despliegue exitoso (sin errores en logs)
- [ ] Admin accesible en `/admin/`
- [ ] Superusuario creado
- [ ] API responde en `/api/tratamientos/`
- [ ] ConfiguracionArduino actualizada
- [ ] Arduino actualizado con URLs de producción
- [ ] Prueba de conexión Arduino → Backend exitosa

---

## 🎉 ¡Listo!

Tu backend Django está desplegado en Render. Ahora puedes:

- Acceder al admin desde cualquier lugar
- El Arduino puede obtener tratamientos desde internet
- Las notificaciones de Telegram funcionan
- La sincronización automática funciona (si resuelves el problema de red local)

**URL de tu backend:** `https://pastillero-backend.onrender.com`

---

## 📞 Soporte

Si tienes problemas:

1. Revisa los logs en Render Dashboard
2. Verifica las variables de entorno
3. Asegúrate de que todas las migraciones se ejecutaron
4. Prueba los endpoints con Postman o curl

**Logs en Render:**
- Dashboard → Tu servicio → "Logs"
- Busca errores en rojo
