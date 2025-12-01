from django.db import models
from django.utils import timezone
from datetime import timedelta

class ConfiguracionNotificaciones(models.Model):
    telegram_activo = models.BooleanField(default=True)

    def __str__(self):
        return f"Notificaciones (Telegram activo: {self.telegram_activo})"


class ConfiguracionArduino(models.Model):
    """Configuración para comunicación con el Arduino ESP32"""
    ip_arduino = models.GenericIPAddressField(default="10.147.1.1")
    puerto = models.IntegerField(default=80)
    activo = models.BooleanField(default=True)
    
    class Meta:
        verbose_name = "Configuración Arduino"
        verbose_name_plural = "Configuración Arduino"
    
    def __str__(self):
        return f"Arduino en {self.ip_arduino}:{self.puerto}"


class Tratamiento(models.Model):
    REPETICION_CHOICES = [
        ("DIARIO", "Diario"),
        ("SEMANAL", "Semanal"),
        ("CADA_X_HORAS", "Cada X horas"),
    ]

    compartimento = models.IntegerField(choices=[(i, f"Compartimento {i}") for i in range(1, 5)])
    nombre_pastilla = models.CharField(max_length=100)
    dosis = models.PositiveIntegerField(default=1)
    stock = models.PositiveIntegerField(default=0)
    repeticion = models.CharField(max_length=20, choices=REPETICION_CHOICES)
    intervalo_horas = models.PositiveIntegerField(null=True, blank=True)
    hora_toma = models.TimeField(null=True, blank=True)
    dia_semana = models.IntegerField(null=True, blank=True, choices=[(i, day) for i, day in enumerate(["Lunes","Martes","Miércoles","Jueves","Viernes","Sábado","Domingo"])])
    fecha_inicio = models.DateField(null=True, blank=True)
    fecha_fin = models.DateField(null=True, blank=True)
    activo = models.BooleanField(default=True)

    def __str__(self):
        return f"{self.nombre_pastilla} - Compartimento {self.compartimento}"

    def save(self, *args, **kwargs):
        """Calcula fecha_fin automáticamente para tratamientos diarios si no se ha proporcionado.

        Regla: si repeticion == "DIARIO" y fecha_fin es None, y existen fecha_inicio, stock y dosis > 0,
        entonces:
            dias = stock // dosis
            fecha_fin = fecha_inicio + (dias - 1) días
        """
        if (
            self.repeticion == "DIARIO"
            and not self.fecha_fin
            and self.fecha_inicio
            and self.stock is not None
            and self.dosis
            and self.dosis > 0
        ):
            dias_tratamiento = self.stock // self.dosis
            if dias_tratamiento > 0:
                # Ejemplo: 10 pastillas, dosis 1, fecha_inicio 01 → dura 10 días (01 al 10)
                self.fecha_fin = self.fecha_inicio + timedelta(days=dias_tratamiento - 1)

        super().save(*args, **kwargs)


class HistorialToma(models.Model):
    ESTADO_CHOICES = [
        ("TOMADA", "Tomada"),
        ("OMITIDA", "Omitida"),
    ]

    tratamiento = models.ForeignKey(Tratamiento, on_delete=models.CASCADE, related_name='historial')
    nombre_pastilla = models.CharField(max_length=100)  # Guardar nombre por si se borra el tratamiento
    compartimento = models.IntegerField()
    hora_programada = models.TimeField()
    fecha_hora_real = models.DateTimeField(default=timezone.now)
    estado = models.CharField(max_length=10, choices=ESTADO_CHOICES, default="TOMADA")
    
    class Meta:
        ordering = ['-fecha_hora_real']
        verbose_name = "Historial de Toma"
        verbose_name_plural = "Historial de Tomas"

    def __str__(self):
        return f"{self.nombre_pastilla} - {self.estado} - {self.fecha_hora_real.strftime('%d/%m/%Y %H:%M')}"