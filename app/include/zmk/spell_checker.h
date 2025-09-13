#ifndef ZMK_SPELL_CHECKER_H
#define ZMK_SPELL_CHECKER_H

#include <stdbool.h>

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
