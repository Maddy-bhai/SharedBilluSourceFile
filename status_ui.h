//===== status_ui.h =======

#pragma once
#include "Billu_RoboEyes_EmoPack.h"
#include "lcd_compat.h"

// ============ CONFIG ============
#ifndef STATUS_USE_OVERLAY
#define STATUS_USE_OVERLAY 1   // 1 = centered overlay; 0 = print via lcd_compat area
#endif
#ifndef STATUS_TEXT_SIZE
#define STATUS_TEXT_SIZE 2    // 1=small, 2=large (you can try 3 if you want jumbo)
#endif
#ifndef STATUS_DEFAULT_MS
#define STATUS_DEFAULT_MS 3500   // default show time (ms)
#endif

// ============ CORE HELPERS ============
inline String toAscii(const String& s){
  String out; out.reserve(s.length());
  for (unsigned int i=0;i<s.length();++i){
    unsigned char c = s[i];
    if (c>=32 && c<127) out += (char)c;  // strip non-ASCII (emoji/arrows etc.)
  }
  return out;
}

inline void statusShow(const String& s, uint16_t ms=STATUS_DEFAULT_MS){
  if (ms < STATUS_DEFAULT_MS) ms = STATUS_DEFAULT_MS;
  String msg = toAscii(s);  // keep ASCII safe
#if STATUS_USE_OVERLAY
  Eyes_flash(msg, ms, STATUS_TEXT_SIZE); // pass desired size to overlay
#else
  lcd.clear(); lcd.setCursor(0,0); lcd.print(msg); lcd.flush();
#endif
}

inline String titleWord(String s){ s.toLowerCase(); if (s.length()) s.setCharAt(0, toupper(s[0])); return s; }
template<size_t LEN_> inline const char* pick(const char* const (&arr)[LEN_]){ return arr[random((long)LEN_)]; }
inline bool chance(int oneIn){ return random(oneIn)==0; }
inline String addSuffix(const String& base, const char* const* pool, int n){
  if (!n || chance(3)) return base; // ~33% keep clean
  return base + " " + String(pool[random(n)]);
}

// ============ PHRASE BANKS (ASCII ONLY) ============
// LED power
static const char* PH_LED_ON[]  = {
  "LED ONLINE","Glow engaged","Photon cannon ON","Bright mode","Deploying lumens",
  "Beacons booted","Hello photons","Light saber ignited","Beaming","Room unlocked (light)"
};
static const char* PH_LED_OFF[] = {
  "LED OFFLINE","Lights out","Stealth mode","Darkness deployed","Power save",
  "Beacons docked","Ninja mode","Blackout","Shadows win","Cloak engaged"
};
// Fun suffixes (ASCII)
static const char* PH_SUFFIX[] = { "[OK]","GG","EZ","++","//","<3","::","!!","--" };

// Effects / Patterns / Color
static const char* PH_EFFECT_PREFIX[] = { "Effect: ","Mode: ","Vibe: ","Pattern: ","Style: " };
static const char* PH_EFFECT_SUFFIX[] = { " online"," locked"," engaged"," active" };

static const char* PH_PATTERN_PREFIX[] = { "Pattern: ","Weave: ","Layout: " };
static const char* PH_COLOR_PREFIX[]   = { "Color: ","Hue: ","Shade: ","Tint: " };
static const char* PH_PALETTE_SET[]    = { "Palette set","Palette loaded","Color deck ready" };
static const char* PH_PALETTE_UPD[]    = { "Palette updated","Palette +","Chromatic merge" };
static const char* PH_COLOR_ADD[]      = { "Added ","Injected ","Summoned " };
static const char* PH_COLOR_REM[]      = { "Removed ","Purged ","Banished " };

// Brightness / Speed
static const char* PH_BRIGHT_HI[] = { "Brightness: RETINA","Sunglasses advised","Gamma boost ++","Sun mode","Stage lights","MAX glow" };
static const char* PH_BRIGHT_LO[] = { "Brightness: stealth","Night vision","Zen dim","Candle mode","Shadow crawl" };
static const char* PH_BRIGHT_MID[] = { "Brightness tuned","Glow balanced","Ambient set","Mid-beam online" };

static const char* PH_SPEED_FAST[] = { "Speed: WARP","Overdrive","Sonic mode","Turbo","Mach-10","Nitro engaged" };
static const char* PH_SPEED_SLOW[] = { "Speed: bullet time","Slow and steady","Meditation pace","Glacier mode" };
static const char* PH_SPEED_DEF[]  = { "Speed: default","Factory rhythm","Baseline tempo","Clocked standard" };

// LED counts / range / region
static const char* PH_RANGE_PREFIX[]   = { "Range: ","Span: ","Window: " };
static const char* PH_REGION_PREFIX[]  = { "Region: ","Zone: ","Area: " };
static const char* PH_NUMLEDS_PREFIX[] = { "Active LEDs: ","Pixels armed: ","LED count: " };

// Eyes position
static const char* PH_POS_PREFIX[] = { "Eyes: ","Gaze: ","Look: " };

// Mood
static const char* PH_MOOD_PREFIX[] = { "Mood: ","Vibe: ","Feeling: " };
static const char* PH_MOOD_JOIN[]   = { " | "," - "," : " };

// Rain
static const char* PH_RAIN_LIGHT[]   = { "RAIN: light","Drizzle ops","Cloud tickle" };
static const char* PH_RAIN_MED[]     = { "RAIN: medium","Steady rain","Ponchos on" };
static const char* PH_RAIN_HEAVY[]   = { "RAIN: HEAVY","Downpour.exe","Buckets inbound" };
static const char* PH_RAIN_THUNDER[] = { "THUNDERSTORM","Storm protocol","Zeus online" };

// Relays
static const char* PH_LIGHT_ON[]  = { "Light ON","Room light online","Ceiling sun ignited","Lamp deployed","Photon lamp ++" };
static const char* PH_LIGHT_OFF[] = { "Light OFF","Room light offline","Ceiling sun docked","Lamp stowed" };
static const char* PH_FAN_ON[]    = { "Fan ON","Breeze online","Cooling engaged","Wind turbine spin","Whoosh ++" };
static const char* PH_FAN_OFF[]   = { "Fan OFF","Breeze offline","Cooling paused","Wind turbine rest" };

// Stop/Continue + Errors
static const char* PH_RESUMED[]       = { "Resume","Back online","Continue","Rolling","We are live" };
static const char* PH_STOPPED[]       = { "Stopped","Paused","Standby","Halt","Freeze" };
static const char* PH_ERROR_GENERIC[] = { "Error","Oops","Invalid","Not understood","Try again" };

// ============ ASCII TAILS ============
inline String effectTail(String e){ // small ASCII flair per effect
  e.toLowerCase();
  if (e=="rainbow") return " [RGB]";
  if (e=="wave" || e=="center_wave" || e=="bounce_wave") return " ~~";
  if (e=="blink") return " ;)";
  if (e=="chase") return " >>";
  if (e=="pulse" || e=="heartbeat") return " <3";
  if (e=="strobe" || e=="flash") return " !!";
  if (e=="twinkle" || e=="star_rain") return " **";
  if (e=="party_flash" || e=="fireworks") return " *BOOM*";
  if (e=="fire_glow") return " [HEAT]";
  if (e=="thunder") return " [ZAP]";
  if (e=="fade_loop" || e=="soft_glow") return " ...";
  if (e=="color_comet") return " --o";
  if (e=="drizzle" || e=="rain") return " ///";
  return "";
}

inline String moodTail(String m){
  m.toLowerCase();
  if (m=="happy") return " :)";
  if (m=="sad") return " :(";
  if (m=="angry") return " >:/";
  if (m=="chaotic") return " *chaos*";
  if (m=="calm") return " zen";
  if (m=="love") return " <3";
  if (m=="energetic") return " ++";
  if (m=="relaxed") return " zzz";
  if (m=="thoughtful") return " ...";
  return " [AI]";
}

// ============ PUBLIC UI HELPERS ============
inline void showLED(bool on){
  String base = on ? pick(PH_LED_ON) : pick(PH_LED_OFF);
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1100);
}

inline void showEffect(String effectUpper){
  effectUpper.toLowerCase();
  String nice = titleWord(effectUpper);
  String base = String(pick(PH_EFFECT_PREFIX)) + nice + String(pick(PH_EFFECT_SUFFIX)) + effectTail(effectUpper);
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1200);
}

inline void showPattern(String patternUpper){
  patternUpper.toLowerCase();
  String nice = titleWord(patternUpper);
  String base = String(pick(PH_PATTERN_PREFIX)) + nice;
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1100);
}

inline void showColor(String colorUpper){
  colorUpper.toLowerCase();
  String nice = titleWord(colorUpper);
  String base = String(pick(PH_COLOR_PREFIX)) + nice;
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

inline void showRGB(int r,int g,int b){
  String base = "RGB ("+String(r)+","+String(g)+","+String(b)+")";
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

// Palettes
inline void showPaletteSet(){     statusShow(addSuffix(String(pick(PH_PALETTE_SET)), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 950); }
inline void showPaletteUpdated(){ statusShow(addSuffix(String(pick(PH_PALETTE_UPD)), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 950); }
inline void showAddedColor(String c){ String base = String(pick(PH_COLOR_ADD)) + titleWord(c); statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900); }
inline void showRemovedColor(String c){ String base = String(pick(PH_COLOR_REM)) + titleWord(c); statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900); }

// Brightness / Speed
inline void showBrightness(int pct){
  const char* msg;
  if (pct >= 85) msg = pick(PH_BRIGHT_HI);
  else if (pct <= 15) msg = pick(PH_BRIGHT_LO);
  else msg = pick(PH_BRIGHT_MID);
  String base = String(msg) + " (" + String(pct) + "%)";
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

inline void showSpeed(int v){
  const char* msg;
  if (v >= 300) msg = pick(PH_SPEED_SLOW);
  else if (v <= 30) msg = pick(PH_SPEED_FAST);
  else msg = "Speed tuned";
  String base = String(msg) + " (" + String(v) + ")";
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 950);
}
inline void showSpeedDefault(){ statusShow(addSuffix(String(pick(PH_SPEED_DEF)), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900); }

// LED counts / range
inline void showNumLeds(int n){
  String base = String(pick(PH_NUMLEDS_PREFIX)) + String(n);
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900);
}
inline void showLedIndex(int i){
  String base = "LED# " + String(i);
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900);
}
inline void showRange(int s,int e){
  String base = String(pick(PH_RANGE_PREFIX)) + String(s) + "-" + String(e);
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

// Regions
inline void showRegion(String regionLower){
  String nice = titleWord(regionLower);
  String base = String(pick(PH_REGION_PREFIX)) + nice;
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

// Eyes position
inline void showPos(String posUpper){
  posUpper.toLowerCase();
  String nice = titleWord(posUpper);
  String base = String(pick(PH_POS_PREFIX)) + nice;
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900);
}

// Mood
inline void showMood(String primaryLower, String subLower){
  String prim = titleWord(primaryLower);
  String sub  = titleWord(subLower.length()? subLower : String("Default"));
  String join = pick(PH_MOOD_JOIN);
  String base = String(pick(PH_MOOD_PREFIX)) + prim + join + sub + moodTail(primaryLower);
  statusShow(addSuffix(base, PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1100);
}

// Rain modes
inline void showRain(String modeLower){
  modeLower.toLowerCase();
  const char* base =
    (modeLower=="light")        ? pick(PH_RAIN_LIGHT) :
    (modeLower=="medium")       ? pick(PH_RAIN_MED)   :
    (modeLower=="heavy")        ? pick(PH_RAIN_HEAVY) :
    (modeLower=="thunderstorm") ? pick(PH_RAIN_THUNDER) :
                                  pick(PH_ERROR_GENERIC);
  statusShow(addSuffix(String(base), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

// Relays
inline void showRelay(const String& deviceLower, const String& stateLower){
  String d = deviceLower; String s = stateLower;
  d.toLowerCase(); s.toLowerCase();
  const char* base = "Relay";
  if (d=="light") base = (s=="on") ? pick(PH_LIGHT_ON) : pick(PH_LIGHT_OFF);
  else if (d=="fan") base = (s=="on") ? pick(PH_FAN_ON) : pick(PH_FAN_OFF);
  else { statusShow("Unknown device", 900); return; }
  statusShow(addSuffix(String(base), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 1000);
}

// Stop / Continue / Errors
inline void showStop(){     statusShow(addSuffix(String(pick(PH_STOPPED)), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 800); }
inline void showContinue(){ statusShow(addSuffix(String(pick(PH_RESUMED)), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900); }
inline void showErrorGeneric(){ statusShow(addSuffix(String(pick(PH_ERROR_GENERIC)), PH_SUFFIX, sizeof(PH_SUFFIX)/sizeof(PH_SUFFIX[0])), 900); }
