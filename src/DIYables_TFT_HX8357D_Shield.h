#ifndef DIYables_TFT_HX8357D_Shield_H
#define DIYables_TFT_HX8357D_Shield_H

#include "DIYables_TFT_Touch_Shield_Base.h"

class DIYables_TFT_HX8357D_Shield : public DIYables_TFT_Touch_Shield_Base {
public:
  /// Default: native display code + default touch pins (6, A1, A2, 7).
  DIYables_TFT_HX8357D_Shield();

  /// Native display code + custom touch pins.
  /// Use this when the board is supported by the low-level driver but the
  /// touch panel is wired to different pins than the default Uno/Mega layout.
  DIYables_TFT_HX8357D_Shield(uint8_t ts_xp, uint8_t ts_yp,
                              uint8_t ts_xm, uint8_t ts_ym);

  /// Arduino-API display code + custom touch pins.
  /// Use this when the board requires explicit pin assignments for both
  /// the 8-bit parallel data bus and the touch panel.
  DIYables_TFT_HX8357D_Shield(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                              uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
                              uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
                              uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym);

  // HX8357D follows the MIPI standard for rotation and inversion, so we only
  // need to override begin() to send the HX8357D-specific init sequence.
  void begin() override;
};

#endif // DIYables_TFT_HX8357D_Shield_H
