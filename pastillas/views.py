from rest_framework import viewsets, status
from rest_framework.decorators import api_view
from rest_framework.response import Response
from django.utils import timezone
from django.conf import settings
import requests
from .models import Tratamiento, HistorialToma, ConfiguracionNotificaciones
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


def enviar_mensaje_telegram(texto: str) -> bool:
    """Envía un mensaje de texto sencillo al chat configurado en settings.

    Devuelve True si aparentemente se envió bien, False en caso contrario.
    """
    try:
        config = ConfiguracionNotificaciones.objects.first()
        if config and not config.telegram_activo:
            return False
    except Exception:
        pass

    bot_token = getattr(settings, "TELEGRAM_BOT_TOKEN", "")
    chat_id = getattr(settings, "TELEGRAM_CHAT_ID", "")

    if not bot_token or not chat_id:
        # Configuración incompleta
        return False

    url = f"https://api.telegram.org/bot{bot_token}/sendMessage"
    try:
        resp = requests.post(url, json={"chat_id": chat_id, "text": texto})
        # Consideramos éxito si Telegram responde 200 y ok=True
        if resp.status_code == 200:
            data = resp.json()
            return bool(data.get("ok"))
    except Exception:
        return False

    return False


@api_view(["GET"])
def test_telegram(request):
    """Endpoint de prueba para enviar un mensaje básico a Telegram.

    GET /api/test-telegram/
    """
    ok = enviar_mensaje_telegram("✅ Prueba de notificación desde el Pastillero Inteligente")
    if not ok:
        return Response(
            {"detail": "No se pudo enviar el mensaje. Revisa TELEGRAM_BOT_TOKEN y TELEGRAM_CHAT_ID en settings."},
            status=status.HTTP_500_INTERNAL_SERVER_ERROR,
        )
    return Response({"detail": "Mensaje enviado a Telegram correctamente"})


@api_view(["POST"])
def notificar_recordatorio(request):
    """Endpoint para enviar un recordatorio de toma a Telegram.

    POST /api/notificar-recordatorio/
    Body: { "tratamiento_id": 1 }
    """
    tratamiento_id = request.data.get("tratamiento_id")
    if tratamiento_id is None:
        return Response({"detail": "tratamiento_id es requerido"}, status=status.HTTP_400_BAD_REQUEST)

    try:
        tratamiento = Tratamiento.objects.get(id=tratamiento_id)
    except Tratamiento.DoesNotExist:
        return Response({"detail": "Tratamiento no encontrado"}, status=status.HTTP_404_NOT_FOUND)

    mensaje = (
        f"🕒 Es hora de tomar tu medicamento\n"
        f"Pastilla: {tratamiento.nombre_pastilla}\n"
        f"Compartimento: {tratamiento.compartimento}\n"
        f"Hora programada: {tratamiento.hora_toma}"
    )

    ok = enviar_mensaje_telegram(mensaje)
    if not ok:
        return Response(
            {"detail": "No se pudo enviar el mensaje a Telegram"},
            status=status.HTTP_500_INTERNAL_SERVER_ERROR,
        )

    return Response({"detail": "Recordatorio enviado a Telegram"}, status=status.HTTP_200_OK)


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

    # Enviar notificación a Telegram sobre el resultado de la toma
    try:
        if estado == "TOMADA":
            mensaje = (
                f"✅ Toma registrada como TOMADA\n"
                f"Pastilla: {tratamiento.nombre_pastilla}\n"
                f"Compartimento: {tratamiento.compartimento}\n"
                f"Hora programada: {tratamiento.hora_toma}"
            )
        else:
            mensaje = (
                f"⚠️ Toma registrada como OMITIDA\n"
                f"Pastilla: {tratamiento.nombre_pastilla}\n"
                f"Compartimento: {tratamiento.compartimento}\n"
                f"Hora programada: {tratamiento.hora_toma}"
            )

        enviar_mensaje_telegram(mensaje)
    except Exception:
        # No romper el flujo si hay algún problema con Telegram
        pass

    return Response(
        HistorialTomaSerializer(historial).data,
        status=status.HTTP_201_CREATED
    )