#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
{ MP_ROM_QSTR(MP_QSTR_P1_13), MP_ROM_PTR(&pin_P1_13) },
  { MP_ROM_QSTR(MP_QSTR_P1_15), MP_ROM_PTR(&pin_P1_15) },
  { MP_ROM_QSTR(MP_QSTR_P0_02), MP_ROM_PTR(&pin_P0_02) },
  { MP_ROM_QSTR(MP_QSTR_P0_29), MP_ROM_PTR(&pin_P0_29) },
  { MP_ROM_QSTR(MP_QSTR_P0_31), MP_ROM_PTR(&pin_P0_31) },
  { MP_ROM_QSTR(MP_QSTR_P0_26), MP_ROM_PTR(&pin_P0_26) },
  { MP_ROM_QSTR(MP_QSTR_P0_04), MP_ROM_PTR(&pin_P0_04) },
  { MP_ROM_QSTR(MP_QSTR_P0_06), MP_ROM_PTR(&pin_P0_06) },
  { MP_ROM_QSTR(MP_QSTR_P0_08), MP_ROM_PTR(&pin_P0_08) },
  { MP_ROM_QSTR(MP_QSTR_P1_09), MP_ROM_PTR(&pin_P1_09) },
  { MP_ROM_QSTR(MP_QSTR_P0_12), MP_ROM_PTR(&pin_P0_12) },
  
  { MP_ROM_QSTR(MP_QSTR_P0_10), MP_ROM_PTR(&pin_P0_10) },
  { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_P0_10) },
  { MP_ROM_QSTR(MP_QSTR_P0_09), MP_ROM_PTR(&pin_P0_09) },
  { MP_ROM_QSTR(MP_QSTR_P1_00), MP_ROM_PTR(&pin_P1_00) },
  { MP_ROM_QSTR(MP_QSTR_P0_24), MP_ROM_PTR(&pin_P0_24) },
  { MP_ROM_QSTR(MP_QSTR_P0_22), MP_ROM_PTR(&pin_P0_22) },
  { MP_ROM_QSTR(MP_QSTR_P0_20), MP_ROM_PTR(&pin_P0_20) },
// RESET  { MP_ROM_QSTR(MP_QSTR_P0_18), MP_ROM_PTR(&pin_P0_18) },
  { MP_ROM_QSTR(MP_QSTR_P0_15), MP_ROM_PTR(&pin_P0_15) },
  { MP_ROM_QSTR(MP_QSTR_P0_13), MP_ROM_PTR(&pin_P0_13) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
