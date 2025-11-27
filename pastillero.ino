#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <ESP32Servo.h> 

const char* ssid = "Alicia";
const char* password = "lizzy1506";
const char* server = "http://10.147.1.123:8000/api/tratamientos/";
const char* registrarTomaURL = "http://10.147.1.123:8000/api/registrar-toma/";
const char* notificarRecordatorioURL = "http://10.147.1.123:8000/api/notificar-recordatorio/";

const int LED_PIN = 2;
const int BUTTON_PIN = 4;
const int BUZZER_PIN = 15;
const int SERVO_PINS[4] = {5, 18, 19, 21};

Servo servos[4];
bool servoStates[4] = {false, false, false, false};

bool tomaRegistrada[4] = {false, false, false, false};

bool yaAbrio[4] = {false, false, false, false};

int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

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

// Refresco periódico de tratamientos desde el backend
unsigned long ultimoFetchTratamientos = 0;
const unsigned long INTERVALO_FETCH_MS = 30000;  // 30 segundos

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

  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    Serial.printf("Hora Perú: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    Serial.println("Error al obtener hora NTP");
  }

  obtenerTratamientos();
}

void loop() {
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
  // - Si hay compartimientos en alarma (countCompartimientosEnAlarma > 0 y alarmaActiva): abrir y registrar TOMADA.
  // - Si no hay alarma activa: permitir cerrar compartimientos que quedaron abiertos.
  manejarBoton(compartimientosEnAlarma, countCompartimientosEnAlarma);

  // Refrescar tratamientos desde el backend cada cierto tiempo cuando no hay alarma activa
  if (!alarmaActiva) {
    unsigned long ahora = millis();
    if (ahora - ultimoFetchTratamientos >= INTERVALO_FETCH_MS) {
      obtenerTratamientos();
      ultimoFetchTratamientos = ahora;
    }
  }

  delay(100);
}

void obtenerTratamientos() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
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
        // Importante: NO resetear aquí tomaRegistrada ni yaAbrio para no
        // reactivar alarmas ya atendidas cuando se refrescan los datos.
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

          // Guardar valores anteriores para saber si realmente cambió algo
          int idAnterior = tratamientos[idx].id;
          String horaAnterior = tratamientos[idx].hora_toma;

          if (tratamientos[idx].compartimento == -1) {
            // Nuevo tratamiento en este compartimento
            tratamientos[idx].id = t["id"];
            tratamientos[idx].compartimento = comp;
            tratamientos[idx].nombre_pastilla = String((const char*)t["nombre_pastilla"]);
            tratamientos[idx].repeticion = String((const char*)t["repeticion"]);
            tratamientos[idx].hora_toma = horaStr;

            // Como es nuevo, aseguramos que pueda sonar la alarma
            tomaRegistrada[idx] = false;
            yaAbrio[idx] = false;
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

              // Solo resetear flags si realmente cambió el tratamiento (id u hora)
              if (tratamientos[idx].id != idAnterior || tratamientos[idx].hora_toma != horaAnterior) {
                tomaRegistrada[idx] = false;
                yaAbrio[idx] = false;
              }
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

  if(buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
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