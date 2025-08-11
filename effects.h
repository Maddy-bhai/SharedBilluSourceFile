#pragma once
// =====================
// 🎨 LED EFFECTS MODULE
// =====================


bool shimmerActive = false;

void resetEffectState() {
  waveIndex = 0;
  blinkCount = 0;
  blinkCounter = 0;
  chaseIndex = 0;
  chaseRep = 0;
  blinkOn = false;
  strobeOn = false;
  strobeCount = 0;
  centerIndex = 0;
  pulseBrightness = 0;
  pulseUp = true;
  bounceForward = true;
  thunderState = true;
  fadeHue = 0;
  rainbowHue = 0;
}

// =====================
// ✅ RUN EFFECTS
// =====================
void runCurrentEffect() {
  if (currentEffect == NONE) return;
  unsigned long now = millis();
  if (now - lastMillis < effectSpeed) return;
  lastMillis = now;

  uint16_t len = ledEnd - ledStart + 1;

  switch (currentEffect) {

    // 🌊 Wave effect
// 🌊 Wave (brightness modulation over base frame)
case WAVE: {
  // Mark brightness-wave active so refreshCurrentPattern() re-captures
  shimmerActive = true;

  // Ensure base captured from current pattern/colors
  if (!scrollBaseCaptured || scrollBaseLen <= 0) captureScrollBase();

  static float phase = 0.0f;        // time offset
  const float spatialK = 0.15f;     // waves per LED (lower = wider)
  const float base     = 0.35f;     // min brightness scale (0..1)
  const float range    = 0.65f;     // amplitude (base+range ≤ 1)

  for (int i = 0; i < scrollBaseLen; i++) {
    uint32_t c = scrollBase[i];
    uint8_t r0 = (c >> 16) & 0xFF;
    uint8_t g0 = (c >> 8)  & 0xFF;
    uint8_t b0 =  c        & 0xFF;

    float s = base + range * (0.5f * (sinf(i * spatialK + phase) + 1.0f));
    uint8_t r = (uint8_t)constrain((int)(r0 * s), 0, 255);
    uint8_t g = (uint8_t)constrain((int)(g0 * s), 0, 255);
    uint8_t b = (uint8_t)constrain((int)(b0 * s), 0, 255);
    strip.setPixelColor(ledStart + i, strip.Color(r, g, b));
  }
  strip.show();

  phase += 0.20f;                   // travel speed
  if (phase > 6.28318f) phase -= 6.28318f;
  break;
}


    // 💡 Blink effect
    case BLINK:
      if (blinkOn) fillAll(currentColor);
      else { strip.clear(); strip.show(); }
      blinkOn = !blinkOn;
      break;

    // 🏃‍♂️ Chase effect
    case CHASE:
      strip.clear();
      strip.setPixelColor(ledStart + (chaseIndex % len), currentColor);
      strip.setPixelColor(ledStart + ((chaseIndex + 2) % len), currentColor);
      strip.setPixelColor(ledStart + ((chaseIndex + 4) % len), currentColor);
      strip.show();
      if (++chaseIndex >= len) { chaseIndex = 0; chaseRep++; }
      if (chaseRep >= 10) currentEffect = NONE;
      break;

    // ⚡ Strobe effect
    case STROBE: {
      static uint8_t flashCount = 0;
      static bool flashing = false;
      static bool lightOn = false;
      static unsigned long pauseTimer = 0;

      const uint8_t BURST_FLASHES = 4;
      const uint8_t FLASH_SPEED = 40;
      const uint16_t PAUSE_AFTER_BURST = 500;

      if (!flashing && millis() < pauseTimer) {
        strip.clear();
        strip.show();
        break;
      }
      if (!flashing) {
        flashing = true;
        flashCount = 0;
      }
      if (lightOn) {
        strip.clear();
        strip.show();
      } else {
        uint32_t flashColor = (currentColor == 0) ? strip.Color(255, 255, 255) : currentColor;
        for (uint16_t i = ledStart; i <= ledEnd; i++) {
          strip.setPixelColor(i, flashColor);
        }
        strip.show();
      }
      lightOn = !lightOn;
      if (!lightOn) flashCount++;
      if (flashCount >= BURST_FLASHES) {
        flashing = false;
        pauseTimer = millis() + PAUSE_AFTER_BURST;
      }
      break;
    }

    // 🌌 Pulse effect
    case PULSE:
      strip.setBrightness(pulseBrightness);
      fillAll(currentColor);
      pulseBrightness += (pulseUp ? 5 : -5);
      if (pulseBrightness >= 255) pulseUp = false;
      if (pulseBrightness <= 10) pulseUp = true;
      break;

// 🏛 Center wave (brightness ring expanding from center)
case CENTER_WAVE: {
  shimmerActive = true;
  if (!scrollBaseCaptured || scrollBaseLen <= 0) captureScrollBase();

  static float phase = 0.0f;
  const float k      = 0.22f;  // spatial frequency
  const float base   = 0.35f;
  const float range  = 0.65f;

  float mid = (scrollBaseLen - 1) * 0.5f;

  for (int i = 0; i < scrollBaseLen; i++) {
    int idx = ledStart + i;
    uint32_t c = scrollBase[i];
    uint8_t r0 = (c >> 16) & 0xFF, g0 = (c >> 8) & 0xFF, b0 = c & 0xFF;

    float d = fabsf(i - mid);
    // use cos(|x|*k - phase) so the bright band moves outward symmetrically
    float s = base + range * (0.5f * (cosf(d * k - phase) + 1.0f));

    uint8_t r = (uint8_t)constrain((int)(r0 * s), 0, 255);
    uint8_t g = (uint8_t)constrain((int)(g0 * s), 0, 255);
    uint8_t b = (uint8_t)constrain((int)(b0 * s), 0, 255);
    strip.setPixelColor(idx, strip.Color(r, g, b));
  }
  strip.show();
  phase += 0.20f;
  if (phase > 6.28318f) phase -= 6.28318f;
  break;
}

// ↔ Bounce wave (brightness lobe bouncing left↔right)
case BOUNCE_WAVE: {
  shimmerActive = true;
  if (!scrollBaseCaptured || scrollBaseLen <= 0) captureScrollBase();

  static float pos = 0.0f;
  static float v   = 0.8f;   // pixels per frame
  const float k    = 0.28f;  // spatial frequency for the lobe shape
  const float base = 0.35f;
  const float range= 0.65f;

  // Bounce position
  pos += v;
  if (pos >= scrollBaseLen - 1) { pos = scrollBaseLen - 1; v = -v; }
  if (pos <= 0)                  { pos = 0;                v = -v; }

  for (int i = 0; i < scrollBaseLen; i++) {
    uint32_t c = scrollBase[i];
    uint8_t r0 = (c >> 16) & 0xFF, g0 = (c >> 8) & 0xFF, b0 = c & 0xFF;

    float x = (i - pos) * k;
    // bright lobe around 'pos' using cos falloff
    float s = base + range * (0.5f * (cosf(x) + 1.0f));

    uint8_t r = (uint8_t)constrain((int)(r0 * s), 0, 255);
    uint8_t g = (uint8_t)constrain((int)(g0 * s), 0, 255);
    uint8_t b = (uint8_t)constrain((int)(b0 * s), 0, 255);
    strip.setPixelColor(ledStart + i, strip.Color(r, g, b));
  }
  strip.show();
  break;
}


    // ✨ Twinkle effect
    case TWINKLE:
      strip.setPixelColor(ledStart + random(len), currentColor);
      strip.setPixelColor(ledStart + random(len), 0);
      strip.show();
      break;

    // 🎉 Party Flash effect
    case PARTY_FLASH: {
      static uint8_t state = 0;
      static uint8_t frameCount = 0;
      static uint16_t offset = 0;
      static uint8_t framesPerState = 18;

      static int bouncePos = 0;
      static bool bounceForward = true;
      static uint8_t bounceTrigger = 0;
      static const uint8_t bounceColorCount = 5;
      static uint32_t bounceColors[bounceColorCount];

      const uint8_t maxState = 5;
      const uint8_t blockSize = 10;
      const uint8_t numColors = 6;

      uint32_t colors[numColors] = {
        strip.Color(255, 0, 0),
        strip.Color(0, 255, 0),
        strip.Color(0, 0, 255),
        strip.Color(255, 255, 0),
        strip.Color(255, 0, 255),
        strip.Color(0, 255, 255)
      };

      strip.clear();

      switch (state) {
        case 0:  // Rotating Color Bands
          for (int i = 0; i < len; i++) {
            int colorIndex = ((i + offset) / blockSize) % numColors;
            strip.setPixelColor(ledStart + i, colors[colorIndex]);
          }
          break;

        case 1:  // Random Sparkles
          for (int i = 0; i < 10; i++) {
            int idx = ledStart + random(len);
            strip.setPixelColor(idx, strip.Color(random(255), random(255), random(255)));
          }
          break;

        case 2: {  // Strong White Flash
          static bool flashOn = true;
          static uint8_t flashToggle = 0;

          if (flashOn) {
            for (uint16_t i = 0; i < len; i++)
              strip.setPixelColor(ledStart + i, strip.Color(255, 255, 255));
          } else {
            strip.clear();
          }

          flashOn = !flashOn;
          flashToggle++;
          if (flashToggle >= 4) {
            flashToggle = 0;
            state = (state + 1) % maxState;
          }
          break;
        }

        case 3:  // Spark Wave
          for (int i = 0; i < len; i++) {
            uint8_t wave = (sin((i + offset) * 0.3) + 1) * 127;
            strip.setPixelColor(ledStart + i, strip.Color(wave, random(100), random(255)));
          }
          break;

        case 4: {  // Bounce LEDs
          if (bounceTrigger == 0) {
            bouncePos = random(len - bounceColorCount);
            bounceForward = random(2);
            for (int i = 0; i < bounceColorCount; i++) {
              bounceColors[i] = strip.Color(random(255), random(255), random(255));
            }
          }

          for (int i = 0; i < bounceColorCount; i++) {
            int pos = bouncePos + i;
            if (pos >= 0 && pos < len)
              strip.setPixelColor(ledStart + pos, bounceColors[i]);
          }

          if (bounceForward) bouncePos += 2;
          else bouncePos -= 2;

          if (bouncePos <= 0 || bouncePos >= len - bounceColorCount)
            bounceForward = !bounceForward;

          bounceTrigger++;
          if (bounceTrigger >= 12) {
            bounceTrigger = 0;
            state = 0;
          }
          break;
        }
      }

      strip.show();
      frameCount++;
      offset++;
      if (state != 2 && state != 4 && frameCount >= framesPerState) {
        frameCount = 0;
        state = (state + 1) % maxState;
      }
      break;
    }

    // 🔥 Fire Glow effect
    case FIRE_GLOW:
      for (uint16_t i = 0; i < len; i++) {
        int flicker = random(160, 255);
        strip.setPixelColor(ledStart + i, strip.Color(flicker, flicker / 6, 0));
      }
      strip.show();
      break;

    // 🌠 Color Comet effect
    case COLOR_COMET: {
      const uint8_t tailLength = 10;
      strip.clear();

      for (int i = 0; i < tailLength; i++) {
        int index = waveIndex - i;
        if (index < 0) continue;
        float fade = 1.0 - (float)i / tailLength;
        uint8_t r = (uint8_t)((currentColor >> 16 & 0xFF) * fade);
        uint8_t g = (uint8_t)((currentColor >> 8 & 0xFF) * fade);
        uint8_t b = (uint8_t)((currentColor & 0xFF) * fade);
        strip.setPixelColor(ledStart + index, strip.Color(r, g, b));
      }

      strip.show();
      waveIndex++;
      if (waveIndex >= len) waveIndex = 0;
      break;
    }

    // ⚡ Thunder effect
    case THUNDER: {
      static uint8_t stage = 0;
      static uint8_t flickerCount = 0;
      static unsigned long nextEvent = 0;

      if (millis() < nextEvent) break;

      switch (stage) {
        case 0:
          flickerCount = random(2, 5);
          stage = 1;
          break;

        case 1:
          if (flickerCount > 0) {
            uint8_t brightness = random(50, 180); 
            for (uint16_t i = ledStart; i <= ledEnd; i++) {
              strip.setPixelColor(i, strip.Color(brightness, brightness, brightness));
            }
            strip.show();

            delay(30);
            strip.clear();
            strip.show();

            flickerCount--;
            nextEvent = millis() + random(50, 120);
          } else {
            stage = 2;
          }
          break;

        case 2:
          for (uint16_t i = ledStart; i <= ledEnd; i++) {
            strip.setPixelColor(i, strip.Color(255, 255, 255));
          }
          strip.show();
          delay(60);
          strip.clear();
          strip.show();

          stage = 3;
          nextEvent = millis() + random(100, 500);
          break;

        case 3:
          if (random(0, 2)) { 
            uint8_t afterGlow = random(50, 120);
            for (uint16_t i = ledStart; i <= ledEnd; i++) {
              strip.setPixelColor(i, strip.Color(afterGlow, afterGlow, afterGlow));
            }
            strip.show();
            delay(40);
            strip.clear();
            strip.show();
          }
          stage = 0;
          nextEvent = millis() + random(2000, 6000);
          break;
      }
      break;
    }

    // 🌈 Fade loop
    case FADE_LOOP:
      for (uint16_t i = 0; i < len; i++)
        strip.setPixelColor(ledStart + i, strip.gamma32(strip.ColorHSV(fadeHue)));
      strip.show();
      fadeHue = (fadeHue + 256) % 65536;
      break;

    // 🌈 Rainbow
    case RAINBOW:
      for (uint16_t i = 0; i < len; i++) {
        uint16_t hue = rainbowHue + (i * 65536L / len);
        strip.setPixelColor(ledStart + i, strip.gamma32(strip.ColorHSV(hue)));
      }
      strip.show();
      rainbowHue = (rainbowHue + 256) % 65536;
      break;

    // 🌟 Soft glow
    case SOFT_GLOW: {
      static uint8_t glowLevel = 0;
      static bool glowUp = true;

      strip.setBrightness(glowLevel);
      fillAll(currentColor);

      if (glowUp) glowLevel += 2;
      else glowLevel -= 2;

      if (glowLevel >= 120) glowUp = false;
      if (glowLevel <= 10) glowUp = true;

      break;
    }

    // ❤️ Heartbeat
    case HEARTBEAT: {
      static uint8_t stage = 0;
      static uint16_t frameCounter = 0;
      uint8_t r = 255, g = 5, b = 5;
      uint32_t hbColor = strip.Color(r, g, b);

      switch (stage) {
        case 0: fillAll(hbColor); break;
        case 1: strip.clear(); break;
        case 2: fillAll(hbColor); break;
        case 3: strip.clear(); break;
        case 4: {
          float fade = 1.0 - (frameCounter / 80.0);
          uint8_t fr = r * fade;
          uint8_t fg = g * fade;
          uint8_t fb = b * fade;
          fillAll(strip.Color(fr, fg, fb));
          break;
        }
        default: strip.clear(); break;
      }

      strip.show();
      frameCounter++;

      if ((stage == 0 || stage == 2) && frameCounter >= 10) { stage++; frameCounter = 0; }
      else if ((stage == 1 || stage == 3) && frameCounter >= 15) { stage++; frameCounter = 0; }
      else if (stage == 4 && frameCounter >= 80) { stage = 0; frameCounter = 0; }
      break;
    }

    // 🌠 Star Rain
    case STAR_RAIN: {
      static uint8_t sparkleCount = 10;
      static uint8_t fadeAmount = 20;

      for (int i = ledStart; i <= ledEnd; i++) {
        uint32_t color = strip.getPixelColor(i);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        r = (r >= fadeAmount) ? r - fadeAmount : 0;
        g = (g >= fadeAmount) ? g - fadeAmount : 0;
        b = (b >= fadeAmount) ? b - fadeAmount : 0;
        strip.setPixelColor(i, r, g, b);
      }

      for (int i = 0; i < sparkleCount; i++) {
        int idx = ledStart + random(ledEnd - ledStart + 1);
        strip.setPixelColor(idx, strip.Color(random(200, 255), random(200, 255), random(200, 255)));
      }
      strip.show();
      break;
    }

    // 🎆 Fireworks
    case FIREWORKS: {
      static const uint8_t maxFireworks = 8;
      static struct {
        int pos;
        uint32_t baseColor;
        uint8_t age;
        bool active;
        uint16_t nextLaunchIn;
      } fireworks[maxFireworks];

      for (int i = ledStart; i <= ledEnd; i++) {
        uint32_t col = strip.getPixelColor(i);
        uint8_t r = (col >> 16) & 0xFF;
        uint8_t g = (col >> 8) & 0xFF;
        uint8_t b = col & 0xFF;
        r = (r > 10) ? r - 10 : 0;
        g = (g > 10) ? g - 10 : 0;
        b = (b > 10) ? b - 10 : 0;
        strip.setPixelColor(i, r, g, b);
      }

      for (int i = 0; i < maxFireworks; i++) {
        if (!fireworks[i].active) {
          if (fireworks[i].nextLaunchIn == 0) {
            fireworks[i].pos = random(ledStart + 10, ledEnd - 10);

            uint8_t r = 0, g = 0, b = 0;
            switch (random(6)) {
              case 0: r = 255; break;
              case 1: g = 255; break;
              case 2: b = 255; break;
              case 3: r = 255; g = 100; break;
              case 4: g = 255; b = 100; break;
              case 5: r = 100; b = 255; break;
            }

            fireworks[i].baseColor = strip.Color(r, g, b);
            fireworks[i].age = 0;
            fireworks[i].active = true;
          } else {
            fireworks[i].nextLaunchIn--;
          }
        }
      }

      for (int i = 0; i < maxFireworks; i++) {
        if (fireworks[i].active) {
          int center = fireworks[i].pos;
          uint32_t base = fireworks[i].baseColor;
          uint8_t br = (base >> 16) & 0xFF;
          uint8_t bg = (base >> 8) & 0xFF;
          uint8_t bb = base & 0xFF;

          for (int j = -5; j <= 5; j++) {
            int idx = center + j;
            if (idx < ledStart || idx > ledEnd) continue;

            int brightness = max(0, (int)(255 - abs(j) * 70 - fireworks[i].age * 50 + random(-10, 10)));
            if (brightness > 0) {
              uint8_t r = constrain(br + random(-40, 40), 0, 255);
              uint8_t g = constrain(bg + random(-40, 40), 0, 255);
              uint8_t b = constrain(bb + random(-40, 40), 0, 255);
              strip.setPixelColor(idx, strip.Color(r, g, b));
            }
          }

          fireworks[i].age++;
          if (fireworks[i].age > 4) {
            fireworks[i].active = false;
            fireworks[i].nextLaunchIn = random(20, 200);
          }
        }
      }
      strip.show();
      break;
    }

    // 🌧 Drizzle
    case DRIZZLE:
      strip.clear();
      for (int i = 0; i < 3; i++) {
        int drop = ledStart + random(activeLEDCount);
        strip.setPixelColor(drop, strip.Color(0, 50, 255));
      }
      strip.show();
      break;

    // ⚡ Flash effect
    case FLASH: {
      static bool flashState = true;
      static uint8_t flashCounter = 0;
      const uint8_t maxFlashes = 6;

      if (flashState) {
        for (uint16_t i = ledStart; i <= ledEnd; i++) {
          strip.setPixelColor(i, strip.Color(255, 255, 255)); 
        }
      } else {
        strip.clear();
      }

      strip.show();
      flashState = !flashState;
      flashCounter++;
      if (flashCounter >= maxFlashes) {
        currentEffect = NONE;
        flashCounter = 0;
      }
      break;
    }

    // 🌧 Rain
    case RAIN: {

      // 🌧 Always add rain drops for all rain modes
      for (int i = 0; i < rainIntensity; i++) {
        int drop = ledStart + random(activeLEDCount);
        strip.setPixelColor(drop, strip.Color(0, 80, 255));  // normal rain blue
      }

      // 🌊 Fade old drops for shimmer effect
      for (int i = ledStart; i <= ledEnd; i++) {
        uint32_t col = strip.getPixelColor(i);
        uint8_t r = (col >> 16) & 0xFF;
        uint8_t g = (col >> 8) & 0xFF;
        uint8_t b = col & 0xFF;
        r = (r > 5) ? r - 5 : 0;
        g = (g > 5) ? g - 5 : 0;
        b = (b > 10) ? b - 10 : 0;
        strip.setPixelColor(i, r, g, b);
      }

      // ⚡ Lightning system used for BOTH heavy rain & thunderstorm
      static uint8_t stage = 0;
      static uint8_t flickerCount = 0;
      static unsigned long nextEvent = 0;
      static uint8_t strikeType = 0; // 0=small, 1=vein, 2=big
      static uint8_t bigThunderCounter = 0;

      // ⚡ Trigger conditions
      bool triggerLightning = false;
      if (rainMode == "thunderstorm") {
          triggerLightning = true;   // always active
      } else if (rainMode == "heavy" && random(0, 100) == 0) {  // rare lightning in heavy rain
          triggerLightning = true;
      }

      // 🌊 Add extra rain ONLY in thunderstorm mode (4× intensity)
      if (rainMode == "thunderstorm") {
          for (int i = 0; i < rainIntensity * 4; i++) {
            int drop = ledStart + random(activeLEDCount);
            // occasional brighter “storm blue” drops
            if (random(0, 8) == 0) {
              strip.setPixelColor(drop, strip.Color(120, 180, 255));  // bright storm blue
            } else {
              strip.setPixelColor(drop, strip.Color(0, 100, 255));    // normal storm blue
            }
          }
      }

      // ⚡ Run lightning effect if triggered
      if (triggerLightning && millis() > nextEvent) {

        switch (stage) {
          case 0: {  
            bigThunderCounter++;
            if (bigThunderCounter >= 4) {  
              strikeType = 2;  // 🌩 Big thunder every 4th strike
              bigThunderCounter = 0;
            } else {
              strikeType = random(0, 2); // 0=small, 1=vein
            }
            flickerCount = (strikeType == 2) ? random(3, 6) : random(2, 4); 
            stage = 1;
            break;
          }

          case 1: {  
            // ⚡ Pre-flickers before main strike
            if (flickerCount > 0) {
              uint8_t brightness = random(50, 180);
              int segStart = ledStart + random(activeLEDCount - 30);
              int segEnd = segStart + random(10, 40);

              for (int i = segStart; i <= segEnd && i <= ledEnd; i++) {
                strip.setPixelColor(i, strip.Color(brightness, brightness, brightness));
              }
              strip.show();

              delay(30);
              strip.clear();
              strip.show();

              flickerCount--;
              nextEvent = millis() + random(50, 120);
            } else {
              stage = 2;
            }
            break;
          }

          case 2: {  
            // ⚡ Main strike types
            if (strikeType == 0) {
              // Small segment lightning
              int segStart = ledStart + random(activeLEDCount - 50);
              int segEnd = segStart + random(20, 80);
              for (int i = segStart; i <= segEnd && i <= ledEnd; i++) {
                strip.setPixelColor(i, strip.Color(255, 255, 255));
              }
            } 
            else if (strikeType == 1) {
              // 🌩 Vein lightning
              int startPos = ledStart + random(activeLEDCount - 20);
              for (int v = 0; v < random(6, 12); v++) {
                int branchLength = random(5, 12);         
                int direction = (random(0, 2) == 0) ? 1 : -1;

                for (int i = 0; i < branchLength; i++) {
                  int index = startPos + (i * direction);
                  if (index >= ledStart && index <= ledEnd) {
                    if (random(0, 3) == 0) {
                      strip.setPixelColor(index, strip.Color(180, 220, 255)); 
                    } else {
                      strip.setPixelColor(index, strip.Color(255, 255, 255)); 
                    }
                    if (index + 1 <= ledEnd) strip.setPixelColor(index + 1, strip.Color(150, 200, 255));
                    if (index - 1 >= ledStart) strip.setPixelColor(index - 1, strip.Color(150, 200, 255));
                  }
                }
                startPos += random(-8, 8);
                if (startPos < ledStart) startPos = ledStart;
                if (startPos > ledEnd) startPos = ledEnd;
              }
              strip.show();
              delay(100);
              strip.clear();
              strip.show();
            }
            else if (strikeType == 2) {
              // 🌩 Big thunder: full strip
              for (uint16_t i = ledStart; i <= ledEnd; i++) {
                strip.setPixelColor(i, strip.Color(255, 255, 255));
              }
            }

            strip.show();
            delay((strikeType == 2) ? 120 : 60);
            strip.clear();
            strip.show();

            stage = 3;
            nextEvent = millis() + random(100, 500);
            break;
          }

          case 3: {  
            // 🌫 Afterglow
            if (random(0, 2)) {  
              uint8_t afterGlow = random(50, 120);
              int afterSegStart = ledStart + random(activeLEDCount - 30);
              int afterSegEnd = afterSegStart + random(10, 30);
              for (int i = afterSegStart; i <= afterSegEnd && i <= ledEnd; i++) {
                strip.setPixelColor(i, strip.Color(afterGlow, afterGlow, afterGlow));
              }
              strip.show();

              delay(40);
              strip.clear();
              strip.show();
            }
            stage = 0;
            nextEvent = millis() + ((strikeType == 2) ? random(4000, 7000) : random(2000, 5000)); 
            break;
          }
        }
      }

      strip.show();
      break;
    }

    // 🛑 Default
    default:
      break;
  }
}
