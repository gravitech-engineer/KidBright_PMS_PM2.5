#include "PMS.h"
#include <KBChain_LED7SEG.h>
#include "Wire.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
PMS pms(Serial1);
PMS::DATA data;
byte i2cAddress_Dot = 0x70;
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();
static const uint8_t PROGMEM
TC1_1[] =
{ B00000110,
  B00000000,
  B00000000,
  B00110010,
  B01001010,
  B01000110,
  B00100010,
  B00000000
},
TC1_2[] =
{ B00000000,
  B01001100,
  B01010010,
  B01010010,
  B01110010,
  B00000000,
  B00000000,
  B00000110
};
KBChain_7segment Display1 = KBChain_7segment();
TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);
#define RXD2 23
#define TXD2 19
#define LEDR  2
#define LEDY  15
#define LEDG  12
void setup()
{
  pinMode(LEDR, OUTPUT);
  pinMode(LEDY, OUTPUT);
  pinMode(LEDG, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}
void loop()
{

  //  Serial.println("Loop");
  if (pms.read(data))
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    Display(data.PM_AE_UG_2_5);
    Display_Light(data.PM_AE_UG_2_5);
    Display_dot();
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
void Display_dot()
{
  I2Ctwo.begin(21, 22);
  matrix.begin(i2cAddress_Dot);  // pass in the address
  String text = "2.5";
  matrix.setTextSize(1);
  matrix.setTextWrap(true);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  matrix.setRotation(0);

  matrix.clear();
  matrix.drawBitmap(0, 8, TC1_1, 8, 8, LED_ON);
  matrix.writeDisplay();
  matrix.drawBitmap(0, 0, TC1_2, 8, 8, LED_ON);
  matrix.writeDisplay();
  I2Ctwo.endTransmission();
}
void Display(int CC) {
  I2Cone.begin(4, 5);
  Display1.begin(0x71);
  Display1.setBrightness(15);
  Display1.clear();
  Display1.print(CC, DEC);
  Display1.drawColon(false);
  Display1.writeDisplay();
  I2Cone.endTransmission();
}
