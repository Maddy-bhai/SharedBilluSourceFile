// =====================
// Billu SerialCommand 2.0v (CLEAN STRUCTURE)
// =====================

#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ----------------------
// 🟢 GLOBAL DEFINITIONS
// ----------------------
#define LED_PIN    5
#define NUM_LEDS   300
#define LIGHT_RELAY 26
#define FAN_RELAY 27
#define MAX_QUEUE 10

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ----------------------
// 🟢 GLOBAL VARIABLES
// ----------------------
uint16_t activeLEDCount = NUM_LEDS;
uint16_t ledStart = 0;
uint16_t ledEnd = NUM_LEDS - 1;
uint32_t currentColor = strip.Color(255, 255, 255);

const uint8_t DEFAULT_BRIGHTNESS_PCT = 30;
uint8_t brightnessPct = DEFAULT_BRIGHTNESS_PCT;
uint8_t brightness;

bool ledState = false;
String currentPattern = "none";

// 🌈 Composite Color Mode
bool compositeMode = false;
uint32_t compositeColor1 = 0;
uint32_t compositeColor2 = 0;

// Effect System
enum EffectType {
  NONE, WAVE, BLINK, CHASE, STROBE, PULSE, CENTER_WAVE, BOUNCE_WAVE, TWINKLE,
  PARTY_FLASH, FIRE_GLOW, THUNDER, FADE_LOOP, COLOR_COMET, SOFT_GLOW,
  HEARTBEAT, STAR_RAIN, FIREWORKS, DRIZZLE, RAINBOW, FLASH, RAIN
};

EffectType lastEffect = NONE;
String lastBasePattern = "";

String rainMode = "medium";
uint8_t rainIntensity = 3;
EffectType currentEffect = NONE;

unsigned long lastMillis = 0;
uint16_t effectSpeed = 100;
bool customSpeed = false;

// Effect-specific states
uint16_t waveIndex = 0, blinkCount = 0, blinkCounter = 0, chaseIndex = 0, chaseRep = 0;
bool blinkOn = false, strobeOn = false;
uint16_t strobeCount = 0, centerIndex = 0;
uint8_t pulseBrightness = 0;
bool pulseUp = true, bounceForward = true;
bool thunderState = true;
uint16_t fadeHue = 0;
uint16_t rainbowHue = 0;

// ----------------------
// 📂 INCLUDE MODULES
// ----------------------
#include "utils.h"
#include "colors.h"
#include "patterns.h"
#include "effects.h"
#include "moods.h"
#include "commands.h"

// ----------------------
// 🚀 SETUP + LOOP
// ----------------------
void setup() {
  Serial.begin(115200);
  brightness = map(brightnessPct, 0, 100, 0, 255);
  strip.begin();
  strip.setBrightness(brightness);
  strip.show();

  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  digitalWrite(LIGHT_RELAY, HIGH);  // Default ON
  digitalWrite(FAN_RELAY, HIGH);    // Default ON  

  lcd.init();
  lcd.backlight();
  lcd.print("Billu Ready!");
}

void loop() {
  handleSerialCommands();   // ✅ Collect new commands
  updateActivePattern(); 
  
  // ✅ Process all queued commands at once
  if (queueStart != queueEnd && !processingCommands) {
    processingCommands = true;

    while (queueStart != queueEnd) {
      String cmd = commandQueue[queueStart];
      processCommand(cmd);      
      queueStart = (queueStart + 1) % MAX_QUEUE;
    }

if (!scrollMode) strip.show();
              
    processingCommands = false;
  }

if (scrollMode && currentEffect != NONE) {
  currentEffect = NONE;  // 💀 force kill any effect trying to run
}

if (currentEffect != NONE && !scrollMode) {
  runCurrentEffect();
}

}
