#pragma once

extern uint8_t multiColors[10][3];
extern int multiColorCount;

void renderMultiColorsBlock();
void refreshCurrentPattern();
// Scroll State (so colors.h can access it)
extern bool scrollMode;
extern bool scrollBaseCaptured;
void captureScrollBase();
// Add this
extern bool shimmerActive;

extern EffectType lastEffect;
extern String lastBasePattern;
