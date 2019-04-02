#include "PMS.h"
#include "Wire.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

//HardwareSerial Serial1(1);
PMS pms(Serial1);
PMS::DATA data;
byte i2cAddress_Dot = 0x70;
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();
#define RXD2 23
#define TXD2 19
#define LEDR  2
#define LEDY  15
#define LEDG  12

void setup()
{
  Wire.begin();
  pinMode(LEDR, OUTPUT);
  pinMode(LEDY, OUTPUT);
  pinMode(LEDG, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop()
{
  if (pms.read(data))
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    Display_Light(data.PM_AE_UG_2_5);
    Display_dot(data.PM_AE_UG_2_5);
  }
}
void Display_Light(int temp_CC) {

  if (temp_CC <= 50) {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDY, HIGH);
    digitalWrite(LEDG, LOW);
  }
  else if (temp_CC > 50 && temp_CC <= 100) {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDY, LOW);
    digitalWrite(LEDG, HIGH);
  }
  else if (temp_CC > 100 ) {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDY, HIGH);
    digitalWrite(LEDG, HIGH);
  }
  else {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDY, HIGH);
    digitalWrite(LEDG, HIGH);
  }
}


void Display_dot(int CC)
{
  int text = (int)CC;
  matrix.begin(i2cAddress_Dot);  // pass in the address
  matrix.setTextSize(1);
  matrix.setTextWrap(true);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  if (text < 100)
  {
    matrix.setRotation(1);
    matrix.clear();
    matrix.setCursor(3, 0);
    matrix.print(text);
    matrix.writeDisplay();
  }
  else
  {
    for (int8_t x = 40; x >= -32; x--)
    {
      matrix.setRotation(1);
      matrix.clear();
      matrix.setCursor(x, 0);
      matrix.print(text);
      matrix.writeDisplay();
      delay(50);
    }
  }
}
