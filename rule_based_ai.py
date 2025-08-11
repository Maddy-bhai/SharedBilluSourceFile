# ============================================
# Billu RULE BASED AI (rule_based_ai.py)
# ============================================

import serial
import time
import re
import random

# ✅ Import all phrases and mappings
from phrases import (
    LED_ON_PHRASES, LED_OFF_PHRASES,
    LIGHT_ON_PHRASES, LIGHT_OFF_PHRASES,
    FAN_ON_PHRASES, FAN_OFF_PHRASES,
    STOP_PHRASES, LCD_PHRASES,
    SUPPORTED_COLORS, SUPPORTED_EFFECTS,
    RAIN_MODES, MOOD_MAP, SUB_MOOD_LIST,
    REGION_KEYWORDS, PATTERN_KEYWORDS
)

# ========== GLOBAL DEFAULT ==========
current_speed = 100
current_brightness = 50
DEBUG_MODE = True  # Set False for clean output later

# ========== SERIAL SETUP ==========
try:
    esp = serial.Serial('COM3', 115200, timeout=1)  # Change COM port if needed
    time.sleep(2)
    print("✅ Connected to ESP32 on COM3")
except Exception as e:
    print("❌ Could not connect to ESP32:", e)
    esp = None


# ========== SEND COMMAND FUNCTION ==========
def send_command(cmd: str):
    """ Sends CMD to ESP32 via serial """
    if esp and esp.is_open:
        esp.write((cmd + "\n").encode())
        print(f"📤 SENT → {cmd}")
    else:
        print(f"⚠️ (Simulated) Would send: {cmd}")



# ========== INPUT CLEANER ==========
def clean_input_text(raw_text: str) -> str:
    """ Lowercase, strip junk words, fix typos """
    text = raw_text.lower().strip()

    # 🚮 Remove filler words
    junk_words = [
        "please", "now", "just", "can you", "will you", "billu", "hey",
        "kind of", "sort of", "maybe", "could you", "let's", "i want to",
        "make it", "do it", "start", "begin", "the"
    ]

    for junk in junk_words:
        text = re.sub(rf"\b{junk}\b", "", text)

    # 🔄 Fix common typos
    corrections = {
        "centerwave": "center wave",
        "bouncewave": "bounce wave",
        "fireglow": "fire glow",
        "fade loop": "fade_loop",
        "partyflash": "party flash",
        "full bright": "full brightness",
        "super fast": "speed 10",
        "super slow": "speed 600",
    }
    for wrong, correct in corrections.items():
        if wrong in text:
            text = text.replace(wrong, correct)

    # 🔢 Normalize numbers like "50 %"
    text = re.sub(r'(\d+)\s*%', r'\1', text)

    # ✅ Remove extra spaces
    text = re.sub(r'\s+', ' ', text).strip()

    return text


# ========== HELPER MATCHERS ==========
def match_from_list(text, phrases):
    return any(phrase in text for phrase in phrases)

def detect_color(text):
    for color in SUPPORTED_COLORS:
        if color in text:
            return color
    return None

def detect_effect(text):
    for effect in SUPPORTED_EFFECTS:
        if effect in text:
            return effect
    return None

def detect_pattern(text):
    """ Finds pattern (stripe, gradient, split) in the text """
    for phrase, pattern in PATTERN_KEYWORDS.items():
        if phrase in text:
            return pattern
    return None


def detect_rain_mode(text):
    for mode, keywords in RAIN_MODES.items():
        if any(k in text for k in keywords):
            return mode
    return None

def detect_mood(text):
    for mood, keywords in MOOD_MAP.items():
        if any(k in text for k in keywords):
            return mood
    return None

def detect_speed_intensity(text):
    """ Map speed phrases → values """
    text = text.lower()
    if "as fast as possible" in text or "max speed" in text: return 5
    elif "super fast" in text: return 10
    elif "very fast" in text: return 20
    elif "faster" in text: return 30
    elif "bit faster" in text: return 50
    elif "fast" in text: return 80
    elif "normal" in text: return 150
    elif "bit slow" in text: return 200
    elif "slow" in text: return 300
    elif "very slow" in text: return 400
    elif "super slow" in text: return 500
    elif "ultra slow" in text: return 600
    return None

def detect_brightness_intensity(text):
    """ Map brightness phrases → percentage or numeric value """
    text = text.lower()

    # ✅ 1. Direct number extraction (e.g. 'brightness 80', 'set brightness to 50%')
    num_match = re.search(r'(\d{1,3})', text)
    if num_match:
        value = int(num_match.group(1))
        if 0 <= value <= 100:
            return value   # ✅ Use the number directly

    # ✅ 2. Word-based fallback mapping
    if "maximum brightness" in text or "full brightness" in text: return 100
    elif "very bright" in text: return 90
    elif "bright" in text: return 70
    elif "normal brightness" in text: return 50
    elif "bit dim" in text: return 30
    elif "dim" in text: return 20
    elif "very dim" in text or "super dim" in text: return 10
    elif "off" in text and "brightness" in text: return 0
    return None



# ========== FALLBACK PARSER ==========
def fallback_parser(user_text):
    """ Basic hardcoded phrases if NLP fails """
    commands = []
    text = user_text.lower()

    if "turn on led" in text: commands.append("CMD:LED=ON")
    if "turn off led" in text: commands.append("CMD:LED=OFF")
    if "light on" in text: commands.append("CMD:RELAYSWITCH=light=on")
    if "light off" in text: commands.append("CMD:RELAYSWITCH=light=off")
    if "fan on" in text: commands.append("CMD:RELAYSWITCH=fan=on")
    if "fan off" in text: commands.append("CMD:RELAYSWITCH=fan=off")

    stop_phrases = ["stop everything", "cancel all", "shutdown", "halt", "terminate"]
    if any(phrase in text for phrase in stop_phrases):
        commands.append("CMD:STOP")

    return commands


# ========== NLP PARSER ==========
def parse_input(user_text):
    """ Core NLP brain: converts natural language → CMD list (now with regex + synonym support & combo handling) """
    text = clean_input_text(user_text)
    commands = []

    # ✅ 🔹 STEP 2 – Combo Command Splitting
    split_words = [" and ", " then ", " also ", " after that "]
    parts = [text]

    for word in split_words:
        new_parts = []
        for part in parts:
            new_parts.extend([p.strip() for p in part.split(word) if p.strip()])
        parts = new_parts

    if len(parts) > 1:
        combo_cmds = []
        for part in parts:
            sub_cmds = parse_input(part) if part != user_text else []
            if sub_cmds:
                combo_cmds.extend(sub_cmds)

        # ✅ Smart ordering: LED → COLOR → EFFECT → RELAYSWITCH
        priority_order = ["CMD:LED", "CMD:COLOR", "CMD:EFFECT", "CMD:RELAYSWITCH"]
        combo_cmds.sort(key=lambda x: next((i for i, p in enumerate(priority_order) if x.startswith(p)), 99))

        return list(dict.fromkeys(combo_cmds))  # ✅ return early if combo detected

    # ✅ 1️⃣ Regex Matching
    regex_result = regex_match_command(text)
    if regex_result:
        # 🎯 Device ON/OFF
        if 'device' in regex_result:
            device = regex_result['device']
            if re.search(r"(turn on|switch on|start|activate)", text):
                if device == "led": commands.append("CMD:LED=ON")
                elif device == "fan": commands.append("CMD:RELAYSWITCH=fan=on")
                elif device == "light": commands.append("CMD:RELAYSWITCH=light=on")
            elif re.search(r"(turn off|switch off|stop|kill)", text):
                if device == "led": commands.append("CMD:LED=OFF")
                elif device == "fan": commands.append("CMD:RELAYSWITCH=fan=off")
                elif device == "light": commands.append("CMD:RELAYSWITCH=light=off")

        # 🎨 Color
        if 'color' in regex_result and regex_result['color']:
            commands.append(f"CMD:COLOR={regex_result['color']}")

        # 🎭 Mood
        if 'mood' in regex_result and regex_result['mood']:
            commands.append(f"CMD:MOOD={regex_result['mood']}")

    # ✅ 2️⃣ Synonym Matching (extra safety net)
    from phrases import LED_ON_SYNONYMS, LED_OFF_SYNONYMS, FAN_ON_SYNONYMS, FAN_OFF_SYNONYMS, LIGHT_ON_SYNONYMS, LIGHT_OFF_SYNONYMS

    if any(word in text for word in LED_ON_SYNONYMS):
        commands.append("CMD:LED=ON")
    if any(word in text for word in LED_OFF_SYNONYMS):
        commands.append("CMD:LED=OFF")

    if any(word in text for word in FAN_ON_SYNONYMS):
        commands.append("CMD:RELAYSWITCH=fan=on")
    if any(word in text for word in FAN_OFF_SYNONYMS):
        commands.append("CMD:RELAYSWITCH=fan=off")

    if any(word in text for word in LIGHT_ON_SYNONYMS):
        commands.append("CMD:RELAYSWITCH=light=on")
    if any(word in text for word in LIGHT_OFF_SYNONYMS):
        commands.append("CMD:RELAYSWITCH=light=off")

    # ✅ 3️⃣ Keep Old Detection as Fallback
    if match_from_list(text, LED_ON_PHRASES): commands.append("CMD:LED=ON")
    if match_from_list(text, LED_OFF_PHRASES): commands.append("CMD:LED=OFF")

    if match_from_list(text, LIGHT_ON_PHRASES): commands.append("CMD:RELAYSWITCH=light=on")
    if match_from_list(text, LIGHT_OFF_PHRASES): commands.append("CMD:RELAYSWITCH=light=off")
    if match_from_list(text, FAN_ON_PHRASES): commands.append("CMD:RELAYSWITCH=fan=on")
    if match_from_list(text, FAN_OFF_PHRASES): commands.append("CMD:RELAYSWITCH=fan=off")

    if match_from_list(text, STOP_PHRASES): commands.append("CMD:STOP")

    color = detect_color(text)
    if color: commands.append(f"CMD:COLOR={color}")

    effect = detect_effect(text)
    if effect: commands.append(f"CMD:EFFECT={effect}")

    pattern = detect_pattern(text)
    if pattern:
        commands.append(f"CMD:PATTERN={pattern}")

    rain_mode = detect_rain_mode(text)
    if rain_mode: commands.append(f"CMD:RAIN={rain_mode}")

    mood = detect_mood(text)
    if mood: commands.append(f"CMD:MOOD={mood}")

    speed = detect_speed_intensity(text)
    if speed: commands.append(f"CMD:SPEED={speed}")

    brightness = detect_brightness_intensity(text)
    if brightness: commands.append(f"CMD:BRIGHTNESS={brightness}")

    for lcd_word in LCD_PHRASES:
        if lcd_word in text:
            lcd_match = re.search(rf'{lcd_word}\s(.+)', text)
            if lcd_match:
                lcd_text = lcd_match.group(1).strip()
                commands.append(f"CMD:LCD={lcd_text}")
            break

    # ✅ INTENT GUESSING (if no clear CMD found)
    if not commands:
        from phrases import INTENT_WORD_MAP
        for word, action in INTENT_WORD_MAP.items():
            if word in text:
                if "mood" in action:
                    commands.append(f"CMD:MOOD={action['mood']}")
                    print(f"🎭 Billu guesses mood: {action['mood']}")
                elif "effect" in action:
                    commands.append(f"CMD:EFFECT={action['effect']}")
                    print(f"🎉 Billu sets effect: {action['effect']}")
                elif "rain" in action:
                    commands.append(f"CMD:RAIN={action['rain']}")
                    print(f"🌧 Billu sets rain mode: {action['rain']}")
                break


    if not commands:
        commands = fallback_parser(user_text)

    commands = list(dict.fromkeys(commands))
    return commands



# ==== SYNONYMS =======

def regex_match_command(text):
    """
    Detect structured commands like 'turn on led', 'set color to red', etc.
    Returns (action, target, value)
    """
    patterns = [
        r"(turn on|switch on|start|activate) (?P<device>led|fan|light)",
        r"(turn off|switch off|stop|kill) (?P<device>led|fan|light)",
        r"set (the )?color to (?P<color>[a-z\s]+)",
        r"make it (?P<color>[a-z\s]+)",
        r"set mood to (?P<mood>[a-z\s]+)",
        r"i feel (?P<mood>[a-z\s]+)"
    ]

    for pat in patterns:
        match = re.search(pat, text)
        if match:
            return match.groupdict()
    return None