import paho.mqtt.client as mqtt
import json
import logging

logger = logging.getLogger(__name__)

# Configuración de HiveMQ Cloud
MQTT_BROKER = "ece2495cb268449ca2bd2d7c0463b42d.s1.eu.hivemq.cloud"
MQTT_PORT = 8883
MQTT_USER = "esp32"
MQTT_PASSWORD = "aL533TEC5822"
MQTT_TOPIC_ACTUALIZAR = "pastillero/actualizar"
MQTT_TOPIC_STATUS = "pastillero/status"


def publicar_tratamientos():
    """
    Publica todos los tratamientos activos al ESP32 vía MQTT.
    
    Retorna True si se publicó correctamente, False en caso contrario.
    """
    try:
        from .models import Tratamiento
        
        # Obtener todos los tratamientos activos
        tratamientos = list(Tratamiento.objects.filter(activo=True).values(
            'id', 'compartimento', 'nombre_pastilla', 'repeticion', 'hora_toma', 'activo'
        ))
        
        logger.info(f"📋 Preparando {len(tratamientos)} tratamiento(s) activo(s) para publicar")
        
        # Convertir a JSON (default=str para manejar time objects)
        mensaje = json.dumps(tratamientos, default=str)
        
        # Crear cliente MQTT
        client = mqtt.Client(client_id="django_backend", protocol=mqtt.MQTTv311)
        client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
        client.tls_set()  # Habilitar TLS/SSL
        
        # Callbacks para debug
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                logger.info(f"✅ Conectado exitosamente a MQTT broker")
            else:
                logger.error(f"❌ Error de conexión MQTT. Código: {rc}")
        
        def on_publish(client, userdata, mid):
            logger.info(f"✅ Mensaje publicado exitosamente (mid: {mid})")
        
        client.on_connect = on_connect
        client.on_publish = on_publish
        
        # Conectar al broker
        logger.info(f"🔌 Conectando a MQTT: {MQTT_BROKER}:{MQTT_PORT}")
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        
        # Publicar mensaje
        result = client.publish(MQTT_TOPIC_ACTUALIZAR, mensaje, qos=1)
        
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            logger.info(f"📤 Mensaje enviado al topic '{MQTT_TOPIC_ACTUALIZAR}'")
            logger.debug(f"📄 Contenido: {mensaje}")
        else:
            logger.error(f"❌ Error al publicar mensaje. Código: {result.rc}")
            client.disconnect()
            return False
        
        # Esperar a que se complete la publicación
        client.loop(timeout=2.0)
        client.disconnect()
        
        logger.info(f"🔌 Desconectado de MQTT broker")
        return True
        
    except Exception as e:
        logger.error(f"❌ Error en publicar_tratamientos: {str(e)}", exc_info=True)
        return False


def publicar_mensaje_personalizado(topic, mensaje_dict):
    """
    Publica un mensaje personalizado a un topic específico.
    
    Args:
        topic (str): Topic MQTT al que publicar
        mensaje_dict (dict): Diccionario que se convertirá a JSON
    
    Retorna True si se publicó correctamente, False en caso contrario.
    """
    try:
        mensaje = json.dumps(mensaje_dict, default=str)
        
        client = mqtt.Client(client_id="django_backend", protocol=mqtt.MQTTv311)
        client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
        client.tls_set()
        
        logger.info(f"🔌 Conectando a MQTT para mensaje personalizado")
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        
        result = client.publish(topic, mensaje, qos=1)
        
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            logger.info(f"📤 Mensaje personalizado enviado a '{topic}'")
            logger.debug(f"📄 Contenido: {mensaje}")
        else:
            logger.error(f"❌ Error al publicar mensaje personalizado. Código: {result.rc}")
            client.disconnect()
            return False
        
        client.loop(timeout=2.0)
        client.disconnect()
        
        return True
        
    except Exception as e:
        logger.error(f"❌ Error en publicar_mensaje_personalizado: {str(e)}", exc_info=True)
        return False
