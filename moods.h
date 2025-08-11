#pragma once
#include "colors.h"
#include "effects.h"
#include "shared_state.h"

// === Mood Enum ===
enum MoodType {
  MOOD_HAPPY,
  MOOD_SAD,
  MOOD_ANGRY,
  MOOD_CHAOTIC,
  MOOD_CALM,
  MOOD_LOVE,
  MOOD_ENERGETIC,
  MOOD_RELAXED,
  MOOD_THOUGHTFUL,
  MOOD_NONE
};

// === Mood Display Names ===
String getMoodName(MoodType mood) {
  switch (mood) {
    case MOOD_HAPPY: return "happy";
    case MOOD_SAD: return "sad";
    case MOOD_ANGRY: return "angry";
    case MOOD_CHAOTIC: return "chaotic";
    case MOOD_CALM: return "calm";
    case MOOD_LOVE: return "love";
    case MOOD_ENERGETIC: return "energetic";
    case MOOD_RELAXED: return "relaxed";
    case MOOD_THOUGHTFUL: return "thoughtful";
    default: return "none";
  }
}

// === Mood Handler ===
void applyMood(MoodType mood, String subMood = "") {
  stopScrollMode();
  shimmerActive = false;
  currentEffect = NONE;
  multiColorCount = 0;
  effectSpeed = 100;
  brightness = 100;
  strip.setBrightness(brightness);

  // 🔥 MAIN MOOD SYSTEM
  if (mood == MOOD_HAPPY) {
    if (subMood == "excited") {
      handleCOLORN("yellow,orange,pink");
      currentEffect = PARTY_FLASH;
    } else if (subMood == "cheerful") {
      handleCOLORN("sky blue,yellow,pink");
      currentEffect = BOUNCE_WAVE;
    } else {
      handleCOLOR("yellow");
      currentEffect = PULSE;
    }
  }

  else if (mood == MOOD_SAD) {
    if (subMood == "lonely") {
      handleCOLOR("blue");
      currentEffect = SOFT_GLOW;
    } else if (subMood == "hopeless") {
      handleCOLOR("dull gray");
      currentEffect = SOFT_GLOW;
    } else {
      handleCOLOR("cyan");
      currentEffect = FADE_LOOP;
    }
  }

  else if (mood == MOOD_ANGRY) {
    if (subMood == "rage") {
      handleCOLOR("deep red");
      currentEffect = STROBE;
    } else if (subMood == "irritated") {
      handleCOLOR("orange");
      currentEffect = BLINK;
    } else {
      handleCOLOR("red");
      currentEffect = FLASH;
    }
  }

  else if (mood == MOOD_CHAOTIC) {
    if (subMood == "madness") {
      handleCOLORN("red,blue,green,purple");
      currentEffect = FIREWORKS;
    } else if (subMood == "glitch") {
      handleCOLORN("magenta,cyan,yellow");
      currentEffect = TWINKLE;
    } else {
      handleCOLORN("random");
      currentEffect = PARTY_FLASH;
    }
  }

  else if (mood == MOOD_CALM) {
    if (subMood == "peaceful") {
      handleCOLOR("mint");
      currentEffect = WAVE;
    } else if (subMood == "dreamy") {
      handleCOLOR("lavender");
      currentEffect = FADE_LOOP;
    } else {
      handleCOLOR("sky blue");
      currentEffect = SOFT_GLOW;
    }
  }

  else if (mood == MOOD_LOVE) {
    if (subMood == "romantic") {
      handleCOLOR("pink");
      currentEffect = HEARTBEAT;
    } else if (subMood == "longing") {
      handleCOLOR("purple");
      currentEffect = FADE_LOOP;
    } else {
      handleCOLOR("red");
      currentEffect = PULSE;
    }
  }

  else if (mood == MOOD_ENERGETIC) {
    if (subMood == "power") {
      handleCOLOR("orange");
      currentEffect = CHASE;
    } else if (subMood == "wild") {
      handleCOLORN("red,green,blue");
      currentEffect = PARTY_FLASH;
    } else {
      handleCOLOR("yellow");
      currentEffect = BLINK;
    }
  }

  else if (mood == MOOD_RELAXED) {
    if (subMood == "sleepy") {
      handleCOLOR("soft white");
      currentEffect = SOFT_GLOW;
    } else if (subMood == "satisfied") {
      handleCOLOR("warm white");
      currentEffect = WAVE;
    } else {
      handleCOLOR("cool white");
      currentEffect = PULSE;
    }
  }

  else if (mood == MOOD_THOUGHTFUL) {
    if (subMood == "focused") {
      handleCOLOR("ocean");
      currentEffect = CENTER_WAVE;
    } else if (subMood == "lost") {
      handleCOLOR("deep purple");
      currentEffect = STAR_RAIN;
    } else {
      handleCOLOR("lavender");
      currentEffect = BOUNCE_WAVE;
    }
  }

  else {
    handleCOLOR("white");
    currentEffect = NONE;
  }

  strip.setBrightness(brightness);
  runCurrentEffect();

  // ✅ Final log
  Serial.println("✅ Mood set → " + getMoodName(mood) + " > " + subMood);
  Serial.print("🎨 Mood Color (if single): ");
  Serial.println(currentColor);
}
