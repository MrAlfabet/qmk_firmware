/* Copyright 2020 Snipeye
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

enum uno_keycode
{
  UNO = SAFE_RANGE
};

static uint16_t pressTimer = 0xFFFF;
#define CUSTOM_LONGPRESS 150
#define CUSTOM_LONGERPRESS 1000
#define CUSTOM_STRING "What the fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills. I am trained in gorilla warfare and I'm the top sniper in the entire US armed forces. You are nothing to me but just another target. I will wipe you the fuck out with precision the likes of which has never been seen before on this Earth, mark my fucking words. You think you can get away with saying that shit to me over the Internet? Think again, fucker. As we speak I am contacting my secret network of spies across the USA and your IP is being traced right now so you better prepare for the storm, maggot. The storm that wipes out the pathetic little thing you call your life. You're fucking dead, kid. I can be anywhere, anytime, and I can kill you in over seven hundred ways, and that's just with my bare hands. Not only am I extensively trained in unarmed combat, but I have access to the entire arsenal of the United States Marine Corps and I will use it to its full extent to wipe your miserable ass off the face of the continent, you little shit. If only you could have known what unholy retribution your little “clever” comment was about to bring down upon you, maybe you would have held your fucking tongue. But you couldn't, you didn't, and now you're paying the price, you goddamn idiot. I will shit fury all over you and you will drown in it. You're fucking dead, kiddo."
#define MENU_LENGTH 5000
#define RESET_LENGTH 10000
const uint8_t PROGMEM RGBLED_RAINBOW_MOOD_INTERVALS[] = { 10, 25, 50 };

char stringToSend[2] = "a";
char maxLetter = 'z';

uint8_t presetCounter = 0;

bool firstTime = true;
bool inMenu = false;

#define COUNTER X_A


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
      [0] = LAYOUT(
            UNO
          )
};

bool intro() {
    switch (presetCounter) {
        case 0:
            //SEND_STRING(SS_LCMD("n"));
            SEND_STRING("Hi Lieffie! Hele fijne verjaardag!");
            return true;
        break;
        case 1:
            SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Van mij, voor jou; een feel-good cadeautje met mood-verlichting ;)");
            rgblight_sethsv_noeeprom(255, 255, 255);
            rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_MOOD);
            return true;
        break;
        case 2:
            SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Niet vergeten: ik hou van jou!"SS_TAP(X_ENTER)SS_TAP(X_ENTER));
            return false;
        break;
        default:
            return false;
        break;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
		case UNO:
            if (record->event.pressed) {
                pressTimer = timer_read();
            } else {
                uint16_t timeElapsed = timer_elapsed(pressTimer);
                switch (presetCounter) {
                    case 0:
                        //SEND_STRING(SS_LCMD("n"));
                        SEND_STRING("Hi Lieffie! Hele fijne verjaardag!");
                    break;
                    case 1:
                        SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Van mij, voor jou; een feel-good cadeautje met mood-verlichting ;)");
                        rgblight_sethsv_noeeprom(255, 255, 255);
                        rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_MOOD);
                    break;
                    case 2:
                        SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Niet vergeten: ik hou van jou!"SS_TAP(X_ENTER)SS_TAP(X_ENTER));
                    break;
                    default:
                        if (timeElapsed < CUSTOM_LONGPRESS) {
                            // Normal press.  We're going to send the current letter and increment the counter.
                            if (firstTime) {
                                firstTime = false;
                            } else
                            {
                                SEND_STRING(SS_TAP(X_BSPACE));
                            }
                            send_string(stringToSend);
                            if (stringToSend[0] < maxLetter && stringToSend[0] != ' ') {
                                stringToSend[0]++;
                            } else if (stringToSend[0] == maxLetter) {
                                stringToSend[0] = ' ';
                            } else if (stringToSend[0] == ' ') {
                                stringToSend[0] = 'a';
                            }
                        } else if (timeElapsed < CUSTOM_LONGERPRESS) {
                            // Long press, confirm the current letter, reset counter
                            stringToSend[0] = 'a';
                            send_string(stringToSend);
                        } else if (timeElapsed < MENU_LENGTH) {
                            if (!inMenu) {
                                // Enter menu
                                rgblight_sethsv_noeeprom(HSV_RED);
                                rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
                                inMenu = true;
                            } else {
                                rgblight_sethsv_noeeprom(255, 255, 255);
                                rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_MOOD);
                                inMenu = false;
                            }
                        } else if (timeElapsed < RESET_LENGTH) {
                            // Longer press, display macro.
                            SEND_STRING(CUSTOM_STRING);
                        } else {
                            reset_keyboard();
                        }
                        presetCounter--;
                    break;
                }
                presetCounter++;
            }
        break;
    }
    return false;
}

void keyboard_post_init_user(void) {
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(0, 0, 0);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}
