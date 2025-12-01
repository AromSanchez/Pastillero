from django.contrib import admin
from .models import Tratamiento, HistorialToma, ConfiguracionNotificaciones, ConfiguracionArduino

@admin.register(Tratamiento)
class TratamientoAdmin(admin.ModelAdmin):
    list_display = ('nombre_pastilla', 'compartimento', 'hora_toma', 'repeticion', 'activo', 'stock')
    list_filter = ('activo', 'repeticion', 'compartimento')
    search_fields = ('nombre_pastilla',)

@admin.register(HistorialToma)
class HistorialTomaAdmin(admin.ModelAdmin):
    list_display = ('nombre_pastilla', 'compartimento', 'estado', 'fecha_hora_real')
    list_filter = ('estado', 'compartimento')
    date_hierarchy = 'fecha_hora_real'

@admin.register(ConfiguracionNotificaciones)
class ConfiguracionNotificacionesAdmin(admin.ModelAdmin):
    list_display = ('telegram_activo',)

@admin.register(ConfiguracionArduino)
class ConfiguracionArduinoAdmin(admin.ModelAdmin):
    list_display = ('ip_arduino', 'puerto', 'activo')
    fieldsets = (
        ('Conexión', {
            'fields': ('ip_arduino', 'puerto', 'activo')
        }),
    )

