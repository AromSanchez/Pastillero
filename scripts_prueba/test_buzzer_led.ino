/*
 * Script de Prueba: Buzzer y LED del Pastillero
 * Propósito: Validar patrón de alarma y retroalimentación visual
 * Uso: Cargar en ESP32 y usar Serial Monitor
 */

const int LED_PIN = 2;
const int BUZZER_PIN = 15;

// Configuración del patrón de beeps
unsigned long BUZZER_TIEMPO_ON = 300;   // ms
unsigned long BUZZER_TIEMPO_OFF = 200;  // ms

bool buzzerEncendido = false;
unsigned long ultimoCambioBuzzer = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("=================================");
  Serial.println("  PRUEBA DE BUZZER Y LED");
  Serial.println("=================================");
  Serial.println();
  Serial.println("Comandos disponibles:");
  Serial.println("  LED_ON  - Encender LED");
  Serial.println("  LED_OFF - Apagar LED");
  Serial.println("  BEEP    - Beep único (500ms)");
  Serial.println("  ALARMA  - Patrón de alarma (10 segundos)");
  Serial.println("  CONFIG  - Configurar tiempos de beep");
  Serial.println("  TEST    - Prueba completa");
  Serial.println();
  Serial.println("Configuración actual:");
  Serial.printf("  Tiempo ON: %lu ms\n", BUZZER_TIEMPO_ON);
  Serial.printf("  Tiempo OFF: %lu ms\n", BUZZER_TIEMPO_OFF);
  Serial.println();
  Serial.println("Esperando comandos...");
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    comando.toUpperCase();
    
    procesarComando(comando);
  }
}

void procesarComando(String cmd) {
  Serial.println();
  Serial.println(">>> Comando: " + cmd);
  
  if (cmd == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("✅ LED encendido");
  } 
  else if (cmd == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("✅ LED apagado");
  } 
  else if (cmd == "BEEP") {
    beepUnico();
  } 
  else if (cmd == "ALARMA") {
    patronAlarma();
  } 
  else if (cmd == "CONFIG") {
    configurarTiempos();
  } 
  else if (cmd == "TEST") {
    pruebaCompleta();
  } 
  else {
    Serial.println("❌ Comando no reconocido");
  }
  
  Serial.println();
  Serial.println("Esperando comandos...");
}

void beepUnico() {
  Serial.println("🔊 Emitiendo beep único...");
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("✅ Beep completado");
}

void patronAlarma() {
  Serial.println("🚨 Iniciando patrón de alarma (10 segundos)...");
  Serial.printf("   ON: %lu ms | OFF: %lu ms\n", BUZZER_TIEMPO_ON, BUZZER_TIEMPO_OFF);
  Serial.println();
  
  digitalWrite(LED_PIN, HIGH);
  
  unsigned long inicio = millis();
  unsigned long duracion = 10000; // 10 segundos
  int ciclos = 0;
  
  buzzerEncendido = false;
  ultimoCambioBuzzer = millis();
  
  while (millis() - inicio < duracion) {
    unsigned long ahora = millis();
    unsigned long intervalo = buzzerEncendido ? BUZZER_TIEMPO_ON : BUZZER_TIEMPO_OFF;
    
    if (ahora - ultimoCambioBuzzer >= intervalo) {
      buzzerEncendido = !buzzerEncendido;
      ultimoCambioBuzzer = ahora;
      digitalWrite(BUZZER_PIN, buzzerEncendido ? HIGH : LOW);
      
      if (buzzerEncendido) {
        ciclos++;
        Serial.printf("  Ciclo %d - BEEP\n", ciclos);
      }
    }
  }
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println();
  Serial.printf("✅ Patrón completado - %d ciclos en 10 segundos\n", ciclos);
  
  // Calcular frecuencia esperada
  float frecuenciaEsperada = 10000.0 / (BUZZER_TIEMPO_ON + BUZZER_TIEMPO_OFF);
  Serial.printf("   Frecuencia esperada: %.1f ciclos\n", frecuenciaEsperada);
  Serial.printf("   Diferencia: %.1f ciclos\n", abs(ciclos - frecuenciaEsperada));
}

void configurarTiempos() {
  Serial.println("Configurar tiempos de beep");
  Serial.println("Ingrese tiempo ON en ms (actual: " + String(BUZZER_TIEMPO_ON) + "):");
  
  while (!Serial.available()) {
    delay(100);
  }
  
  String input = Serial.readStringUntil('\n');
  int nuevoOn = input.toInt();
  
  if (nuevoOn > 0 && nuevoOn < 2000) {
    BUZZER_TIEMPO_ON = nuevoOn;
    Serial.println("✅ Tiempo ON actualizado: " + String(BUZZER_TIEMPO_ON) + " ms");
  } else {
    Serial.println("❌ Valor inválido");
    return;
  }
  
  Serial.println("Ingrese tiempo OFF en ms (actual: " + String(BUZZER_TIEMPO_OFF) + "):");
  
  while (!Serial.available()) {
    delay(100);
  }
  
  input = Serial.readStringUntil('\n');
  int nuevoOff = input.toInt();
  
  if (nuevoOff > 0 && nuevoOff < 2000) {
    BUZZER_TIEMPO_OFF = nuevoOff;
    Serial.println("✅ Tiempo OFF actualizado: " + String(BUZZER_TIEMPO_OFF) + " ms");
  } else {
    Serial.println("❌ Valor inválido");
  }
}

void pruebaCompleta() {
  Serial.println("========================================");
  Serial.println("  PRUEBA COMPLETA DE BUZZER Y LED");
  Serial.println("========================================");
  Serial.println();
  
  // Prueba 1: LED
  Serial.println("Prueba 1: LED");
  Serial.println("----------------------------------------");
  Serial.println("Encendiendo LED...");
  digitalWrite(LED_PIN, HIGH);
  delay(2000);
  Serial.println("Apagando LED...");
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  // Prueba 2: Beeps individuales
  Serial.println();
  Serial.println("Prueba 2: 3 beeps individuales");
  Serial.println("----------------------------------------");
  for (int i = 1; i <= 3; i++) {
    Serial.printf("Beep %d/3\n", i);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
  
  delay(1000);
  
  // Prueba 3: Patrón de alarma
  Serial.println();
  Serial.println("Prueba 3: Patrón de alarma (5 segundos)");
  Serial.println("----------------------------------------");
  
  digitalWrite(LED_PIN, HIGH);
  unsigned long inicio = millis();
  buzzerEncendido = false;
  ultimoCambioBuzzer = millis();
  
  while (millis() - inicio < 5000) {
    unsigned long ahora = millis();
    unsigned long intervalo = buzzerEncendido ? BUZZER_TIEMPO_ON : BUZZER_TIEMPO_OFF;
    
    if (ahora - ultimoCambioBuzzer >= intervalo) {
      buzzerEncendido = !buzzerEncendido;
      ultimoCambioBuzzer = ahora;
      digitalWrite(BUZZER_PIN, buzzerEncendido ? HIGH : LOW);
    }
  }
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println();
  Serial.println("========================================");
  Serial.println("  PRUEBA COMPLETA FINALIZADA");
  Serial.println("========================================");
}
