#define PWMA 6  // выходы arduino
#define PWMB 11
#define AIN1 8
#define AIN2 7
#define BIN1 9
#define BIN2 10
#define STBY 12

String inString = "";    // string to hold input
int RightMotor = 0;
int LeftMotor = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  for (int spd = 0; spd <= 255; spd++) { // плавный розгон
    MtrsSpdSet(spd, spd); // чтобы не  было бросков напряжений и перезагрузок ардуины
    delay(5);
  }

  delay(1000);

  for (int spd = 255; spd >= 0; spd--) { // плавное убавление оборотов
    MtrsSpdSet(spd, spd);
    delay(10);
  }

  Serial.println(" Start ");
}

void loop()
{
  if (Serial.available() > 0) {
    inString = Serial.readString();

    if (inString.substring(0, 4) == "mr0 ") {
      inString = inString.substring(4);
      RightMotor = inString.toInt();
      //Serial.print("RightMotor = ");
      //Serial.println(RightMotor);
    }

    if (inString.substring(0, 4) == "ml0 ") {
      inString = inString.substring(4);
      LeftMotor = inString.toInt();
      //Serial.print("LeftMotor = ");
      //Serial.println(LeftMotor);
    }
    inString = "";
  }
  MtrsSpdSet(RightMotor, LeftMotor);
}

void MtrsSpdSet(int rMotor, int lMotor) {
  if (rMotor > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (rMotor == 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  } else if (rMotor < 0) {
    rMotor = abs(rMotor);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  }

  if (lMotor > 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else if (lMotor == 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  } else if (lMotor < 0) {
    lMotor = abs(lMotor);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }

  analogWrite(PWMA, rMotor);
  analogWrite(PWMB, lMotor);

  // AI1 -> HIGH, AI2 -> HIGH - Short Brake
  // AI1 -> LOW, AI2 -> LOW - Stop
  // AI1 -> HIGH, AI2 -> LOW - Clockwise
  // AI1 -> LOW, AI2 -> HIGH - Counterclockwise
  //
  // BI1 -> HIGH, BI2 -> HIGH - Short Brake
  // BI1 -> LOW, BI2 -> LOW - Stop
  // BI1 -> HIGH, BI2 -> LOW - Clockwise
  // BI1 -> LOW, BI2 -> HIGH - Counterclockwise
}
