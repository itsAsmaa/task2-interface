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

// Time tracking for velocity calculation
unsigned long lastTime = 0;
int lastEncoderCount = 0;

// Encoder specifications
const int pulsesPerRevolution = 400; // Adjust according to your encoder specs
const float wheelCircumference = 0.1; // Example: 0.1 meters

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
  String direction;
  if (potValue < 512) {  // Clockwise rotation
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
    direction = "Clockwise";
  } else {               // Counterclockwise rotation
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, HIGH);
    direction = "Counterclockwise";
  }

  // Set motor speed
  analogWrite(motorEnable, speed);

  // Calculate velocity and position
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 100) { // Update every 100 ms
    int encoderDelta = encoderCount - lastEncoderCount;
    float velocity = (encoderDelta / (float)pulsesPerRevolution) * wheelCircumference / ((currentTime - lastTime) / 1000.0);
    float position = (encoderCount / (float)pulsesPerRevolution) * wheelCircumference;

    // Print results to serial monitor
    Serial.print("Direction: ");
    Serial.print(direction);
    Serial.print(" | Velocity: ");
    Serial.print(velocity, 2);
    Serial.print(" m/s | Position: ");
    Serial.print(position, 2);
    Serial.println(" m");

    // Update last values
    lastEncoderCount = encoderCount;
    lastTime = currentTime;
  }

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
