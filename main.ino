#include <Thread.h>
#include <ThreadController.h>
#include <Ultrasonic.h>

#define echoPin 9 // Define a porta 9 do Arduino como recebimento do Echo do
// sensor Ultrassônico.
#define trigPin 8 // Define a porta 8 como emissor do pulso ultrassônico
// do sensor.
#define vccPin A3 // Define a porta A3 como recebimento do VCC do sensor Infravermelho (linha).

Ultrasonic ultrasonic(8, 9);

// Definição das portas Arduino ligadas a ponte H.
int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;
int ENA = 11;
int ENB = 10;

// Definição da variável de recebimento dos dados do sensor Ultrassônico.
float sen;

//Definição da variável de recebimento dos dados do sensor Infravermelho (linha).
const int sensorInfra = A3;
int valorSensorInfra = 0;

// Definição das threads para o controle das rodas e leitura do sensor.
Thread sonicThread = Thread();
Thread wheelThread = Thread();
Thread infraThread = Thread();

// Definição do controlador de threads.
ThreadController controller = ThreadController();

// ------------------------Seção das funções---------------------------------

// Função de leitura do sensor Ultrassônico.
void captSensor() {
  sen = ultrasonic.Ranging(CM); // Armazena o valor de leitura do sensor em
  // centímetros.
  Serial.println(sen);
}

void controlRodas() {

  if (sen <= 30) { // Se o sensor identificar obstáculo a 30 cm ou menos...
    Serial.println("Alvo identificado! Andando pra frente");
    analogWrite(ENA, 255);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, 255);
  } else {
    Serial.println("Andando em círculos");
    analogWrite(ENA, 50);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, 150);
  }
}

void captSensor2() {

  valorSensorInfra = analogRead(sensorInfra);

  if (valorSensorInfra < 1000) {
    Serial.println("Linha identificada!");
    analogWrite(ENA, 150);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, 255);
  }
  else {
    Serial.println("Linha não identificada!");
    analogWrite(ENA, 255);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, 255);


  }
  Serial.println(valorSensorInfra);

}

// ------------------------Seção de setur do Arduino-------------------------
void setup() {
  Serial.begin(9600); // Inicializa comunicação serial para Debug na IDE.
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(A3, INPUT);

  infraThread.onRun(captSensor2);
  infraThread.setInterval(30); // Seta ativação de thread em 6 ms.

  sonicThread.onRun(captSensor);
  sonicThread.setInterval(30); // Seta ativação de thread em 6 ms.

  wheelThread.onRun(controlRodas);
  wheelThread.setInterval(50); // Seta ativação de thread em 10 ms.

  controller.add(&infraThread);
  controller.add(&sonicThread);
  controller.add(&wheelThread);

  delay(5000); // Espera 5000 ms (5 segundos) para o robô iniciar ação.
}

void loop() {
  controller.run();
}