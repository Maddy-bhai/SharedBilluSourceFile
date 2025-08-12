#pragma once
// =============================================================
// Billu_RoboEyes_EmoPack — header module for integration
// Turns your standalone code into 3 functions:
//   Eyes_init(); Eyes_update(); Eyes_tryHandleCommand(cmd)
// =============================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>    // For SH1106. If you have SSD1306, tell me.
#include <FluxGarage_RoboEyes.h>

// ===== I2C pins & OLED =====
#ifndef I2C_SDA
#define I2C_SDA   21
#endif
#ifndef I2C_SCL
#define I2C_SCL   22
#endif
#ifndef I2C_ADDR
#define I2C_ADDR  0x3C
#endif

// MUST be named exactly 'display' (RoboEyes uses it internally)
extern Adafruit_SH1106G display;
static roboEyes eyes;

// =============================================================
// EmoAnimator — lightweight overlay animations (macro-safe)
// =============================================================
class EmoAnimator {
public:
  enum class Type { E_NONE=0, E_BLINK, E_HAPPY, E_SAD, E_ANGRY, E_LOVE, E_SURPRISED, E_TIRED, E_CONFUSED, E_LAUGH, E_WINK_L, E_WINK_R };

  explicit EmoAnimator(Adafruit_SH110X &d) : oled(d) {}

  void play(Type t, uint16_t ms=900){ mode=t; start=millis(); duration=msFor(t,ms); msgUntil=0; }
  void blink(){ play(Type::E_BLINK, 140); }
  void winkLeft(){ play(Type::E_WINK_L, 160); }
  void winkRight(){ play(Type::E_WINK_R, 160); }

  // NEW: message popup that keeps the previously chosen text size
  void flash(const String& s, uint16_t ms=2000){
    message = s;
    msgUntil = millis() + ms;
    // msgTextSize stays as-is
  }

  // Existing: message popup with explicit text size
  void flash(const String& s, uint16_t ms, uint8_t size){
    message = s;
    msgUntil = millis() + ms;
    msgTextSize = size;   // remember desired size
  }

  // Busy if an animation is running or a flash message is on screen
  bool isBusy() const { return mode!=Type::E_NONE || (msgUntil && (int32_t)(millis()-msgUntil)<0); }

  // default bigger font (1=small, 2=large)
  uint8_t msgTextSize = 2;

  void update(){
    uint32_t now = millis();
    bool showMsg = (msgUntil && (int32_t)(now - msgUntil) < 0);

    // finish animation when expired
    if (mode!=Type::E_NONE && (int32_t)(now - (start+duration)) >= 0) mode = Type::E_NONE;

    if (!isBusy()) return;

    oled.clearDisplay();

    Eye L{40, 34, 22, 12, 0, 0};
    Eye R{88, 34, 22, 12, 0, 0};

    switch(mode){
      case Type::E_BLINK:      drawBlink(now,L,R); break;
      case Type::E_HAPPY:      drawHappy(now,L,R); break;
      case Type::E_SAD:        drawSad(now,L,R); break;
      case Type::E_ANGRY:      drawAngry(now,L,R); break;
      case Type::E_LOVE:       drawLove(now,L,R); break;
      case Type::E_SURPRISED:  drawSurprised(now,L,R); break;
      case Type::E_TIRED:      drawTired(now,L,R); break;
      case Type::E_CONFUSED:   drawConfused(now,L,R); break;
      case Type::E_LAUGH:      drawLaugh(now,L,R); break;
      case Type::E_WINK_L:     drawWink(now,true,L,R); break;
      case Type::E_WINK_R:     drawWink(now,false,L,R); break;
      case Type::E_NONE:       break;
    }

    if (showMsg) drawWrappedCenteredText(message, msgTextSize);

    oled.display();
  }

private:
  Adafruit_SH110X &oled;
  uint32_t start=0, duration=0, msgUntil=0;
  String message; 
  Type mode=Type::E_NONE;
  struct Eye{ int16_t cx,cy,rx,ry, px,py; };

  static int16_t sin16(uint32_t now, uint16_t period, int16_t amp){
    float t = (now % period) / (float)period; return (int16_t)(sin(t*2*PI)*amp);
  }
void centerText(const String& s){
  oled.setFont(NULL);           // use classic 5x7 font (safe on all versions)
  oled.setTextSize(1);
  oled.setTextColor(SH110X_WHITE);
  oled.setTextWrap(false);

  int16_t x1, y1; uint16_t w, h;
  oled.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (128 - (int)w)/2 - x1;   // account for baseline offsets
  int16_t y = (64  - (int)h)/2 - y1;

  oled.setCursor(x, y);
  oled.print(s);
}
  void outlineEye(const Eye& e){ ellipse(e.cx,e.cy,e.rx,e.ry); }
  void pupil(const Eye& e){ oled.fillCircle(e.cx+e.px, e.cy+e.py, 4, SH110X_WHITE); }
  void lidClosed(const Eye& e){ oled.drawLine(e.cx-e.rx, e.cy, e.cx+e.rx, e.cy, SH110X_WHITE); }
  void ellipse(int16_t x0,int16_t y0,int16_t a,int16_t b){
    long a2=a*a, b2=b*b; long x=0,y=b; long dx=0, dy=2*a2*y; long err=b2 - a2*b + a2/4;
    while (dx<dy){ plot4(x0,y0,x,y); x++; dx+=2*b2; if(err>=0){ y--; dy-=2*a2; err-=dy; } err+=b2+dx; }
    err = b2*(x+0.5)*(x+0.5) + a2*(y-1)*(y-1) - a2*b2;
    while (y>=0){ plot4(x0,y0,x,y); y--; dy-=2*a2; if(err<=0){ x++; dx+=2*b2; err+=dx; } err+=a2-dy; }
  }
  void plot4(int16_t x0,int16_t y0,int16_t x,int16_t y){
    oled.drawPixel(x0+x,y0+y,SH110X_WHITE);
    oled.drawPixel(x0-x,y0+y,SH110X_WHITE);
    oled.drawPixel(x0+x,y0-y,SH110X_WHITE);
    oled.drawPixel(x0-x,y0-y,SH110X_WHITE);
  }
  void drawBlink(uint32_t now, Eye& L, Eye& R){
    uint32_t t = now - start; uint32_t h = duration/2;
    bool closed = (t>h/2 && t< h + h/2);
    if (!closed){ outlineEye(L); outlineEye(R); pupil(L); pupil(R); }
    else { lidClosed(L); lidClosed(R); }
  }
  void drawHappy(uint32_t now, Eye& L, Eye& R){
    int16_t bob = sin16(now, 1600, 3); L.cy+=bob; R.cy+=bob;
    outlineEye(L); outlineEye(R); pupil(L); pupil(R);
    smile(64, 52, 18); twinkle(now);
  }
  void drawSad(uint32_t now, Eye& L, Eye& R){
    L.ry=R.ry=10; outlineEye(L); outlineEye(R); pupil(L); pupil(R);
    sadMouth(64, 52, 16); tear(36,45, now); tear(84,45, now+300);
  }
  
void drawWrappedCenteredText(const String& s, uint8_t size){
  // Classic 5x7 font metrics (approx) scaled by size
  const int charW = 6 * size;
  const int lineH = 8 * size;
  const int maxW  = 128;
  const int maxCols = max(1, maxW / charW);
  const int lineSpacing = 2;

  // --- tokenize into words (space/newline) using fixed arrays ---
  const int MAX_WORDS = 64;
  String words[MAX_WORDS];
  int wcount = 0;
  String cur = "";

  for (uint16_t i = 0; i < s.length(); ++i){
    char c = s[i];
    if (c==' ' || c=='\n' || c=='\r' || c=='\t'){
      if (cur.length() && wcount < MAX_WORDS) { words[wcount++] = cur; cur = ""; }
      if (c=='\n' && wcount < MAX_WORDS)      { words[wcount++] = "\n"; }
    } else {
      cur += c;
    }
  }
  if (cur.length() && wcount < MAX_WORDS) words[wcount++] = cur;

  // --- wrap into lines ---
  const int MAX_LINES = 6;  // 64px tall screen; at size=2 you’ll usually get <= 4 lines
  String lines[MAX_LINES];
  int lineCount = 0;
  String line = "";

  auto flushLineNoLambda = [&](void){
    if (lineCount < MAX_LINES) lines[lineCount++] = line;
    line = "";
  };

  for (int i = 0; i < wcount; ++i){
    String w = words[i];
    if (w == "\n") { flushLineNoLambda(); continue; }

    // hard-split a very long word
    while ((int)w.length() > maxCols){
      String head = w.substring(0, maxCols);
      w = w.substring(maxCols);
      if (line.length() == 0) { line = head; flushLineNoLambda(); }
      else { flushLineNoLambda(); line = head; flushLineNoLambda(); }
    }

    String probe = (line.length()==0) ? w : (line + " " + w);
    if ((int)probe.length() <= maxCols) {
      line = probe;
    } else {
      flushLineNoLambda();
      line = w;
    }
  }
  if (line.length()) flushLineNoLambda();

  // --- draw centered block ---
  oled.setFont(NULL);
  oled.setTextSize(size);
  oled.setTextColor(SH110X_WHITE);

  int blockH = lineCount * lineH + (lineCount ? (lineCount-1)*lineSpacing : 0);
  int y = max(0, (64 - blockH)/2);

  for (int i = 0; i < lineCount; ++i){
    int linePixels = lines[i].length() * charW;
    int x = max(0, (128 - linePixels)/2);
    oled.setCursor(x, y);
    oled.print(lines[i]);
    y += lineH + lineSpacing;
  }
}

  void drawAngry(uint32_t now, Eye& L, Eye& R){ outlineEye(L); outlineEye(R); pupil(L); pupil(R); brows(); }
  void drawLove(uint32_t now, Eye& L, Eye& R){ outlineEye(L); outlineEye(R); pupil(L); pupil(R); heart(30,14,5); heart(98,14,5); }
  void drawSurprised(uint32_t now, Eye& L, Eye& R){ L.rx=R.rx=14; L.ry=R.ry=14; outlineEye(L); outlineEye(R); oled.drawCircle(64,52,10,SH110X_WHITE); }
  void drawTired(uint32_t now, Eye& L, Eye& R){ L.ry=R.ry=8; outlineEye(L); outlineEye(R); pupil(L); pupil(R); zzz(now); }
  void drawConfused(uint32_t now, Eye& L, Eye& R){ outlineEye(L); outlineEye(R); pupil(L); pupil(R); squiggle(64,54, now); }
  void drawLaugh(uint32_t now, Eye& L, Eye& R){ int16_t bob = sin16(now, 300, 2); L.cy+=bob; R.cy+=bob; outlineEye(L); outlineEye(R); openMouth(64, 48, 18); }
  void drawWink(uint32_t now, bool left, Eye& L, Eye& R){ outlineEye(L); outlineEye(R); pupil(L); pupil(R); if (left) lidClosed(L); else lidClosed(R); }
  void smile(int16_t cx,int16_t cy,int16_t r){ for(int x=-r;x<=r;x++){ int y=(int)(sqrt(max(0,r*r-x*x))*0.5); oled.drawPixel(cx+x, cy+y, SH110X_WHITE);} }
  void sadMouth(int16_t cx,int16_t cy,int16_t r){ for(int x=-r;x<=r;x++){ int y=(int)(-sqrt(max(0,r*r-x*x))*0.5); oled.drawPixel(cx+x, cy+y, SH110X_WHITE);} }
  void openMouth(int16_t cx,int16_t cy,int16_t r){ oled.drawCircle(cx,cy,r,SH110X_WHITE); oled.drawLine(cx-r,cy, cx+r,cy, SH110X_WHITE);}  
  void brows(){ oled.drawLine(22,18,56,10,SH110X_WHITE); oled.drawLine(70,10,106,18,SH110X_WHITE);}  
  void twinkle(uint32_t now){ int t=(now/120)%3; star(16,12,t==0); star(64,8,t==1); star(112,14,t==2);} 
  void star(int x,int y,bool on){ if(!on) return; oled.drawPixel(x,y,1); oled.drawPixel(x-1,y,1); oled.drawPixel(x+1,y,1); oled.drawPixel(x,y-1,1); oled.drawPixel(x,y+1,1);}  
  void heart(int x,int y,int r){ oled.drawCircle(x-r,y,r,1); oled.drawCircle(x+r,y,r,1); oled.drawLine(x-(2*r),y, x,y+(2*r),1); oled.drawLine(x+(2*r),y, x,y+(2*r),1);}  
  void zzz(uint32_t now){ int ph=(now/350)%3; int x=100,y=10; if(ph>=0) Z(x,y); if(ph>=1) Z(x-12,y-8); if(ph>=2) Z(x-24,y-16);} 
  void Z(int x,int y){ oled.drawLine(x,y,x+6,y,1); oled.drawLine(x+6,y,x,y+6,1); oled.drawLine(x,y+6,x+6,y+6,1);} 
  void squiggle(int x,int y,uint32_t now){ for(int i=0;i<12;i++){ int xx=x-22+i*4; int yy=y + (int)(sin((now/120.0)+(i*0.6))*2); oled.drawPixel(xx,yy,1);} }
  void tear(int x,int y,uint32_t now){ if(((now/200)%4)<2){ oled.drawLine(x,y, x-1,y+3,1); oled.drawPixel(x-2,y+5,1);} }
  static uint16_t msFor(Type t, uint16_t fallback){
    switch(t){
      case Type::E_BLINK: return 140; case Type::E_WINK_L: case Type::E_WINK_R: return 220;
      case Type::E_HAPPY: case Type::E_LOVE: case Type::E_LAUGH: return 1200; 
      case Type::E_SAD: return 1400; case Type::E_ANGRY: return 900; case Type::E_SURPRISED: return 900; 
      case Type::E_TIRED: return 1500; case Type::E_CONFUSED: return 1200; default: return fallback;
    }
  }
};

static EmoAnimator animator(display);

// ===== Small helpers used by command parser =====
static String upperTrim(const String& s) { String t=s; t.trim(); t.toUpperCase(); return t; }
static bool isOn(const String& v)  { return v=="ON" || v=="1" || v=="TRUE"; }
static bool isOff(const String& v) { return v=="OFF"|| v=="0" || v=="FALSE"; }

// =============================================================
// Public API — call these from your main sketch
// =============================================================
inline void Eyes_init(){
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(I2C_ADDR, false)) {
    // OLED init failed — keep going silently, or block if you prefer
    // while (1) { delay(1000); }
  }
  display.clearDisplay();
  display.display();

  eyes.begin(128, 64, 100);
  eyes.setPosition(DEFAULT);
  eyes.setWidth(40, 40);
  eyes.setHeight(26, 26);
  eyes.setBorderradius(10, 10);
  eyes.setSpacebetween(10);
  eyes.setCuriosity(ON);
  eyes.setAutoblinker(ON, 3, 2);
  eyes.setIdleMode(ON, 3, 1);
}

inline void Eyes_update(){
  if (animator.isBusy()) animator.update();
  else eyes.update();
}

// size-agnostic (keeps last size)
inline void Eyes_flash(const String& s, uint16_t ms=1000){
  animator.flash(s, ms);
}

// explicit size (used by status_ui.h)
inline void Eyes_flash(const String& s, uint16_t ms, uint8_t size){
  animator.flash(s, ms, size);
}


// Returns true if this module handled the command (so your main handler can 'return')
inline bool Eyes_tryHandleCommand(String raw){
  raw.trim(); if (!raw.length()) return false;

  // Accept both "ANIM=..." and "CMD:ANIM=..."
  if (raw.startsWith("CMD:")) raw = raw.substring(4);
  int eq = raw.indexOf('=');
  String key = upperTrim(eq >= 0 ? raw.substring(0, eq) : raw);
  String val = upperTrim(eq >= 0 ? raw.substring(eq + 1) : "");

  // Single word
  if (eq < 0) {
    if (key == "BLINK") { eyes.blink(); Serial.println("OK BLINK"); return true; }
    if (key == "CONFUSED" || key == "ANIM_CONFUSED") { eyes.anim_confused(); Serial.println("OK ANIM CONFUSED"); return true; }
    if (key == "LAUGH" || key == "ANIM_LAUGH") { eyes.anim_laugh(); Serial.println("OK ANIM LAUGH"); return true; }
    return false;
  }

  // MOOD
  if (key == "MOOD") {
    if (val == "DEFAULT")      eyes.setMood(DEFAULT);
    else if (val == "HAPPY")   eyes.setMood(HAPPY);
    else if (val == "ANGRY")   eyes.setMood(ANGRY);
    else if (val == "TIRED")   eyes.setMood(TIRED);
    else { Serial.println("ERR Unknown MOOD"); return true; }
    Serial.println("OK MOOD"); return true;
  }

  // ANIM (RoboEyes + Emo overlay)
  if (key == "ANIM") {
    if (val == "LAUGH") { eyes.anim_laugh(); Serial.println("OK ANIM"); return true; }
    if (val == "CONFUSED") { eyes.anim_confused(); Serial.println("OK ANIM"); return true; }
    if (val == "EMO_BLINK") { animator.play(EmoAnimator::Type::E_BLINK); Serial.println("OK EMO"); return true; }
    if (val == "EMO_HAPPY") { animator.play(EmoAnimator::Type::E_HAPPY, 1200); Serial.println("OK EMO"); return true; }
    if (val == "EMO_SAD") { animator.play(EmoAnimator::Type::E_SAD, 1400); Serial.println("OK EMO"); return true; }
    if (val == "EMO_ANGRY") { animator.play(EmoAnimator::Type::E_ANGRY, 900); Serial.println("OK EMO"); return true; }
    if (val == "EMO_LOVE") { animator.play(EmoAnimator::Type::E_LOVE, 1200); Serial.println("OK EMO"); return true; }
    if (val == "EMO_SURPRISED") { animator.play(EmoAnimator::Type::E_SURPRISED, 900); Serial.println("OK EMO"); return true; }
    if (val == "EMO_TIRED") { animator.play(EmoAnimator::Type::E_TIRED, 1500); Serial.println("OK EMO"); return true; }
    if (val == "EMO_CONFUSED") { animator.play(EmoAnimator::Type::E_CONFUSED, 1200); Serial.println("OK EMO"); return true; }
    if (val == "EMO_LAUGH") { animator.play(EmoAnimator::Type::E_LAUGH, 1200); Serial.println("OK EMO"); return true; }
    if (val == "EMO_WINK_L") { animator.play(EmoAnimator::Type::E_WINK_L, 220); Serial.println("OK EMO"); return true; }
    if (val == "EMO_WINK_R") { animator.play(EmoAnimator::Type::E_WINK_R, 220); Serial.println("OK EMO"); return true; }
    Serial.println("ERR Unknown ANIM"); return true;
  }

  // IDLE
  if (key == "IDLE") {
    if (isOn(val)) { eyes.setIdleMode(ON, 3, 1); Serial.println("OK IDLE ON"); }
    else if (isOff(val)) { eyes.setIdleMode(OFF, 0, 0); Serial.println("OK IDLE OFF"); }
    else Serial.println("ERR IDLE expects ON/OFF");
    return true;
  }

  // AUTO_BLINK
  if (key == "AUTO_BLINK") {
    if (isOn(val)) { eyes.setAutoblinker(ON, 3, 2); Serial.println("OK AUTO_BLINK ON"); }
    else if (isOff(val)) { eyes.setAutoblinker(OFF, 0, 0); Serial.println("OK AUTO_BLINK OFF"); }
    else Serial.println("ERR AUTO_BLINK expects ON/OFF");
    return true;
  }

  // POSITION
  if (key == "POS" || key == "POSITION") {
    if (val == "DEFAULT" || val == "CENTER") { eyes.setPosition(DEFAULT); }
    else if (val == "LEFT")      { eyes.setPosition(W); }
    else if (val == "RIGHT")     { eyes.setPosition(E); }
    else if (val == "UP")        { eyes.setPosition(N); }
    else if (val == "DOWN")      { eyes.setPosition(S); }
    else if (val == "UPLEFT"  || val == "UL" || val == "NW") { eyes.setPosition(NW); }
    else if (val == "UPRIGHT" || val == "UR" || val == "NE") { eyes.setPosition(NE); }
    else if (val == "DOWNLEFT"|| val == "DL" || val == "SW") { eyes.setPosition(SW); }
    else if (val == "DOWNRIGHT"|| val == "DR" || val == "SE") { eyes.setPosition(SE); }
    else { Serial.println("ERR Unknown POS"); return true; }
    Serial.println("OK POS"); return true;
  }

  // FLICKERS
  if (key == "HFICKER" || key == "HFLICKER") {
    int amp = val.toInt();
    if (amp <= 0) { eyes.setHFlicker(OFF); Serial.println("OK HFLICKER OFF"); }
    else { eyes.setHFlicker(ON, amp); Serial.print("OK HFLICKER "); Serial.println(amp); }
    return true;
  }
  if (key == "VFLICKER") {
    int amp = val.toInt();
    if (amp <= 0) { eyes.setVFlicker(OFF); Serial.println("OK VFLICKER OFF"); }
    else { eyes.setVFlicker(ON, amp); Serial.print("OK VFLICKER "); Serial.println(amp); }
    return true;
  }

  // GEOMETRY
  if (key == "WIDTH")  { int w = val.toInt(); if (w<=0){ Serial.println("ERR WIDTH"); return true;} eyes.setWidth(w,w); Serial.println("OK WIDTH"); return true; }
  if (key == "HEIGHT") { int h = val.toInt(); if (h<=0){ Serial.println("ERR HEIGHT"); return true;} eyes.setHeight(h,h); Serial.println("OK HEIGHT"); return true; }
  if (key == "SPACE" || key == "GAP") { int s = val.toInt(); eyes.setSpacebetween(s); Serial.println("OK SPACE"); return true; }
  if (key == "BORDER") { int r = val.toInt(); if (r<0){ Serial.println("ERR BORDER"); return true;} eyes.setBorderradius(r,r); Serial.println("OK BORDER"); return true; }

  // FLASH MESSAGE OVERLAY
  if (key == "MSG") { animator.flash(val, 1000); Serial.println("OK MSG"); return true; }

  // Not ours
  return false;
}
