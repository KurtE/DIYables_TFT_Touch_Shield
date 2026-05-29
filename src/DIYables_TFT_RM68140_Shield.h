#ifndef DIYables_TFT_RM68140_Shield_H
#define DIYables_TFT_RM68140_Shield_H

#include "DIYables_TFT_Touch_Shield_Base.h"

class DIYables_TFT_RM68140_Shield : public DIYables_TFT_Touch_Shield_Base {
public:
  /// Default: native display code + default touch pins (6, A1, A2, 7).
  DIYables_TFT_RM68140_Shield();

  /// Native display code + custom touch pins.
  /// Use this when the board is supported by the low-level driver but the
  /// touch panel is wired to different pins than the default Uno/Mega layout.
  DIYables_TFT_RM68140_Shield(uint8_t ts_xp, uint8_t ts_yp,
                              uint8_t ts_xm, uint8_t ts_ym);

  /// Arduino-API display code + custom touch pins.
  /// Use this when the board requires explicit pin assignments for both
  /// the 8-bit parallel data bus and the touch panel.
  DIYables_TFT_RM68140_Shield(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                              uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
                              uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
                              uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym);

  // Driver-specific overrides. Everything else (turnOn/Off, touch API,
  // drawing primitives) is inherited unchanged from the base class.
  void begin() override;
  void setRotation(uint8_t r) override;
  void invertDisplay(bool i) override;
};

#endif // DIYables_TFT_RM68140_Shield_H
