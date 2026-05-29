// DIYables_TFT_Touch_Shield_Base.h
// ----------------------------------------------------------------------------
// Self-contained base class for the DIYables 3.5" TFT LCD Touch Shield.
// Originally derived from DIYables_TFT_ILI9486_Shield (DIYables TFT Shield
// library) and merged into this library so this library no longer depends
// on DIYables TFT Shield.
//
// What this class provides:
//   * 8-bit parallel bus setup, hardware reset, command/data writes
//   * All Adafruit_GFX drawing primitives (fillScreen, fillRect, drawPixel,
//     drawRGBBitmap, pushColors, setAddrWindow, fast H/V lines)
//   * MIPI-standard setRotation() and invertDisplay() implementations
//   * Display content control: turnOn() / turnOff()
//   * Resistive touch panel API: setTouchCalibration / setADCResolution /
//     readTouchRaw / getTouch (common to every shield variant)
//
// What this class deliberately does NOT contain:
//   * Any controller-specific init sequence (ILI9486 / RM68140 / HX8357D / …).
//     Derived classes must call Base::begin() first (bus + RST pulse), then
//     issue their own init via writeCommand()/writeData(), and finally call
//     setRotation(0) before returning from their begin().
//   * Anything specific to one panel that the other panel does not need.
//     Such behaviour lives in the derived class and is exposed as a virtual
//     in this base (begin / setRotation / invertDisplay).
// ----------------------------------------------------------------------------

#ifndef DIYables_TFT_Touch_Shield_Base_H
#define DIYables_TFT_Touch_Shield_Base_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "DIYables_TouchScreen.h"

// ===========================================================================
// Board-specific pin / bus macros — copied verbatim from DIYables TFT Shield
// (DIYables_TFT_Shield.h) so this library no longer depends on it.
// ===========================================================================

// Data pins mapping
#ifndef D0_PIN
#define D0_PIN 8
#define D1_PIN 9
#define D2_PIN 2
#define D3_PIN 3
#define D4_PIN 4
#define D5_PIN 5
#define D6_PIN 6
#define D7_PIN 7
#endif

// Control pins
#ifndef API_PIN_RD
#define API_PIN_RD     A0
#define API_PIN_WR     A1
#define API_PIN_CD     A2
#define API_PIN_CS     A3
#define API_PIN_RESET  A4
#endif

#if defined(__AVR_ATmega328P__)
#define RD_PORT PORTC
#define RD_PIN  0
#define WR_PORT PORTC
#define WR_PIN  1
#define CD_PORT PORTC
#define CD_PIN  2
#define CS_PORT PORTC
#define CS_PIN  3
#define RESET_PORT PORTC
#define RESET_PIN  4

#define BMASK 0x03
#define DMASK 0xFC
#define WRITE_8(x) do { \
  PORTB = (PORTB & ~BMASK) | ((x) & BMASK); \
  PORTD = (PORTD & ~DMASK) | ((x) & DMASK); \
} while(0)
#define SET_DATA_DIR_OUT() do { DDRB |= BMASK; DDRD |= DMASK; } while(0)
#define SET_CONTROL_DIR_OUT() do { \
  DDRC |= (1 << RD_PIN) | (1 << WR_PIN) | (1 << CD_PIN) | (1 << CS_PIN) | (1 << RESET_PIN); \
} while(0)
#define PIN_LOW(p, b) (p) &= ~(1 << (b))
#define PIN_HIGH(p, b) (p) |= (1 << (b))
#define PIN_OUTPUT(p, b) *(&p - 1) |= (1 << (b))

#elif defined(__AVR_ATmega2560__)
#define RD_PORT PORTF
#define RD_PIN  0
#define WR_PORT PORTF
#define WR_PIN  1
#define CD_PORT PORTF
#define CD_PIN  2
#define CS_PORT PORTF
#define CS_PIN  3
#define RESET_PORT PORTF
#define RESET_PIN  4

#define EMASK 0x38
#define GMASK 0x20
#define HMASK 0x78
#define WRITE_8(x) do { \
  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
  PORTH |= (((x) & (3 << 0)) << 5); \
  PORTE |= (((x) & (3 << 2)) << 2); \
  PORTG |= (((x) & (1 << 4)) << 1); \
  PORTE |= (((x) & (1 << 5)) >> 2); \
  PORTH |= (((x) & (3 << 6)) >> 3); \
} while(0)
#define SET_DATA_DIR_OUT() do { DDRH |= HMASK; DDRG |= GMASK; DDRE |= EMASK; } while(0)
#define SET_CONTROL_DIR_OUT() DDRF |= 0x1F
#define PIN_LOW(p, b) (p) &= ~(1 << (b))
#define PIN_HIGH(p, b) (p) |= (1 << (b))
#define PIN_OUTPUT(p, b) *(&p - 1) |= (1 << (b))

#elif defined(__SAM3X8E__)
#define RD_PORT    PIOA
#define RD_PIN     16
#define WR_PORT    PIOA
#define WR_PIN     24
#define CD_PORT    PIOA
#define CD_PIN     23
#define CS_PORT    PIOA
#define CS_PIN     22
#define RESET_PORT PIOA
#define RESET_PIN  6

#define DUE_BMASK (1UL<<25)
#define DUE_CMASK ((1UL<<21)|(1UL<<22)|(1UL<<23)|(1UL<<24)|(1UL<<25)|(1UL<<26)|(1UL<<28))
#define WRITE_8(val) do { \
  PIOB->PIO_CODR = DUE_BMASK; \
  PIOC->PIO_CODR = DUE_CMASK; \
  PIOB->PIO_SODR = (((val) & (1<<2)) ? (1UL<<25) : 0); \
  PIOC->PIO_SODR = \
    (((val) & (1<<0)) ? (1UL<<22) : 0) | \
    (((val) & (1<<1)) ? (1UL<<21) : 0) | \
    (((val) & (1<<3)) ? (1UL<<28) : 0) | \
    (((val) & (1<<4)) ? (1UL<<26) : 0) | \
    (((val) & (1<<5)) ? (1UL<<25) : 0) | \
    (((val) & (1<<6)) ? (1UL<<24) : 0) | \
    (((val) & (1<<7)) ? (1UL<<23) : 0); \
} while(0)
#define SET_DATA_DIR_OUT() do { \
  PIOB->PIO_OER = DUE_BMASK; \
  PIOC->PIO_OER = DUE_CMASK; \
} while(0)
#define SET_CONTROL_DIR_OUT() do { \
  PIOA->PIO_OER = (1UL<<16)|(1UL<<24)|(1UL<<23)|(1UL<<22)|(1UL<<6); \
} while(0)
#define PIN_LOW(p, b) (p)->PIO_CODR = (1UL << (b))
#define PIN_HIGH(p, b) (p)->PIO_SODR = (1UL << (b))
#define PIN_OUTPUT(p, b) (p)->PIO_OER = (1UL << (b))

#elif defined(ARDUINO_UNOR4_MINIMA)
#define RD_PORT    R_PORT0
#define RD_PIN     14
#define WR_PORT    R_PORT0
#define WR_PIN     0
#define CD_PORT    R_PORT0
#define CD_PIN     1
#define CS_PORT    R_PORT0
#define CS_PIN     2
#define RESET_PORT R_PORT1
#define RESET_PIN  1

#define DATA_PORT3 R_PORT3
#define DATA_PORT1 R_PORT1
#define DATA_MASK3 0x18
#define DATA_MASK1 0xFC

#define WRITE_8(val) do { \
    DATA_PORT3->PODR = (DATA_PORT3->PODR & ~DATA_MASK3) \
        | (((val & 0x01) ? 0x10 : 0) | ((val & 0x02) ? 0x08 : 0)); \
    DATA_PORT1->PODR = (DATA_PORT1->PODR & ~DATA_MASK1) \
        | (((val & 0x04) ? 0x20 : 0) | ((val & 0x08) ? 0x10 : 0) \
        | ((val & 0x10) ? 0x08 : 0) | ((val & 0x20) ? 0x04 : 0) \
        | ((val & 0x40) ? 0x40 : 0) | ((val & 0x80) ? 0x80 : 0)); \
} while (0)
#define SET_DATA_DIR_OUT() do { \
    DATA_PORT3->PDR |= DATA_MASK3; \
    DATA_PORT1->PDR |= DATA_MASK1; \
} while (0)
#define SET_CONTROL_DIR_OUT() do { \
    RD_PORT->PDR |= (1 << RD_PIN); \
    WR_PORT->PDR |= (1 << WR_PIN); \
    CD_PORT->PDR |= (1 << CD_PIN); \
    CS_PORT->PDR |= (1 << CS_PIN); \
    RESET_PORT->PDR |= (1 << RESET_PIN); \
} while (0)
#define PIN_LOW(port, pin)   ((port)->PODR &= ~(1 << (pin)))
#define PIN_HIGH(port, pin)  ((port)->PODR |=  (1 << (pin)))
#define PIN_OUTPUT(port, pin) ((port)->PDR |= (1 << (pin)))

#elif defined(ARDUINO_UNOR4_WIFI)
#define RD_PORT    R_PORT0
#define RD_PIN     14
#define WR_PORT    R_PORT0
#define WR_PIN     0
#define CD_PORT    R_PORT0
#define CD_PIN     1
#define CS_PORT    R_PORT0
#define CS_PIN     2
#define RESET_PORT R_PORT1
#define RESET_PIN  1

#define DATA_PORT3 R_PORT3
#define DATA_PORT1 R_PORT1
#define DATA_MASK3 0x18
#define DATA_MASK1 ((1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<11)|(1<<12))

#define WRITE_8(val) do { \
    DATA_PORT3->PODR = (DATA_PORT3->PODR & ~DATA_MASK3) \
        | (((val & 0x01) ? (1<<4) : 0) | ((val & 0x02) ? (1<<3) : 0)); \
    DATA_PORT1->PODR = (DATA_PORT1->PODR & ~DATA_MASK1) \
        | (((val & 0x04) ? (1<<4) : 0) | ((val & 0x08) ? (1<<5) : 0) \
        | ((val & 0x10) ? (1<<6) : 0) | ((val & 0x20) ? (1<<7) : 0) \
        | ((val & 0x40) ? (1<<11) : 0) | ((val & 0x80) ? (1<<12) : 0)); \
} while (0)
#define SET_DATA_DIR_OUT() do { \
    DATA_PORT3->PDR |= DATA_MASK3; \
    DATA_PORT1->PDR |= DATA_MASK1; \
} while (0)
#define SET_CONTROL_DIR_OUT() do { \
    RD_PORT->PDR |= (1 << RD_PIN); \
    WR_PORT->PDR |= (1 << WR_PIN); \
    CD_PORT->PDR |= (1 << CD_PIN); \
    CS_PORT->PDR |= (1 << CS_PIN); \
    RESET_PORT->PDR |= (1 << RESET_PIN); \
} while (0)
#define PIN_LOW(port, pin)   ((port)->PODR &= ~(1 << (pin)))
#define PIN_HIGH(port, pin)  ((port)->PODR |=  (1 << (pin)))
#define PIN_OUTPUT(port, pin) ((port)->PDR |= (1 << (pin)))

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#undef D0_PIN
#undef D1_PIN
#undef D2_PIN
#undef D3_PIN
#undef D4_PIN
#undef D5_PIN
#undef D6_PIN
#undef D7_PIN
#define D0_PIN 21
#define D1_PIN 46
#define D2_PIN 18
#define D3_PIN 17
#define D4_PIN 19
#define D5_PIN 20
#define D6_PIN 3
#define D7_PIN 14

#define RD_PORT      0
#define RD_PIN       2
#define WR_PORT      0
#define WR_PIN       1
#define CD_PORT      0
#define CD_PIN       7
#define CS_PORT      0
#define CS_PIN       6
#define RESET_PORT   0
#define RESET_PIN    5

#define WRITE_8(val) do { \
    uint32_t set0 = 0, clr0 = 0; \
    uint32_t set1 = 0, clr1 = 0; \
    if ((val) & 0x01) set0 |= (1UL << 21); else clr0 |= (1UL << 21); \
    if ((val) & 0x02) set1 |= (1UL << (46 - 32)); else clr1 |= (1UL << (46 - 32)); \
    if ((val) & 0x04) set0 |= (1UL << 18); else clr0 |= (1UL << 18); \
    if ((val) & 0x08) set0 |= (1UL << 17); else clr0 |= (1UL << 17); \
    if ((val) & 0x10) set0 |= (1UL << 19); else clr0 |= (1UL << 19); \
    if ((val) & 0x20) set0 |= (1UL << 20); else clr0 |= (1UL << 20); \
    if ((val) & 0x40) set0 |= (1UL << 3);  else clr0 |= (1UL << 3);  \
    if ((val) & 0x80) set0 |= (1UL << 14); else clr0 |= (1UL << 14); \
    REG_WRITE(GPIO_OUT_W1TS_REG, set0); \
    REG_WRITE(GPIO_OUT_W1TC_REG, clr0); \
    REG_WRITE(GPIO_OUT1_W1TS_REG, set1); \
    REG_WRITE(GPIO_OUT1_W1TC_REG, clr1); \
} while (0)

#define SET_DATA_DIR_OUT() do { \
    static const uint8_t d_pins[] = {D0_PIN, D1_PIN, D2_PIN, D3_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN}; \
    for (uint8_t i = 0; i < 8; i++) { pinMode(d_pins[i], OUTPUT); } \
} while (0)
#define SET_CONTROL_DIR_OUT() do { \
    pinMode(RD_PIN, OUTPUT); \
    pinMode(WR_PIN, OUTPUT); \
    pinMode(CD_PIN, OUTPUT); \
    pinMode(CS_PIN, OUTPUT); \
    pinMode(RESET_PIN, OUTPUT); \
} while (0)
#define PIN_LOW(port, pin) do { \
    if ((pin) < 32) REG_WRITE(GPIO_OUT_W1TC_REG, (1UL << (pin))); \
    else REG_WRITE(GPIO_OUT1_W1TC_REG, (1UL << ((pin) - 32))); \
} while (0)
#define PIN_HIGH(port, pin) do { \
    if ((pin) < 32) REG_WRITE(GPIO_OUT_W1TS_REG, (1UL << (pin))); \
    else REG_WRITE(GPIO_OUT1_W1TS_REG, (1UL << ((pin) - 32))); \
} while (0)
#define PIN_OUTPUT(port, pin) pinMode(pin, OUTPUT)

#elif defined(ARDUINO_GIGA)
#define RD_PORT    GPIOC
#define RD_PIN     4
#define WR_PORT    GPIOC
#define WR_PIN     5
#define CD_PORT    GPIOB
#define CD_PIN     0
#define CS_PORT    GPIOB
#define CS_PIN     1
#define RESET_PORT GPIOC
#define RESET_PIN  3

#define GIGA_AMASK ((1UL<<2)|(1UL<<3)|(1UL<<7))
#define GIGA_BMASK ((1UL<<4)|(1UL<<8)|(1UL<<9))
#define GIGA_DMASK (1UL<<13)
#define GIGA_JMASK (1UL<<8)

#define WRITE_8(val) do { \
    uint32_t a_set = 0, a_clr = 0; \
    uint32_t b_set = 0, b_clr = 0; \
    uint32_t d_set = 0, d_clr = 0; \
    uint32_t j_set = 0, j_clr = 0; \
    if ((val) & 0x01) b_set |= (1UL<<8);  else b_clr |= (1UL<<8);  \
    if ((val) & 0x02) b_set |= (1UL<<9);  else b_clr |= (1UL<<9);  \
    if ((val) & 0x04) a_set |= (1UL<<3);  else a_clr |= (1UL<<3);  \
    if ((val) & 0x08) a_set |= (1UL<<2);  else a_clr |= (1UL<<2);  \
    if ((val) & 0x10) j_set |= (1UL<<8);  else j_clr |= (1UL<<8);  \
    if ((val) & 0x20) a_set |= (1UL<<7);  else a_clr |= (1UL<<7);  \
    if ((val) & 0x40) d_set |= (1UL<<13); else d_clr |= (1UL<<13); \
    if ((val) & 0x80) b_set |= (1UL<<4);  else b_clr |= (1UL<<4);  \
    GPIOA->BSRR = a_set | (a_clr << 16); \
    GPIOB->BSRR = b_set | (b_clr << 16); \
    GPIOD->BSRR = d_set | (d_clr << 16); \
    GPIOJ->BSRR = j_set | (j_clr << 16); \
} while(0)
#define SET_DATA_DIR_OUT() do { \
    pinMode(8, OUTPUT); pinMode(9, OUTPUT); \
    pinMode(2, OUTPUT); pinMode(3, OUTPUT); \
    pinMode(4, OUTPUT); pinMode(5, OUTPUT); \
    pinMode(6, OUTPUT); pinMode(7, OUTPUT); \
} while(0)
#define SET_CONTROL_DIR_OUT() do { \
    pinMode(A0, OUTPUT); pinMode(A1, OUTPUT); \
    pinMode(A2, OUTPUT); pinMode(A3, OUTPUT); \
    pinMode(A4, OUTPUT); \
} while(0)
#define PIN_LOW(port, pin)   (port)->BSRR = (1UL << ((pin) + 16))
#define PIN_HIGH(port, pin)  (port)->BSRR = (1UL << (pin))
#define PIN_OUTPUT(port, pin) /* handled by SET_*_DIR_OUT */

#else
// Fallback: use the Arduino API on any board without a native fast path.
#ifndef ARDUINO_API_USED
#define ARDUINO_API_USED
#endif
static const uint8_t dataPins[] = { D0_PIN, D1_PIN, D2_PIN, D3_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN };
#define WRITE_8(val) do { \
  digitalWrite(D0_PIN, (val >> 0) & 0x01); \
  digitalWrite(D1_PIN, (val >> 1) & 0x01); \
  digitalWrite(D2_PIN, (val >> 2) & 0x01); \
  digitalWrite(D3_PIN, (val >> 3) & 0x01); \
  digitalWrite(D4_PIN, (val >> 4) & 0x01); \
  digitalWrite(D5_PIN, (val >> 5) & 0x01); \
  digitalWrite(D6_PIN, (val >> 6) & 0x01); \
  digitalWrite(D7_PIN, (val >> 7) & 0x01); \
} while (0)
#define SET_DATA_DIR_OUT() do { \
  for (uint8_t i = 0; i < 8; i++) { pinMode(dataPins[i], OUTPUT); } \
} while(0)
#define SET_CONTROL_DIR_OUT() do { \
  pinMode(API_PIN_RD, OUTPUT); \
  pinMode(API_PIN_WR, OUTPUT); \
  pinMode(API_PIN_CD, OUTPUT); \
  pinMode(API_PIN_CS, OUTPUT); \
  pinMode(API_PIN_RESET, OUTPUT); \
} while(0)
#define PIN_LOW(p)  digitalWrite(p, LOW)
#define PIN_HIGH(p) digitalWrite(p, HIGH)
#define PIN_OUTPUT(p) pinMode(p, OUTPUT)
#endif

// Native fast-path WR strobe. Only used inside `#ifndef ARDUINO_API_USED` /
// `if (!_useAPI)` blocks, so WR_PORT / WR_PIN are always defined where this
// expands.  Expanding to a `do { ... } while(0)` keeps it a single statement
// and is fully inlined — identical machine code to writing the three
// operations out by hand.
#ifndef ARDUINO_API_USED
#define WR_STROBE() do { \
    PIN_LOW(WR_PORT, WR_PIN); \
    asm volatile("nop"); \
    PIN_HIGH(WR_PORT, WR_PIN); \
} while (0)
#endif

// ===========================================================================
// Default touch panel pins (Uno/Mega layout)
// ===========================================================================
#ifndef DIYABLES_TFT_DEFAULT_TS_XP
#define DIYABLES_TFT_DEFAULT_TS_XP 6
#define DIYABLES_TFT_DEFAULT_TS_YP A1
#define DIYABLES_TFT_DEFAULT_TS_XM A2
#define DIYABLES_TFT_DEFAULT_TS_YM 7
#endif


class DIYables_TFT_Touch_Shield_Base : public Adafruit_GFX {
public:
  /// Default: native bus + default touch pins (6, A1, A2, 7).
  DIYables_TFT_Touch_Shield_Base(uint8_t ts_xp = DIYABLES_TFT_DEFAULT_TS_XP,
                                 uint8_t ts_yp = DIYABLES_TFT_DEFAULT_TS_YP,
                                 uint8_t ts_xm = DIYABLES_TFT_DEFAULT_TS_XM,
                                 uint8_t ts_ym = DIYABLES_TFT_DEFAULT_TS_YM);

  /// Arduino-API bus + custom touch pins.
  DIYables_TFT_Touch_Shield_Base(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
                                 uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
                                 uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym);

  // -------------------------------------------------------------------------
  // Lifecycle. Base::begin() performs ONLY the bus + RST setup that every
  // panel needs. Derived classes MUST override begin(), call Base::begin()
  // first, then send their controller-specific init sequence, then call
  // setRotation(0) before returning.
  // -------------------------------------------------------------------------
  virtual void begin();

  // -------------------------------------------------------------------------
  // Rotation / inversion — default implementations follow MIPI standard
  // (MADCTL 0x36 / INVON 0x21 / INVOFF 0x20). Controllers that need
  // different handling (e.g. RM68140 routes MX/MY through 0xB6) override.
  // -------------------------------------------------------------------------
  void setRotation(uint8_t r) override;
  void invertDisplay(bool i) override;

  // -------------------------------------------------------------------------
  // Display content control — identical on every shield variant.
  // turnOff() puts the panel into Display-Off + Sleep-In; the backlight
  // remains on (hardwired on the PCB). turnOn() restores the previous
  // frame buffer content without redrawing.
  // -------------------------------------------------------------------------
  void turnOn();
  void turnOff();

  // -------------------------------------------------------------------------
  // Resistive touch API — identical on every shield variant. Calibration
  // values are expressed in PORTRAIT (rotation 0) raw ADC units and are
  // mapped to the current rotation inside getTouch().
  // -------------------------------------------------------------------------
  void setTouchCalibration(int min_x, int max_x, int min_y, int max_y);
  void setADCResolution(uint8_t bits);
  void readTouchRaw(int &x, int &y, int &z);
  bool getTouch(int &screenX, int &screenY);

  // -------------------------------------------------------------------------
  // Adafruit_GFX overrides / drawing helpers
  // -------------------------------------------------------------------------
  void fillScreen(uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
  void pushColors(uint16_t *data, uint32_t len);
  void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

  /** Convert 8-bit R,G,B to 16-bit 565 colour. */
  static uint16_t colorRGB(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

protected:
  // --- 8-bit parallel bus pins / state ---
  uint8_t _d[8];
  uint8_t _rd, _wr, _cd, _cs, _rst;
  bool    _useAPI;

  // --- Touch panel state ---
  DIYables_TouchScreen _ts;
  uint8_t _ts_xp, _ts_yp, _ts_xm, _ts_ym;
  int touch_min_x = 136;
  int touch_max_x = 907;
  int touch_min_y = 942;
  int touch_max_y = 139;

  // --- Bus helpers (NOT inline — must be callable from derived classes
  //     across translation units) ---
  void reset();
  void writeCommand(uint8_t cmd);
  void writeData(uint8_t data);
  void write8(uint8_t val);
  void writeBus(uint8_t val);
  void pulseWR();
  void setWriteDir();
  void writeData16(uint16_t data, uint32_t count);

  // Configure all 8 data + 5 control pins as OUTPUT and drive RD HIGH, CS LOW.
  // Used by begin() at startup and by readTouchRaw() to restore the bus after
  // the touch driver leaves YP/XM in INPUT (analog) mode.
  void initBusPins();

  // Standard MIPI MADCTL byte for rotation 0..3 (MX|BGR, MV|BGR,
  // MY|ML|BGR, MY|MX|MV|ML|BGR). Used by both Base::setRotation() and the
  // RM68140 override (which then re-routes the MX/MY bits into 0xB6).
  static uint8_t madctlForRotation(uint8_t r);
};

// Short alias kept for source-compatibility with sketches that used the
// original DIYables_TFT::colorRGB(...) helper.
using DIYables_TFT = DIYables_TFT_Touch_Shield_Base;

#endif // DIYables_TFT_Touch_Shield_Base_H
