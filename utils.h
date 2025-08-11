#pragma once
// =====================
// 🔧 Billu Utility Functions
// =====================

// 🌈 Composite Color Mode (global flags)
extern bool compositeMode;
extern uint32_t compositeColor1;
extern uint32_t compositeColor2;

// ✅ Fill all active LEDs with a single or composite color
void fillAll(uint32_t col) {
  if (compositeMode) {
    // Alternate between the two composite colors
    for (uint16_t i = ledStart; i <= ledEnd && i < NUM_LEDS; i++) {
      if (i % 2 == 0) strip.setPixelColor(i, compositeColor1);
      else strip.setPixelColor(i, compositeColor2);
    }
    strip.show();
  } else {
    // Fill with a single color
    for (uint16_t i = ledStart; i <= ledEnd && i < NUM_LEDS; i++) {
      strip.setPixelColor(i, col);
    }
    strip.show();
  }
}

// ✅ Parse color names into RGB values
uint32_t parseColor(String n) {
  n.toLowerCase();

  // 🎨 Helper: logs the color being set
  auto logColor = [&](String name, uint8_t r, uint8_t g, uint8_t b) {
    Serial.print("🎨 Color set → ");
    Serial.print(name);
    Serial.print(" [");
    Serial.print(r); Serial.print(", ");
    Serial.print(g); Serial.print(", ");
    Serial.print(b); Serial.println("]");
    return strip.Color(r, g, b);
  };

  // ✅ Primary Colors
  if (n == "red") return logColor("red", 255, 0, 0);
  if (n == "green") return logColor("green", 0, 255, 0);
  if (n == "blue") return logColor("blue", 0, 0, 255);
  if (n == "yellow") return logColor("yellow", 255, 255, 0);
  if (n == "orange") return logColor("orange", 255, 85, 0);
  if (n == "pink") return logColor("pink", 255, 20, 147);
  if (n == "purple") return logColor("purple", 160, 0, 160);
  if (n == "violet") return logColor("violet", 180, 0, 255);
  if (n == "cyan") return logColor("cyan", 0, 255, 255);
  if (n == "magenta") return logColor("magenta", 255, 0, 255);

  // ✅ Whites & Grays
  if (n == "warm white") return logColor("warm white", 255, 210, 60);
  if (n == "cool white") return logColor("cool white", 199, 220, 255);
  if (n == "soft white") return logColor("soft white", 245, 245, 245);
  if (n == "dim white") return logColor("dim white", 180, 180, 180);
  if (n == "gray") return logColor("gray", 128, 128, 128);
  if (n == "dull gray") return logColor("dull gray", 90, 90, 90);

  // ✅ Special Shades
  if (n == "gold") return logColor("gold", 255, 215, 0);
  if (n == "amber") return logColor("amber", 255, 191, 0);
  if (n == "peach") return logColor("peach", 255, 229, 180);
  if (n == "coral") return logColor("coral", 255, 127, 80);
  if (n == "rose") return logColor("rose", 255, 100, 180);
  if (n == "lavender") return logColor("lavender", 230, 180, 255);
  if (n == "mint") return logColor("mint", 180, 255, 200);
  if (n == "moonlight") return logColor("moonlight", 200, 200, 255);
  if (n == "sunset") return logColor("sunset", 252, 94, 3);
  if (n == "ocean") return logColor("ocean", 0, 102, 255);
  if (n == "sky blue") return logColor("sky blue", 60, 160, 255);
  if (n == "dream blue") return logColor("dream blue", 100, 150, 255);
  if (n == "frost white") return logColor("frost white", 240, 255, 255);

  // ✅ Pastels & Pale Shades
  if (n == "pale blue") return logColor("pale blue", 175, 210, 255);
  if (n == "pale purple") return logColor("pale purple", 220, 190, 255);
  if (n == "pale pink") return logColor("pale pink", 255, 182, 193);
  if (n == "pale green") return logColor("pale green", 152, 251, 152);
  if (n == "soft pink") return logColor("soft pink", 255, 160, 180);
  if (n == "light blue") return logColor("light blue", 173, 216, 230);
  if (n == "light yellow") return logColor("light yellow", 255, 255, 153);

  // ✅ Dim / Deep Variants
  if (n == "dim red") return logColor("dim red", 120, 0, 0);
  if (n == "dim blue") return logColor("dim blue", 50, 80, 120);
  if (n == "deep red") return logColor("deep red", 200, 0, 0);
  if (n == "deep purple") return logColor("deep purple", 110, 0, 180);

  // 🚨 Unknown Color Handling
  Serial.println("❗ Unknown color: " + n);
  lcd.clear(); lcd.print("❌ Unknown color");
  return strip.Color(255, 255, 255);
}

// ✅ Activate composite mode with two colors
void setCompositeColor(String colorA, String colorB) {
  compositeMode = true;
  compositeColor1 = parseColor(colorA);
  compositeColor2 = parseColor(colorB);
  Serial.println("🎨 Composite Mode: " + colorA + " + " + colorB);
}
