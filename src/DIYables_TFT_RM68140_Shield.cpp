#include "DIYables_TFT_RM68140_Shield.h"

// All constructors simply forward to the base.  Touch state lives in the base.
DIYables_TFT_RM68140_Shield::DIYables_TFT_RM68140_Shield()
  : DIYables_TFT_Touch_Shield_Base() {}

DIYables_TFT_RM68140_Shield::DIYables_TFT_RM68140_Shield(
    uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : DIYables_TFT_Touch_Shield_Base(ts_xp, ts_yp, ts_xm, ts_ym) {}

DIYables_TFT_RM68140_Shield::DIYables_TFT_RM68140_Shield(
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
    uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
    uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : DIYables_TFT_Touch_Shield_Base(d0, d1, d2, d3, d4, d5, d6, d7,
                                   rd, wr, cd, cs, rst,
                                   ts_xp, ts_yp, ts_xm, ts_ym) {}

void DIYables_TFT_RM68140_Shield::begin() {
    // Base::begin() sets pin directions, drives RD HIGH + CS LOW, and pulses
    // RST.  It does NOT send any controller init — that's our job below.
    DIYables_TFT_Touch_Shield_Base::begin();

    writeCommand(0x01);   // Software Reset — clear all registers to defaults
    delay(120);

    writeCommand(0x11);   // Sleep Out
    delay(120);

    writeCommand(0x3A);   // Interface Pixel Format
    writeData(0x55);      // 16-bit/pixel (RGB565)

    // Power Control
    writeCommand(0xC0);
    writeData(0x0D); writeData(0x0D);
    writeCommand(0xC1);
    writeData(0x43); writeData(0x00);
    writeCommand(0xC2);
    writeData(0x00);

    // VCOM Control — affects colour balance and contrast
    writeCommand(0xC5);
    writeData(0x00); writeData(0x48);
    writeData(0x00); writeData(0x48);

    // Display Function Control
    writeCommand(0xB6);
    writeData(0x00);
    writeData(0x22); // SS=1, GS=0, REV=1
    writeData(0x3B); // NL = 480 lines

    // Positive Gamma Correction
    writeCommand(0xE0);
    writeData(0x0F); writeData(0x21); writeData(0x1C); writeData(0x0B);
    writeData(0x0E); writeData(0x08); writeData(0x49); writeData(0x98);
    writeData(0x38); writeData(0x09); writeData(0x11); writeData(0x03);
    writeData(0x14); writeData(0x10); writeData(0x00);

    // Negative Gamma Correction
    writeCommand(0xE1);
    writeData(0x0F); writeData(0x2F); writeData(0x2B); writeData(0x0C);
    writeData(0x0E); writeData(0x06); writeData(0x47); writeData(0x76);
    writeData(0x37); writeData(0x07); writeData(0x11); writeData(0x04);
    writeData(0x23); writeData(0x1E); writeData(0x00);

    writeCommand(0x13);   // Normal Display Mode ON
    writeCommand(0x29);   // Display ON
    delay(25);

    setRotation(0);       // Issues MADCTL (0x36) + Display Function Control (0xB6)
}

// RM68140 ships colours-correct WITHOUT software inversion, while the standard
// MIPI invertDisplay(true) → INVON would invert them.  Flip the semantic so
// the user-facing API stays consistent ("invert(true)" = visibly inverted).
void DIYables_TFT_RM68140_Shield::invertDisplay(bool i) {
  DIYables_TFT_Touch_Shield_Base::invertDisplay(!i);
}

// The RM68140 routes MX/MY through Display Function Control (0xB6) rather than
// MADCTL (0x36).  Putting MX/MY in MADCTL causes mirrored display on
// rotations 1 and 2.  Same handling as MCUFRIEND_kbv for LCD ID 0x6814.
void DIYables_TFT_RM68140_Shield::setRotation(uint8_t r) {
  Adafruit_GFX::setRotation(r);

  uint8_t val = madctlForRotation(r);

  // Move MY -> GS (bit6) and MX -> SS (bit5) into Display Function Control
  uint8_t GS  = (val & 0x80) ? (1 << 6) : 0;  // MY -> GS
  uint8_t _SS = (val & 0x40) ? (1 << 5) : 0;  // MX -> SS
  val &= 0x28;  // Keep only MV and BGR in MADCTL

  writeCommand(0xB6);
  writeData(0x00);
  writeData(GS | _SS | 0x02);  // GS, SS, REV=1
  writeData(0x3B);             // NL = 480 lines

  writeCommand(0x36);
  writeData(val);
}
