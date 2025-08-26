import serial
import time
import json
import requests
import re  # for number extraction
import random


# Serial port setup
esp = serial.Serial('COM3', 115200, timeout=1)
time.sleep(2)

LAST_RANDOM_COLOR = None


# Common phrases fallback (hardcoded safety net)
# === LED STRIP COMMANDS ===

LED_ON_PHRASES = [
    "turn on led", "switch on led", "led on", "activate led", "led start",
    "power on led", "led strip on", "start glowing", "wake up the led", "led should glow",
    "fire up the led", "glow the strip", "initiate light", "turn everything bright",
    "kickstart led", "led on bro", "billu start the light", "billu light on",
    "light led up", "billu glow on", "strip glow", "start the strip",
    "let's turn on the strip", "come on, glow", "light it already",
    "start up the glow", "beam the light", "glow please", "make it lit"
]

LED_OFF_PHRASES = [
    "turn off led", "switch off led", "led off", "stop led", "cut the led",
    "shutdown leds", "power down led", "stop glowing", "kill led", "disable led",
    "stop the strip", "strip off", "mute the led", "turn it off",
    "led off bro", "billu stop light", "shut the strip", "off the led",
    "shutdown effect", "cancel the glow", "stop glowing now please",
    "wrap it up", "turn off the led strip", "stop the brightness", "no more glow"
]


# === BULB/LIGHT RELAY COMMANDS ===

LIGHT_ON_PHRASES = [
    "turn on light", "switch on light", "light on", "put the light",
    "bring light", "enable the light", "start the lights",
    "light it up", "light up this place", "give me light",
    "make the room glow", "illuminate this room", "light the room",
    "start light", "light up bro", "wake up the light", "light please"
]

LIGHT_OFF_PHRASES = [
    "turn off light", "switch off light", "light off", "cut the light",
    "shut the light", "kill the light", "disable light",
    "billu light off", "put the light to rest", "lights out",
    "turn off all lights", "turn off the bulb", "shut the bulb",
    "darken the room", "cut the glow", "stop showing light"
]


# === FAN RELAY COMMANDS ===

FAN_ON_PHRASES = [
    "turn on fan", "fan on", "switch on fan", "start fan",
    "activate fan", "power up fan", "enable fan", "billu fan on",
    "fan please", "turn the fan on", "breeze on", "start the wind"
]

FAN_OFF_PHRASES = [
    "turn off fan", "fan off", "switch off fan", "stop fan",
    "kill fan", "power down fan", "billu fan off", "shut the fan",
    "fan stop", "disable fan", "turn the fan off"
]



LCD_PHRASES = {
    "show welcome message": "Welcome to Billu",
    "greet me": "Hello, User!",
    "how are you": "I'm always lit 🔥",
    "who are you": "I'm Billu, your LED buddy",
    "introduce yourself": "Billu here – lighting your life 😎",
    "status": "All systems online ✅",
    "are you online": "Yes boss, fully alive ⚡",
    "say something funny": "Why did LED go to school? To get brighter! 😄",
    "motivate me": "Keep going! Your future is bright 💡",
    "say hi": "Hi there 👋",
    "good morning": "Rise and shine ☀️",
    "good night": "Dream in RGB 🌙",
    "what time is it": "I'm timeless 😜",
    "are you ready": "Born ready 💪",
    "i love you": "I love you too, in binary ❤️",
    "thanks": "Always here for you 🤖",
    "are you real": "Real as light itself ✨",
    "what can you do": "Light, react, dazzle, obey 😎",
    "make me smile": "😊 <3 You light up my code!",
    "are you smart": "Smarter than your average strip 😏",
    "dance with me": "Let's vibe with some LEDs 💃"
}



# ✅ Full list matching Arduino's parseColor()
SUPPORTED_COLORS = [
    # Primary Colors
    "red", "green", "blue", "yellow", "orange", "pink", "purple", "violet", "cyan", "magenta",

    # Whites & Grays
    "warm white", "cool white", "soft white", "dim white", "gray", "dull gray",

    # Special Shades
    "gold", "amber", "peach", "coral", "rose", "lavender", "mint", "moonlight", 
    "sunset", "ocean", "sky blue", "dream blue", "frost white",

    # Pastels & Pale Shades
    "pale blue", "pale purple", "pale pink", "pale green", "soft pink", "light blue", "light yellow",

    # Dim / Dull / Deep Variants
    "dim red", "dim blue", "dim green", "dim purple", "dull red", 
    "deep red", "deep purple", "deep cyan", "blood red", "blood orange", "forest green",

    # Neon Colors
    "neon pink", "neon green", "neon blue", "glow green"
]



COLOR_PATTERNS = [
    "make it",
    "turn on",
    "set color to",
    "switch to",
    "go",
    "i want",
    "activate",
    "paint it",
    "show me",
    "enable",
    "bring in",
    "display",
    "light up with",
    "illuminate in",
    "start",
    "use",
    "change to",
    "use the color",
    "can we go",
    "turn everything",
    "let's go",
    "give me",
    "wrap the strip in",
    "wash the room with",
    "load",
    "beam in",
    "send out",
    "glow in",
    "fire up",
    "make this place",
    "throw some",
    "can you go",
    "i feel like",
    "just go",
    "i want everything",
    "wrap everything in",
    "go full",
    "paint everything",
    "bathe the room in",
"throw on some",
"billu make it",
"let’s go with",
"paint the room in",
"give it a touch of",
"soak everything in",
"drench the lights in",
"fill it with",
"splash some",
"light it all in",
"can you color it",
"drip some",
"turn the lights to",
"cover the room in",
"make the vibe",
"change the vibe to",
"i want to see",
"load this color",
"surround me with",
"push the color to"

]

SUPPORTED_EFFECTS = [
    "center_wave",
    "bounce_wave",
    "party_flash",
    "fire_glow",
    "fade_loop",
    "twinkle",
    "chase",
    "pulse",
    "blink",
    "wave",
    "thunder",
    "rainbow",
    "drizzle",
    "color_comet",
    "soft_glow",
    "fireworks",
    "heartbeat",
    "star_rain",
    "flash",
    "rain"
]



EFFECT_PATTERNS = [
    "start",
    "activate",
    "run",
    "launch",
    "enable",
    "turn on",
    "play",
    "show",
    "begin",
    "trigger",
    "open",
    "set effect to",
    "switch to",
    "go with",
    "glow with",
    "begin the",
    "make it",
    "fire up",
    "use",
    "test",
    "rotate",
    "flash",
    "spark",
    "jam the",
    "blast the",
    "load",
    "bring up",
    "initiate",
    "cycle",
    "loop the",
"it",
"make it do",
"can you show",
"do a",
"run the pattern",
"billu do",
"let's activate",
"throw an effect",
"fire some effect",
"i want some effect",
"put on",
"make the strip dance",
"light show",
"animation time",
"run something fancy",
"give me a vibe",
"pulse something",
"blast an effect",
"show the magic",
"give it movement",
"can you light it animated",
"rotate some colors",
"do your thing"

]


CHANGE_COLOR_PHRASES = [
    "change the color",
    "switch the color",
    "rotate color",
    "different color",
    "another color",
    "next color",
    "cycle color",
    "give me a new color",
    "refresh the color",
    "color change",
    "change it",
    "change color",
    "change it up"
]

# === RAIN MODE PHRASES ===
RAIN_MODES = {
    "light": [
        "light rain", "a bit of rain", "drizzle", "make it drizzle",
        "soft rain", "tiny rain", "sprinkle rain"
    ],
    "medium": [
        "rain mode", "normal rain", "medium rain",
        "make it rain", "turn on rain", "rain vibe"
    ],
    "heavy": [
        "heavy rain", "storm rain", "big rain", "pouring rain",
        "hard rain", "make it heavy rain"
    ],
    "thunderstorm": [
        "thunderstorm", "lightning rain", "storm mode", 
        "rain with thunder", "crazy storm", "turn on thunderstorm"
    ]
}

# === ADVANCED MOOD DETECTION ===
MOOD_MAP = {
    "happy": ["happy", "joyful", "cheerful", "smiling", "delighted", "glad"],
    "sad": ["sad", "lonely", "depressed", "grieving", "crying", "nostalgic"],
    "anger": ["angry", "mad", "furious", "jealous", "annoyed", "frustrated"],
    "fear": ["scared", "fearful", "nervous", "anxious", "shy", "paranoid"],
    "love": ["romantic", "flirty", "intimate", "caring", "affectionate"],
    "calm": ["calm", "relaxed", "peaceful", "meditative", "balanced"],
    "excited": ["excited", "hopeful", "curious", "ecstatic"],
    "disgust": ["disgusted", "ashamed", "guilty", "embarrassed"],
    "playful": ["playful", "silly", "cheeky", "mischievous"],
    "neutral": ["neutral", "confused", "reflective", "indifferent"],
    "tired": ["tired", "sleepy", "burnt out", "sick", "drained"]
}

# Map sub-moods (secondary words) for more precision
SUB_MOOD_MAP = {
    "cheerful": ["cheerful", "joyful", "smiling"],
    "confident": ["confident", "proud"],
    "furious": ["furious", "enraged"],
    "flirty": ["flirty", "teasing"],
    "romantic": ["romantic", "in love", "lovey"],
    "relaxed": ["relaxed", "chill"],
    "nostalgic": ["nostalgic", "missing old days"],
    "guilty": ["guilty", "feeling bad"],
    "sleepy": ["sleepy", "ready to sleep"],
    "annoyed": ["annoyed", "irritated"],
    "jealous": ["jealous", "envious"],
    "nervous": ["nervous", "shaky"],
    "caring": ["caring", "sweet"],
    "frustrated": ["frustrated", "upset"]
}


def detect_advanced_mood(text: str):
    text = text.lower()
    detected_primary = None
    detected_sub = None

    # Check for primary mood
    for mood, keywords in MOOD_MAP.items():
        for word in keywords:
            if word in text:
                detected_primary = mood
                break
        if detected_primary:
            break

    # Check for sub-mood
    for sub, keywords in SUB_MOOD_MAP.items():
        for word in keywords:
            if word in text:
                detected_sub = sub
                break
        if detected_sub:
            break

    # If we found primary and sub, return both
    if detected_primary:
        return detected_primary, detected_sub if detected_sub else "default"

    return None, None


SPEED_PHRASES = [
    "set speed to",
    "change speed to",
    "speed",
    "speed at",
    "make speed",
    "speed value",
    "adjust speed to",
    "run speed",
    "effect speed",
    "set effect speed to"
]

SPEED_DEFAULT_PHRASES = [
    "reset speed",
    "default speed",
    "speed default",
    "go back to default speed",
    "normal speed",
    "original speed",
    "revert speed"
]

BRIGHTNESS_PHRASES = [
    "set brightness to",
    "brightness",
    "make it brighter",
    "dim the lights",
    "adjust brightness",
    "brightness level",
    "change brightness",
    "make it less bright",
    "increase brightness",
    "reduce brightness",
    "too bright",
    "not bright enough",
    "full brightness",
    "turn up the brightness",
    "turn down the brightness",
    "brightness at",
    "brightness value",
    "set the lights to",
"this is too bright",
"turn down the brightness please",
"make it a bit softer",
"reduce the glow",
"lower the intensity",
"less light please",
"take the brightness down",
"cut down on the glow",
"kill the brightness a bit",
"can you dim it",
"turn the lights low",
"light level down",
"ease the brightness",
"billu make it chill",
"tone down the brightness",
"turn the brightness all the way up",
"turn the brightness all the way down",
"crank up the brightness",
"bring the lights to max",
"billu, blind me",
"set soft brightness",
"burn my eyes with light"

]


NUMLEDS_PHRASES = [
    "set leds to",
    "num leds",
    "number of leds",
    "active leds",
    "change led count",
    "leds on",
    "only use",
    "show with",
    "use",
    "activate only",
    "turn on only",
    "glow only",
    "how many leds",
    "limit leds to",
    "just use",
    "restrict leds to",
"i want only",
"let only",
"only glow",
"just activate",
"use just",
"can we glow",
"power up only",
"light a few",
"light up this many",
"limit it to",
"keep only",
"illuminate only",
"make only",
"turn on these many",
"how about only",
"use this much",
"glow just",
"i want this number of leds",
"restrict to",
"just this count"

]


NUMLEDS_REGEX_PATTERNS = [
    r"i want (\d+) lights? on",
    r"make only (\d+) lights? (on|glow)?",
    r"just use (\d+) leds?",
    r"use only (\d+) leds?",
    r"show only (\d+) leds?",
    r"light up (\d+) leds?",
    r"enable (\d+) leds?",
    r"activate (\d+) lights?",
    r"turn on (\d+) leds?",
    r"glow (\d+) leds?",
    r"only (\d+) leds?",
    r"put (\d+) lights? on",
]



LEDINDEX_PHRASES = [
    "led index",
    "highlight led",
    "show led",
    "light up led",
    "turn on led number",
    "glow led",
    "flash led",
    "focus led",
    "light single led",
    "turn only led",
    "just led number",
    "glow only led",
    "show only led",
    "focus on led",
    "led point",
    "led number",
"focus light on led",
"i want led number",
"select led number",
"only this led",
"just glow led",
"point to led",
"target led",
"glow led index",
"turn only this led",
"give me led",
"highlight number",
"billu flash led",
"make led glow",
"ignite led",
"burn led number",
"spotlight led",
"beam on led",
"just show led",
"call out led",
"pick led number"

]


LEDRANGE_PHRASES = [
    "led range",
    "range",
    "set range",
    "select led range",
    "glow from",
    "light up from",
    "start from led",
    "between led",
    "use leds from",
    "run leds from",
    "from led to",
    "start at",
    "end at",
    "set segment",
    "active led range",
    "led band from",
    "segment leds",
"illuminate range",
"light between",
"from this to that",
"use lights from",
"billu light between",
"glow these leds",
"just glow this range",
"active from",
"cover leds",
"led span",
"light this portion",
"select this band",
"choose segment",
"glow a group",
"run a light zone",
"this strip section",
"highlight leds between",
"light between these",
"i want to light",
"stretch from led",
"segment between leds"

]

range_patterns = [
    r"(led range|ledrange|range)[^\d]*(\d+)[,\s]+(\d+)",
    r"(from|between)[^\d]*(\d+)[^\d]+(\d+)",
    r"(\d+)[^\d]+(to|and)[^\d]+(\d+)"
]

STOP_PHRASES = [
    "stop",
    "stop everything",
    "kill effects",
    "turn off effect",
    "stop all",
    "end animation",
    "halt",
    "cancel",
    "terminate",
    "shutdown effect",
    "Kill all animations"
]


# Intensity-based speed phrases
def detect_speed_intensity(text):
    text = text.lower()
    if "as fast as possible" in text or "max speed" in text:
        return 5
    elif "super fast" in text or "so so fast" in text or "very very fast" in text:
        return 10
    elif "very fast" in text or "really fast" in text:
        return 20
    elif "faster" in text or "more fast" in text:
        return 30
    elif "bit faster" in text or "slightly faster" in text:
        return 50
    elif "fast" in text:
        return 80
    elif "normal" in text:
        return 150
    elif "bit slow" in text or "slightly slow" in text:
        return 200
    elif "slow" in text:
        return 300
    elif "very slow" in text:
        return 400
    elif "very very slow" in text or "more slow" in text or "super slow" in text:
        return 500
    elif "dead slow" in text or "ultra slow" in text:
        return 600
    return None

def detect_brightness_intensity(text):
    text = text.lower()
    if "maximum brightness" in text or "full brightness" in text or "brightest" in text:
        return 100
    elif "very bright" in text or "so bright" in text:
        return 90
    elif "bright" in text:
        return 70
    elif "normal brightness" in text or "medium brightness" in text:
        return 50
    elif "bit dim" in text or "slightly dim" in text:
        return 30
    elif "dim" in text:
        return 20
    elif "very dim" in text or "super dim" in text:
        return 10
    elif "lowest brightness" in text or "no brightness" in text:
        return 0
    return None


def clean_input_text(raw_text):
    text = raw_text.lower().strip()

    # Remove unnecessary words that confuse the parser
    junk_words = [
        "please", "now", "some", "bit of", "a bit", "just", "kinda", "sort of",
        "can you", "will you", "i want to", "let’s", "let me", "need to",
        "give me", "make it", "do it", "billu", "can u", "wanna"
    ]
    for junk in junk_words:
        text = text.replace(junk, "")

    # Fix known typos or common phrases
    corrections = {
        "centerwave": "center_wave",
        "bouncewave": "bounce_wave",
        "fireglow": "fire_glow",
        "fade loop": "fade_loop",
        "partyflash": "party_flash",
        "fastest": "as fast as possible",
        "full bright": "full brightness",
        "too bright": "brightness 30",  # Optional override
        "super fast": "speed 10",
        "super slow": "speed 600",
        "brightness 100 percent": "brightness 100",
        "brightness 150": "brightness 100",  # clamp
        "brightness 200": "brightness 100",
        "speed minus fast": "speed 20",  # map junk to fixed
    }
    for wrong, correct in corrections.items():
        if wrong in text:
            text = text.replace(wrong, correct)

    # Remove % symbols in brightness
    text = re.sub(r'(\d+)\s*%', r'\1', text)

    # Clamp numeric values
    text = re.sub(r'brightness\s*[:=]?\s*(\d+)', lambda m: f"brightness {min(int(m.group(1)), 100)}", text)
    text = re.sub(r'speed\s*[:=]?\s*(\d+)', lambda m: f"speed {min(int(m.group(1)), 1000)}", text)

    return text


# Phrase matching fallback (multi-match version)
def match_quick_command(nl_text):
    text = nl_text.lower()
    result = {}

    # LED Strip ON/OFF
    for phrase in LED_ON_PHRASES:
        if phrase in text:
            result["led"] = "on"
            break
    for phrase in LED_OFF_PHRASES:
        if phrase in text:
            result["led"] = "off"
            break

    # Relay: LIGHT
    for phrase in LIGHT_ON_PHRASES:
        if phrase in text:
            result["relay"] = {"target": "light", "state": "on"}
            break
    for phrase in LIGHT_OFF_PHRASES:
        if phrase in text:
            result["relay"] = {"target": "light", "state": "off"}
            break

    # Relay: FAN
    for phrase in FAN_ON_PHRASES:
        if phrase in text:
            result["relay"] = {"target": "fan", "state": "on"}
            break
    for phrase in FAN_OFF_PHRASES:
        if phrase in text:
            result["relay"] = {"target": "fan", "state": "off"}
            break

    # 🌧 Rain Mode detection
    for mode, keywords in RAIN_MODES.items():
        for k in keywords:
            if k in text:
                result["rain"] = mode
                break

    for pattern in range_patterns:
        match = re.search(pattern, text)
        if match:
            numbers = [int(s) for s in match.groups() if s.isdigit()]
            if len(numbers) == 2:
                start, end = sorted(numbers)  # ✅ smart auto-correction
                if 0 <= start < end <= 299:
                    result["ledrange"] = f"{start},{end}"
                    break


    # Improved natural language color matching
    for color in SUPPORTED_COLORS:
        if color in text:
            # Look for any color intent pattern
            if any(p in text for p in COLOR_PATTERNS):
                result["color"] = color
                return result
            # If no pattern, still match if there's no effect or unrelated term
            if not any(effect in text for effect in SUPPORTED_EFFECTS):
                result["color"] = color
                return result



    # Change color without specific color
    for phrase in CHANGE_COLOR_PHRASES:
        if phrase in text:
            global LAST_RANDOM_COLOR

            available_colors = [c for c in SUPPORTED_COLORS if c not in ["white", "warm white"]]
            if LAST_RANDOM_COLOR in available_colors:
                available_colors.remove(LAST_RANDOM_COLOR)

            if not available_colors:
                available_colors = [c for c in SUPPORTED_COLORS if c not in ["white", "warm white"]]

            random_color = random.choice(available_colors)
            result["color"] = random_color
            LAST_RANDOM_COLOR = random_color

            print(f"🎲 Random color selected: {random_color}")
            break



    # Smart override for center_wave and bounce_wave
    if "center wave" in text or "wave from the center" in text or "from the middle" in text:
        result["effect"] = "center_wave"
        return result

    if "bounce wave" in text or "wave that bounces" in text or "bounce in both directions" in text:
        result["effect"] = "bounce_wave"
        return result

    # Universal EFFECT matching (Improved)
    for effect in SUPPORTED_EFFECTS:
        if effect in text:
            result["effect"] = effect
            break



    # SPEED
    # Regex first – exact number extraction
    match = re.search(r"(speed|set speed to|change speed to)[^\d]*(\d+)", text)
    if match:
       result["speed"] = int(match.group(2))
    else:
        speed_value = detect_speed_intensity(text)
        if speed_value is not None:
            result["speed"] = speed_value




    # BRIGHTNESS
    match = re.search(r"(brightness|set brightness to|brightness level|adjust brightness)[^\d]*(\d+)", text)
    if match:
        result["brightness"] = int(match.group(2))
    else:
        bright_value = detect_brightness_intensity(text)
        if bright_value is not None:
            result["brightness"] = bright_value


    # LED INDEX
    match = re.search(r"(led index|highlight led|led number)[^\d]*(\d+)", text)
    if match:
        result["ledindex"] = match.group(2)

    # LED RANGE
    match = re.search(r"(from|between|range)[^\d]*(\d+)[^\d]+(\d+)", text)
    if match:
        start, end = match.group(2), match.group(3)
        result["ledrange"] = f"{start},{end}"

        # STOP
    if any(phrase in text for phrase in STOP_PHRASES):
        result["stop"] = True

    # NUMLEDS natural language extractor
    numleds_value = extract_numleds(text)
    if numleds_value is not None:
        result["numleds"] = numleds_value

    # ✅ Final cleanup: if NUMLEDS is the only intent, keep only LED ON + NUMLEDS
    if "numleds" in result:
        # If no other keys like color, effect, brightness, lcd, speed
        if not any(k in result for k in ["effect", "color", "brightness", "speed", "lcd"]):
            result = {
                "led": "on",
                "numleds": result["numleds"]
            }

    return result if result else None



# Utility function for safe JSON extraction
def safe_json_extract(text):
    try:
        # Try normal JSON first
        match = re.search(r"\{.*\}", text, re.DOTALL)
        if match:
            return json.loads(match.group(0))
    except:
        pass

    # Fallback: attempt to clean and re-parse
    try:
        cleaned = (
            text.replace("'", '"')  # Fix single quotes
                 .replace("True", "true")
                 .replace("False", "false")
                 .replace("None", "null")
        )
        # Try regex again
        match = re.search(r"\{.*?\}", cleaned, re.DOTALL)
        if match:
            return json.loads(match.group(0))
    except:
        pass

    # Final fallback attempt: extract by key pattern
    try:
        approx = {}
        for key in ["led", "lcd", "color", "effect", "brightness", "speed", "numleds", "ledindex", "ledrange", "stop"]:
            key_match = re.search(fr'"?{key}"?\s*[:=]\s*"?([a-zA-Z0-9_, ]+)"?', text)
            if key_match:
                value = key_match.group(1).strip()
                if key in ["brightness", "speed", "numleds", "ledindex"]:
                    try:
                        approx[key] = int(value)
                    except:
                        approx[key] = value
                elif key == "stop":
                    approx[key] = True
                else:
                    approx[key] = value
        return approx if approx else None
    except:
        return None


def ask_phi2(nl_text):
    # First, try phrase-match fallback
    quick_match = match_quick_command(nl_text)
    if quick_match:
        return quick_match

    # Escape braces to avoid format crashes
    safe_text = nl_text.replace("{", "{{").replace("}", "}}")

    # 🔑 AI Prompt for Phi with Relay included
    prompt = f"""
You are a helpful assistant. Convert the following natural language command into a JSON object.

Valid keys:
- "led": "on" / "off"
- "lcd": "Any message"
- "color": "red", "blue", etc. or "random"
- "effect": "wave", "blink", "pulse", "twinkle", "fire_glow", "fade_loop", "party_flash", "center_wave", "bounce_wave", "chase", "thunder", "rainbow"
- "mood": "party" | "romantic" | "calm" | "wild" | "night"
- "brightness": 0–100
- "speed": number or "DEFAULT"
- "numleds": number
- "ledindex": number
- "ledrange": "start,end"
- "relay": {{ "target": "light" or "fan", "state": "on" or "off" }}
- "stop": true

Examples:
- "turn on the led" => {{ "led": "on" }}
- "kill the lights" => {{ "led": "off" }}
- "make it red" => {{ "color": "red" }}
- "rainbow effect" => {{ "effect": "rainbow" }}
- "set brightness to 80" => {{ "brightness": 80 }}
- "turn on led number 50" => {{ "ledindex": 50 }}
- "from 0 to 149" => {{ "ledrange": "0,149" }}
- "stop everything" => {{ "stop": true }}
- "turn on the fan" => {{ "relay": {{ "target": "fan", "state": "on" }} }}
- "light off" => {{ "relay": {{ "target": "light", "state": "off" }} }}

Now convert this:
"{safe_text}"

Only respond with valid JSON.
"""

    try:
        # 🧠 Call the AI
        response = requests.post(
            "http://localhost:11434/api/generate",
            json={"model": "phi", "prompt": prompt, "stream": False}
        )
        text = response.json().get('response', '').strip()
        info = safe_json_extract(text)
        if info:
            return info
        else:
            raise ValueError("Invalid JSON")

    except Exception as e:
        print("⚠️ JSON decode failed:", e)

        # Try cleanup attempt
        if 'text' in locals():
            try:
                cleaned = text.replace("'", '"')
                info = safe_json_extract(cleaned)
                if info:
                    print("⚠️ Cleaned AI response used:", cleaned)
                    return info
            except:
                pass

        print("❌ AI gave invalid JSON. Using fallback.")
        print("🧠 AI response was:", text if 'text' in locals() else "[None]")
        return match_quick_command(nl_text)



# Send command to ESP32
def send_command(cmd):
    esp.write((cmd + '\n').encode())
    time.sleep(0.03)
    print(f"➡️ Sent: {cmd}")
    while esp.in_waiting:
        print("🔁 ESP32 says:", esp.readline().decode().strip())

def scroll_lcd_message(msg, delay=0.3):
    window = 16  # Your LCD width
    padded = msg + " " * window
    for i in range(len(padded) - window + 1):
        display = padded[i:i+window]
        send_command(f"CMD:LCD={display}")
        time.sleep(delay)


def extract_numleds(text):
    # 1. Try regex phrases
    for pattern in NUMLEDS_REGEX_PATTERNS:
        match = re.search(pattern, text)
        if match:
            try:
                value = int(match.group(1))
                if 0 < value <= 300:
                    return value
            except:
                pass
    # 2. Fallback: keyword match + number extract
    for phrase in NUMLEDS_PHRASES:
        if phrase in text:
            nums = re.findall(r'\d+', text)
            if nums:
                value = int(nums[0])
                if 0 < value <= 300:
                    return value
    return None




# Parse AI result and send to ESP (with combo phrase support)
def parse_and_send_commands(nl_text):
    nl_text = clean_input_text(nl_text)
    parts = re.split(r"\b(?:and|also|then|with|in|plus|along with)\b", nl_text)

    final_info = {}

    # 🔎 Try to detect mood locally first (rule-based)
    primary, sub = detect_advanced_mood(nl_text)
    if primary:
        final_info["mood"] = f"{primary}:{sub}" if sub else primary



    # 🔁 Step 1: Combo parsing loop
    for part in parts:
        info = ask_phi2(part.strip())
        if info:
            final_info.update(info)

    # 🔁 Step 2: Fallback if combo failed
    if not final_info:
        print("🤖 Fallback AI rescue attempt...")
        fallback_info = ask_phi2(nl_text)  # Full original input
        if fallback_info:
            final_info.update(fallback_info)

    # ❌ Still nothing found
    if not final_info:
        print("❌ No recognizable command even after fallback.")
        return

    # 🔁 Step 3: Command execution (same as before)
    if "led" in final_info:
        led_cmd = final_info["led"].lower()
        if led_cmd in ["on", "off"]:
            send_command(f"CMD:LED={led_cmd.upper()}")
        else:
            print("⚠️ Invalid LED value:", led_cmd)

    if "lcd" in final_info:
        lcd_msg = final_info["lcd"]
        if len(lcd_msg) <= 16:
            send_command(f"CMD:LCD={lcd_msg}")
        else:
            scroll_lcd_message(lcd_msg)

    if "relay" in final_info:
        try:
            relay = final_info["relay"]
            target = relay.get("target")
            state = relay.get("state")
            if target in ["light", "fan"] and state in ["on", "off"]:
                send_command(f"CMD:RELAYSWITCH={target}={state}")
                send_command(f"CMD:LCD={target.title()} → {state.upper()}")
            else:
                print("⚠️ Invalid relay command:", relay)
        except Exception as e:
            print("⚠️ Error in relay parsing:", e)

    if "color" in final_info:
        color = final_info["color"].lower()
        if color in SUPPORTED_COLORS:
            send_command(f"CMD:COLOR={color}")
            send_command(f"CMD:LCD=Color: {color.title()}")
        else:
            print("⚠️ Unsupported color:", color)

    if "effect" in final_info:
        effect = final_info["effect"].lower()
        if effect in SUPPORTED_EFFECTS:
            send_command("CMD:STOP")
            send_command("CMD:LED=OFF")
            time.sleep(0.05)
            send_command(f"CMD:EFFECT={effect}")
            send_command(f"CMD:LCD=Effect: {effect.replace('_',' ').title()}")
        else:
            print("⚠️ Unsupported effect:", effect)

    if "rain" in final_info:
        mode = final_info["rain"].lower()
        if mode in ["light", "medium", "heavy", "thunderstorm"]:
            send_command(f"CMD:RAIN={mode}")
            send_command(f"CMD:LCD=Rain: {mode.title()}")


    # 🔥 If we got a mood, just tell ESP32 to handle the preset
    if "mood" in final_info:
        mood = final_info["mood"].lower()
        send_command(f"CMD:MOOD={mood}")
        # If you want mood to override everything else, uncomment this:
        # return


    if "ledrange" in final_info:
        try:
            start, end = map(int, final_info["ledrange"].split(","))
            if 0 <= start < end <= 299:
                send_command(f"CMD:LEDRANGE={final_info['ledrange']}")
                send_command(f"CMD:LCD=LED Range: {final_info['ledrange']}")
            else:
                print("⚠️ Invalid LED range:", final_info["ledrange"])
        except:
            print("⚠️ Invalid LED range format.")

    if "speed" in final_info:
        speed = final_info["speed"]
        if speed == "DEFAULT":
            send_command("CMD:SPEED=DEFAULT")
            send_command("CMD:LCD=Speed: DEFAULT")
        elif speed == "FAST":
            send_command("CMD:SPEED=10")
            send_command("CMD:LCD=Speed: FAST")
        elif speed == "SLOW":
            send_command("CMD:SPEED=300")
            send_command("CMD:LCD=Speed: SLOW")
        else:
            try:
                speed = int(speed)
                if 1 <= speed <= 1000:
                    send_command(f"CMD:SPEED={speed}")
                    send_command(f"CMD:LCD=Speed: {speed}")
                else:
                    print("⚠️ Speed must be 1–1000.")
            except:
                print("⚠️ Invalid speed value:", speed)

    if "brightness" in final_info:
        try:
            brightness = int(final_info["brightness"])
            if 0 <= brightness <= 100:
                send_command(f"CMD:BRIGHTNESS={brightness}")
                send_command(f"CMD:LCD=Brightness: {brightness}%")
            else:
                print("⚠️ Brightness must be 0–100.")
        except:
            print("⚠️ Invalid brightness value.")

    if "numleds" in final_info:
        try:
            num = int(final_info["numleds"])
            if 1 <= num <= 300:
                send_command(f"CMD:NUMLEDS={num}")
                send_command(f"CMD:LCD=LEDs Active: {num}")
            else:
                print("⚠️ numleds must be between 1 and 300.")
        except:
            print("⚠️ Invalid numleds value.")

    if "ledindex" in final_info:
        try:
            index = int(final_info["ledindex"])
            if 0 <= index <= 299:
                send_command(f"CMD:LEDINDEX={index}")
                send_command(f"CMD:LCD=LED #{index} is ON")
            else:
                print("⚠️ LED index must be 0–299.")
        except:
            print("⚠️ Invalid LED index.")

    if "stop" in final_info:
        send_command("CMD:STOP")
        send_command("CMD:LCD=All effects stopped")



# 🧠 MAIN LOOP
print("🧠 Ready! Talk to Billu in plain English.")
while True:
    try:
        nl = input("🗣️  You: ")
        if nl.strip().lower() in ["exit", "quit"]:
            break
        parse_and_send_commands(nl)
    except KeyboardInterrupt:
        break
