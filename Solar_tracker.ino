#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//deklarasi Pin
#define PIN_SERVO_BASE    9
#define PIN_SERVO_TAIL    10
#define PIN_POTENSIO      A2
#define PIN_LDR_LEFT      A0
#define PIN_LDR_RIGHT     A1
#define PIN_BTN_INC       7
#define PIN_BTN_DEC       6
#define PIN_BTN_MODE      5    

//definisi variabel limit servo
#define BASE_MIN          10
#define BASE_MAX          170
#define TAIL_MIN          10
#define TAIL_MAX          170
#define TAIL_STEP         3      
#define SERVO_DELAY       10     

//threshold LDR
#define LDR_THRESHOLD     30    
#define LDR_DEADZONE      15     

// Ubah ke false untuk mode AUTO (LDR)
// Ubah ke true  untuk mode MANUAL (Potensio + Button)
bool manualMode = true;

//button debounce
#define DEBOUNCE_MS       50
unsigned long lastBtnInc  = 0;
unsigned long lastBtnDec  = 0;
unsigned long lastBtnMode = 0; 

Servo servoBase;
Servo servoTail;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

//variabel 
int baseAngle  = 90;
int tailAngle  = 90;
int lastBaseAngle = -1;
int lastTailAngle = -1;
bool lastModeManual = !manualMode; // paksa update pertama kali saat dinyalakan

unsigned long lastLCDUpdate = 0;
#define LCD_REFRESH_MS  300

//lcd karakter 
byte sunChar[8] = {
  0b00100,
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101,
  0b00100,
  0b00000
};

byte arrowRight[8] = {
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b00110,
  0b00100,
  0b00000,
  0b00000
};


void setup() {
  Serial.begin(9600);

  servoBase.attach(PIN_SERVO_BASE);
  servoTail.attach(PIN_SERVO_TAIL);
  servoBase.write(baseAngle);
  servoTail.write(tailAngle);
  
  pinMode(PIN_BTN_INC,  INPUT_PULLUP);
  pinMode(PIN_BTN_DEC,  INPUT_PULLUP);
  pinMode(PIN_BTN_MODE, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, sunChar);
  lcd.createChar(1, arrowRight);

  lcd.setCursor(0, 0);
  lcd.print("  SOLAR TRACKER ");
  lcd.setCursor(0, 1);
  lcd.print("   Initializing..");
  delay(1500);
  lcd.clear();

  Serial.println("Solar Tracker Ready");
  Serial.println("Mode: " + String(manualMode ? "MANUAL" : "AUTO"));
}


void loop() {
  unsigned long now = millis();
  //triger mode vias button
  if (digitalRead(PIN_BTN_MODE) == LOW && (now - lastBtnMode) > DEBOUNCE_MS) {
    lastBtnMode = now;
    manualMode  = !manualMode;
    lcd.clear();  
    Serial.println(">> Mode: " + String(manualMode ? "MANUAL" : "AUTO"));
  }

  //triger mode via serial
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'A' || cmd == 'a') { manualMode = false; lcd.clear(); Serial.println(">> Mode: AUTO"); }
    if (cmd == 'M' || cmd == 'm') { manualMode = true;  lcd.clear(); Serial.println(">> Mode: MANUAL"); }
  }

  if (manualMode) {
    runManualMode();
  } else {
    runAutoMode();
  }

  updateLCD();
  delay(20);
}

//fungsi control manual
void runManualMode() {
  int potVal  = analogRead(PIN_POTENSIO);         
  int target  = map(potVal, 0, 1023, BASE_MIN, BASE_MAX);
  baseAngle   = smoothMove(baseAngle, target, 2);  
  servoBase.write(baseAngle);

  unsigned long now = millis();

  if (digitalRead(PIN_BTN_INC) == LOW && (now - lastBtnInc) > DEBOUNCE_MS) {
    lastBtnInc = now;
    tailAngle  = constrain(tailAngle + TAIL_STEP, TAIL_MIN, TAIL_MAX);
    servoTail.write(tailAngle);
    Serial.println("Tail+ : " + String(tailAngle) + "°");
  }

  if (digitalRead(PIN_BTN_DEC) == LOW && (now - lastBtnDec) > DEBOUNCE_MS) {
    lastBtnDec = now;
    tailAngle  = constrain(tailAngle - TAIL_STEP, TAIL_MIN, TAIL_MAX);
    servoTail.write(tailAngle);
    Serial.println("Tail- : " + String(tailAngle) + "°");
  }
}

// fungsi auto
void runAutoMode() {
  int ldrLeft  = analogRead(PIN_LDR_LEFT);
  int ldrRight = analogRead(PIN_LDR_RIGHT);
  int diff     = ldrLeft - ldrRight;

  if (abs(diff) > LDR_THRESHOLD) {
    if (diff > 0) {
      // Cahaya lebih kuat di kiri maka putar ke kiri (kurangi angle)
      baseAngle = constrain(baseAngle - 1, BASE_MIN, BASE_MAX);
    } else {
      // Cahaya lebih kuat di kanan maka putar ke kanan
      baseAngle = constrain(baseAngle + 1, BASE_MIN, BASE_MAX);
    }
    servoBase.write(baseAngle);
  }

  // Tail Servo: gunakan rata-rata LDR sebagai referensi elevasi
  // Semakin terang maka naikkan panel (tailAngle turun menuju tengah)
  int avgLight = (ldrLeft + ldrRight) / 2;
  int targetTail = map(avgLight, 0, 1023, TAIL_MAX, TAIL_MIN);
  targetTail = constrain(targetTail, TAIL_MIN, TAIL_MAX);

  if (abs(tailAngle - targetTail) > LDR_DEADZONE) {
    tailAngle = smoothMove(tailAngle, targetTail, 1);
    servoTail.write(tailAngle);
  }

  // Debug Serial
  Serial.print("LDR L:");  Serial.print(ldrLeft);
  Serial.print(" R:");     Serial.print(ldrRight);
  Serial.print(" | Base:"); Serial.print(baseAngle);
  Serial.print("° Tail:");  Serial.println(tailAngle);
}

// fungsi tampil LCD
void updateLCD() {
  unsigned long now = millis();
  if (now - lastLCDUpdate < LCD_REFRESH_MS) return;
  lastLCDUpdate = now;

  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // sun icon
  lcd.print(manualMode ? " MANUAL " : " AUTO   ");
  lcd.write(byte(1));  // arrow icon
  lcd.print(manualMode ? "M" : "A");  

  // Baris 1: Sudut Base & Tail
  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(padAngle(baseAngle));
  lcd.print((char)223);   // derajat °
  lcd.print(" T:");
  lcd.print(padAngle(tailAngle));
  lcd.print((char)223);
  lcd.print("     ");
}

//fungsi - fungsi pembantu
// Gerak servo bertahap (smooth)
int smoothMove(int current, int target, int step) {
  if (current < target) return min(current + step, target);
  if (current > target) return max(current - step, target);
  return current;
}

// Format angle 3 digit
String padAngle(int angle) {
  if (angle < 10)  return "  " + String(angle);
  if (angle < 100) return " " + String(angle);
  return String(angle);
}
