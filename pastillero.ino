#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <ESP32Servo.h>
#include <WebServer.h> 

const char* ssid = "Alicia";
const char* password = "lizzy1506";
const char* backendURL = "http://10.147.1.123:8000/api/tratamientos/";
const char* registrarTomaURL = "http://10.147.1.123:8000/api/registrar-toma/";
const char* notificarRecordatorioURL = "http://10.147.1.123:8000/api/notificar-recordatorio/";

const int LED_PIN = 2;
const int BUTTON_PIN = 4;
const int BUZZER_PIN = 15;
const int SERVO_PINS[4] = {5, 18, 19, 21};

Servo servos[4];
bool servoStates[4] = {false, false, false, false};

// Servidor HTTP para recibir actualizaciones del backend
WebServer server(80);

bool tomaRegistrada[4] = {false, false, false, false};

bool yaAbrio[4] = {false, false, false, false};

int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// Variables para modo de llenado de pastillas
unsigned long buttonPressStartTime = 0;
const unsigned long LONG_PRESS_TIME = 3000;  // 3 segundos para activar modo llenado
bool modoLlenado = false;
bool todosAbiertos = false;  // Estado de los compartimentos en modo llenado

// Variables para polling (consulta periódica al backend)
unsigned long ultimaConsultaBackend = 0;
const unsigned long INTERVALO_CONSULTA = 60000;  // Consultar cada 60 segundos (1 minuto)

bool buzzerSilenciado = false;

// Estado del patrón de sonido del buzzer (beeps agradables)
bool buzzerEncendido = false;
unsigned long ultimoCambioBuzzer = 0;
const unsigned long BUZZER_TIEMPO_ON = 300;   // ms encendido
const unsigned long BUZZER_TIEMPO_OFF = 200;  // ms apagado

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

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi!");

  configTime(desfaseGMT_segundos, horarioVerano_segundos, servidorNTP);
  delay(2000);
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());

  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    Serial.printf("Hora Perú: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    Serial.println("Error al obtener hora NTP");
  }

  // Configurar servidor HTTP
  server.on("/actualizar", HTTP_POST, handleActualizar);
  server.begin();
  Serial.println("Servidor HTTP iniciado en puerto 80");

  obtenerTratamientos();
}

void loop() {
  // Procesar peticiones HTTP entrantes (para sincronización local si está disponible)
  server.handleClient();
  
  // Consultar backend periódicamente para obtener cambios
  if (millis() - ultimaConsultaBackend >= INTERVALO_CONSULTA) {
    Serial.println("\n[POLLING] Consultando backend por actualizaciones...");
    obtenerTratamientos();
    ultimaConsultaBackend = millis();
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

  // Si hay nuevos compartimientos detectados en la hora exacta y no había alarma, iniciarla
  if (countDetectados > 0 && !alarmaActiva) {
    alarmaActiva = true;
    tiempoInicioAlarma = millis();
    countCompartimientosEnAlarma = countDetectados;
    for (int i = 0; i < countDetectados; i++) {
      compartimientosEnAlarma[i] = compartimientosDetectados[i];
    }

    Serial.println("¡ALARMA ACTIVADA!");

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
      // Patrón de beeps no bloqueante
      unsigned long ahora = millis();
      unsigned long intervalo = buzzerEncendido ? BUZZER_TIEMPO_ON : BUZZER_TIEMPO_OFF;
      if (ahora - ultimoCambioBuzzer >= intervalo) {
        buzzerEncendido = !buzzerEncendido;
        ultimoCambioBuzzer = ahora;
        digitalWrite(BUZZER_PIN, buzzerEncendido ? HIGH : LOW);
      }
    } else {
      // Si está silenciado, asegurarnos de que esté en LOW
      buzzerEncendido = false;
      digitalWrite(BUZZER_PIN, LOW);
    }

    if (millis() - tiempoInicioAlarma > TIMEOUT_ALARMA) {
      Serial.println("Timeout - Medicamento OMITIDO");
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
    // No hay alarma activa
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    buzzerSilenciado = false;
    buzzerEncendido = false;
    // No cerrar servos automáticamente aquí: se cerrarán solo cuando el usuario
    // vuelva a presionar el botón (modo sin toma activa en manejarBoton).
  }

  // Manejar pulsos del botón en cualquier estado:
  // - Pulsación larga (2s): Modo llenado de pastillas (solo si no hay alarma activa)
  // - Pulsación corta con alarma: abrir y registrar TOMADA
  // - Pulsación corta sin alarma: cerrar compartimientos abiertos
  manejarBoton(compartimientosEnAlarma, countCompartimientosEnAlarma);

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
        // Resetear el estado de la toma con la nueva programación
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

      for (int i = 0; i < 4; i++) {
        if (tratamientos[i].compartimento != -1) {
          Serial.println("Tratamiento en compartimento " + String(i+1) +
                         " (ID: " + String(tratamientos[i].id) + ")" +
                         ": " + tratamientos[i].nombre_pastilla +
                         " a las " + tratamientos[i].hora_toma);
        }
      }

    } else {
      Serial.println("Error al obtener tratamientos: " + String(httpCode));
    }
    http.end();
  }
}

void handleActualizar() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }
  
  String body = server.arg("plain");
  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    Serial.println("Error al parsear JSON: " + String(error.c_str()));
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }
  
  // Actualizar tratamientos desde el JSON recibido
  actualizarTratamientosDesdeJSON(doc);
  
  server.send(200, "text/plain", "Tratamientos actualizados");
  Serial.println("✅ Tratamientos actualizados desde backend");
}

void actualizarTratamientosDesdeJSON(DynamicJsonDocument& doc) {
  // Resetear todos los tratamientos
  for (int i = 0; i < 4; i++) {
    tratamientos[i].id = -1;
    tratamientos[i].compartimento = -1;
    tratamientos[i].nombre_pastilla = "";
    tratamientos[i].repeticion = "";
    tratamientos[i].hora_toma = "";
    tomaRegistrada[i] = false;
    yaAbrio[i] = false;
  }
  
  // Cargar nuevos datos (misma lógica que obtenerTratamientos)
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
  
  // Mostrar tratamientos actualizados
  Serial.println("\n=== TRATAMIENTOS ACTUALIZADOS ===");
  for (int i = 0; i < 4; i++) {
    if (tratamientos[i].compartimento != -1) {
      Serial.println("Tratamiento en compartimento " + String(i+1) +
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
      Serial.println(http.getString());
    }

    http.end();
  }
}

void manejarBoton(int compartimientosActivos[], int count) {
  int buttonState = digitalRead(BUTTON_PIN);

  // Detectar inicio de pulsación
  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStartTime = millis();
    lastDebounceTime = millis();
  }

  // Detectar pulsación larga (3 segundos) - SOLO si NO hay alarma activa
  if (buttonState == LOW && lastButtonState == LOW && !alarmaActiva) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    
    if (pressDuration >= LONG_PRESS_TIME && !modoLlenado) {
      // Activar modo llenado
      modoLlenado = true;
      Serial.println("🔧 MODO LLENADO ACTIVADO");
      
      if (!todosAbiertos) {
        // Abrir todos los compartimentos uno por uno
        Serial.println("Abriendo compartimentos...");
        for (int i = 0; i < 4; i++) {
          servos[i].write(90);
          servoStates[i] = true;
          Serial.println("  Compartimento " + String(i+1) + " → ABIERTO");
          delay(500);  // 500ms entre cada apertura para evitar pico de corriente
        }
        todosAbiertos = true;
        Serial.println("✅ Todos los compartimentos abiertos");
      } else {
        // Cerrar todos los compartimentos uno por uno
        Serial.println("Cerrando compartimentos...");
        for (int i = 0; i < 4; i++) {
          servos[i].write(0);
          servoStates[i] = false;
          Serial.println("  Compartimento " + String(i+1) + " → CERRADO");
          delay(500);  // 500ms entre cada cierre
        }
        todosAbiertos = false;
        Serial.println("✅ Todos los compartimentos cerrados");
      }
      
      // Esperar a que suelte el botón
      while(digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
      modoLlenado = false;
      lastButtonState = HIGH;
      return;
    }
  }

  // Pulsación corta - comportamiento normal
  if(buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
    // Verificar que no sea una pulsación larga en progreso
    if (millis() - buttonPressStartTime < LONG_PRESS_TIME) {
      
      if (count > 0 && alarmaActiva) {
        // Modo "toma activa": abrir compartimiento y registrar TOMADA una sola vez
        // Primer pulsado válido: silenciar buzzer y apagar LED
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        buzzerSilenciado = true;
        buzzerEncendido = false;

        for (int i = 0; i < count; i++) {
          int idx = compartimientosActivos[i];

          // Si ya se registró la toma para este compartimiento, no hacer nada
          if (tomaRegistrada[idx]) {
            continue;
          }

          // Si aún no se abrió en esta toma, abrir una sola vez y registrar TOMADA
          if (!yaAbrio[idx]) {
            servos[idx].write(90);
            servoStates[idx] = true;
            yaAbrio[idx] = true;
            Serial.println("Compartimento " + String(tratamientos[idx].compartimento) + " → 90° (abierto)");

            registrarTomaEnBackend(tratamientos[idx].id, "TOMADA");
            tomaRegistrada[idx] = true;

            delay(1000);
          }
        }

        // Resetear alarma después de gestionar el compartimiento
        alarmaActiva = false;
      } else {
        // Modo "sin toma activa": solo permitir cerrar compartimentos de la lista recibida,
        // en el mismo orden y con 1 segundo de diferencia entre cada uno.
        for (int i = 0; i < count; i++) {
          int idx = compartimientosActivos[i];
          if (servoStates[idx]) {
            servos[idx].write(0);
            servoStates[idx] = false;
            Serial.println("Cerrar compartimento " + String(idx + 1) + " tras toma registrada");
            delay(1000);
          }
        }
      }

      lastDebounceTime = millis();
    }
  }

  lastButtonState = buttonState;
}

void registrarTomaEnBackend(int tratamientoId, String estado) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(registrarTomaURL);
    http.addHeader("Content-Type", "application/json");

    // Crear JSON
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
      Serial.println(http.getString());
    }

    http.end();
  }
}