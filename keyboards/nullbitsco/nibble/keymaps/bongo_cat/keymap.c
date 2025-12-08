/* Copyright 2021 Jay Greco
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "animation_frames.h"
#include "oled/oled_driver.h"
#include "wpm.h"

#define IDLE_FRAME_DURATION 200

void oled_write_compressed_P(const void* input_block_map, const void* input_block_list);

static uint32_t anim_timer         = 0;
static uint32_t anim_sleep         = 0;
static uint8_t  current_idle_frame = 0;
static bool     tap_anim           = false;
static bool     tap_anim_toggle    = false;

enum layer_names {
  _BASE,
  _VIA1,
  _VIA2,
  _VIA3
};

enum custom_keycodes {
  PROG = SAFE_RANGE,
  _NUM_CUST_KCS,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_all(
             QK_GESC,     KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_MINS,  KC_EQL, KC_BSPC,  KC_GRV,
    KC_MUTE,  KC_TAB,     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_LBRC, KC_RBRC, KC_BSLS,  KC_DEL,
  MO(_VIA1), KC_CAPS,     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,           KC_ENT, KC_PGUP,
    KC_BRIU, KC_LSFT,  KC_NUBS,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RSFT,   KC_UP, KC_PGDN,
    KC_BRID, KC_LCTL,  KC_LALT, KC_LGUI,                             KC_SPC,                 MO(_VIA1), KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),

  [_VIA1] = LAYOUT_all(
              QK_BOOT,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, _______, _______,
    UG_TOGG,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, KC_VOLD, KC_VOLU, KC_MUTE, _______, _______, KC_LEFT, KC_DOWN, KC_UP,  KC_RIGHT, _______, _______,          _______, KC_HOME,
    _______,  _______, KC_MPRV, KC_MNXT, KC_MPLY, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_END,
    _______,  _______, _______, _______,                            _______,                   _______, _______, _______, _______, _______, _______
  ),

  [_VIA2] = LAYOUT_all(
              _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______, _______,                            _______,                   _______, _______, _______, _______, _______, _______
  ),

  [_VIA3] = LAYOUT_all(
              _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______, _______,                            _______,                   _______, _______, _______, _______, _______, _______
  ),

};

// Set the OLED initial rotation to 270 degrees.
// Return the chosen OLED rotation.
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }

// Allocate an in-RAM buffer for the decompressed OLED frame.
// Iterate over each byte position and reconstruct decompressed data using block maps.
// Write the decompressed frame to the OLED using the driver.
void oled_write_compressed_P(const void* input_block_map, const void* input_block_list) {
    uint8_t buffer[NUM_OLED_BYTES];
    uint16_t block_index = 0;

    for (uint16_t i = 0; i < NUM_OLED_BYTES; ++i) {
        uint8_t bit = i % 8;
        uint8_t map_index = i / 8;
        uint8_t _block_map = (uint8_t)pgm_read_byte_near((const char *)input_block_map + map_index);
        uint8_t nonzero_byte = (_block_map & (1 << bit));
        if (nonzero_byte) {
            buffer[i] = (uint8_t)pgm_read_byte_near((const char *)input_block_list + block_index);
            ++block_index;
        } else {
            buffer[i] = 0x00;
        }
    }

    oled_write_raw((const char*)buffer, NUM_OLED_BYTES);
}

// If not in tap animation, update the current idle frame index.
// Compute the index for the idle frame to render.
// Render the compressed or raw idle frame to the OLED.
static void animation_phase(void) {
    if (!tap_anim) {
        current_idle_frame = (current_idle_frame + 1) % NUM_IDLE_FRAMES;
        uint8_t idx = abs((NUM_IDLE_FRAMES - 1) - current_idle_frame);
        oled_write_compressed_P(idle_block_map[idx], idle_frames[idx]);
    }
}

// Update animation frame timing and sleep timer based on WPM activity.
static void render_anim(void) {
    if (get_current_wpm() != 0) {
        oled_on();
        if (timer_elapsed32(anim_timer) > IDLE_FRAME_DURATION) {
            anim_timer = timer_read32();
            animation_phase();
        }
        anim_sleep = timer_read32();
    } else {
        if (timer_elapsed32(anim_sleep) > OLED_TIMEOUT) {
            oled_off();
        } else {
            if (timer_elapsed32(anim_timer) > IDLE_FRAME_DURATION) {
                anim_timer = timer_read32();
                animation_phase();
            }
        }
    }
}

// OLED task to render animation and WPM counter
bool oled_task_user(void) {
    render_anim();
    oled_set_cursor(0, 14);

    uint8_t n = get_current_wpm();
    char wpm_counter[6];
    wpm_counter[5] = '\0';
    wpm_counter[4] = '0' + n % 10;
    wpm_counter[3] = '0' + (n /= 10) % 10;
    wpm_counter[2] = '0' + n / 10 ;
    wpm_counter[1] = '0';
    wpm_counter[0] = '>';
    oled_write_ln(wpm_counter, false);
    return false;
}

// Process keypresses and animate OLED tap animation
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if ((keycode >= KC_A && keycode <= KC_0) || (keycode >= KC_TAB && keycode <= KC_SLASH)) {
        if (record->event.pressed) {
            tap_anim_toggle = !tap_anim_toggle;
            oled_write_compressed_P(tap_block_map[tap_anim_toggle], tap_frames[tap_anim_toggle]);
        }
    }

    switch(keycode) {
        case PROG:
          if (record->event.pressed) {
            rgblight_disable_noeeprom();
            oled_off();
            bootloader_jump();
          }
        break;

        default:
        break;
    }

    return true;
}

// Rotary encoder - Volume control
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_VOLU);
    } else {
        tap_code(KC_VOLD);
    }
    return true;
}

// Initialize remote keyboard, if connected
void matrix_init_user(void) {
    matrix_init_remote_kb();
}

// Scan and parse keystrokes from remote keyboard, if connected
void matrix_scan_user(void) {
    matrix_scan_remote_kb();
}
