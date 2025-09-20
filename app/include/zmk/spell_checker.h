#ifndef ZMK_SPELL_CHECKER_H
#define ZMK_SPELL_CHECKER_H

#include <stdbool.h>
#include <zmk/hid.h>

/**
 * Main keystroke handler for spell checking
 * Called by ZMK's HID system for each key press
 * @param key The key code that was pressed
 * @return 0 on success
 */
int zmk_autocorrect_keyboard_press(zmk_key_t key);

/**
 * Toggle spell checker on/off
 */
void zmk_spell_checker_toggle(void);

/**
 * Check if spell checker is currently enabled
 * @return true if enabled, false if disabled
 */
bool zmk_spell_checker_is_enabled(void);

#endif // ZMK_SPELL_CHECKER_H
