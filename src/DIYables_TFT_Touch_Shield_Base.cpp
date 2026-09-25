// DIYables_TFT_Touch_Shield_Base.cpp
// Vendored from the DIYables TFT Shield library's DIYables_TFT_ILI9486_Shield,
// with the following changes:
//   * Class renamed to DIYables_TFT_Touch_Shield_Base
//   * begin() reduced to bus setup + hardware reset only.  No controller-
//     specific init — derived classes (RM68140 / HX8357D / ...) supply that.
//   * `inline` removed from helper methods so derived classes in another
//     translation unit can call them (writeCommand / writeData / reset / etc.).
//   * setRotation / invertDisplay are now virtual with MIPI-standard
//     defaults; derived classes override only if they need different bits.
//   * Adds turnOn / turnOff and the resistive-touch API (setTouchCalibration,
//     setADCResolution, readTouchRaw, getTouch) — identical implementations
//     that previously lived in each derived class.

#include "DIYables_TFT_Touch_Shield_Base.h"

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

DIYables_TFT_Touch_Shield_Base::DIYables_TFT_Touch_Shield_Base(
    uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : Adafruit_GFX(320, 480),
    _ts(ts_xp, ts_yp, ts_xm, ts_ym, 300),
    _ts_xp(ts_xp), _ts_yp(ts_yp), _ts_xm(ts_xm), _ts_ym(ts_ym) {
  _d[0] = D0_PIN; _d[1] = D1_PIN; _d[2] = D2_PIN; _d[3] = D3_PIN;
  _d[4] = D4_PIN; _d[5] = D5_PIN; _d[6] = D6_PIN; _d[7] = D7_PIN;
  _rd = API_PIN_RD; _wr = API_PIN_WR; _cd = API_PIN_CD;
  _cs = API_PIN_CS; _rst = API_PIN_RESET;
  _useAPI = false;
}

DIYables_TFT_Touch_Shield_Base::DIYables_TFT_Touch_Shield_Base(
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
    uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
    uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : Adafruit_GFX(320, 480),
    _ts(ts_xp, ts_yp, ts_xm, ts_ym, 300),
    _ts_xp(ts_xp), _ts_yp(ts_yp), _ts_xm(ts_xm), _ts_ym(ts_ym) {
  _d[0] = d0; _d[1] = d1; _d[2] = d2; _d[3] = d3;
  _d[4] = d4; _d[5] = d5; _d[6] = d6; _d[7] = d7;
  _rd = rd; _wr = wr; _cd = cd; _cs = cs; _rst = rst;
  _useAPI = true;
}

// ---------------------------------------------------------------------------
// Bus helpers (note: NOT inline — must have external linkage so derived
// classes in other translation units can call them)
// ---------------------------------------------------------------------------

void DIYables_TFT_Touch_Shield_Base::writeBus(uint8_t val) {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    WRITE_8(val);
  } else
  #endif
  {
    digitalWrite(_d[0], (val >> 0) & 0x01);
    digitalWrite(_d[1], (val >> 1) & 0x01);
    digitalWrite(_d[2], (val >> 2) & 0x01);
    digitalWrite(_d[3], (val >> 3) & 0x01);
    digitalWrite(_d[4], (val >> 4) & 0x01);
    digitalWrite(_d[5], (val >> 5) & 0x01);
    digitalWrite(_d[6], (val >> 6) & 0x01);
    digitalWrite(_d[7], (val >> 7) & 0x01);
  }
}

void DIYables_TFT_Touch_Shield_Base::reset() {
  // Added pinMode to output here of the reset pin and not part of the 
  // SET_CONTROL_DIR_OUT, as if the pinMode call is part of that
  // macro, the TouchScreen code, calling it will leave the display
  // in a reset state.
  pinMode(_rst, OUTPUT);
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_LOW(RESET_PORT, RESET_PIN);
  } else
  #endif
  {
    digitalWrite(_rst, LOW);
  }

  delay(20);
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_HIGH(RESET_PORT, RESET_PIN);
  } else
  #endif
  {
    digitalWrite(_rst, HIGH);
  }

  delay(120);
}

void DIYables_TFT_Touch_Shield_Base::setWriteDir() {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    SET_DATA_DIR_OUT();
  } else
  #endif
  {
    for (uint8_t i = 0; i < 8; i++) { pinMode(_d[i], OUTPUT); }
  }
}

void DIYables_TFT_Touch_Shield_Base::write8(uint8_t val) {
  writeBus(val);
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_LOW(WR_PORT, WR_PIN);
    asm volatile("nop");
  } else
  #endif
  {
    digitalWrite(_wr, LOW);
  }

  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_HIGH(WR_PORT, WR_PIN);
  } else
  #endif
  {
    digitalWrite(_wr, HIGH);
  }
}

void DIYables_TFT_Touch_Shield_Base::pulseWR() {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_LOW(WR_PORT, WR_PIN);
    asm volatile("nop");
  } else
  #endif
  {
    digitalWrite(_wr, LOW);
    asm volatile("nop");
  }

  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_HIGH(WR_PORT, WR_PIN);
  } else
  #endif
  {
    digitalWrite(_wr, HIGH);
  }
}

void DIYables_TFT_Touch_Shield_Base::writeCommand(uint8_t cmd) {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_LOW(CD_PORT, CD_PIN);
  } else
  #endif
  {
    digitalWrite(_cd, LOW);
  }
  write8(cmd);
}

void DIYables_TFT_Touch_Shield_Base::writeData(uint8_t data) {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_HIGH(CD_PORT, CD_PIN);
  } else
  #endif
  {
    digitalWrite(_cd, HIGH);
  }
  write8(data);
}

// ---------------------------------------------------------------------------
// begin() — bus + hardware reset only.  Driver-specific init is done by the
// derived class's begin() after calling Base::begin().
// ---------------------------------------------------------------------------
void DIYables_TFT_Touch_Shield_Base::begin() {
  initBusPins();
  reset();
}

// Configure all bus pins as OUTPUT, idle RD HIGH, assert CS LOW.
void DIYables_TFT_Touch_Shield_Base::initBusPins() {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    SET_DATA_DIR_OUT();
    SET_CONTROL_DIR_OUT();
    PIN_HIGH(RD_PORT, RD_PIN);
    PIN_LOW(CS_PORT, CS_PIN);
  } else
  #endif
  {
    for (uint8_t i = 0; i < 8; i++) { pinMode(_d[i], OUTPUT); }
    pinMode(_rd, OUTPUT);
    pinMode(_wr, OUTPUT);
    pinMode(_cd, OUTPUT);
    pinMode(_cs, OUTPUT);
    pinMode(_rst, OUTPUT);
    digitalWrite(_rd, HIGH);
    digitalWrite(_cs, LOW);
  }
}

// ---------------------------------------------------------------------------
// Standard MIPI rotation / inversion. RM68140 overrides setRotation()
// because it routes MX/MY through register 0xB6 rather than MADCTL.
// ---------------------------------------------------------------------------
uint8_t DIYables_TFT_Touch_Shield_Base::madctlForRotation(uint8_t r) {
  switch (r & 3) {
    case 0:  return 0x48; // Portrait:       MX | BGR
    case 1:  return 0x28; // Landscape:      MV | BGR
    case 2:  return 0x98; // Portrait 180:   MY | ML | BGR
    default: return 0xF8; // Landscape 270:  MY | MX | MV | ML | BGR
  }
}

void DIYables_TFT_Touch_Shield_Base::setRotation(uint8_t r) {
  Adafruit_GFX::setRotation(r);
  writeCommand(0x36);
  writeData(madctlForRotation(r));
}

void DIYables_TFT_Touch_Shield_Base::invertDisplay(bool i) {
  writeCommand(i ? 0x21 : 0x20);
}

// ---------------------------------------------------------------------------
// Display power
// ---------------------------------------------------------------------------

// Turn the display content back on after turnOff(). Frame buffer content is
// preserved by the controller, so the previous image reappears unchanged.
void DIYables_TFT_Touch_Shield_Base::turnOn() {
  writeCommand(0x00); // NOP - ensure not in data mode
  writeCommand(0x11); // Sleep Out
  delay(120);
  writeCommand(0x29); // Display ON
  delay(20);
}

// Turn off the display content. The backlight stays on (it is hardwired on
// the shield PCB and cannot be software-controlled), but the panel goes blank.
// turnOn() restores the previous content without redrawing.
void DIYables_TFT_Touch_Shield_Base::turnOff() {
  writeCommand(0x00); // NOP - ensure not in data mode
  writeCommand(0x28); // Display OFF
  delay(20);
  writeCommand(0x10); // Sleep In (low power mode)
  delay(120);
}

// ---------------------------------------------------------------------------
// Drawing primitives — unchanged from the parent ILI9486 implementation.
// ---------------------------------------------------------------------------

void DIYables_TFT_Touch_Shield_Base::fillScreen(uint16_t color) {
  setAddrWindow(0, 0, width() - 1, height() - 1);
  writeData16(color, (uint32_t)width() * height());
}

void DIYables_TFT_Touch_Shield_Base::writeData16(uint16_t data, uint32_t count) {
  uint8_t hi = data >> 8;
  uint8_t lo = data & 0xFF;

  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_HIGH(CD_PORT, CD_PIN);
#ifndef DELAY_WR_STOBE
    if (hi == lo) {
      WRITE_8(hi);
      while (count >= 8) {
        WR_STROBE(); WR_STROBE(); WR_STROBE(); WR_STROBE();
        WR_STROBE(); WR_STROBE(); WR_STROBE(); WR_STROBE();
        WR_STROBE(); WR_STROBE(); WR_STROBE(); WR_STROBE();
        WR_STROBE(); WR_STROBE(); WR_STROBE(); WR_STROBE();
        count -= 8;
      }
      while (count--) {
        WR_STROBE(); WR_STROBE();
      }
    } else {
#endif      
      while (count >= 8) {
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        WRITE_8(hi); WR_STROBE(); WRITE_8(lo); WR_STROBE();
        count -= 8;
      }
      while (count--) {
        WRITE_8(hi); WR_STROBE();
        WRITE_8(lo); WR_STROBE();
      }
#ifndef DELAY_WR_STOBE
    }
#endif
    return;
  }
  #endif

  // API path
  digitalWrite(_cd, HIGH);
  if (hi == lo) {
    writeBus(hi);
    while (count >= 8) {
      pulseWR(); pulseWR(); pulseWR(); pulseWR();
      pulseWR(); pulseWR(); pulseWR(); pulseWR();
      pulseWR(); pulseWR(); pulseWR(); pulseWR();
      pulseWR(); pulseWR(); pulseWR(); pulseWR();
      count -= 8;
    }
    while (count--) {
      pulseWR(); pulseWR();
    }
  } else {
    while (count >= 8) {
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      writeBus(hi); pulseWR(); writeBus(lo); pulseWR();
      count -= 8;
    }
    while (count--) {
      writeBus(hi); pulseWR();
      writeBus(lo); pulseWR();
    }
  }
}

void DIYables_TFT_Touch_Shield_Base::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_LOW(CD_PORT, CD_PIN);
    WRITE_8(0x2A);      WR_STROBE();
    PIN_HIGH(CD_PORT, CD_PIN);
    WRITE_8(x0 >> 8);   WR_STROBE();
    WRITE_8(x0 & 0xFF); WR_STROBE();
    WRITE_8(x1 >> 8);   WR_STROBE();
    WRITE_8(x1 & 0xFF); WR_STROBE();
    PIN_LOW(CD_PORT, CD_PIN);
    WRITE_8(0x2B);      WR_STROBE();
    PIN_HIGH(CD_PORT, CD_PIN);
    WRITE_8(y0 >> 8);   WR_STROBE();
    WRITE_8(y0 & 0xFF); WR_STROBE();
    WRITE_8(y1 >> 8);   WR_STROBE();
    WRITE_8(y1 & 0xFF); WR_STROBE();
    PIN_LOW(CD_PORT, CD_PIN);
    WRITE_8(0x2C);      WR_STROBE();
    return;
  }
  #endif

  writeCommand(0x2A);
  writeData(x0 >> 8); writeData(x0 & 0xFF);
  writeData(x1 >> 8); writeData(x1 & 0xFF);

  writeCommand(0x2B);
  writeData(y0 >> 8); writeData(y0 & 0xFF);
  writeData(y1 >> 8); writeData(y1 & 0xFF);

  writeCommand(0x2C);
}

void DIYables_TFT_Touch_Shield_Base::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_LOW(CD_PORT, CD_PIN);
    WRITE_8(0x2A);         WR_STROBE();
    PIN_HIGH(CD_PORT, CD_PIN);
    WRITE_8(x >> 8);       WR_STROBE();
    WRITE_8(x & 0xFF);     WR_STROBE();
    WRITE_8(x >> 8);       WR_STROBE();
    WRITE_8(x & 0xFF);     WR_STROBE();
    PIN_LOW(CD_PORT, CD_PIN);
    WRITE_8(0x2B);         WR_STROBE();
    PIN_HIGH(CD_PORT, CD_PIN);
    WRITE_8(y >> 8);       WR_STROBE();
    WRITE_8(y & 0xFF);     WR_STROBE();
    WRITE_8(y >> 8);       WR_STROBE();
    WRITE_8(y & 0xFF);     WR_STROBE();
    PIN_LOW(CD_PORT, CD_PIN);
    WRITE_8(0x2C);         WR_STROBE();
    PIN_HIGH(CD_PORT, CD_PIN);
    WRITE_8(color >> 8);   WR_STROBE();
    WRITE_8(color & 0xFF); WR_STROBE();
    return;
  }
  #endif

  setAddrWindow(x, y, x, y);
  writeData(color >> 8);
  writeData(color & 0xFF);
}

void DIYables_TFT_Touch_Shield_Base::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (x >= width() || y >= height() || w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if ((x + w) > width())  w = width()  - x;
  if ((y + h) > height()) h = height() - y;

  setAddrWindow(x, y, x + w - 1, y + h - 1);
  writeData16(color, (uint32_t)w * h);
}

void DIYables_TFT_Touch_Shield_Base::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void DIYables_TFT_Touch_Shield_Base::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void DIYables_TFT_Touch_Shield_Base::pushColors(uint16_t *data, uint32_t len) {
  #ifndef ARDUINO_API_USED
  if (!_useAPI) {
    PIN_HIGH(CD_PORT, CD_PIN);
    while (len--) {
      uint16_t color = *data++;
      WRITE_8(color >> 8);   WR_STROBE();
      WRITE_8(color & 0xFF); WR_STROBE();
    }
    return;
  }
  #endif

  digitalWrite(_cd, HIGH);
  while (len--) {
    uint16_t color = *data++;
    writeBus(color >> 8); pulseWR();
    writeBus(color & 0xFF); pulseWR();
  }
}

void DIYables_TFT_Touch_Shield_Base::drawRGBBitmap(int16_t x, int16_t y,
  const uint16_t bitmap[], int16_t w, int16_t h) {
  if (x >= width() || y >= height() || (x + w) <= 0 || (y + h) <= 0) return;

  if (x >= 0 && y >= 0 && (x + w) <= width() && (y + h) <= height()) {
    setAddrWindow(x, y, x + w - 1, y + h - 1);

    uint32_t count = (uint32_t)w * h;

    #ifndef ARDUINO_API_USED
    if (!_useAPI) {
      PIN_HIGH(CD_PORT, CD_PIN);
      for (uint32_t i = 0; i < count; i++) {
        uint16_t color = pgm_read_word(&bitmap[i]);
        WRITE_8(color >> 8);   WR_STROBE();
        WRITE_8(color & 0xFF); WR_STROBE();
      }
    } else
    #endif
    {
      digitalWrite(_cd, HIGH);
      for (uint32_t i = 0; i < count; i++) {
        uint16_t color = pgm_read_word(&bitmap[i]);
        writeBus(color >> 8); pulseWR();
        writeBus(color & 0xFF); pulseWR();
      }
    }
  } else {
    for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i, py = y + j;
        if (px >= 0 && px < width() && py >= 0 && py < height()) {
          drawPixel(px, py, pgm_read_word(&bitmap[j * w + i]));
        }
      }
    }
  }
}

void DIYables_TFT_Touch_Shield_Base::drawRGBBitmap(int16_t x, int16_t y,
  uint16_t *bitmap, int16_t w, int16_t h) {
  if (x >= width() || y >= height() || (x + w) <= 0 || (y + h) <= 0) return;

  if (x >= 0 && y >= 0 && (x + w) <= width() && (y + h) <= height()) {
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    pushColors(bitmap, (uint32_t)w * h);
  } else {
    for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i, py = y + j;
        if (px >= 0 && px < width() && py >= 0 && py < height()) {
          drawPixel(px, py, bitmap[j * w + i]);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Touch API — identical implementations previously duplicated in each
// derived class.  Uses the bundled DIYables_TouchScreen driver.
// ---------------------------------------------------------------------------

void DIYables_TFT_Touch_Shield_Base::setTouchCalibration(int min_x, int max_x, int min_y, int max_y) {
  touch_min_x = min_x;
  touch_max_x = max_x;
  touch_min_y = min_y;
  touch_max_y = max_y;
}

void DIYables_TFT_Touch_Shield_Base::setADCResolution(uint8_t bits) {
  _ts.setADCResolution(bits);
}

void DIYables_TFT_Touch_Shield_Base::readTouchRaw(int &x, int &y, int &z) {
  PIN_HIGH(CS_PORT, CS_PIN);
  TSPoint tp = _ts.getPoint();
  PIN_LOW(CS_PORT, CS_PIN);

  // DIYables_TouchScreen::getPoint() leaves YP and XM in INPUT (analog) mode,
  // but these pins are shared with the TFT data/control bus and must be
  // restored to OUTPUT before any TFT operation.
  pinMode(_ts_yp, OUTPUT);
  pinMode(_ts_xm, OUTPUT);

  initBusPins();

  x = tp.x;
  y = tp.y;
  z = tp.z;
}

bool DIYables_TFT_Touch_Shield_Base::getTouch(int &screenX, int &screenY) {
  int raw_x, raw_y, z;
  readTouchRaw(raw_x, raw_y, z);

  if (z > 10) {
    // Calibration is captured in PORTRAIT (rotation 0) raw ADC space.
    // Map to current pixel orientation; same approach as MCUFRIEND_kbv.
    switch (getRotation()) {
      case 0:
        screenX = map(raw_x, touch_max_x, touch_min_x, 0, width() - 1);
        screenY = map(raw_y, touch_min_y, touch_max_y, 0, height() - 1);
        break;
      case 1:
        screenX = map(raw_y, touch_min_y, touch_max_y, 0, width() - 1);
        screenY = map(raw_x, touch_min_x, touch_max_x, 0, height() - 1);
        break;
      case 2:
        screenX = map(raw_x, touch_min_x, touch_max_x, 0, width() - 1);
        screenY = map(raw_y, touch_max_y, touch_min_y, 0, height() - 1);
        break;
      case 3:
        screenX = map(raw_y, touch_max_y, touch_min_y, 0, width() - 1);
        screenY = map(raw_x, touch_max_x, touch_min_x, 0, height() - 1);
        break;
      default:
        screenX = map(raw_x, touch_max_x, touch_min_x, 0, width() - 1);
        screenY = map(raw_y, touch_min_y, touch_max_y, 0, height() - 1);
        break;
    }

    screenX = constrain(screenX, 0, width() - 1);
    screenY = constrain(screenY, 0, height() - 1);
    return true;
  }
  return false;
}
