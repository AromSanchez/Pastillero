from rest_framework import viewsets, status
from rest_framework.decorators import api_view
from rest_framework.response import Response
from django.utils import timezone
from .models import Tratamiento, HistorialToma
from .serializers import TratamientoSerializer, HistorialTomaSerializer, RegistrarTomaSerializer

class TratamientoViewSet(viewsets.ModelViewSet):
    queryset = Tratamiento.objects.all()
    serializer_class = TratamientoSerializer


class HistorialTomaViewSet(viewsets.ReadOnlyModelViewSet):
    """
    ViewSet de solo lectura para consultar el historial.
    El frontend puede hacer GET para ver el historial.
    """
    queryset = HistorialToma.objects.all()
    serializer_class = HistorialTomaSerializer
    
    def get_queryset(self):
        queryset = super().get_queryset()
        # Filtrar por fecha si se proporciona
        fecha = self.request.query_params.get('fecha', None)
        if fecha:
            queryset = queryset.filter(fecha_hora_real__date=fecha)
        return queryset


@api_view(['POST'])
def registrar_toma(request):
    """
    Endpoint para que el ESP32 registre cuando se toma (o no) un medicamento.
    
    POST /api/registrar-toma/
    Body: {
        "tratamiento_id": 1,
        "estado": "TOMADA"  // o "OMITIDA"
    }
    """
    serializer = RegistrarTomaSerializer(data=request.data)
    
    if not serializer.is_valid():
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
    
    tratamiento_id = serializer.validated_data['tratamiento_id']
    estado = serializer.validated_data['estado']
    
    try:
        tratamiento = Tratamiento.objects.get(id=tratamiento_id)
    except Tratamiento.DoesNotExist:
        return Response(
            {"error": "Tratamiento no encontrado"}, 
            status=status.HTTP_404_NOT_FOUND
        )
    
    # Crear registro en el historial
    historial = HistorialToma.objects.create(
        tratamiento=tratamiento,
        nombre_pastilla=tratamiento.nombre_pastilla,
        compartimento=tratamiento.compartimento,
        hora_programada=tratamiento.hora_toma,
        fecha_hora_real=timezone.now(),
        estado=estado
    )
    
    # Si se tomó, reducir el stock
    if estado == "TOMADA" and tratamiento.stock >= tratamiento.dosis:
        tratamiento.stock -= tratamiento.dosis
        tratamiento.save()
    
    return Response(
        HistorialTomaSerializer(historial).data,
        status=status.HTTP_201_CREATED
    )