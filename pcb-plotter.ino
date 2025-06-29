volatile long encoderCount = 0;
volatile int lastEncoded = 0;

#define PIN_A_PRINTER 12
#define PIN_B_PRINTER 13
//#define PIN_A_PRINTER 14
//#define PIN_B_PRINTER 27

#define PIN_A_ROT 14
#define PIN_B_ROT 27

//const int PIN_A_PRINTER = 12;
//const int PIN_B_PRINTER = 13;

void setup() {
  pinMode(PIN_A_PRINTER, INPUT);
  pinMode(PIN_B_PRINTER, INPUT);

  // Optionally enable internal pull-ups
  pinMode(PIN_A_PRINTER, INPUT_PULLUP);
  pinMode(PIN_B_PRINTER, INPUT_PULLUP);

  // Read initial state
  int stateA = digitalRead(PIN_A_PRINTER);
  int stateB = digitalRead(PIN_B_PRINTER);
  lastEncoded = (stateA << 1) | stateB;

  // Attach interrupts to both encoder channels
  attachInterrupt(digitalPinToInterrupt(PIN_A_PRINTER), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_B_PRINTER), updateEncoder, CHANGE);

  Serial.begin(115200);
}

void loop() {
  static long lastCount = 0;
  if (lastCount != encoderCount) {
    Serial.print("Steps: ");
    Serial.print(encoderCount);
    Serial.print(" | Direction: ");
    Serial.println((encoderCount > lastCount) ? "CW" : "CCW");
    lastCount = encoderCount;
  }
  delay(50); // Adjust for display speed
}


void change1() {
  Serial.println("1");
}

void change2() {
  Serial.println("2");
}

void updateEncoder() {
  int MSB = digitalRead(PIN_A_PRINTER); // Most Significant Bit
  int LSB = digitalRead(PIN_B_PRINTER); // Least Significant Bit

  int encoded = (MSB << 1) | LSB;           // Combine A and B into 2-bit value
  int sum = (lastEncoded << 2) | encoded;   // 4-bit state transition

  // Use a lookup table to determine direction
  switch (sum) {
    case 0b1101:
    case 0b0100:
    case 0b0010:
    case 0b1011:
      encoderCount++;
      break;
    case 0b1110:
    case 0b0111:
    case 0b0001:
    case 0b1000:
      encoderCount--;
      break;
  }

  lastEncoded = encoded;
}
