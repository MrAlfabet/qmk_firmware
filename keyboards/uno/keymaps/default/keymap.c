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
#include <string.h>

enum uno_keycode
{
  UNO = SAFE_RANGE
};

// General
static uint16_t pressTimer = 0xFFFF;
#define MENU_LENGTH 3000
#define RESET_LENGTH 10000
#define COUNTER X_A
bool firstTime = true;
char stringToSend[2] = "";

// Menu
#define MODE_MENU 0
#define MODE_KEYBOARD 1
#define MODE_MORSE 2
#define MODE_COMPLIMENTS 3
#define MODE_LIGHTS 4
#define MODE_MAX 4 // edit to the highest possible number
uint8_t mode = MODE_KEYBOARD;
uint8_t menuCounter = 0;

// Keyboard
#define CUSTOM_LONGPRESS 200
#define CUSTOM_LONGERPRESS 1000
#define CUSTOM_STRING "fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills. I am trained in gorilla warfare and I'm the top sniper in the entire US armed forces. You are nothing to me but just another target. I will wipe you the fuck out with precision the likes of which has never been seen before on this Earth, mark my fucking words. You think you can get away with saying that shit to me over the Internet? Think again, fucker. As we speak I am contacting my secret network of spies across the USA and your IP is being traced right now so you better prepare for the storm, maggot. The storm that wipes out the pathetic little thing you call your life. You're fucking dead, kid. I can be anywhere, anytime, and I can kill you in over seven hundred ways, and that's just with my bare hands. Not only am I extensively trained in unarmed combat, but I have access to the entire arsenal of the United States Marine Corps and I will use it to its full extent to wipe your miserable ass off the face of the continent, you little shit. If only you could have known what unholy retribution your little “clever” comment was about to bring down upon you, maybe you would have held your fucking tongue. But you couldn't, you didn't, and now you're paying the price, you goddamn idiot. I will shit fury all over you and you will drown in it. You're fucking dead, kiddo."
uint8_t presetCounter = 0;

// Morse
static uint16_t releaseTimer = 0xFFFF;
#define MORSE_LONG 200
#define MORSE_NEW_LETTER 1000
#define MORSE_NEW_WORD 3000
#define MORSE_MAX_INPUT 6
uint16_t timeSinceLast = 0;
uint8_t morseHistory[MORSE_MAX_INPUT] = {0};

// Lovey Dovey
char ch_arr[3][10] = {
    "spike",
    "tom",
    "jerry"
};
char sendingThis[256] = "asdfesafdsafeslisjldkfjaslijf";

// Lights
const uint8_t PROGMEM RGBLED_RAINBOW_MOOD_INTERVALS[] = { 10, 25, 50 };

// Keyboard layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
      [0] = LAYOUT(
            UNO
          )
};

// Say hi to Jeanne in keyboard mode
bool intro(void) {
    switch (presetCounter) {
        case 0:
            SEND_STRING(SS_LWIN("r"));
            _delay_ms(500);
            SEND_STRING("notepad"SS_TAP(X_ENTER));
            _delay_ms(500);
            SEND_STRING("Hi Lieffie! Hele fijne verjaardag!");
            presetCounter++;
            return true;
            break;
        case 1:
            SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Van mij, voor jou; een feel-good cadeautje met mood-verlichting ;)");
            rgblight_sethsv_noeeprom(255, 255, 255);
            rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_MOOD);
            presetCounter++;
            return true;
            break;
        case 2:
            SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Korte tap voor letterselectie, iets langer voor de volgende letter."SS_TAP(X_ENTER)"Hou de toets 1 seconde in als je boos bent,"SS_TAP(X_ENTER)"3 seconden voor het menu.");
            presetCounter++;
            return true;
            break;
        case 3:
            SEND_STRING(SS_TAP(X_ENTER)SS_TAP(X_ENTER)"Niet vergeten: ik hou van jou!"SS_TAP(X_ENTER)SS_TAP(X_ENTER));
            presetCounter++;
            return true;
            break;
        default:
            return false;
            break;
    }
}

void show_menu(void) {
    rgblight_sethsv_noeeprom(HSV_RED);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_BREATHING);
    SEND_STRING(SS_LWIN("r"));
    _delay_ms(250);
    SEND_STRING("notepad"SS_TAP(X_ENTER));
    _delay_ms(250);
    SEND_STRING("Keuzemenu met de volgende smaken:"SS_TAP(X_ENTER));
    SEND_STRING("  - Tik 1x kort om te schakelen naar toetsenbord."SS_TAP(X_ENTER));
    SEND_STRING("  - Tik 2x kort om te schakelen naar morse."SS_TAP(X_ENTER));
    SEND_STRING("  - Tik 3x kort om te schakelen naar extra lieve modus."SS_TAP(X_ENTER));
    SEND_STRING("  - Tik 4x kort om de verlichting in te stellen."SS_TAP(X_ENTER));
    SEND_STRING("Bevestig je keuze door de knop 1s in te drukken"SS_TAP(X_ENTER));
    menuCounter = 0;
    mode = MODE_MENU;
    stringToSend[0] = '\0';
}

bool addToMorseHistory(uint8_t press) {
    if (morseHistory[MORSE_MAX_INPUT - 1]) {
        return 1;
    }
    for (uint8_t i=0; i < MORSE_MAX_INPUT; i++) {
        if (morseHistory[i] == 0) {
            morseHistory[i] = press;
            break;
        }
    }
    return 0;
}

void resetMorseHistory(void) {
    for (uint8_t i=0; i < MORSE_MAX_INPUT; i++) {
        morseHistory[i] = 0;
    }
}

void printMorseLetter(char letter) {

}

void morseError(void) {
    resetMorseHistory();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
		case UNO:
            if (record->event.pressed) {
                pressTimer = timer_read();
                timeSinceLast = timer_elapsed(releaseTimer);
            } else {
                releaseTimer = timer_read();
                uint16_t timeElapsed = timer_elapsed(pressTimer);
                switch (mode) {
                    case MODE_MENU:
                        if (timeElapsed < CUSTOM_LONGERPRESS) {
                            // Count menu choice
                            menuCounter++;
                        } else {
                            if (menuCounter > 0 && menuCounter <= MODE_MAX) {
                                // Leave menu
                                mode = menuCounter;
                                switch (menuCounter) {
                                    case MODE_KEYBOARD:
                                        SEND_STRING("Je koos voor toetsenbord!");
                                        break;
                                    case MODE_MORSE:
                                        SEND_STRING("Je koos voor Morse!");
                                        break;
                                    case MODE_COMPLIMENTS:
                                        SEND_STRING("Je koos voor complimentjes!");
                                        break;
                                }
                                _delay_ms(500);
                                // alt-f4, tab, Enter
                                SEND_STRING(SS_LALT(SS_TAP(X_F4)) SS_DELAY(100) SS_TAP(X_TAB) SS_DELAY(100) SS_TAP(X_ENTER));
                                rgblight_sethsv_noeeprom(255, 255, 255);
                                rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_MOOD);
                                firstTime = true;
                            } else {
                                SEND_STRING("Daar ging iets fout... try again!" SS_TAP(X_ENTER));
                                menuCounter = 0;
                            }
                        }
                        break;
                    case MODE_KEYBOARD:
                        if (timeElapsed < CUSTOM_LONGPRESS) {
                            if (intro()) {
                                return false;
                            }
                            // Normal press.  We're going to send the current letter and increment the counter.
                            if (firstTime || stringToSend[0] == '\0') {
                                firstTime = false;
                            } else {
                                SEND_STRING(SS_TAP(X_BSPACE));
                            }
                            if (stringToSend[0] != 'z' && stringToSend[0] != ' ' && stringToSend[0] != '9' && stringToSend[0] != 'Z' && stringToSend[0] != '/' && stringToSend[0] != '@' && stringToSend[0] != '\0') {
                                stringToSend[0]++;
                            } else if (stringToSend[0] == 'z') {
                                stringToSend[0] = '0';
                            } else if (stringToSend[0] == '9') {
                                stringToSend[0] = 'A';
                            } else if (stringToSend[0] == 'Z') {
                                stringToSend[0] = '!';
                            } else if (stringToSend[0] == '/') {
                                stringToSend[0] = ':';
                            } else if (stringToSend[0] == '@') {
                                stringToSend[0] = ' ';
                            } else if (stringToSend[0] == '\0') {
                                stringToSend[0] = ' ';
                            } else if (stringToSend[0] == ' ') {
                                stringToSend[0] = 'a';
                            }
                            send_string(stringToSend);
                        } else if (timeElapsed < CUSTOM_LONGERPRESS) {
                            if (intro()) {
                                return false;
                            }
                            // Long press, confirm the current letter, reset counter
                            stringToSend[0] = ' ';
                            send_string(stringToSend);
                        } else if (timeElapsed < MENU_LENGTH) {
                            // Longer press, display macro.
                            SEND_STRING("What " SS_DELAY(1000) "the " SS_DELAY(1000) CUSTOM_STRING);
                        } else if (timeElapsed < RESET_LENGTH) {
                            // Enter menu
                            show_menu();
                        } else {
                            reset_keyboard();
                        }
                        break;
                    case MODE_MORSE:
                        if (timeSinceLast < MORSE_NEW_LETTER) {
                            // do nothing
                        } else if (timeSinceLast < MORSE_NEW_WORD) {
                            // New letter, clear morse history
                            resetMorseHistory();
                        } else {
                            if (!firstTime) {
                                SEND_STRING(" ");
                            } else {
                                firstTime = false;
                            }
                            // Clear morse history
                            resetMorseHistory();
                        }
                        if (timeElapsed < MORSE_LONG) {
                            // Short press
                            if (addToMorseHistory(1)) {
                                break;
                            }
                        } else if (timeElapsed < MENU_LENGTH) {
                            // Long press
                            if (addToMorseHistory(2)) {
                                morseError();
                                break;
                            }
                        } else if (timeElapsed < RESET_LENGTH) {
                            // Enter menu
                            show_menu();
                            break;
                        } else {
                            reset_keyboard();
                        }
                        // Print morse
                        if (morseHistory[0] == 1) {
                            if (morseHistory[1] == 0) { // .
                                // E
                                stringToSend[0] = 'e';
                            } else if (morseHistory[1] == 1) {
                                if (morseHistory[2] == 0) { // ..
                                    // I
                                    stringToSend[0] = 'i';
                                } else if (morseHistory[2] == 1) {
                                    if (morseHistory[3] == 0) { // ...
                                        // S
                                        stringToSend[0] = 's';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // ....
                                            // H
                                            stringToSend[0] = 'h';
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // .....
                                                // 5
                                                stringToSend[0] = '5';
                                            } else {
                                                morseError();
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            if (morseHistory[5] == 0) { // ....-
                                                // 4
                                                stringToSend[0] = '4';
                                            } else {
                                                morseError();
                                            }
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // ...-
                                            // V
                                            stringToSend[0] = 'v';
                                        } else if (morseHistory[4] == 1) {
                                            morseError();               // ...-.
                                        } else if (morseHistory[4] == 2) {
                                            if (morseHistory[5] == 0) { // ...--
                                                // 3
                                                stringToSend[0] = '3';
                                            } else {
                                                morseError();
                                            }
                                        }
                                    }
                                } else if (morseHistory[2] == 2) {
                                    if (morseHistory[3] == 0) { // ..-
                                        // U
                                        stringToSend[0] = 'u';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // ..-.
                                            // F
                                            stringToSend[0] = 'f';
                                        } else {
                                            morseError();
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // ..--
                                            // Just wait
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // ..--.
                                                // Just wait
                                            } else if (morseHistory[5] == 1) { // ..--..
                                                // ?
                                                stringToSend[0] = '?';
                                            } else if (morseHistory[5] == 2) { // ..--.-
                                                morseError();
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            if (morseHistory[5] == 0) { // ..---
                                                // 2
                                                stringToSend[0] = '2';
                                            } else {
                                                morseError();
                                            }
                                        }
                                    }
                                }
                            } else if (morseHistory[1] == 2) {
                                if (morseHistory[2] == 0) { // .-
                                    // A
                                    stringToSend[0] = 'a';
                                } else if (morseHistory[2] == 1) {
                                    if (morseHistory[3] == 0) { // .-.
                                        // R
                                        stringToSend[0] = 'r';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // .-..
                                            // L
                                            stringToSend[0] = 'l';
                                        } else {
                                            morseError();
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // .-.-
                                            // Just wait
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // .-.-.
                                                // Just wait
                                            } else if (morseHistory[5] == 1) { // .-.-..
                                                morseError();
                                            } else if (morseHistory[5] == 2) { // .-.-.-
                                                // .
                                                stringToSend[0] = '.';
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            morseError();               // .-.--
                                        }
                                    }
                                } else if (morseHistory[2] == 2) {
                                    if (morseHistory[3] == 0) { // .--
                                        // W
                                        stringToSend[0] = 'w';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // .--.
                                            // P
                                            stringToSend[0] = 'p';
                                        } else if (morseHistory[4] == 1) { // .--..
                                            morseError();
                                        } else if (morseHistory[4] == 2) { // .--.-
                                            if (morseHistory[5] == 0) {
                                                // @
                                                stringToSend[0] = '@';
                                            } else {
                                                morseError();
                                            }
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // .---
                                            // J
                                            stringToSend[0] = 'j';
                                        } else if (morseHistory[4] == 1) {
                                            morseError();               // .---.
                                        } else if (morseHistory[4] == 2) {
                                            if (morseHistory[5] == 0) { // .----
                                                // 1
                                                stringToSend[0] = '1';
                                            } else {
                                                morseError();
                                            }
                                        }
                                    }
                                }
                            }
                        } else if (morseHistory[0] == 2) {
                            if (morseHistory[1] == 0) { // -
                                // T
                                stringToSend[0] = 't';
                            } else if (morseHistory[1] == 1) {
                                if (morseHistory[2] == 0) { // -.
                                    // N
                                    stringToSend[0] = 'n';
                                } else if (morseHistory[2] == 1) {
                                    if (morseHistory[3] == 0) { // -..
                                        // D
                                        stringToSend[0] = 'd';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // -...
                                            // B
                                            stringToSend[0] = 'b';
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // -....
                                                // 6
                                                stringToSend[0] = '6';
                                            } else {
                                                morseError();
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            morseError();               // -...-
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // -..-
                                            // X
                                            stringToSend[0] = 'x';
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // -..-.
                                                // /
                                                stringToSend[0] = '/';
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            morseError();               // -..--
                                        }
                                    }
                                } else if (morseHistory[2] == 2) {
                                    if (morseHistory[3] == 0) { // -.-
                                        // K
                                        stringToSend[0] = 'k';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // -.-.
                                            // C
                                            stringToSend[0] = 'c';
                                        } else {
                                            morseError();
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // -.--
                                            // Y
                                            stringToSend[0] = 'y';
                                        } else {
                                            morseError();
                                        }
                                    }
                                }
                            } else if (morseHistory[1] == 2) {
                                if (morseHistory[2] == 0) { // --
                                    // M
                                    stringToSend[0] = 'm';
                                } else if (morseHistory[2] == 1) {
                                    if (morseHistory[3] == 0) { // --.
                                        // G
                                        stringToSend[0] = 'g';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // --..
                                            // Z
                                            stringToSend[0] = 'z';
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // --...
                                                // 7
                                                stringToSend[0] = '7';
                                            } else {
                                                morseError();
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            if (morseHistory[5] == 0) { // --..-
                                                // Just wait
                                            } else if (morseHistory[5] == 1) { // --..-.
                                                morseError();
                                            } else if (morseHistory[5] == 2) { // --..--
                                                // ,
                                                stringToSend[0] = ',';
                                            }
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // --.-
                                            // Q
                                            stringToSend[0] = 'q';
                                        } else {
                                            morseError();
                                        }
                                    }
                                } else if (morseHistory[2] == 2) {
                                    if (morseHistory[3] == 0) { // ---
                                        // O
                                        stringToSend[0] = 'o';
                                    } else if (morseHistory[3] == 1) {
                                        if (morseHistory[4] == 0) { // ---.
                                            // Just wait
                                        } else if (morseHistory[4] == 1) { // ---..
                                            // 8
                                            stringToSend[0] = '8';
                                        } else if (morseHistory[4] == 2) { // ---.-
                                            morseError();
                                        }
                                    } else if (morseHistory[3] == 2) {
                                        if (morseHistory[4] == 0) { // ----
                                            // Just wait
                                        } else if (morseHistory[4] == 1) {
                                            if (morseHistory[5] == 0) { // ----.
                                                // 9
                                                stringToSend[0] = '9';
                                            } else {
                                                morseError();
                                            }
                                        } else if (morseHistory[4] == 2) {
                                            if (morseHistory[5] == 0) { // -----
                                                // 0
                                                stringToSend[0] = '0';
                                            } else {
                                                morseError();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (morseHistory[1]) {
                            SEND_STRING(SS_TAP(X_BSPACE));
                        }
                        send_string(stringToSend);
                        break;
                    case MODE_COMPLIMENTS:
                        if (timeElapsed < MENU_LENGTH) {
                            // Normal press
                            strcpy(sendingThis, ch_arr[0]);
                            send_string(sendingThis);
                        } else if (timeElapsed < RESET_LENGTH) {
                            // Enter menu
                            show_menu();
                        } else {
                            reset_keyboard();
                        }
                        break;
                    case MODE_LIGHTS:
                        mode = MODE_KEYBOARD;
                        SEND_STRING("TBA" SS_TAP(X_ENTER));
                        break;
                    default:
                        break;
                }
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
