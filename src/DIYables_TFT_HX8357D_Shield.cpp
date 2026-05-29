#include "DIYables_TFT_HX8357D_Shield.h"

DIYables_TFT_HX8357D_Shield::DIYables_TFT_HX8357D_Shield()
  : DIYables_TFT_Touch_Shield_Base() {}

DIYables_TFT_HX8357D_Shield::DIYables_TFT_HX8357D_Shield(
    uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : DIYables_TFT_Touch_Shield_Base(ts_xp, ts_yp, ts_xm, ts_ym) {}

DIYables_TFT_HX8357D_Shield::DIYables_TFT_HX8357D_Shield(
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
    uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
    uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : DIYables_TFT_Touch_Shield_Base(d0, d1, d2, d3, d4, d5, d6, d7,
                                   rd, wr, cd, cs, rst,
                                   ts_xp, ts_yp, ts_xm, ts_ym) {}

void DIYables_TFT_HX8357D_Shield::begin() {
    // Base::begin() sets pin directions, drives RD HIGH + CS LOW, and pulses
    // RST.  It does NOT send any controller init — that's our job below.
    DIYables_TFT_Touch_Shield_Base::begin();

    // Minimal HX8357D init based on MCUFRIEND_kbv table for LCD ID 0x8357.
    // Default gamma / power / VCOM values from the controller's ROM are
    // good enough for this shield; only the registers below are required.

    // SETEXTC: enable extension command set. MUST be the first command
    // after reset; without it every 0xB6 / 0x3A / etc. below is silently
    // dropped and the panel shows an all-white screen.
    writeCommand(0xB9);
    writeData(0xFF); writeData(0x83); writeData(0x57);
    delay(50);

    // SETCOM: VCOM = -1.52 V (recommended default for HX8357D shields)
    writeCommand(0xB6);
    writeData(0x25);

    // COLMOD: 16-bit/pixel (RGB565) — required so inherited fillRect /
    // drawPixel data writes are interpreted correctly.
    writeCommand(0x3A);
    writeData(0x55);

    // MADCTL: portrait, BGR colour order
    writeCommand(0x36);
    writeData(0x48); // MX | BGR

    writeCommand(0x11);   // Sleep Out
    delay(150);

    writeCommand(0x21);   // Display Inversion ON (HX8357D ships expecting inverted polarity)

    writeCommand(0x29);   // Display ON
    delay(50);

    setRotation(0);
}
