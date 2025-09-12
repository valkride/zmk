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

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Simple tracking for "teh " pattern
static char buffer[5] = {0};  // Track last 4 chars
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

// Perform the correction
static void do_correction(void) {
    if (fixing) return;
    fixing = true;
    
    LOG_INF("AUTOCORRECT: teh -> the");
    
    // Delete "teh " (4 backspaces)
    for (int i = 0; i < 4; i++) {
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true);
        k_msleep(10);
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false);
        k_msleep(10);
    }
    
    // Type "the "
    send_key_event(HID_USAGE_KEY_KEYBOARD_T, true);
    send_key_event(HID_USAGE_KEY_KEYBOARD_T, false);
    k_msleep(10);
    send_key_event(HID_USAGE_KEY_KEYBOARD_H, true);
    send_key_event(HID_USAGE_KEY_KEYBOARD_H, false);
    k_msleep(10);
    send_key_event(HID_USAGE_KEY_KEYBOARD_E, true);
    send_key_event(HID_USAGE_KEY_KEYBOARD_E, false);
    k_msleep(10);
    send_key_event(HID_USAGE_KEY_KEYBOARD_SPACEBAR, true);
    send_key_event(HID_USAGE_KEY_KEYBOARD_SPACEBAR, false);
    
    // Reset
    memset(buffer, 0, sizeof(buffer));
    fixing = false;
    
    LOG_INF("Correction done");
}

// Track keystrokes
int zmk_autocorrect_keyboard_press(zmk_key_t key) {
    if (fixing) return 0;
    
    char c = key_to_char(key);
    if (c == 0) return 0;
    
    // Shift buffer and add new char
    memmove(buffer, buffer + 1, 3);
    buffer[3] = c;
    
    // Check for "teh " when space is pressed
    if (c == ' ' && strncmp(buffer, "teh ", 4) == 0) {
        do_correction();
    }
    
    return 0;
}

// Initialize
static int autocorrect_init(const struct device *dev) {
    LOG_INF("Autocorrect initialized: teh -> the");
    memset(buffer, 0, sizeof(buffer));
    fixing = false;
    return 0;
}

SYS_INIT(autocorrect_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
