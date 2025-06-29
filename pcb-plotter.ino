#define PIN_ENC_X_A 12
#define PIN_ENC_X_B 13

#define PIN_ENC_Y_A 14
#define PIN_ENC_Y_B 27

/*
volatile long encoderCountX = 0;
volatile int lastEncodedX = 0;
volatile long encoderCountY = 0;
volatile int lastEncodedY = 0;
*/

struct EncoderData {
  int pinA;
  int pinB;
  volatile int count;
  volatile int lastEncoded;
  void (*isr)();
};

void IRAM_ATTR updateEncoderXWrapper();
void IRAM_ATTR updateEncoderYWrapper();

EncoderData encoderX = {PIN_ENC_X_A, PIN_ENC_X_B, 0, 0, updateEncoderXWrapper};
EncoderData encoderY = {PIN_ENC_Y_A, PIN_ENC_Y_B, 0, 0, updateEncoderYWrapper};



//const int PIN_ENC_X_A = 12;
//const int PIN_ENC_X_B = 13;

/*
void setupPrinterEncoderX() {
  pinMode(PIN_ENC_X_A, INPUT);
  pinMode(PIN_ENC_X_B, INPUT);

  pinMode(PIN_ENC_X_A, INPUT_PULLUP);
  pinMode(PIN_ENC_X_B, INPUT_PULLUP);

  // Read initial state
  int stateA = digitalRead(PIN_ENC_X_A);
  int stateB = digitalRead(PIN_ENC_X_B);
  lastEncodedX = (stateA << 1) | stateB;

  // Attach interrupts to both encoder channels
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_X_A), updateEncoderX, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_X_B), updateEncoderX, CHANGE);
}

void setupPrinterEncoderY() {
  pinMode(PIN_ENC_Y_A, INPUT);
  pinMode(PIN_ENC_Y_B, INPUT);

  pinMode(PIN_ENC_Y_A, INPUT_PULLUP);
  pinMode(PIN_ENC_Y_B, INPUT_PULLUP);

  // Read initial state
  int stateA = digitalRead(PIN_ENC_Y_A);
  int stateB = digitalRead(PIN_ENC_Y_B);
  lastEncodedY = (stateA << 1) | stateB;

  // Attach interrupts to both encoder channels
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_Y_A), updateEncoderY, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_Y_B), updateEncoderY, CHANGE);
}
*/


void setup() {

  //setupPrinterEncoderX();
  //setupPrinterEncoderY();
  setupEncoder(&encoderY);
  setupEncoder(&encoderX);


  Serial.begin(115200);
}

  // Static variables to track previous state
  static long lastCountX = 0;
  static long lastCountY = 0;

void loop() {
  /*
  static long lastCount = 0;
  if (lastCount != encoderCountX) {
    Serial.print("Steps: ");
    Serial.print(encoderCountX);
    Serial.print(" | Direction: ");
    Serial.println((encoderCountX > lastCount) ? "CW" : "CCW");
    lastCount = encoderCountX;
  }
  */

  static long lastCountX = 0;
  static long lastCountY = 0;

  long currentX, currentY;

  // Safely copy shared variables
  noInterrupts();
  currentX = encoderX.count;
  currentY = encoderY.count;
  interrupts();

  if (lastCountX != currentX) {
    Serial.print("[X] Steps: ");
    Serial.print(currentX);
    Serial.print(" | Direction: ");
    Serial.println((currentX > lastCountX) ? "CW" : "CCW");
    lastCountX = currentX;
  }

  if (lastCountY != currentY) {
    Serial.print("[Y] Steps: ");
    Serial.print(currentY);
    Serial.print(" | Direction: ");
    Serial.println((currentY > lastCountY) ? "CW" : "CCW");
    lastCountY = currentY;
  }

  delay(50); // Adjust for display speed
}


void change1() {
  Serial.println("1");
}

void change2() {
  Serial.println("2");
}
/*
void updateEncoderX() {
  int MSB = digitalRead(PIN_ENC_X_A); // Most Significant Bit
  int LSB = digitalRead(PIN_ENC_X_B); // Least Significant Bit

  int encoded = (MSB << 1) | LSB;           // Combine A and B into 2-bit value
  int sum = (lastEncodedX << 2) | encoded;   // 4-bit state transition

  // Use a lookup table to determine direction
  switch (sum) {
    case 0b1101:
    case 0b0100:
    case 0b0010:
    case 0b1011:
      encoderCountX++;
      break;
    case 0b1110:
    case 0b0111:
    case 0b0001:
    case 0b1000:
      encoderCountX--;
      break;
  }

  lastEncodedX = encoded;
}

void updateEncoderY() {
  int MSB = digitalRead(PIN_ENC_Y_A); // Most Significant Bit
  int LSB = digitalRead(PIN_ENC_Y_B); // Least Significant Bit

  int encoded = (MSB << 1) | LSB;           // Combine A and B into 2-bit value
  int sum = (lastEncodedX << 2) | encoded;   // 4-bit state transition

  // Use a lookup table to determine direction
  switch (sum) {
    case 0b1101:
    case 0b0100:
    case 0b0010:
    case 0b1011:
      encoderCountY++;
      break;
    case 0b1110:
    case 0b0111:
    case 0b0001:
    case 0b1000:
      encoderCountY--;
      break;
  }

  lastEncodedY = encoded;
}
*/



void updateEncoder(EncoderData* enc) {
  int MSB = digitalRead(enc->pinA);
  int LSB = digitalRead(enc->pinB);

  int encoded = (MSB << 1) | LSB;
  int sum = (enc->lastEncoded << 2) | encoded;

  switch (sum) {
    case 0b1101:
    case 0b0100:
    case 0b0010:
    case 0b1011:
      enc->count++;
      break;
    case 0b1110:
    case 0b0111:
    case 0b0001:
    case 0b1000:
      enc->count--;
      break;
  }

  enc->lastEncoded = encoded;
}

void IRAM_ATTR updateEncoderXWrapper() {
  updateEncoder(&encoderX);
}

void IRAM_ATTR updateEncoderYWrapper() {
  updateEncoder(&encoderY);
}

void setupEncoder(EncoderData* enc) {
  pinMode(enc->pinA, INPUT_PULLUP);
  pinMode(enc->pinB, INPUT_PULLUP);

  // Read initial state
  int stateA = digitalRead(enc->pinA);
  int stateB = digitalRead(enc->pinB);
  enc->lastEncoded = (stateA << 1) | stateB;

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(enc->pinA), enc->isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enc->pinB), enc->isr, CHANGE);
}
