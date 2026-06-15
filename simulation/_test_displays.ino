/*
 * DISPLAY PROBE — paste this into Wokwi sketch.ino temporarily.
 * Goal: fill each of the 4 panels a solid colour + print to Serial.
 *   - If panels turn RED / GREEN / BLUE / WHITE  -> SPI+init+CS all good,
 *     the bug is in text drawing.
 *   - If panels stay BLACK -> init/wiring/library problem (look at Serial).
 * Same diagram.json as the real project.
 */
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_SCLK 18
#define TFT_MOSI 23
#define TFT_DC   21
#define TFT_RST  22
#define CS1 5
#define CS2 13
#define CS3 14
#define CS4 4

Adafruit_ILI9341 t1 = Adafruit_ILI9341(CS1, TFT_DC, -1);
Adafruit_ILI9341 t2 = Adafruit_ILI9341(CS2, TFT_DC, -1);
Adafruit_ILI9341 t3 = Adafruit_ILI9341(CS3, TFT_DC, -1);
Adafruit_ILI9341 t4 = Adafruit_ILI9341(CS4, TFT_DC, -1);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("PROBE start");

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);

  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH); delay(10);
  digitalWrite(TFT_RST, LOW);  delay(20);
  digitalWrite(TFT_RST, HIGH); delay(150);

  Serial.println("begin t1"); t1.begin(); t1.setRotation(1);
  Serial.println("begin t2"); t2.begin(); t2.setRotation(1);
  Serial.println("begin t3"); t3.begin(); t3.setRotation(1);
  Serial.println("begin t4"); t4.begin(); t4.setRotation(1);

  t1.fillScreen(ILI9341_RED);
  t2.fillScreen(ILI9341_GREEN);
  t3.fillScreen(ILI9341_BLUE);
  t4.fillScreen(ILI9341_WHITE);
  Serial.println("filled. PROBE done");
}

void loop() {}
