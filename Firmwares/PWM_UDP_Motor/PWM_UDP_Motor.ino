/*
ESPArduinoPWMCar
https://github.com/techn0man1ac/ESPArduinoPWMCar

by Techn0man1ac Labs, 2021
*/

// драйвер моторов TB6612FNG
#define PWMA 6  
#define PWMB 11 
#define AIN1 8
#define AIN2 7
#define BIN1 9
#define BIN2 10
#define STBY 12

String inString = "";    // string to hold input
int RightMotor = 0; // Тут скорость правого двигателя
int LeftMotor = 0;

void setup() {
  Serial.begin(115200); // Скорость обмена между ESP8266 Arduino Nano должна быть одинаковая
  Serial.setTimeout(5);  // 5 мс должно хватить на один символ

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH); // Отключаю режим сна драйвера

  for (int spd = 0; spd <= 255; spd++) { // плавный тестовый прогон двигателей
    MtrsSpdSet(spd, spd); // чтобы не  было бросков напряжений и перезагрузок ардуины
    delay(5);
  }

  delay(1000);

  for (int spd = 255; spd >= 0; spd--) { // плавное убавление оборотов
    MtrsSpdSet(spd, spd);
    delay(10);
  }

  Serial.println(" Start "); // Мы готовы принимать комманды
}

void loop()
{
  if (Serial.available() > 0) { //Если в буфере UART'а есть что-то
    inString = Serial.readString(); // Считываем содержимое в строку

    if (inString.substring(0, 4) == "mr0 ") { // Если полученая комманда начинается с "mr0 ", начинаем смотреть с нулевой позиции в строке, комманда занимает 4 символа
      inString = inString.substring(4); // Если комманда получена, тогда смещаемся на 4 символа, и преображаем всё что после 4-го символа("mr0 ") в число типа Int(как отрицательное так и положительное)
      RightMotor = inString.toInt(); 
      //Serial.print("RightMotor = "); // Для отладки
      //Serial.println(RightMotor); // Уже нет необходимости
    }

    if (inString.substring(0, 4) == "ml0 ") { // тоже что и выше 
      inString = inString.substring(4);
      LeftMotor = inString.toInt();
      //Serial.print("LeftMotor = ");
      //Serial.println(LeftMotor);
    }
    inString = ""; // Очистка буфера, ведь мы уже обработали комманды
  }
  MtrsSpdSet(RightMotor, LeftMotor); // Управляем двигателями, RightMotor, как и LeftMotor могут принимать значения -255..255, соответственно -255 это макс скорость в одну сторону, 255 - в другую
}

void MtrsSpdSet(int rMotor, int lMotor) {
  if (rMotor > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (rMotor == 0) { // когада задаем нуль тогда используем режим " Stop", то есть допускается инерция, а не жёсткий тормоз
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

  analogWrite(PWMA, rMotor); // тут задается скорость двигателей
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
