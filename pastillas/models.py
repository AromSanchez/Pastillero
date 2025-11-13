from django.db import models
from django.utils import timezone

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