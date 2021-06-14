#include QMK_KEYBOARD_H
#include "print.h"

#define _BASE 0
#define _FN 1

#define STX 0x02
#define ETX 0x03
#define ESC_SEQ 0x1A

#define ESC_SEQ_02 0x30
#define ESC_SEQ_03 0x31
#define ESC_SEQ_1A 0x32

#define OLED_CMD 0x30
#define RGB_CMD 0x31
#define RGB_RELEASE 0x30
#define RGB_HSV_WRITE_SINGLE 0x31
#define RGB_HSV_WRITE_RANGE 0x32
#define RGB_SPIRAL 0x33
#define RGB_READY 0x34

char oledDispStrings[3][16] = {
    {0},
    {0},
    {0}
};



uint8_t serBuffer[32] = {0};
uint8_t serBufLen = 0;


rgblight_config_t originalConfig;

// enum my_keycodes {
//     RGB_INCR_LED = SAFE_RANGE,
// };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BASE] = LAYOUT( \
    KC_NUMLOCK          ,KC_AUDIO_VOL_DOWN   ,KC_AUDIO_VOL_UP     ,KC_ESC      ,KC_F1   ,KC_F2            ,KC_F3            ,KC_F4          ,KC_F5   ,KC_F6   ,KC_F7   ,KC_F8   ,KC_F9   ,KC_F10  ,KC_F11  ,KC_F12  ,KC_INS    ,KC_DEL    ,\
    KC_MEDIA_PLAY_PAUSE ,KC_MEDIA_PREV_TRACK ,KC_MEDIA_NEXT_TRACK ,KC_GRAVE    ,KC_1    ,KC_2             ,KC_3             ,KC_4           ,KC_5    ,KC_6    ,KC_7    ,KC_8    ,KC_9    ,KC_0    ,KC_MINS ,KC_EQL  ,KC_BSPC   ,KC_HOME   ,\
    KC_KP_7             ,KC_KP_8             ,KC_KP_9             ,KC_TAB      ,KC_Q    ,KC_W             ,KC_E             ,KC_R           ,KC_T    ,KC_Y    ,KC_U    ,KC_I    ,KC_O    ,KC_P    ,KC_LBRC ,KC_RBRC ,KC_BSLASH ,KC_END    ,\
    KC_KP_4             ,KC_KP_5             ,KC_KP_6             ,KC_CAPSLOCK ,KC_A    ,KC_S             ,KC_D             ,KC_F           ,KC_G    ,KC_H    ,KC_J    ,KC_K    ,KC_L    ,KC_SCLN ,KC_QUOT ,KC_ENT  ,XXXXXXX   ,KC_PGUP   ,\
    KC_KP_1             ,KC_KP_2             ,KC_KP_3             ,KC_LSHIFT   ,KC_Z    ,KC_X             ,KC_C             ,KC_V           ,KC_B    ,KC_N    ,KC_M    ,KC_COMM ,KC_DOT  ,KC_SLSH ,KC_RSFT ,XXXXXXX ,KC_UP     ,KC_PGDN   ,\
    KC_KP_0             ,KC_KP_DOT           ,LCTL_T(KC_KP_ENTER) ,KC_LCTL     ,MO(_FN) ,KC_LWIN          ,KC_LALT          ,LSFT_T(KC_SPC) ,KC_SPC  ,KC_SPC  ,KC_BSPC ,KC_RALT ,KC_RWIN ,KC_RCTL ,KC_PSCR ,KC_LEFT ,KC_DOWN   ,KC_RGHT \
),

[_FN] = LAYOUT( \
    _______             ,KC_BRIGHTNESS_DOWN  ,KC_BRIGHTNESS_UP    ,_______     ,RGB_TOG ,RGB_MODE_FORWARD ,RGB_MODE_REVERSE ,RGB_HUI        ,RGB_SAI ,RGB_VAI ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______   ,_______   ,\
    _______             ,_______             ,_______             ,_______     ,_______ ,_______          ,_______          ,_______        ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______   ,_______   ,\
    _______             ,_______             ,_______             ,_______     ,_______ ,_______          ,_______          ,_______        ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______   ,_______   ,\
    _______             ,_______             ,_______             ,_______     ,_______ ,_______          ,_______          ,_______        ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______   ,_______   ,\
    _______             ,_______             ,_______             ,_______     ,_______ ,_______          ,_______          ,_______        ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______   ,_______   ,\
    _______             ,_______             ,_______             ,_______     ,_______ ,_______          ,_______          ,_______        ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______ ,_______   ,_______ \
)
};

// void keyboard_post_init_user(void) {
//     // rgblight_setrange(255);
//     // Customise these values to desired behaviour
//     debug_enable=true;
//     // debug_matrix=true;
//     debug_keyboard=true;
//     //debug_mouse=true;
// }


bool escFlag = false;
void virtser_recv(uint8_t serIn) {
    serBuffer[serBufLen] = serIn;
    serBufLen++;
    uprintf("RX: %X\n", serIn);
}

void housekeeping_task_user(void) {
    if (serBuffer[0] != STX) {
        serBufLen = 0; //we are in the middle of a message?? start over
    }
    else if (serBuffer[serBufLen - 1] == ETX) { //check if message is completed, then we might start processing it
        //first, decode escaped sequences
        bool escFlag = false;
        uint8_t logicalBuf[32] = {0};
        uint8_t logicalBufLen = 0;
        for (uint8_t i = 0; i < serBufLen; i++) {
            if (serBuffer[i] == ESC_SEQ) {
                escFlag = true;
                print("Esc flag set");
            }
            else {
                uint8_t inByte;
                if (escFlag) {
                    switch (serBuffer[i])
                    {
                    case ESC_SEQ_02:
                        inByte = 0x02;
                        break;
                    case ESC_SEQ_03:
                        inByte = 0x03;
                        break;
                    case ESC_SEQ_1A:
                        inByte = 0x1A;
                        break;
                    default:
                        break;
                    }
                    escFlag = false;
                }
                else {
                    inByte = serBuffer[i];
                }
                logicalBuf[logicalBufLen] = inByte;
                logicalBufLen++;
                uprintf("LogBuf: %X\n", inByte);
            }
        }
        serBufLen = 0; //finished processing of this command, delete buffer
        
        uint8_t cmd = logicalBuf[1];

        switch (cmd)
        {
        case OLED_CMD:
            print("In OLED\n");
            uint8_t lineNum = logicalBuf[2];
            memcpy( &oledDispStrings[lineNum][0], &logicalBuf[3], logicalBufLen - 4 );
            break;
        
        case RGB_CMD:
            print("In RGB\n");
            uint8_t effectType = logicalBuf[2];
            if (effectType == RGB_RELEASE) {
                print("In RGB release\n");
                // rgblight_setrange(255);
                rgblight_reload_from_eeprom();
            }
            else if (effectType == RGB_SPIRAL) {
                print("In RGB spiral\n");
                rgblight_enable_noeeprom();
                rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_SWIRL + 5);
            }
            else if (effectType == RGB_HSV_WRITE_SINGLE) {
                print("In RGB write single\n");
                rgblight_sethsv_at(logicalBuf[4], logicalBuf[5], logicalBuf[6], logicalBuf[3]);
            }
            else if (effectType == RGB_HSV_WRITE_RANGE) {
                print("In RGB write range\n");
                rgblight_sethsv_range(logicalBuf[5], logicalBuf[6], logicalBuf[7], logicalBuf[3], logicalBuf[4]);
            }
            else if (effectType == RGB_READY) {
                print("In RGB ready\n");
                rgblight_enable_noeeprom();
                rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
                rgblight_sethsv_range(0, 0, 0, 0, RGBLED_NUM);
            }


            break;
        default:
            break;
        }
        
    }
}


#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    // Host Keyboard LED Status
    oled_set_cursor(0, 3);
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR(" ") : PSTR("1"), !led_state.num_lock);
    oled_write_P(PSTR(" "), false);
    oled_write_P(led_state.caps_lock ? PSTR("A") : PSTR(" "), false);
    oled_write_P(PSTR(" "), false);

    // Host Keyboard Layer Status
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("  "), false);
            break;
        case _FN:
            oled_write_P(PSTR("Fn"), false);
            break;
        default:
            oled_write_P(PSTR("??"), false);
            break;
    }

    for (uint8_t i = 0; i < 3; i++) {
        oled_set_cursor(0, i);
        oled_write(oledDispStrings[i], false);
    }
}
#endif