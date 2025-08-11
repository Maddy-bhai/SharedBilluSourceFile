#pragma once
#include <pgmspace.h>
#include "shared_state.h"


// ======================
// 🌈 MULTICOLOR STORAGE (for patterns & effects)
// ======================
uint8_t multiColors[10][3];   // actual allocation
int multiColorCount = 0;


void renderMultiColorsBlock();
void handleRGB(String cmd);
void addColorToMulti(String name);
void removeColorFromMulti(String name);
void refreshCurrentPattern();

// from patterns.h
void stopScrollMode();

// ======================
// 🎨 COLOR ENGINE MODULE
// ======================

// 🎨 Basic Color List (Core colors only)
struct ColorEntry {
    const char* name;
    uint8_t r, g, b;
};

const ColorEntry baseColors[] PROGMEM = {
    {"red", 255, 0, 0},
    {"green", 0, 255, 0},
    {"blue", 0, 0, 255},
    {"yellow", 255, 255, 0},
    {"orange", 255, 85, 0},
    {"pink", 255, 20, 147},
    {"purple", 160, 0, 160},
    {"cyan", 0, 255, 255},
    {"white", 255, 255, 255},
    {"warm white", 255, 210, 60},
    {"cool white", 199, 220, 255},
    {"gold", 255, 215, 0},
    {"lavender", 230, 180, 255},
    {"mint", 180, 255, 200},
    {"sunset", 252, 94, 3},
    {"ocean", 0, 102, 255},
    {"sky blue", 60, 160, 255},
    {"deep red", 200, 0, 0},
    {"deep purple", 110, 0, 180}
};
const int BASE_COLOR_COUNT = sizeof(baseColors) / sizeof(baseColors[0]);


// ======================
// 🔍 LOOKUP FUNCTION
// ======================
bool lookupColor(String name, uint8_t &r, uint8_t &g, uint8_t &b) {
    name.toLowerCase();
    for (int i = 0; i < BASE_COLOR_COUNT; i++) {
        const char* storedName = (const char*)pgm_read_ptr(&(baseColors[i].name));
        if (name.equals(storedName)) {
            r = pgm_read_byte(&(baseColors[i].r));
            g = pgm_read_byte(&(baseColors[i].g));
            b = pgm_read_byte(&(baseColors[i].b));
            return true;
        }
    }
    return false;
}

// ======================
// 🎯 COLOR COMMAND HANDLERS
// ======================


// ✅ CMD:RGB=R,G,B → set one solid color
void handleRGB(String cmd) {
    stopScrollMode();
    String value = cmd.substring(8); // after CMD:RGB=
    int c1 = value.indexOf(',');
    int c2 = value.indexOf(',', c1 + 1);
    int r = value.substring(0, c1).toInt();
    int g = value.substring(c1 + 1, c2).toInt();
    int b = value.substring(c2 + 1).toInt();

    currentColor = strip.Color(r, g, b);
    compositeMode = false;
    multiColorCount = 0;

    if (ledState) fillAll(currentColor);

    Serial.print("🎨 RGB Set: ");
    Serial.print(r); Serial.print(", ");
    Serial.print(g); Serial.print(", ");
    Serial.println(b);
}


void handleRGBN(String cmd) {
    stopScrollMode();
    String rgbValues = cmd.substring(9);

    // reset
    multiColorCount = 0;

    int start = 0;
    while (start < rgbValues.length() && multiColorCount < 10) {
        int comma1 = rgbValues.indexOf(',', start);
        int comma2 = rgbValues.indexOf(',', comma1 + 1);
        int semi = rgbValues.indexOf(';', comma2 + 1);
        if (semi == -1) semi = rgbValues.length();

        multiColors[multiColorCount][0] = rgbValues.substring(start, comma1).toInt();
        multiColors[multiColorCount][1] = rgbValues.substring(comma1 + 1, comma2).toInt();
        multiColors[multiColorCount][2] = rgbValues.substring(comma2 + 1, semi).toInt();

        multiColorCount++;
        start = semi + 1;
    }

    // draw
    renderMultiColorsBlock();
    
    Serial.print("🎨 RGBN applied with ");
    Serial.print(multiColorCount);
    Serial.println(" colors");
}



// ✅ CMD:COLOR=name → look up name, convert to RGB
void handleCOLOR(String cmd) {
    stopScrollMode();
    String colorName = cmd.substring(10);
    uint8_t r, g, b;
    if (lookupColor(colorName, r, g, b)) {
        handleRGB("CMD:RGB=" + String(r) + "," + String(g) + "," + String(b));
        Serial.print("🎨 Named color set: "); 
        Serial.println(colorName);
    } else {
        Serial.print("❌ Unknown color name: "); 
        Serial.println(colorName);
    }
}

// ✅ CMD:COLORN=name1,name2,name3 → multiple named colors
void handleCOLORN(String cmd) {
  stopScrollMode();
  String allNames = cmd.substring(11);
  allNames.trim();
  Serial.println("🧪 handleCOLORN received: " + allNames);

  multiColorCount = 0;
  String name = "";
  int start = 0;

  while (start < allNames.length()) {
    int comma = allNames.indexOf(',', start);
    if (comma == -1) comma = allNames.length();

    name = allNames.substring(start, comma);
    name.trim();  // ✅ Extra safety
    Serial.println("🔍 Parsed color: " + name);

    uint8_t r, g, b;
    if (lookupColor(name, r, g, b)) {
      multiColors[multiColorCount][0] = r;
      multiColors[multiColorCount][1] = g;
      multiColors[multiColorCount][2] = b;
      multiColorCount++;
    } else {
      Serial.print("❌ Unknown color in COLORN: ");
      Serial.println(name);
    }

    start = comma + 1;
  }

  refreshCurrentPattern();
  Serial.print("🎨 RGBN applied with ");
  Serial.print(multiColorCount);
  Serial.println(" colors");
}


void addColorToMulti(String name) {
    bool wasScrolling = scrollMode;
    stopScrollMode();
    name.trim();
    uint8_t r, g, b;

    if (!lookupColor(name, r, g, b)) {
        Serial.print("❌ Unknown color: "); Serial.println(name);
        return;
    }

    if (multiColorCount >= 10) {
        Serial.println("⚠️ Max color limit reached (10)");
        return;
    }

    // Avoid duplicates
    for (int i = 0; i < multiColorCount; i++) {
        if (multiColors[i][0] == r && multiColors[i][1] == g && multiColors[i][2] == b) {
            Serial.print("⚠️ Color already exists: "); Serial.println(name);
            return;
        }
    }

    // Add color
    multiColors[multiColorCount][0] = r;
    multiColors[multiColorCount][1] = g;
    multiColors[multiColorCount][2] = b;
    multiColorCount++;

    Serial.print("✅ Color added: "); Serial.println(name);

    // If we were scrolling, turn it back on BEFORE refresh so it re-captures
    if (wasScrolling) { scrollMode = true; }

    refreshCurrentPattern();  // will re-capture base if scrollMode == true

    if (wasScrolling) {
        Serial.println("🔁 Scroll resumed after color change");
    }
}


void removeColorFromMulti(String name) {
    bool wasScrolling = scrollMode;
    stopScrollMode();
    name.trim();
    uint8_t r, g, b;

    if (!lookupColor(name, r, g, b)) {
        Serial.print("❌ Unknown color: "); Serial.println(name);
        return;
    }

    bool found = false;
    for (int i = 0; i < multiColorCount; i++) {
        if (multiColors[i][0] == r && multiColors[i][1] == g && multiColors[i][2] == b) {
            found = true;
            for (int j = i; j < multiColorCount - 1; j++) {
                multiColors[j][0] = multiColors[j + 1][0];
                multiColors[j][1] = multiColors[j + 1][1];
                multiColors[j][2] = multiColors[j + 1][2];
            }
            multiColorCount--;
            break;
        }
    }

    if (!found) {
        Serial.print("⚠️ Color not found in current list: "); Serial.println(name);
        return;
    }

    Serial.print("✅ Color removed: "); Serial.println(name);

    if (wasScrolling) { scrollMode = true; }

    refreshCurrentPattern();  // will re-capture base if scrollMode == true

    if (wasScrolling) {
        Serial.println("🔁 Scroll resumed after color change");
    }
}


void refreshCurrentPattern();  // declaration only


void renderMultiColorsBlock() {
    if (multiColorCount <= 0) return;

    int totalLEDs = ledEnd - ledStart + 1;
    int ledsPerColor = (multiColorCount > 0) ? totalLEDs / multiColorCount : totalLEDs;
    int extraLEDs = (multiColorCount > 0) ? totalLEDs % multiColorCount : 0;

    int ledIndex = ledStart;
    for (int c = 0; c < multiColorCount; c++) {
        int count = ledsPerColor + (c < extraLEDs ? 1 : 0);
        for (int i = 0; i < count && ledIndex <= ledEnd; i++) {
            strip.setPixelColor(
              ledIndex,
              strip.Color(multiColors[c][0], multiColors[c][1], multiColors[c][2])
            );
            ledIndex++;
        }
    }
    strip.show();
}
