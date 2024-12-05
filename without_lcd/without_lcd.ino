// Pin Definitions
const int potPin = A0;      // Potentiometer connected to analog pin A0
const int motorEnable = 9;  // Enable pin (PWM) for motor on L298D
const int motorIn1 = 8;     // IN1 pin for motor direction control
const int motorIn2 = 7;     // IN2 pin for motor direction control
const int encoderC1 = 2;    // Encoder C1 connected to digital pin 2 (interrupt pin)
const int encoderC2 = 4;    // Encoder C2 connected to digital pin 4

// Variables to track encoder counts
volatile int encoderCount = 0;  // Encoder pulse count
int lastC2State = LOW;          // Last state of C2 for direction detection

void setup() {
  // Initialize motor pins
  pinMode(motorEnable, OUTPUT);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);

  // Initialize encoder pins
  pinMode(encoderC1, INPUT_PULLUP);
  pinMode(encoderC2, INPUT_PULLUP);

  // Attach interrupt for encoder C1 (rising edge detection)
  attachInterrupt(digitalPinToInterrupt(encoderC1), encoderISR, RISING);

  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read potentiometer value (0 to 1023)
  int potValue = analogRead(potPin);

  // Map potentiometer value to PWM range (0 to 255)
  int speed = map(potValue, 0, 1023, 0, 255);

  // Determine motor direction
  if (potValue < 512) {  // Clockwise rotation
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
  } else {               // Counterclockwise rotation
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, HIGH);
  }

  // Set motor speed
  analogWrite(motorEnable, speed);

  // Optional: Print encoder count and speed to serial monitor
  Serial.print("Encoder Count: ");
  Serial.print(encoderCount);
  Serial.print(" | Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | Motor Speed: ");
  Serial.println(speed);

  delay(50); // Small delay for stability
}

// Interrupt Service Routine (ISR) for encoder C1
void encoderISR() {
  // Read the state of C2 to determine direction
  int c2State = digitalRead(encoderC2);
  
  // If C2 state differs from the last known state, count as forward
  if (c2State != lastC2State) {
    encoderCount++;
  } else { // Otherwise, count as reverse
    encoderCount--;
  }

  // Update lastC2State
  lastC2State = c2State;
}
