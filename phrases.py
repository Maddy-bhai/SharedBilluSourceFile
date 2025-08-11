# ============================================
# BilluAI Phrase Banks (phrases.py)
# ============================================

# 🎯 LED Strip ON/OFF
LED_ON_PHRASES = [
    "turn on led", "switch on led", "led on", "activate led",
    "start led", "power on led", "led strip on", "start glowing",
    "wake up the led", "make the led glow", "turn the strip on"
]

LED_OFF_PHRASES = [
    "turn off led", "switch off led", "led off", "stop led",
    "cut the led", "shutdown led", "power down led", "stop glowing",
    "kill led", "turn the strip off", "stop the led"
]

# 💡 ROOM LIGHT ON/OFF
LIGHT_ON_PHRASES = [
    "turn on light", "switch on light", "light on", "start light",
    "power on light", "activate light", "billu light on",
    "switch on the room light", "turn on the room light", "room light on"
]

LIGHT_OFF_PHRASES = [
    "turn off light", "switch off light", "light off", "stop light",
    "cut the light", "shutdown light", "billu light off",
    "switch off the room light", "turn off the room light", "room light off"
]

# 🌬 FAN ON/OFF
FAN_ON_PHRASES = [
    "turn on fan", "fan on", "switch on fan", "start fan",
    "activate fan", "billu fan on"
]

FAN_OFF_PHRASES = [
    "turn off fan", "fan off", "switch off fan", "stop fan",
    "kill fan", "kill the fan", "billu fan off"
]

# ⚠️ STOP / EMERGENCY STOP
STOP_PHRASES = [
    "stop everything", "stop all", "kill effects", "turn off effect",
    "end animation", "halt all", "cancel all", "terminate all"
]

# 🖥 LCD DISPLAY TRIGGERS
LCD_PHRASES = ["write", "show", "display", "say", "type", "put", "print"]

# 🌈 COLOR NAMES (keep this long list)
SUPPORTED_COLORS = [
    "red", "green", "blue", "yellow", "orange", "pink", "purple", "violet",
    "cyan", "magenta", "warm white", "cool white", "soft white", "dim white",
    "gray", "dull gray", "gold", "amber", "peach", "coral", "rose", "lavender",
    "mint", "moonlight", "sunset", "ocean", "sky blue", "dream blue",
    "frost white", "pale blue", "pale purple", "pale pink", "pale green",
    "soft pink", "light blue", "light yellow", "dim red", "dim blue",
    "dim green", "dim purple", "dull red", "deep red", "deep purple",
    "deep cyan", "blood red", "blood orange", "forest green", "neon pink",
    "neon green", "neon blue", "glow green"
]

# ✨ SUPPORTED EFFECTS
SUPPORTED_EFFECTS = [
    "center_wave", "bounce_wave", "wave", "party_flash", "fireworks", "flash",
    "twinkle", "rainbow", "fire_glow", "fade_loop", "soft_glow", "heartbeat",
    "drizzle", "color_comet", "star_rain", "chase", "pulse", "strobe",
    "thunder", "rain", "blink"
]

# 🌧 RAIN MODES
RAIN_MODES = {
    "light": ["light rain", "a bit of rain", "drizzle", "soft rain"],
    "medium": ["rain mode", "normal rain", "medium rain", "make it rain"],
    "heavy": ["heavy rain", "storm rain", "big rain", "pouring rain"],
    "thunderstorm": ["thunderstorm", "lightning rain", "storm mode"]
}

# 🎭 MOOD DETECTION
MOOD_MAP = {
    "happy": ["happy", "joyful", "cheerful", "smiling", "delighted"],
    "sad": ["sad", "lonely", "depressed", "crying", "nostalgic"],
    "angry": ["angry", "mad", "furious", "annoyed", "frustrated"],
    "fear": ["scared", "fearful", "nervous", "anxious"],
    "love": ["romantic", "flirty", "caring", "affectionate"],
    "calm": ["calm", "relaxed", "peaceful", "meditative"]
}

# 🎭 SUB-MOOD DETECTION
SUB_MOOD_LIST = ["default", "calm", "intense", "soft", "gentle", "wild",
                 "deep", "neutral", "light", "dark"]

# 🌍 REGION KEYWORDS
REGION_KEYWORDS = {
    "first half": "first_half", "second half": "last_half",
    "last half": "last_half", "full strip": "all", "full": "all",
    "entire strip": "all", "middle": "middle", "center": "middle",
    "left side": "left_quarter", "right side": "right_quarter",
    "left quarter": "left_quarter", "right quarter": "right_quarter"
}

# 🎨 PATTERN KEYWORDS
PATTERN_KEYWORDS = {
    "stripe": "stripe", "stripes": "stripe", "gradient": "gradient",
    "split": "split", "split mode": "split"
}

# 🎯 INTENT WORD MAP – vague word triggers
INTENT_WORD_MAP = {
    "vibes": {"mood": "playful:silly"},
    "romantic": {"mood": "love:romantic"},
    "party": {"effect": "party_flash"},
    "stormy": {"rain": "thunderstorm"},
    "relax": {"mood": "calm:relaxed"},
    "sad": {"mood": "sad:lonely"},
    "angry": {"mood": "anger:furious"},
    "rain": {"rain": "medium"},
    "rainy": {"rain": "medium"},
    "rainstorm": {"rain": "thunderstorm"}
}


#=== SYNONYMS =====

LED_ON_SYNONYMS = ["turn on led", "switch on led", "led on", "start led", "light up", "start glowing", "power it on", "wake up led"]
LED_OFF_SYNONYMS = ["turn off led", "switch off led", "led off", "stop led", "kill led", "shutdown led", "turn the led off"]

FAN_ON_SYNONYMS = ["turn on fan", "switch on fan", "start fan", "fan on", "activate fan"]
FAN_OFF_SYNONYMS = ["turn off fan", "switch off fan", "stop fan", "kill fan", "fan off"]

LIGHT_ON_SYNONYMS = ["turn on light", "switch on light", "light on", "start light", "activate light", "power up light"]
LIGHT_OFF_SYNONYMS = ["turn off light", "switch off light", "light off", "stop light", "power down light", "cut the light"]
