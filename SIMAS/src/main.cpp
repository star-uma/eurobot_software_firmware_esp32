#include <Arduino.h>
#include <ESP32Servo.h>

Servo motorIzquierdo;
Servo motorDerecho;

const int pinMotorIzq = 18;
const int pinMotorDer = 19;

// Pines para los sensores infrarrojos (siguelíneas)
const int pinSensorIzq = 32;
const int pinSensorDer = 33;

// Variables para el control del tiempo
unsigned long tiempoInicio;
const unsigned long tiempoSiguelineas = 10000; // 10000 milisegundos = 10 segundos

// Variable para asegurar que la acción final solo se ejecute una vez
bool accionFinalizada = false;

void setup() {
  motorIzquierdo.attach(pinMotorIzq);
  motorDerecho.attach(pinMotorDer);

  // Configuramos los pines de los sensores como entrada de datos
  pinMode(pinSensorIzq, INPUT);
  pinMode(pinSensorDer, INPUT);

  // Registramos el momento exacto en el que se enciende el robot
  tiempoInicio = millis();
}

void loop() {
  // Comprobamos si el tiempo actual menos el tiempo de inicio es menor a 10 segundos
  if (millis() - tiempoInicio < tiempoSiguelineas) {
    
    // --- ESTADO 1: MODO SIGUELÍNEAS ---
    // Leemos qué ven los sensores (HIGH suele ser línea negra, LOW suele ser fondo blanco)
    int lecturaIzq = digitalRead(pinSensorIzq);
    int lecturaDer = digitalRead(pinSensorDer);

    if (lecturaIzq == LOW && lecturaDer == LOW) {
      // Ambos ven blanco: Avanzar recto
      motorIzquierdo.write(180);
      motorDerecho.write(0);
    } 
    else if (lecturaIzq == HIGH && lecturaDer == LOW) {
      // El izquierdo toca la línea negra: Girar a la izquierda para corregir
      motorIzquierdo.write(90); // Detenemos la rueda izquierda
      motorDerecho.write(0);    // La derecha sigue empujando
    } 
    else if (lecturaIzq == LOW && lecturaDer == HIGH) {
      // El derecho toca la línea negra: Girar a la derecha para corregir
      motorIzquierdo.write(180); // La izquierda sigue empujando
      motorDerecho.write(90);    // Detenemos la rueda derecha
    } 
    else {
      // Ambos ven negro (cruce): Avanzar recto
      motorIzquierdo.write(180);
      motorDerecho.write(0);
    }
    
  } else {
    
    // --- ESTADO 2: ACCIÓN POSTERIOR ---
    // Han pasado los 10 segundos. Entramos aquí.
    // Usamos el 'if (!accionFinalizada)' para que esta secuencia ocurra una sola vez y no se repita en bucle infinito.
    if (!accionFinalizada) {
      
      // Ejemplo de acción: Parar en seco durante 1 segundo
      motorIzquierdo.write(90);
      motorDerecho.write(90);
      delay(1000); 

      // Hacer una maniobra (ej: girar sobre sí mismo hacia atrás)
      motorIzquierdo.write(0);
      motorDerecho.write(0);
      delay(1500); 

      // Parar definitivamente
      motorIzquierdo.write(90);
      motorDerecho.write(90);

      // Marcamos la acción como terminada para que el robot se quede inactivo
      accionFinalizada = true;
    }
  }
}