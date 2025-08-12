#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

extern Adafruit_SH1106G display;

struct LCD_Compat {
  bool dirty = false;

  void init() {}
  void backlight() {}

  void clear() {
    display.clearDisplay();
    dirty = true;
  }
  void setCursor(int col, int row) {
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(col * 6, row * 8); // 16x2-ish mapping
  }
  void print(const String& s) { display.print(s); dirty = true; }
  void print(const char* s)   { display.print(s); dirty = true; }
  void print(int v)           { display.print(v); dirty = true; }

  // Call this once per loop to actually push pixels
  void flush() {
    if (dirty) { display.display(); dirty = false; }
  }
};

static LCD_Compat lcd;
