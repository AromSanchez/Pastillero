# Guía Rápida: Desplegar Django en Render

## 🚀 Despliegue Rápido (5 minutos)

### 1. Subir a GitHub

```bash
git add .
git commit -m "Deploy to Render"
git push
```

Si no tienes repositorio:
```bash
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/TU_USUARIO/Pastillero.git
git push -u origin main
```

### 2. Crear Web Service en Render

1. Ve a [render.com](https://render.com) y regístrate con GitHub
2. Click "New +" → "Web Service"
3. Selecciona tu repositorio `Pastillero`
4. Configura:
   - **Name**: `pastillero-backend`
   - **Build Command**: `bash build.sh`
   - **Start Command**: `gunicorn backend.wsgi:application`
   - **Plan**: Free
5. Click "Create Web Service"

### 3. Esperar el Deploy

Render tardará 5-10 minutos. Verás:
- ✅ Installing dependencies
- ✅ Running migrations
- ✅ Collecting static files
- ✅ Starting server

### 4. Acceder a tu Backend

Tu backend estará en:
```
https://pastillero-backend.onrender.com
```

Prueba:
- `/admin/` - Panel de administración
- `/api/tratamientos/` - API de tratamientos

### 5. Crear Admin

Ve a Render Dashboard → Shell y ejecuta:

```python
from django.contrib.auth import get_user_model
User = get_user_model()
User.objects.create_superuser('admin', 'admin@example.com', 'admin123')
```

### 6. Actualizar Arduino

En `pastillero.ino`, cambia las URLs:

```cpp
const char* backendURL = "https://pastillero-backend.onrender.com/api/tratamientos/";
const char* registrarTomaURL = "https://pastillero-backend.onrender.com/api/registrar-toma/";
const char* notificarRecordatorioURL = "https://pastillero-backend.onrender.com/api/notificar-recordatorio/";
```

## ⚠️ Importante: Sincronización Arduino

El backend en Render **NO puede enviar POST al Arduino** si está en red local (10.147.1.1).

**Solución simple:** El Arduino solo hará GET al backend (ya funciona así).

La sincronización automática (POST desde backend a Arduino) solo funcionará en local.

## 🎉 ¡Listo!

- ✅ Backend desplegado
- ✅ Admin accesible
- ✅ API funcionando
- ✅ Arduino puede obtener tratamientos
- ✅ Notificaciones Telegram funcionan

**No necesitas configurar variables de entorno** - todo está hardcodeado para pruebas.

## 🔧 Troubleshooting

### Error al desplegar
- Revisa los logs en Render Dashboard
- Asegúrate de que `db.sqlite3` esté en el repo

### Arduino no conecta
- Verifica que las URLs sean HTTPS
- Revisa Serial Monitor para ver errores

### Base de datos vacía
```bash
git add db.sqlite3
git commit -m "Add database"
git push
```
Luego redeploy en Render.
