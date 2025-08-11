#pragma once
#include "shared_state.h"

// ======================
// 🌈 PATTERN ENGINE MODULE
// ======================
//
// Supports:
//   - stripe / split / gradient (static base patterns)
//   - scroll animation (wrap-around shift of the static frame)
//   - CMD:PATTERN=stop to halt animation
//

// 🎛 PATTERN STATE
String basePattern = "";            // current static pattern (stripe/split/gradient)
bool scrollMode = false;
unsigned long lastPatternUpdate = 0;
int patternSpeed = 100;             // ms between shifts

// --- Scroll engine state ---
uint32_t scrollBase[600];           // raise if you use >600 LEDs
int scrollBaseLen = 0;
bool scrollBaseCaptured = false;
int scrollOffset = 0;

inline void stopScrollMode() {
    scrollMode = false;
    scrollBaseCaptured = false;
    scrollOffset = 0;
}

// Capture the current static frame into scrollBase
void captureScrollBase() {
    int totalLEDs = ledEnd - ledStart + 1;
    if (totalLEDs <= 0) return;
    if (totalLEDs > (int)(sizeof(scrollBase)/sizeof(scrollBase[0])))
        totalLEDs = sizeof(scrollBase)/sizeof(scrollBase[0]);

    for (int i = 0; i < totalLEDs; i++) {
        scrollBase[i] = strip.getPixelColor(ledStart + i);
    }
    scrollBaseLen = totalLEDs;
    scrollOffset = 0;
    scrollBaseCaptured = true;
}

// ======================
// 🎨 BASE PATTERN FUNCTIONS
// ======================

void patternStripe() {
    if (multiColorCount == 0) return;

    int colorIndex = 0;
    for (int i = ledStart; i <= ledEnd; i++) {
        strip.setPixelColor(i, strip.Color(
            multiColors[colorIndex][0],
            multiColors[colorIndex][1],
            multiColors[colorIndex][2]
        ));
        colorIndex++;
        if (colorIndex >= multiColorCount) colorIndex = 0;
    }
    strip.show();
    scrollBaseCaptured = false;
}

// ✅ GRADIENT Pattern (with last→first wraparound)
void patternGradient() {
    if (multiColorCount < 2) {
        Serial.println("⚠️ Gradient needs at least 2 colors");
        return;
    }

    int totalLEDs = ledEnd - ledStart + 1;
    if (totalLEDs <= 0) return;

    // segments == number of color transitions (include wraparound)
    int segments = multiColorCount;                // note: NOT -1 anymore
    int basePerSeg = totalLEDs / segments;
    int extra = totalLEDs % segments;

    int ledIndex = ledStart;

    for (int c = 0; c < segments; c++) {
        // from color c to next color (wrap last→0)
        int c2 = (c + 1) % multiColorCount;

        uint8_t r1 = multiColors[c][0];
        uint8_t g1 = multiColors[c][1];
        uint8_t b1 = multiColors[c][2];

        uint8_t r2 = multiColors[c2][0];
        uint8_t g2 = multiColors[c2][1];
        uint8_t b2 = multiColors[c2][2];

        // fair distribution across all segments
        int segCount = basePerSeg + (c < extra ? 1 : 0);
        if (segCount <= 0) continue;

        for (int i = 0; i < segCount && ledIndex <= ledEnd; i++) {
            float t = (segCount == 1) ? 1.0f : (float)i / (segCount - 1);
            uint8_t r = (uint8_t)(r1 * (1 - t) + r2 * t);
            uint8_t g = (uint8_t)(g1 * (1 - t) + g2 * t);
            uint8_t b = (uint8_t)(b1 * (1 - t) + b2 * t);
            strip.setPixelColor(ledIndex++, strip.Color(r, g, b));
        }
    }

    // safety clamp
    while (ledIndex <= ledEnd) {
        strip.setPixelColor(ledIndex++, strip.Color(
            multiColors[multiColorCount - 1][0],
            multiColors[multiColorCount - 1][1],
            multiColors[multiColorCount - 1][2]
        ));
    }

    strip.show();
    // New static image ready; invalidate old capture
    scrollBaseCaptured = false;
}


void patternSplit() {
    if (multiColorCount < 1) return;

    int totalLEDs = ledEnd - ledStart + 1;
    int sectionSize = totalLEDs / multiColorCount;
    int extraLEDs = totalLEDs % multiColorCount;

    int ledIndex = ledStart;
    for (int c = 0; c < multiColorCount; c++) {
        int count = sectionSize + (c < extraLEDs ? 1 : 0);
        for (int i = 0; i < count; i++) {
            strip.setPixelColor(ledIndex,
                strip.Color(multiColors[c][0], multiColors[c][1], multiColors[c][2]));
            ledIndex++;
        }
    }
    strip.show();
    scrollBaseCaptured = false;
}

// ======================
// 🔁 SCROLL ANIMATION
// ======================
void animateScroll() {
    if (!scrollBaseCaptured || scrollBaseLen <= 1) return;

    scrollOffset = (scrollOffset + 1) % scrollBaseLen;

    for (int i = 0; i < scrollBaseLen; i++) {
        int src = i - scrollOffset;
        if (src < 0) src += scrollBaseLen;
        strip.setPixelColor(ledStart + i, scrollBase[src]);
    }
    strip.show();
}

// ======================
// 🎛 PATTERN COMMAND HANDLER
// ======================
void handlePattern(String cmd) {
    String pattern = cmd.substring(12);
    pattern.toLowerCase();

    if (pattern == "scroll") {
        scrollMode = true;
        currentEffect = NONE;

        if (basePattern == "stripe")        patternStripe();
        else if (basePattern == "gradient") patternGradient();
        else if (basePattern == "split")    patternSplit();
        else if (multiColorCount > 0)       renderMultiColorsBlock();

        captureScrollBase();
        Serial.println("🔁 Scroll animation ENABLED");
        return;
    }

    if (pattern == "stop") {
        stopScrollMode();
        Serial.println("🛑 Pattern animation stopped");
        return;
    }

    // Valid static pattern
    basePattern = pattern;
    stopScrollMode();

    if (pattern == "stripe")        patternStripe();
    else if (pattern == "gradient") patternGradient();
    else if (pattern == "split")    patternSplit();
    else {
        Serial.print("❌ Unknown base pattern: ");
        Serial.println(pattern);
        return;
    }

    Serial.print("🎨 Base Pattern Set → ");
    Serial.println(basePattern);
}

// ======================
// 🔄 LOOP UPDATER
// ======================
void updateActivePattern() {
    if (!scrollMode) return;

    unsigned long now = millis();
    if (now - lastPatternUpdate < patternSpeed) return;
    lastPatternUpdate = now;

    animateScroll();
}

// ✅ Trigger full pattern redraw + scroll re-capture
void refreshCurrentPattern() {
    if (multiColorCount == 0) {
        fillAll(currentColor);
        return;
    }

    if (basePattern == "stripe")        patternStripe();
    else if (basePattern == "gradient") patternGradient();
    else if (basePattern == "split")    patternSplit();
    else                                renderMultiColorsBlock();

    if (scrollMode) {
        captureScrollBase();
    }
}
