#pragma once
#define MAX_QUEUE 10
String commandQueue[MAX_QUEUE];
int queueStart = 0;
int queueEnd = 0;
bool processingCommands = false;
#include "moods.h"

void stopScrollMode();

String effectName(EffectType e) {
    switch (e) {
        case WAVE: return "wave";
        case BLINK: return "blink";
        case CHASE: return "chase";
        case PULSE: return "pulse";
        case RAINBOW: return "rainbow";
        case CENTER_WAVE: return "center_wave";
        case BOUNCE_WAVE: return "bounce_wave";
        case TWINKLE: return "twinkle";
        case PARTY_FLASH: return "party_flash";
        case FIRE_GLOW: return "fire_glow";
        case THUNDER: return "thunder";
        case FADE_LOOP: return "fade_loop";
        case HEARTBEAT: return "heartbeat";
        case DRIZZLE: return "drizzle";
        case FLASH: return "flash";
        case COLOR_COMET: return "color_comet";
        case SOFT_GLOW: return "soft_glow";
        case STAR_RAIN: return "star_rain";
        case FIREWORKS: return "fireworks";
        default: return "none";
    }
}

MoodType resolveMoodType(String name) {
  name.toLowerCase();
  if (name == "happy") return MOOD_HAPPY;
  if (name == "sad") return MOOD_SAD;
  if (name == "angry") return MOOD_ANGRY;
  if (name == "chaotic") return MOOD_CHAOTIC;
  if (name == "calm") return MOOD_CALM;
  if (name == "love") return MOOD_LOVE;
  if (name == "energetic") return MOOD_ENERGETIC;
  if (name == "relaxed") return MOOD_RELAXED;
  if (name == "thoughtful") return MOOD_THOUGHTFUL;
  return MOOD_NONE;
}


// Legacy Composite RGB Setter
void setCompositeColorRGB(uint8_t r1, uint8_t g1, uint8_t b1,
                          uint8_t r2, uint8_t g2, uint8_t b2) {
    compositeMode = true;
    compositeColor1 = strip.Color(r1, g1, b1);
    compositeColor2 = strip.Color(r2, g2, b2);
    Serial.print("🎨 Composite RGB Mode: [");
    Serial.print(r1); Serial.print(","); Serial.print(g1); Serial.print(","); Serial.print(b1);
    Serial.print("] + [");
    Serial.print(r2); Serial.print(","); Serial.print(g2); Serial.print(","); Serial.print(b2);
    Serial.println("]");
}

// ✅ Command Reader
void handleSerialCommands() {
    while (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd.length() > 0) {
            Serial.println("📥 Queued → " + cmd);
            if ((queueEnd + 1) % MAX_QUEUE != queueStart) {
                commandQueue[queueEnd] = cmd;
                queueEnd = (queueEnd + 1) % MAX_QUEUE;
            } else {
                Serial.println("⚠️ Command Queue Full!");
            }
        }
    }
}

// ✅ Processor
void processCommand(String cmd) {
    if (cmd == "CMD:STOP") { currentEffect = NONE; return; }

    if (cmd == "CMD:CONTINUE") {
        if (lastBasePattern != "") {
            basePattern = lastBasePattern;
            refreshCurrentPattern();
            Serial.println("▶️ Pattern resumed");
        }
        if (lastEffect != NONE) {
            currentEffect = lastEffect;
            resetEffectState();
            Serial.print("▶️ Effect resumed: ");
            Serial.println(effectName(currentEffect));
        }
        return;
    }

    // 🎨 Colors and Patterns
    if (cmd.startsWith("CMD:RGB="))       { handleRGB(cmd); return; }
    if (cmd.startsWith("CMD:RGBN="))      { handleRGBN(cmd); return; }
    if (cmd.startsWith("CMD:COLOR="))     { handleCOLOR(cmd); return; }
    if (cmd.startsWith("CMD:COLORN="))    { handleCOLORN(cmd); return; }
    if (cmd.startsWith("CMD:COLOR+="))    { addColorToMulti(cmd.substring(11)); return; }
    if (cmd.startsWith("CMD:COLOR-="))    { removeColorFromMulti(cmd.substring(11)); return; }
    if (cmd.startsWith("CMD:PATTERN="))   { handlePattern(cmd); return; }

    // ✨ Effects
    if (cmd.startsWith("CMD:EFFECT=")) {
        stopScrollMode();
        String effect = cmd.substring(11);
        customSpeed = false;
        resetEffectState();

        if (effect == "wave")               { currentEffect = WAVE; effectSpeed = 30; shimmerActive = true; }
        else if (effect == "center_wave")   { currentEffect = CENTER_WAVE; effectSpeed = 30; shimmerActive = true; }
        else if (effect == "bounce_wave")   { currentEffect = BOUNCE_WAVE; effectSpeed = 30; shimmerActive = true; }
        else if (effect == "blink")         { currentEffect = BLINK; blinkCounter = 0; blinkOn = false; effectSpeed = 300; shimmerActive = false; }
        else if (effect == "chase")         { currentEffect = CHASE; chaseIndex = 0; chaseRep = 0; effectSpeed = 60; shimmerActive = false; }
        else if (effect == "pulse")         { currentEffect = PULSE; pulseBrightness = 0; pulseUp = true; effectSpeed = 20; shimmerActive = false; }
        else if (effect == "rainbow")       { currentEffect = RAINBOW; rainbowHue = 0; effectSpeed = 20; shimmerActive = false; }
        else if (effect == "strobe")        { currentEffect = STROBE; effectSpeed = 40; shimmerActive = false; }
        else if (effect == "twinkle")       { currentEffect = TWINKLE; effectSpeed = 60; shimmerActive = false; }
        else if (effect == "party_flash")   { currentEffect = PARTY_FLASH; effectSpeed = 30; shimmerActive = false; }
        else if (effect == "fire_glow")     { currentEffect = FIRE_GLOW; effectSpeed = 40; shimmerActive = false; }
        else if (effect == "color_comet")   { currentEffect = COLOR_COMET; waveIndex = 0; effectSpeed = 20; shimmerActive = false; }
        else if (effect == "thunder")       { currentEffect = THUNDER; effectSpeed = 30; shimmerActive = false; }
        else if (effect == "fade_loop")     { currentEffect = FADE_LOOP; fadeHue = 0; effectSpeed = 20; shimmerActive = false; }
        else if (effect == "soft_glow")     { currentEffect = SOFT_GLOW; effectSpeed = 20; shimmerActive = false; }
        else if (effect == "heartbeat")     { currentEffect = HEARTBEAT; effectSpeed = 25; shimmerActive = false; }
        else if (effect == "star_rain")     { currentEffect = STAR_RAIN; effectSpeed = 40; shimmerActive = false; }
        else if (effect == "fireworks")     { currentEffect = FIREWORKS; effectSpeed = 30; shimmerActive = false; }
        else if (effect == "drizzle")       { currentEffect = DRIZZLE; effectSpeed = 40; shimmerActive = false; }
        else if (effect == "flash")         { currentEffect = FLASH; effectSpeed = 60; shimmerActive = false; }
        else if (effect == "rain")          { currentEffect = RAIN; effectSpeed = 50; shimmerActive = false; }

        lastEffect = currentEffect;
        return;
    }


    // =====================
    // 💡 LED STRIP ON/OFF
    // =====================
if (cmd == "CMD:LED=ON") {
    ledState = true;
    stopScrollMode();
    currentEffect = NONE;        // don’t fight with effects

    if (multiColorCount > 0) {
        // If a basePattern was set earlier, re-apply it; else draw blocks
        if (basePattern == "stripe")      patternStripe();
        else if (basePattern == "gradient") patternGradient();
        else if (basePattern == "split")    patternSplit();
        else                                renderMultiColorsBlock();
    } else {
        // Single-color mode
        compositeMode = false;   // ensure legacy 2-color mode is off
        fillAll(currentColor);
    }
    return;
}

    if (cmd == "CMD:LED=OFF") { 
        ledState = false; 
        currentEffect = NONE; 
        strip.clear(); 
        strip.show(); 
        return; 
    }

    // =====================
    // 🖥 LCD MESSAGES
    // =====================
    if (cmd.startsWith("CMD:LCD=")) { 
        lcd.clear(); 
        lcd.print(cmd.substring(8)); 
        return; 
    }

    // =====================
    // 🎨 COLOR & PATTERN HANDLERS
    // =====================
    if (cmd.startsWith("CMD:RGB="))       { handleRGB(cmd); return; }
    if (cmd.startsWith("CMD:RGBN="))      { handleRGBN(cmd); return; }
    if (cmd.startsWith("CMD:COLOR="))     { handleCOLOR(cmd); return; }
    if (cmd.startsWith("CMD:COLORN="))    { handleCOLORN(cmd); return; }
    if (cmd.startsWith("CMD:PATTERN="))   { handlePattern(cmd); return; }

    // =====================
    // 🔆 BRIGHTNESS
    // =====================
    if (cmd.startsWith("CMD:BRIGHTNESS=")) {
        brightnessPct = constrain(cmd.substring(15).toInt(), 0, 100);
        brightness = map(brightnessPct, 0, 100, 0, 255);
        strip.setBrightness(brightness);
if (ledState && !scrollMode) {
    if (multiColorCount > 0) {
        if (basePattern == "stripe")      patternStripe();
        else if (basePattern == "gradient") patternGradient();
        else if (basePattern == "split")    patternSplit();
        else                                renderMultiColorsBlock();
    } else {
        fillAll(currentColor);
    }
}

        Serial.print("🔆 Brightness set: "); Serial.println(brightnessPct);
        return;
    }

    // =====================
    // 🎯 LED CONTROL
    // =====================
    if (cmd.startsWith("CMD:LEDINDEX=")) {
        int i = cmd.substring(13).toInt();
        if (i >= 0 && i < activeLEDCount) {
            currentEffect = NONE; 
            strip.clear(); 
            strip.setPixelColor(i, currentColor); 
            strip.show();
        }
        return;
    }

    if (cmd.startsWith("CMD:NUMLEDS=")) {
        strip.clear(); strip.show();
        activeLEDCount = constrain(cmd.substring(12).toInt(), 0, NUM_LEDS);
        ledStart = 0;
        ledEnd = activeLEDCount > 0 ? activeLEDCount - 1 : 0;
        Serial.print("Active LEDs: "); Serial.println(activeLEDCount);
if (ledState && !scrollMode) {
    if (multiColorCount > 0) {
        if (basePattern == "stripe")      patternStripe();
        else if (basePattern == "gradient") patternGradient();
        else if (basePattern == "split")    patternSplit();
        else                                renderMultiColorsBlock();
    } else {
        fillAll(currentColor);
    }
}

        return;
    }

    if (cmd.startsWith("CMD:LEDRANGE=")) {
        strip.clear(); strip.show();
        String p = cmd.substring(13);
        int s = p.substring(0, p.indexOf(',')).toInt();
        int e = p.substring(p.indexOf(',') + 1).toInt();
        s = constrain(s, 0, NUM_LEDS - 1);
        e = constrain(e, 0, NUM_LEDS - 1);
        if (s <= e) {
            ledStart = s;
            ledEnd = e;
            activeLEDCount = e - s + 1;
            Serial.print("LED Range: "); Serial.print(ledStart); Serial.print(" to "); Serial.println(ledEnd);
if (ledState && !scrollMode) {
    if (multiColorCount > 0) {
        if (basePattern == "stripe")      patternStripe();
        else if (basePattern == "gradient") patternGradient();
        else if (basePattern == "split")    patternSplit();
        else                                renderMultiColorsBlock();
    } else {
        fillAll(currentColor);
    }
}

        }
        return;
    }

if (cmd.startsWith("CMD:COLOR+=")) { addColorToMulti(cmd.substring(11)); return; }
if (cmd.startsWith("CMD:COLOR-=")) { removeColorFromMulti(cmd.substring(11)); return; }


    // =====================
    // 😃 MOODS
    // =====================
    if (cmd.startsWith("CMD:MOOD=")) {
        String moodInput = cmd.substring(9);
        moodInput.toLowerCase();
        String primary = moodInput;
        String subMood = "";
        int sepIndex = moodInput.indexOf(':');
        if (sepIndex != -1) {
            primary = moodInput.substring(0, sepIndex);
            subMood = moodInput.substring(sepIndex + 1);
        } else {
            subMood = "default";
        }
        applyMood(resolveMoodType(primary), subMood);
        return;
    }

    // =====================
    // 🌧 RAIN MODES
    // =====================
    if (cmd.startsWith("CMD:RAIN=")) {
        rainMode = cmd.substring(9);
        rainMode.toLowerCase();

        if (rainMode == "light") {
            rainIntensity = 2;
            currentEffect = RAIN;
            lcd.clear(); lcd.print("☔ Rain: Light");
        } 
        else if (rainMode == "medium") {
            rainIntensity = 4;
            currentEffect = RAIN;
            lcd.clear(); lcd.print("🌦 Rain: Medium");
        } 
        else if (rainMode == "heavy") {
            rainIntensity = 6;
            currentEffect = RAIN;
            lcd.clear(); lcd.print("🌧 Rain: Heavy");
        } 
        else if (rainMode == "thunderstorm") {
            rainIntensity = 4;  
            currentEffect = RAIN;
            lcd.clear(); lcd.print("⛈ Thunderstorm");
        }
        else {
            Serial.println("❗ Unknown rain mode");
            lcd.clear(); lcd.print("❌ Invalid rain mode");
        }
        return;
    }

    // =====================
    // ⏩ SPEED
    // =====================
    if (cmd.startsWith("CMD:SPEED=DEFAULT")) {
        customSpeed = false;
        Serial.println("Speed reset to default.");
        return;
    }
    if (cmd.startsWith("CMD:SPEED=")) {
        effectSpeed = constrain(cmd.substring(10).toInt(), 1, 1000);
        customSpeed = true;
        Serial.print("Custom Speed: "); Serial.println(effectSpeed);
        return;
    }

    // =====================
    // 📍 REGION CONTROL
    // =====================
    if (cmd.startsWith("CMD:REGION=")) {
        String region = cmd.substring(11);
        region.toLowerCase();

        if (region == "first_half") {
            ledStart = 0;
            ledEnd = (NUM_LEDS / 2) - 1;
        }
        else if (region == "last_half") {
            ledStart = NUM_LEDS / 2;
            ledEnd = NUM_LEDS - 1;
        }
        else if (region == "all" || region == "full") {
            ledStart = 0;
            ledEnd = NUM_LEDS - 1;
        }
        else if (region == "middle") {
            ledStart = NUM_LEDS / 3;
            ledEnd = (NUM_LEDS * 2 / 3) - 1;
        }
        else if (region == "left_quarter") {
            ledStart = 0;
            ledEnd = NUM_LEDS / 4;
        }
        else if (region == "right_quarter") {
            ledStart = NUM_LEDS * 3 / 4;
            ledEnd = NUM_LEDS - 1;
        } 
        else {
            Serial.println("❗ Unknown region keyword");
            lcd.clear();
            lcd.print("❌ Region invalid");
            return;
        }

        activeLEDCount = ledEnd - ledStart + 1;
        strip.clear();
        strip.show();

        Serial.print("✅ Region set: "); Serial.print(ledStart); Serial.print(" to "); Serial.println(ledEnd);
        lcd.clear(); lcd.print("Region: "); lcd.setCursor(0, 1); lcd.print(region);

if (ledState && !scrollMode) {
    if (multiColorCount > 0) {
        if (basePattern == "stripe")      patternStripe();
        else if (basePattern == "gradient") patternGradient();
        else if (basePattern == "split")    patternSplit();
        else                                renderMultiColorsBlock();
    } else {
        fillAll(currentColor);
    }
}

        return;
    }

    // =====================
    // 🔌 RELAYS (light, fan)
    // =====================
    if (cmd.startsWith("CMD:RELAYSWITCH=")) {
        String args = cmd.substring(16);
        int eqIndex = args.indexOf('=');
        if (eqIndex > 0) {
            String device = args.substring(0, eqIndex);
            String state = args.substring(eqIndex + 1);
            device.trim(); state.trim();
            device.toLowerCase(); state.toLowerCase();

            if (device == "light") {
                digitalWrite(LIGHT_RELAY, (state == "on") ? HIGH : LOW);
                lcd.clear(); lcd.print("Light → " + state);
                Serial.println("✅ LIGHT → " + state);
            } 
            else if (device == "fan") {
                digitalWrite(FAN_RELAY, (state == "on") ? HIGH : LOW);
                lcd.clear(); lcd.print("Fan → " + state);
                Serial.println("✅ FAN → " + state);
            } 
            else {
                Serial.println("❗ Unknown relay device: " + device);
            }
        } else {
            Serial.println("❗ CMD:RELAYSWITCH format error");
        }
        return;
    }


    // =====================
    // 🚨 FALLBACK
    // =====================
    Serial.println("❓ Unknown Command: " + cmd);
}
