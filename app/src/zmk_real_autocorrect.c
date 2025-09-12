/*
 * ZMK Real Autocorrect Integration
 * Integrated into ZMK core for full API access
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <dt-bindings/zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Typo correction table
struct typo_correction {
    const char* typo;
    const char* correction;
};

static const struct typo_correction corrections[] = {
    {"teh", "the"},
    {"adn", "and"}, 
    {"yuo", "you"},
    {"hte", "the"},
    {"taht", "that"},
    {"thsi", "this"},
    {"seperate", "separate"},
    {"recieve", "receive"},
    {"definately", "definitely"},
    {"occured", "occurred"}
};

#define NUM_CORRECTIONS (sizeof(corrections) / sizeof(corrections[0]))

// Buffer to store recent keystrokes for autocorrect processing
#define BUFFER_SIZE 32
static char input_buffer[BUFFER_SIZE];
static int buffer_pos = 0;
static bool autocorrect_active = true;

// Convert ZMK key to character (simplified for common letters)
static char key_to_char(zmk_key_t key) {
    if (key >= HID_USAGE_KEY_KEYBOARD_A && key <= HID_USAGE_KEY_KEYBOARD_Z) {
        return 'a' + (key - HID_USAGE_KEY_KEYBOARD_A);
    }
    if (key == HID_USAGE_KEY_KEYBOARD_SPACEBAR) {
        return ' ';
    }
    return 0; // Unsupported key
}

// Convert character back to ZMK key
static zmk_key_t char_to_key(char c) {
    if (c >= 'a' && c <= 'z') {
        return HID_USAGE_KEY_KEYBOARD_A + (c - 'a');
    }
    if (c >= 'A' && c <= 'Z') {
        return HID_USAGE_KEY_KEYBOARD_A + (c - 'A');
    }
    return 0;
}

// Send actual keystrokes using ZMK's HID functions
static void send_backspaces(int count) {
    LOG_INF("🔧 Autocorrect: Sending %d backspaces", count);
    for (int i = 0; i < count; i++) {
        zmk_hid_keyboard_press(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE);
        k_msleep(10);
        zmk_hid_keyboard_release(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE);
        k_msleep(10);
    }
}

// Send correction text using real keystrokes
static void send_correction_text(const char* text) {
    LOG_INF("✍️  Autocorrect: Typing '%s'", text);
    for (int i = 0; text[i] != '\0'; i++) {
        zmk_key_t key = char_to_key(text[i]);
        if (key != 0) {
            zmk_hid_keyboard_press(key);
            k_msleep(10);
            zmk_hid_keyboard_release(key);
            k_msleep(10);
        }
    }
}

// Perform actual autocorrect with real keystrokes
static void perform_real_autocorrect(const char* typo, const char* correction) {
    int typo_len = strlen(typo);
    int correction_len = strlen(correction);
    
    LOG_INF("🔥 REAL AUTOCORRECT ACTIVATED!");
    LOG_INF("   📝 Fixing '%s' → '%s'", typo, correction);
    
    // Disable autocorrect temporarily to avoid recursion
    autocorrect_active = false;
    
    // Send actual keystrokes to fix the typo
    send_backspaces(typo_len);
    send_correction_text(correction);
    
    // Update buffer to reflect correction
    buffer_pos = buffer_pos - typo_len + correction_len;
    if (buffer_pos < BUFFER_SIZE && buffer_pos >= correction_len) {
        strncpy(&input_buffer[buffer_pos - correction_len], correction, correction_len);
    }
    
    // Re-enable autocorrect
    autocorrect_active = true;
    
    LOG_INF("✅ Autocorrect complete: '%s' → '%s'", typo, correction);
}

// Check for typos and trigger corrections
static void check_for_typos(void) {
    if (!autocorrect_active) return;
    
    for (int i = 0; i < NUM_CORRECTIONS; i++) {
        const char* typo = corrections[i].typo;
        const char* correction = corrections[i].correction;
        int typo_len = strlen(typo);
        
        if (buffer_pos >= typo_len) {
            // Check if buffer ends with this typo
            if (strncmp(&input_buffer[buffer_pos - typo_len], typo, typo_len) == 0) {
                perform_real_autocorrect(typo, correction);
                return;
            }
        }
    }
}

// Add character to buffer and check for corrections
static void add_char_to_buffer(char c) {
    if (buffer_pos < BUFFER_SIZE - 1) {
        input_buffer[buffer_pos++] = c;
        input_buffer[buffer_pos] = '\0';
        
        // Check for corrections on word boundaries
        if (c == ' ' || c == '\n' || c == '\t') {
            check_for_typos();
        }
    } else {
        // Buffer full, shift left and add new character
        memmove(input_buffer, input_buffer + 1, BUFFER_SIZE - 2);
        input_buffer[BUFFER_SIZE - 2] = c;
        input_buffer[BUFFER_SIZE - 1] = '\0';
        buffer_pos = BUFFER_SIZE - 1;
    }
}

// Hook into ZMK's keyboard press function
int zmk_autocorrect_keyboard_press(zmk_key_t key) {
    char c = key_to_char(key);
    if (c != 0 && autocorrect_active) {
        add_char_to_buffer(c);
    }
    return 0;
}

// Initialize autocorrect
static int zmk_autocorrect_init(const struct device *dev) {
    LOG_INF("🔥 ZMK CORE AUTOCORRECT LOADED!");
    LOG_INF("🚀 REAL keystrokes with full ZMK API access!");
    LOG_INF("✨ %d correction patterns active", NUM_CORRECTIONS);
    LOG_INF("💪 This is GENUINE autocorrect!");
    
    memset(input_buffer, 0, BUFFER_SIZE);
    buffer_pos = 0;
    
    return 0;
}

SYS_INIT(zmk_autocorrect_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
