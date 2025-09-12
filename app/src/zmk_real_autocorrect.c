/*
 * ZMK Real Autocorrect Integration
 * Simple and working autocorrect for "teh" -> "the"
 */

#include <zep// Initialize autocorrect
static int zmk_autocorrect_init(const struct device *dev) {
    LOG_INF("🔥 SIMPLE AUTOCORRECT ACTIVE!");
    LOG_INF("📝 Will fix: teh -> the");
    
    // Initialize work item
    k_work_init(&correction_work, correction_work_handler);
    
    memset(last_chars, 0, sizeof(last_chars));
    correction_in_progress = false;
    
    return 0;
}.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <dt-bindings/zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Simple buffer to track last 4 characters for "teh " detection
static char last_chars[5] = {0}; // "teh " + null terminator
static bool correction_in_progress = false;

// Convert ZMK key to character
static char key_to_char(zmk_key_t key) {
    if (key >= HID_USAGE_KEY_KEYBOARD_A && key <= HID_USAGE_KEY_KEYBOARD_Z) {
        return 'a' + (key - HID_USAGE_KEY_KEYBOARD_A);
    }
    if (key == HID_USAGE_KEY_KEYBOARD_SPACEBAR) {
        return ' ';
    }
    return 0;
}

// Send keycode events using the correct ZMK API
static void send_backspace() {
    int64_t timestamp = k_uptime_get();
    
    // Press backspace
    raise_zmk_keycode_state_changed_from_encoded(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true, timestamp);
    k_msleep(20);
    
    // Release backspace  
    raise_zmk_keycode_state_changed_from_encoded(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false, timestamp + 20);
    k_msleep(20);
}

static void send_key(zmk_key_t key) {
    int64_t timestamp = k_uptime_get();
    
    // Press key
    raise_zmk_keycode_state_changed_from_encoded(key, true, timestamp);
    k_msleep(20);
    
    // Release key
    raise_zmk_keycode_state_changed_from_encoded(key, false, timestamp + 20);
    k_msleep(20);
}

// Work item for performing correction asynchronously
static struct k_work correction_work;

static void correction_work_handler(struct k_work *work) {
    LOG_INF("🔥 AUTOCORRECT: teh -> the");
    
    // Send 4 backspaces to delete "teh "
    for (int i = 0; i < 4; i++) {
        send_backspace();
    }
    
    // Type "the "
    send_key(HID_USAGE_KEY_KEYBOARD_T);
    send_key(HID_USAGE_KEY_KEYBOARD_H); 
    send_key(HID_USAGE_KEY_KEYBOARD_E);
    send_key(HID_USAGE_KEY_KEYBOARD_SPACEBAR);
    
    // Clear buffer and reset flag
    memset(last_chars, 0, sizeof(last_chars));
    correction_in_progress = false;
    
    LOG_INF("✅ Correction complete!");
}

// Trigger correction
static void fix_teh_typo() {
    if (correction_in_progress) return;
    correction_in_progress = true;
    
    // Schedule the correction work
    k_work_submit(&correction_work);
}

// Add character and check for "teh " pattern
static void track_keystroke(char c) {
    if (correction_in_progress) return;
    
    // Shift buffer left and add new character
    memmove(last_chars, last_chars + 1, 3);
    last_chars[3] = c;
    last_chars[4] = '\0';
    
    // Check for "teh " pattern when space is pressed
    if (c == ' ' && strcmp(last_chars, "teh ") == 0) {
        fix_teh_typo();
    }
}

// Hook into ZMK's keyboard press function
int zmk_autocorrect_keyboard_press(zmk_key_t key) {
    char c = key_to_char(key);
    if (c != 0) {
        track_keystroke(c);
    }
    return 0;
}

// Initialize autocorrect
static int zmk_autocorrect_init(const struct device *dev) {
    LOG_INF("🔥 SIMPLE AUTOCORRECT ACTIVE!");
    LOG_INF("� Will fix: teh -> the");
    
    memset(last_chars, 0, sizeof(last_chars));
    correction_in_progress = false;
    
    return 0;
}

SYS_INIT(zmk_autocorrect_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
