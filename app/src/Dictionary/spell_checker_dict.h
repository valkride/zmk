#ifndef SPELL_CHECKER_DICT_H
#define SPELL_CHECKER_DICT_H

#include "dictionary_lookup.h"

// Integration with existing spell checker system
// This replaces the old dict_a.h through dict_z.h approach

static bool spell_check_word(const char* word) {
    if (!word || !*word) return false;
    
    // Convert to lowercase for lookup
    char lowercase_word[64];  // Assuming max word length
    size_t len = strlen(word);
    if (len >= sizeof(lowercase_word)) return false;
    
    for (size_t i = 0; i < len; i++) {
        char c = word[i];
        if (c >= 'A' && c <= 'Z') {
            lowercase_word[i] = c - 'A' + 'a';
        } else if (c >= 'a' && c <= 'z') {
            lowercase_word[i] = c;
        } else {
            return false;  // Invalid character
        }
    }
    lowercase_word[len] = '\0';
    
    return dictionary_contains_word(lowercase_word);
}

// Compatibility with old system
static bool is_word_in_dictionary(const char* word) {
    return spell_check_word(word);
}

#endif // SPELL_CHECKER_DICT_H
