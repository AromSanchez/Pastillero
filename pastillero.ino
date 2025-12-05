#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <ESP32Servo.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Configuración WiFi
const char* ssid = "AROM";
const char* password = "73163917";

// Configuración Backend
const char* backendURL = "https://pastillero.onrender.com/api/tratamientos/";
const char* registrarTomaURL = "https://pastillero.onrender.com/api/registrar-toma/";
const char* notificarRecordatorioURL = "https://pastillero.onrender.com/api/notificar-recordatorio/";

// Configuración MQTT HiveMQ Cloud
const char* mqtt_server = "ece2495cb268449ca2bd2d7c0463b42d.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "esp32";
const char* mqtt_password = "aL533TEC5822";
const char* mqtt_topic_subscribe = "pastillero/actualizar";
const char* mqtt_topic_status = "pastillero/status";

// Pines
const int LED_PIN = 2;
const int BUTTON_PIN = 4;
const int BUZZER_PIN = 15;
const int SERVO_PINS[4] = {5, 18, 19, 21};

Servo servos[4];
bool servoStates[4] = {false, false, false, false};

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

bool tomaRegistrada[4] = {false, false, false, false};
bool yaAbrio[4] = {false, false, false, false};

int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// Variables para modo de llenado de pastillas
unsigned long buttonPressStartTime = 0;
const unsigned long LONG_PRESS_TIME = 3000;
bool todosAbiertos = false;
bool esperandoSoltarBoton = false;

bool buzzerSilenciado = false;
bool buzzerEncendido = false;
unsigned long ultimoCambioBuzzer = 0;
const unsigned long BUZZER_TIEMPO_ON = 300;
const unsigned long BUZZER_TIEMPO_OFF = 200;

bool alarmaActiva = false;
unsigned long tiempoInicioAlarma = 0;
const unsigned long TIMEOUT_ALARMA = 300000; 

int compartimientosEnAlarma[4];
int countCompartimientosEnAlarma = 0;

const char* servidorNTP = "pool.ntp.org";
const long desfaseGMT_segundos = -5 * 3600; 
const int horarioVerano_segundos = 0;

struct Tratamiento {
  int id;
  int compartimento;
  String nombre_pastilla;
  String repeticion;
  String hora_toma;
};

Tratamiento tratamientos[4];

// Callback para mensajes MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n🎉🎉🎉 ¡MENSAJE MQTT RECIBIDO! 🎉🎉🎉");
  Serial.println("==========================================");
  Serial.println("📨 Topic: " + String(topic));
  Serial.println("📏 Longitud: " + String(length) + " bytes");
  Serial.println("------------------------------------------");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println("📄 Contenido completo:");
  Serial.println(message);
  Serial.println("==========================================");
  
  if (String(topic) == mqtt_topic_subscribe) {
    Serial.println("✅ Es el topic correcto, procesando...");
    
    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
      Serial.println("❌ ERROR AL PARSEAR JSON:");
      Serial.println("   Código: " + String(error.c_str()));
      Serial.println("   Primer caracter: " + String((int)message[0]));
      Serial.println("==========================================\n");
      return;
    }
    
    Serial.println("✅ JSON parseado correctamente");
    actualizarTratamientosDesdeJSON(doc);
    Serial.println("✅ Tratamientos actualizados vía MQTT");
    
    // Publicar confirmación
    String confirmacion = "{\"status\":\"updated\",\"timestamp\":\"" + String(millis()) + "\"}";
    bool pubResult = mqttClient.publish(mqtt_topic_status, confirmacion.c_str());
    if (pubResult) {
      Serial.println("📤 Confirmación enviada al backend");
    } else {
      Serial.println("📤 ❌ Error al enviar confirmación");
    }
    Serial.println("==========================================\n");
  } else {
    Serial.println("⚠️ Topic no reconocido, ignorando mensaje");
    Serial.println("==========================================\n");
  }
}

// Conectar a MQTT
void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("\n==========================================");
    Serial.println("🔌 INTENTANDO CONECTAR A MQTT...");
    Serial.println("==========================================");
    Serial.println("📍 Servidor: " + String(mqtt_server));
    Serial.println("🔢 Puerto: " + String(mqtt_port));
    Serial.println("👤 Usuario: " + String(mqtt_user));
    Serial.println("🔐 Password: " + String(mqtt_password));
    Serial.println("==========================================");
    
    String clientId = "ESP32_Pastillero_" + String(random(0xffff), HEX);
    Serial.println("🆔 Client ID: " + clientId);
    
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("\n✅✅✅ ¡CONECTADO A MQTT! ✅✅✅");
      Serial.println("==========================================");
      
      // Suscribirse al topic
      bool subResult = mqttClient.subscribe(mqtt_topic_subscribe);
      if (subResult) {
        Serial.println("📬 ✅ Suscrito exitosamente a: " + String(mqtt_topic_subscribe));
      } else {
        Serial.println("📬 ❌ Error al suscribirse a: " + String(mqtt_topic_subscribe));
      }
      
      // Publicar estado online
      String mensaje = "{\"status\":\"online\",\"timestamp\":\"" + String(millis()) + "\"}";
      bool pubResult = mqttClient.publish(mqtt_topic_status, mensaje.c_str());
      if (pubResult) {
        Serial.println("📤 ✅ Estado 'online' publicado en: " + String(mqtt_topic_status));
      } else {
        Serial.println("📤 ❌ Error al publicar estado");
      }
      
      Serial.println("==========================================\n");
      
    } else {
      Serial.println("\n❌❌❌ ERROR DE CONEXIÓN MQTT ❌❌❌");
      Serial.println("==========================================");
      Serial.print("🔴 Código de error: ");
      Serial.println(mqttClient.state());
      Serial.println("\n📋 CÓDIGOS DE ERROR MQTT:");
      Serial.println("  -4 = MQTT_CONNECTION_TIMEOUT");
      Serial.println("  -3 = MQTT_CONNECTION_LOST");
      Serial.println("  -2 = MQTT_CONNECT_FAILED");
      Serial.println("  -1 = MQTT_DISCONNECTED");
      Serial.println("   0 = MQTT_CONNECTED");
      Serial.println("   1 = MQTT_CONNECT_BAD_PROTOCOL");
      Serial.println("   2 = MQTT_CONNECT_BAD_CLIENT_ID");
      Serial.println("   3 = MQTT_CONNECT_UNAVAILABLE");
      Serial.println("   4 = MQTT_CONNECT_BAD_CREDENTIALS");
      Serial.println("   5 = MQTT_CONNECT_UNAUTHORIZED");
      Serial.println("==========================================");
      Serial.println("⏳ Reintentando en 5 segundos...\n");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < 4; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(0);
  }

  // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado a WiFi!");
  Serial.println("📍 IP del ESP32: " + WiFi.localIP().toString());

  // Configurar tiempo NTP
  configTime(desfaseGMT_segundos, horarioVerano_segundos, servidorNTP);
  delay(2000);

  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    Serial.printf("🕐 Hora Perú: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    Serial.println("❌ Error al obtener hora NTP");
  }

  // Configurar MQTT con TLS
  espClient.setInsecure(); // Para HiveMQ Cloud (o puedes agregar certificados)
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(60);
  mqttClient.setSocketTimeout(30);
  
  // Conectar a MQTT
  conectarMQTT();

  // Obtener tratamientos iniciales
  obtenerTratamientos();
}

void loop() {
  // Mantener conexión MQTT
  if (!mqttClient.connected()) {
    Serial.println("⚠️ MQTT desconectado, reconectando...");
    conectarMQTT();
  }
  mqttClient.loop();
  
  // Debug de estado MQTT cada 30 segundos
  static unsigned long lastMqttDebug = 0;
  if (millis() - lastMqttDebug > 30000) {
    Serial.println("\n[DEBUG MQTT]");
    Serial.println("Estado: " + String(mqttClient.connected() ? "✅ Conectado" : "❌ Desconectado"));
    Serial.println("WiFi: " + String(WiFi.status() == WL_CONNECTED ? "✅ Conectado" : "❌ Desconectado"));
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("IP: " + WiFi.localIP().toString());
      Serial.println("RSSI: " + String(WiFi.RSSI()) + " dBm");
    }
    lastMqttDebug = millis();
  }
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    delay(1000);
    return;
  }

  int hora = timeinfo.tm_hour;
  int minuto = timeinfo.tm_min;

  int compartimientosDetectados[4];
  int countDetectados = 0;

  for (int i = 0; i < 4; i++) {
    if (tratamientos[i].compartimento != -1) {
      int th = tratamientos[i].hora_toma.substring(0,2).toInt();
      int tm = tratamientos[i].hora_toma.substring(3,5).toInt();

      if(hora == th && minuto == tm){
        if (!tomaRegistrada[i]) {
          compartimientosDetectados[countDetectados] = i;
          countDetectados++;
        }
      }
    }
  }

  if (countDetectados > 0 && !alarmaActiva) {
    alarmaActiva = true;
    tiempoInicioAlarma = millis();
    countCompartimientosEnAlarma = countDetectados;
    for (int i = 0; i < countDetectados; i++) {
      compartimientosEnAlarma[i] = compartimientosDetectados[i];
    }

    Serial.println("🚨 ¡ALARMA ACTIVADA!");

    for (int i = 0; i < countCompartimientosEnAlarma; i++) {
      int idx = compartimientosEnAlarma[i];
      if (tratamientos[idx].id != -1) {
        notificarRecordatorioEnBackend(tratamientos[idx].id);
      }
    }
  }

  if (alarmaActiva) {
    digitalWrite(LED_PIN, HIGH);
    if(!buzzerSilenciado){
      unsigned long ahora = millis();
      unsigned long intervalo = buzzerEncendido ? BUZZER_TIEMPO_ON : BUZZER_TIEMPO_OFF;
      if (ahora - ultimoCambioBuzzer >= intervalo) {
        buzzerEncendido = !buzzerEncendido;
        ultimoCambioBuzzer = ahora;
        digitalWrite(BUZZER_PIN, buzzerEncendido ? HIGH : LOW);
      }
    } else {
      buzzerEncendido = false;
      digitalWrite(BUZZER_PIN, LOW);
    }

    if (millis() - tiempoInicioAlarma > TIMEOUT_ALARMA) {
      Serial.println("⏰ Timeout - Medicamento OMITIDO");
      for (int i = 0; i < countCompartimientosEnAlarma; i++) {
        int idx = compartimientosEnAlarma[i];
        if (!tomaRegistrada[idx]) {
          registrarTomaEnBackend(tratamientos[idx].id, "OMITIDA");
          tomaRegistrada[idx] = true;
          servos[idx].write(0);
          servoStates[idx] = false;
        }
      }
      alarmaActiva = false;
      buzzerSilenciado = false;
      buzzerEncendido = false;
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      delay(60000); 
    }

  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    buzzerSilenciado = false;
    buzzerEncendido = false;
  }

  manejarBoton();
  delay(100);
}

void obtenerTratamientos() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(backendURL);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(16384);
      deserializeJson(doc, payload);

      for (int i = 0; i < 4; i++) {
        tratamientos[i].id = -1;
        tratamientos[i].compartimento = -1;
        tratamientos[i].nombre_pastilla = "";
        tratamientos[i].repeticion = "";
        tratamientos[i].hora_toma = "";
        tomaRegistrada[i] = false;
        yaAbrio[i] = false;
      }

      struct tm timeinfo;
      getLocalTime(&timeinfo);
      int horaActual = timeinfo.tm_hour * 60 + timeinfo.tm_min;

      for (JsonObject t : doc.as<JsonArray>()) {
        int comp = t["compartimento"];
        bool activo = t["activo"];

        if (activo && comp >= 1 && comp <= 4) {
          int idx = comp - 1;

          String horaStr = String((const char*)t["hora_toma"]);
          int th = horaStr.substring(0,2).toInt();
          int tm = horaStr.substring(3,5).toInt();
          int minutosTratamiento = th * 60 + tm;

          if (tratamientos[idx].compartimento == -1) {
            tratamientos[idx].id = t["id"];
            tratamientos[idx].compartimento = comp;
            tratamientos[idx].nombre_pastilla = String((const char*)t["nombre_pastilla"]);
            tratamientos[idx].repeticion = String((const char*)t["repeticion"]);
            tratamientos[idx].hora_toma = horaStr;
          } else {
            int th_guardado = tratamientos[idx].hora_toma.substring(0,2).toInt();
            int tm_guardado = tratamientos[idx].hora_toma.substring(3,5).toInt();
            int minutosGuardado = th_guardado * 60 + tm_guardado;

            if (abs(minutosTratamiento - horaActual) < abs(minutosGuardado - horaActual)) {
              tratamientos[idx].id = t["id"];
              tratamientos[idx].compartimento = comp;
              tratamientos[idx].nombre_pastilla = String((const char*)t["nombre_pastilla"]);
              tratamientos[idx].repeticion = String((const char*)t["repeticion"]);
              tratamientos[idx].hora_toma = horaStr;
            }
          }
        }
      }

      Serial.println("\n=== TRATAMIENTOS CARGADOS ===");
      for (int i = 0; i < 4; i++) {
        if (tratamientos[i].compartimento != -1) {
          Serial.println("Compartimento " + String(i+1) +
                         " (ID: " + String(tratamientos[i].id) + ")" +
                         ": " + tratamientos[i].nombre_pastilla +
                         " a las " + tratamientos[i].hora_toma);
        }
      }
      Serial.println("=============================\n");

    } else {
      Serial.println("❌ Error al obtener tratamientos: " + String(httpCode));
    }
    http.end();
  }
}

void actualizarTratamientosDesdeJSON(DynamicJsonDocument& doc) {
  for (int i = 0; i < 4; i++) {
    tratamientos[i].id = -1;
    tratamientos[i].compartimento = -1;
    tratamientos[i].nombre_pastilla = "";
    tratamientos[i].repeticion = "";
    tratamientos[i].hora_toma = "";
    tomaRegistrada[i] = false;
    yaAbrio[i] = false;
  }
  
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  int horaActual = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  
  for (JsonObject t : doc.as<JsonArray>()) {
    int comp = t["compartimento"];
    bool activo = t["activo"];
    
    if (activo && comp >= 1 && comp <= 4) {
      int idx = comp - 1;
      String horaStr = String((const char*)t["hora_toma"]);
      int th = horaStr.substring(0,2).toInt();
      int tm = horaStr.substring(3,5).toInt();
      int minutosTratamiento = th * 60 + tm;
      
      if (tratamientos[idx].compartimento == -1) {
        tratamientos[idx].id = t["id"];
        tratamientos[idx].compartimento = comp;
        tratamientos[idx].nombre_pastilla = String((const char*)t["nombre_pastilla"]);
        tratamientos[idx].repeticion = String((const char*)t["repeticion"]);
        tratamientos[idx].hora_toma = horaStr;
      } else {
        int th_guardado = tratamientos[idx].hora_toma.substring(0,2).toInt();
        int tm_guardado = tratamientos[idx].hora_toma.substring(3,5).toInt();
        int minutosGuardado = th_guardado * 60 + tm_guardado;
        
        if (abs(minutosTratamiento - horaActual) < abs(minutosGuardado - horaActual)) {
          tratamientos[idx].id = t["id"];
          tratamientos[idx].compartimento = comp;
          tratamientos[idx].nombre_pastilla = String((const char*)t["nombre_pastilla"]);
          tratamientos[idx].repeticion = String((const char*)t["repeticion"]);
          tratamientos[idx].hora_toma = horaStr;
        }
      }
    }
  }
  
  Serial.println("\n=== TRATAMIENTOS ACTUALIZADOS ===");
  for (int i = 0; i < 4; i++) {
    if (tratamientos[i].compartimento != -1) {
      Serial.println("Compartimento " + String(i+1) +
                     " (ID: " + String(tratamientos[i].id) + ")" +
                     ": " + tratamientos[i].nombre_pastilla +
                     " a las " + tratamientos[i].hora_toma);
    }
  }
  Serial.println("=================================\n");
}

void notificarRecordatorioEnBackend(int tratamientoId) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(notificarRecordatorioURL);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<128> doc;
    doc["tratamiento_id"] = tratamientoId;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpCode = http.POST(jsonString);

    if (httpCode == 200) {
      Serial.println("✅ Recordatorio enviado a backend/Telegram");
    } else {
      Serial.println("❌ Error al enviar recordatorio: " + String(httpCode));
    }

    http.end();
  }
}

void manejarBoton() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (esperandoSoltarBoton) {
    if (buttonState == HIGH) {
      esperandoSoltarBoton = false;
      lastButtonState = HIGH;
      Serial.println("✅ Botón soltado - Listo para nueva pulsación");
    }
    return;
  }

  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("🔘 BOTÓN PRESIONADO");
    buttonPressStartTime = millis();
    lastDebounceTime = millis();
  }

  if (buttonState == LOW && lastButtonState == LOW) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    
    if (pressDuration >= LONG_PRESS_TIME && !alarmaActiva) {
      Serial.println("🔧 PULSACIÓN LARGA DETECTADA - MODO LLENADO");
      
      if (!todosAbiertos) {
        Serial.println("📂 Abriendo todos los compartimentos...");
        for (int i = 0; i < 4; i++) {
          servos[i].write(90);
          servoStates[i] = true;
          Serial.println("  ✓ Compartimento " + String(i+1) + " → ABIERTO");
          delay(500);
        }
        todosAbiertos = true;
        Serial.println("✅ Todos los compartimentos abiertos");
      } else {
        Serial.println("📁 Cerrando todos los compartimentos...");
        for (int i = 0; i < 4; i++) {
          servos[i].write(0);
          servoStates[i] = false;
          Serial.println("  ✓ Compartimento " + String(i+1) + " → CERRADO");
          delay(500);
        }
        todosAbiertos = false;
        Serial.println("✅ Todos los compartimentos cerrados");
      }
      
      esperandoSoltarBoton = true;
      return;
    }
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    unsigned long pressDuration = millis() - lastDebounceTime;
    
    if (pressDuration < LONG_PRESS_TIME && pressDuration > debounceDelay) {
      Serial.println("⚡ PULSACIÓN CORTA DETECTADA");
      
      if (alarmaActiva && countCompartimientosEnAlarma > 0) {
        Serial.println("🚨 HAY ALARMA ACTIVA - Procesando toma");
        
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        buzzerSilenciado = true;
        buzzerEncendido = false;

        for (int i = 0; i < countCompartimientosEnAlarma; i++) {
          int idx = compartimientosEnAlarma[i];

          if (!tomaRegistrada[idx] && !yaAbrio[idx]) {
            servos[idx].write(90);
            servoStates[idx] = true;
            yaAbrio[idx] = true;
            Serial.println("📂 Compartimento " + String(tratamientos[idx].compartimento) + " → ABIERTO");

            registrarTomaEnBackend(tratamientos[idx].id, "TOMADA");
            tomaRegistrada[idx] = true;
            Serial.println("✅ Toma registrada para compartimento " + String(tratamientos[idx].compartimento));
          }
        }

        alarmaActiva = false;
        Serial.println("✅ Alarma desactivada");
        
      } else {
        Serial.println("ℹ SIN ALARMA - Cerrando compartimentos abiertos");
        
        bool algoCerrado = false;
        for (int i = 0; i < 4; i++) {
          if (servoStates[i]) {
            servos[i].write(0);
            servoStates[i] = false;
            Serial.println("📁 Compartimento " + String(i+1) + " → CERRADO");
            algoCerrado = true;
            delay(1000);
          }
        }
        
        if (!algoCerrado) {
          Serial.println("ℹ No hay compartimentos abiertos para cerrar");
        }
      }
    }
  }

  lastButtonState = buttonState;
}

void registrarTomaEnBackend(int tratamientoId, String estado) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(registrarTomaURL);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["tratamiento_id"] = tratamientoId;
    doc["estado"] = estado;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpCode = http.POST(jsonString);

    if (httpCode == 201) {
      Serial.println("✅ Toma registrada: " + estado);
    } else {
      Serial.println("❌ Error al registrar toma: " + String(httpCode));
    }

    http.end();
  }
}