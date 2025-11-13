from rest_framework import serializers
from .models import Tratamiento, HistorialToma

class TratamientoSerializer(serializers.ModelSerializer):
    class Meta:
        model = Tratamiento
        fields = '__all__'


class HistorialTomaSerializer(serializers.ModelSerializer):
    class Meta:
        model = HistorialToma
        fields = '__all__'
        read_only_fields = ['fecha_hora_real']


class RegistrarTomaSerializer(serializers.Serializer):
    """Serializer para que el ESP32 registre una toma"""
    tratamiento_id = serializers.IntegerField()
    estado = serializers.ChoiceField(choices=['TOMADA', 'OMITIDA'], default='TOMADA')