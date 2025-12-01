/*
 * Script de Prueba: Servos del Pastillero
 * Propósito: Probar individualmente cada servo para validar apertura/cierre
 * Uso: Cargar en ESP32 y usar Serial Monitor para controlar
 */

#include <ESP32Servo.h>

const int SERVO_PINS[4] = {5, 18, 19, 21};
Servo servos[4];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=================================");
  Serial.println("  PRUEBA DE SERVOS - PASTILLERO");
  Serial.println("=================================");
  Serial.println();
  
  // Inicializar servos
  for (int i = 0; i < 4; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(0);
    Serial.printf("✓ Servo %d inicializado en pin %d\n", i+1, SERVO_PINS[i]);
  }
  
  Serial.println();
  Serial.println("Comandos disponibles:");
  Serial.println("  1A - Abrir compartimento 1");
  Serial.println("  1C - Cerrar compartimento 1");
  Serial.println("  2A - Abrir compartimento 2");
  Serial.println("  2C - Cerrar compartimento 2");
  Serial.println("  3A - Abrir compartimento 3");
  Serial.println("  3C - Cerrar compartimento 3");
  Serial.println("  4A - Abrir compartimento 4");
  Serial.println("  4C - Cerrar compartimento 4");
  Serial.println("  AA - Abrir TODOS");
  Serial.println("  CC - Cerrar TODOS");
  Serial.println("  TEST - Ciclo de prueba completo");
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
  Serial.println(">>> Comando recibido: " + cmd);
  
  if (cmd == "1A") {
    abrirCompartimento(0);
  } else if (cmd == "1C") {
    cerrarCompartimento(0);
  } else if (cmd == "2A") {
    abrirCompartimento(1);
  } else if (cmd == "2C") {
    cerrarCompartimento(1);
  } else if (cmd == "3A") {
    abrirCompartimento(2);
  } else if (cmd == "3C") {
    cerrarCompartimento(2);
  } else if (cmd == "4A") {
    abrirCompartimento(3);
  } else if (cmd == "4C") {
    cerrarCompartimento(3);
  } else if (cmd == "AA") {
    abrirTodos();
  } else if (cmd == "CC") {
    cerrarTodos();
  } else if (cmd == "TEST") {
    cicloCompleto();
  } else {
    Serial.println("❌ Comando no reconocido");
  }
  
  Serial.println();
  Serial.println("Esperando comandos...");
}

void abrirCompartimento(int idx) {
  unsigned long inicio = millis();
  Serial.printf("⏱️  Abriendo compartimento %d...\n", idx+1);
  
  servos[idx].write(90);
  
  unsigned long fin = millis();
  unsigned long duracion = fin - inicio;
  
  Serial.printf("✅ Compartimento %d ABIERTO (90°)\n", idx+1);
  Serial.printf("⏱️  Tiempo: %lu ms\n", duracion);
}

void cerrarCompartimento(int idx) {
  unsigned long inicio = millis();
  Serial.printf("⏱️  Cerrando compartimento %d...\n", idx+1);
  
  servos[idx].write(0);
  
  unsigned long fin = millis();
  unsigned long duracion = fin - inicio;
  
  Serial.printf("✅ Compartimento %d CERRADO (0°)\n", idx+1);
  Serial.printf("⏱️  Tiempo: %lu ms\n", duracion);
}

void abrirTodos() {
  Serial.println("⏱️  Abriendo TODOS los compartimentos...");
  unsigned long inicio = millis();
  
  for (int i = 0; i < 4; i++) {
    servos[i].write(90);
    Serial.printf("  ✓ Compartimento %d → 90°\n", i+1);
    delay(500); // Delay entre servos para evitar pico de corriente
  }
  
  unsigned long fin = millis();
  Serial.printf("✅ Todos abiertos en %lu ms\n", fin - inicio);
}

void cerrarTodos() {
  Serial.println("⏱️  Cerrando TODOS los compartimentos...");
  unsigned long inicio = millis();
  
  for (int i = 0; i < 4; i++) {
    servos[i].write(0);
    Serial.printf("  ✓ Compartimento %d → 0°\n", i+1);
    delay(500);
  }
  
  unsigned long fin = millis();
  Serial.printf("✅ Todos cerrados en %lu ms\n", fin - inicio);
}

void cicloCompleto() {
  Serial.println("========================================");
  Serial.println("  INICIANDO CICLO DE PRUEBA COMPLETO");
  Serial.println("========================================");
  Serial.println();
  
  // Cerrar todos primero
  Serial.println("Paso 1: Cerrando todos los compartimentos");
  cerrarTodos();
  delay(2000);
  
  // Probar cada compartimento individualmente
  for (int i = 0; i < 4; i++) {
    Serial.println();
    Serial.printf("Paso %d: Probando compartimento %d\n", i+2, i+1);
    Serial.println("----------------------------------------");
    
    abrirCompartimento(i);
    delay(2000);
    
    cerrarCompartimento(i);
    delay(2000);
  }
  
  // Abrir todos simultáneamente
  Serial.println();
  Serial.println("Paso 6: Abriendo todos simultáneamente");
  Serial.println("----------------------------------------");
  abrirTodos();
  delay(3000);
  
  // Cerrar todos
  Serial.println();
  Serial.println("Paso 7: Cerrando todos");
  Serial.println("----------------------------------------");
  cerrarTodos();
  
  Serial.println();
  Serial.println("========================================");
  Serial.println("  CICLO DE PRUEBA COMPLETADO");
  Serial.println("========================================");
}
