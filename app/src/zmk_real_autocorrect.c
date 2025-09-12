/*
 * Simple ZMK Autocorrect - teh -> the
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <dt-bindings/zmk/keys.h>

// Correction table with multiple typos per word
struct correction {
    const char* typo1;
    const char* typo2;
    const char* fix;
    int typo1_len;
    int typo2_len;
};

static const struct correction corrections[] = {
    {"teh", "hte", "the", 3, 3},           // the
    {"adn", "nad", "and", 3, 3},           // and  
    {"yuo", "ouy", "you", 3, 3},           // you
    {"fo", "ov", "of", 2, 2},              // of
    {"taht", "thta", "that", 4, 4},        // that
    {"thsi", "tihs", "this", 4, 4},        // this
    {"frmo", "form", "from", 4, 4},        // from
    {"whcih", "wich", "which", 5, 4},      // which
    {"recieve", "recive", "receive", 7, 6} // receive
};

#define NUM_CORRECTIONS (sizeof(corrections) / sizeof(corrections[0]))
#define MAX_WORD_LEN 8  // Longest typo is 7 chars + space

// Buffer to track recent characters
static char buffer[MAX_WORD_LEN] = {0};
static bool fixing = false;

// Convert key to char
static char key_to_char(zmk_key_t key) {
    if (key >= HID_USAGE_KEY_KEYBOARD_A && key <= HID_USAGE_KEY_KEYBOARD_Z) {
        return 'a' + (key - HID_USAGE_KEY_KEYBOARD_A);
    }
    if (key == HID_USAGE_KEY_KEYBOARD_SPACEBAR) return ' ';
    return 0;
}

// Send key event
static void send_key_event(zmk_key_t key, bool pressed) {
    raise_zmk_keycode_state_changed_from_encoded(key, pressed, k_uptime_get());
}

// Type a string character by character
static void type_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        zmk_key_t key = 0;
        
        if (c >= 'a' && c <= 'z') {
            key = HID_USAGE_KEY_KEYBOARD_A + (c - 'a');
        }
        
        if (key != 0) {
            send_key_event(key, true);
            send_key_event(key, false);
            k_msleep(10);
        }
    }
}

// Perform the correction for any word
static void do_correction_typed(const char* fix_word, int typo_len) {
    if (fixing) return;
    fixing = true;
    
    // Delete the typo (typo_len backspaces) - leave the space
    for (int i = 0; i < typo_len; i++) {
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true);
        k_msleep(10);
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false);
        k_msleep(10);
    }
    
    // Type the correction (without space - original space is still there)
    type_string(fix_word);
    
    // Reset
    memset(buffer, 0, sizeof(buffer));
    fixing = false;
}

// Track keystrokes
int zmk_autocorrect_keyboard_press(zmk_key_t key) {
    if (fixing) return 0;
    
    char c = key_to_char(key);
    if (c == 0) return 0;
    
    // Shift buffer and add new char
    memmove(buffer, buffer + 1, MAX_WORD_LEN - 1);
    buffer[MAX_WORD_LEN - 1] = c;
    
    // Check for corrections when space is pressed
    if (c == ' ') {
        for (int i = 0; i < NUM_CORRECTIONS; i++) {
            const struct correction* corr = &corrections[i];
            
            // Check typo1
            int check_len1 = corr->typo1_len + 1; // typo + space
            if (check_len1 <= MAX_WORD_LEN) {
                int start_pos1 = MAX_WORD_LEN - check_len1;
                if (strncmp(&buffer[start_pos1], corr->typo1, corr->typo1_len) == 0 && 
                    buffer[MAX_WORD_LEN - 1] == ' ') {
                    do_correction_typed(corr->fix, corr->typo1_len);
                    return 0;
                }
            }
            
            // Check typo2
            int check_len2 = corr->typo2_len + 1; // typo + space
            if (check_len2 <= MAX_WORD_LEN) {
                int start_pos2 = MAX_WORD_LEN - check_len2;
                if (strncmp(&buffer[start_pos2], corr->typo2, corr->typo2_len) == 0 && 
                    buffer[MAX_WORD_LEN - 1] == ' ') {
                    do_correction_typed(corr->fix, corr->typo2_len);
                    return 0;
                }
            }
        }
    }
    
    return 0;
}

// Initialize
static int autocorrect_init(const struct device *dev) {
    memset(buffer, 0, sizeof(buffer));
    fixing = false;
    return 0;
}

SYS_INIT(autocorrect_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
