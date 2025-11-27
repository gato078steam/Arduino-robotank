// ===================== LIBRERÍAS =====================
#include <BluetoothSerial.h>
#include <DabbleESP32.h>

// Nombre del dispositivo Bluetooth
BluetoothSerial SerialBT;

// ===================== PINES =====================
// Motores L298N
const int IN1 = 22;
const int IN2 = 21;
const int IN3 = 19;
const int IN4 = 18;
const int ENA = 23;
const int ENB = 17;

// PWM motores
const int pwmFreq = 2000;
const int pwmResolution = 8;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
int baseSpeed = 200;

// Ultrasonido
const int trigPin = 32;
const int echoPin = 35;

// Buzzer (usaremos PWM)
const int buzzerPin = 5;
const int buzzerChannel = 2;


// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  SerialBT.enableSSP();
  SerialBT.begin("ProtoTank");

  Dabble.begin("ProtoTank");

  // Motores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(pwmChannelA, pwmFreq, pwmResolution);
  ledcSetup(pwmChannelB, pwmFreq, pwmResolution);
  ledcAttachPin(ENA, pwmChannelA);
  ledcAttachPin(ENB, pwmChannelB);

  // Ultrasonido
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Buzzer PWM
  ledcSetup(buzzerChannel, 2000, 8);
  ledcAttachPin(buzzerPin, buzzerChannel);

  stopMotors();
}


// ===================== LOOP =====================
void loop() {
  Dabble.processInput();

  long dist = getDistanceCM();

  //  Alarma automática
  if (dist > 0 && dist < 15) {
    stopMotors();
    ledcWriteTone(buzzerChannel, 1800);  // beep
  } 
  else {
    ledcWriteTone(buzzerChannel, 0);     // apaga buzzer
  }

  readGamepad();
  delay(20);
}


// ===================== GAMEPAD =====================
void readGamepad() {

  // Movimiento básico
  if (GamePad.isUpPressed()) moveForward();
  else if (GamePad.isDownPressed()) moveBackward();
  else if (GamePad.isLeftPressed()) turnLeft();
  else if (GamePad.isRightPressed()) turnRight();
  else stopMotors();

  // Ajustar velocidad
  if (GamePad.isSquarePressed()) baseSpeed = constrain(baseSpeed + 20, 0, 255);
  if (GamePad.isCirclePressed()) baseSpeed = constrain(baseSpeed - 20, 0, 255);

  //  BOTÓN TRIANGLE = BUZZER MANUAL
  if (GamePad.isTrianglePressed()) {
    ledcWriteTone(buzzerChannel, 1500);  // tono manual
  }
}


// ===================== MOVIMIENTOS =====================
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, baseSpeed);
  ledcWrite(pwmChannelB, baseSpeed);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(pwmChannelA, baseSpeed);
  ledcWrite(pwmChannelB, baseSpeed);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, baseSpeed);
  ledcWrite(pwmChannelB, baseSpeed);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(pwmChannelA, baseSpeed);
  ledcWrite(pwmChannelB, baseSpeed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, 0);
  ledcWrite(pwmChannelB, 0);
}


// ===================== ULTRASONIDO =====================
long getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}