#include QMK_KEYBOARD_H
#include "raw_hid.h"

#define _BASE 0
#define _FN 1

char oledDispString[16] = {0};

// enum my_keycodes {
//     RGB_INCR_LED = SAFE_RANGE,
// };

enum HIDCommand {
    OLED_CMD,
    RGB_CMD
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BASE] = LAYOUT( \
    KC_NUMLOCK          ,KC_AUDIO_VOL_DOWN   ,KC_AUDIO_VOL_UP     ,KC_ESC      ,KC_F1            ,KC_F2   ,KC_F3   ,KC_F4   ,KC_F5   ,KC_F6   ,KC_F7   ,KC_F8           ,KC_F9   ,KC_F10  ,KC_F11  ,KC_F12     ,KC_DEL     ,KC_INS        ,\
    KC_MEDIA_PLAY_PAUSE ,KC_MEDIA_PREV_TRACK ,KC_MEDIA_NEXT_TRACK ,KC_GRAVE    ,KC_1             ,KC_2    ,KC_3    ,KC_4    ,KC_5    ,KC_6    ,KC_7    ,KC_8            ,KC_9    ,KC_0    ,KC_MINS ,KC_EQL     ,KC_BSPC    ,XXXXXXX       ,\
    KC_KP_7             ,KC_KP_8             ,KC_KP_9             ,KC_TAB      ,KC_Q             ,KC_W    ,KC_E    ,KC_R    ,KC_T    ,KC_Y    ,KC_U    ,KC_I            ,KC_O    ,KC_P    ,KC_LBRC ,KC_RBRC    ,KC_BSLASH  ,KC_PSCR       ,\
    KC_KP_4             ,KC_KP_5             ,KC_KP_6             ,KC_CAPSLOCK ,KC_A             ,KC_S    ,KC_D    ,KC_F    ,KC_G    ,KC_H    ,KC_J    ,KC_K            ,KC_L    ,KC_SCLN ,KC_QUOT ,KC_ENT     ,XXXXXXX    ,KC_PGUP       ,\
    KC_KP_1             ,KC_KP_2             ,KC_KP_3             ,KC_LSHIFT   ,KC_Z             ,KC_X    ,KC_C    ,KC_V    ,KC_B    ,KC_N    ,KC_M    ,KC_COMM         ,KC_DOT  ,KC_SLSH ,KC_RSFT ,XXXXXXX    ,KC_UP      ,KC_PGDN       ,\
    KC_KP_0             ,KC_KP_DOT           ,KC_KP_ENTER         ,KC_LCTL     ,MO(_FN)          ,KC_LWIN ,KC_LALT ,XXXXXXX ,KC_SPC  ,KC_SPC  ,XXXXXXX ,RALT_T(KC_BSPC) ,KC_RCTL ,KC_HOME ,KC_END  ,KC_LEFT    ,KC_DOWN    ,KC_RGHT \
),

[_FN] = LAYOUT( \
    _______             ,_______             ,_______             ,_______     ,RGB_TOG ,RGB_MODE_FORWARD ,RGB_MODE_REVERSE ,_______ ,_______ ,_______ ,_______ ,_______         ,_______ ,_______ ,_______ ,_______ ,_______ ,_______       ,\
    _______             ,_______             ,_______             ,_______     ,_______          ,_______      ,_______ ,_______ ,_______ ,_______ ,_______ ,_______         ,_______ ,_______ ,_______ ,_______ ,_______ ,_______       ,\
    _______             ,_______             ,_______             ,_______     ,_______          ,_______      ,_______ ,_______ ,_______ ,_______ ,_______ ,_______         ,_______ ,_______ ,_______ ,_______ ,_______ ,_______       ,\
    _______             ,_______             ,_______             ,_______     ,_______          ,_______      ,_______ ,_______ ,_______ ,_______ ,_______ ,_______         ,_______ ,_______ ,_______ ,_______ ,_______ ,_______       ,\
    _______             ,_______             ,_______             ,_______     ,_______          ,_______      ,_______ ,_______ ,_______ ,_______ ,_______ ,_______         ,_______ ,_______ ,_______ ,_______ ,_______ ,_______    ,\
    _______             ,_______             ,_______             ,_______     ,_______          ,_______      ,_______ ,_______ ,_______ ,_______ ,_______ ,_______         ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ \
)
};

// bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//     switch (keycode) {
//         default:
//             return true; // Process all other keycodes normally
//     }
// }

// void raw_hid_receive(uint8_t *data, uint8_t length) {
//     if (data[0] == 0xFF) {
//         uint8_t hidCmd = data[1];
//         switch (hidCmd)
//         {
//         case 0:
//             for (uint8_t i = 2; i < length; i++ ) {
//                 oledDispString[i - 2] = data[i];
//             }
//             oledDispString[length - 1] = '\0';
//             break;
        
//         default:
//             break;
//         }
//     }
//     raw_hid_send(data, length);
// }

void virtser_recv(uint8_t serIn) {
    
}



#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _FN:
            oled_write_P(PSTR("Function\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_ln_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);

    oled_write(oledDispString, false);
}
#endif