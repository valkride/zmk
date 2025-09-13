/*
 * Performance: handles 120+ WPM typing
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <string.h>
#include <stdlib.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <zmk/spell_checker.h>
#include "Dictionary/spell_dictionary_map.h"
#define MAX_WORD_LEN 15
#define MAX_EDIT_DISTANCE 2  // Allow up to 2 character errors
#define FAST_TYPER_MODE 1    // Enable optimizations for fast typing
#define MIN_WORD_LENGTH 2    // Don't correct very short words
#define TYPING_TIMEOUT_MS 500  // Consider word complete after this timeout

// Global enable/disable flag
static bool spell_checker_enabled = true;
static int64_t last_keypress_time = 0;

// Buffer for word extraction
static char current_word[MAX_WORD_LEN] = {0};
static int word_pos = 0;
static bool correcting = false;

#if FAST_TYPER_MODE
// Enhanced cache for recent lookups (simple LRU cache)
#define CACHE_SIZE 16
static struct {
    char word[MAX_WORD_LEN];
    const char* result;
    bool valid;
    uint32_t access_count;  // For LRU tracking
} lookup_cache[CACHE_SIZE];
static int cache_index = 0;
static uint32_t cache_access_counter = 0;

// Check cache for recent lookup
static const char* check_cache(const char* word) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (lookup_cache[i].valid && strcmp(lookup_cache[i].word, word) == 0) {
            lookup_cache[i].access_count = ++cache_access_counter;  // Update LRU
            return lookup_cache[i].result;
        }
    }
    return NULL;
}

// Add to cache with LRU replacement
static void add_to_cache(const char* word, const char* result) {
    // Find LRU slot if cache is full
    int target_slot = cache_index;
    if (cache_access_counter >= CACHE_SIZE) {
        uint32_t min_access = UINT32_MAX;
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (!lookup_cache[i].valid || lookup_cache[i].access_count < min_access) {
                min_access = lookup_cache[i].access_count;
                target_slot = i;
            }
        }
    }
    
    strncpy(lookup_cache[target_slot].word, word, MAX_WORD_LEN - 1);
    lookup_cache[target_slot].word[MAX_WORD_LEN - 1] = '\0';
    lookup_cache[target_slot].result = result;
    lookup_cache[target_slot].valid = true;
    lookup_cache[target_slot].access_count = ++cache_access_counter;
    
    cache_index = (cache_index + 1) % CACHE_SIZE;
}

// Capitalization correction rules (standalone "i" handled separately in process_word)
static const struct {
    const char* incorrect;
    const char* correct;
} capitalization_rules[] = {
    {"i'm", "I'm"},
    {"i'll", "I'll"},
    {"i've", "I've"},
    {"i'd", "I'd"},
};
#define CAPITALIZATION_RULES_SIZE (sizeof(capitalization_rules) / sizeof(capitalization_rules[0]))

// Check for capitalization/contraction corrections
static const char* check_capitalization(const char* word) {
    for (int i = 0; i < CAPITALIZATION_RULES_SIZE; i++) {
        if (strcmp(word, capitalization_rules[i].incorrect) == 0) {
            return capitalization_rules[i].correct;
        }
    }
    return NULL;
}

// Check if word should be capitalized at sentence start
static bool should_capitalize_sentence_start = true;  // Start with true (beginning of text)

// Capitalize first letter of a word
static char* capitalize_first_letter(const char* word, char* buffer, int buffer_size) {
    if (word == NULL || buffer == NULL || buffer_size < 2) return NULL;
    
    int len = strlen(word);
    if (len >= buffer_size) len = buffer_size - 1;
    
    strncpy(buffer, word, len);
    buffer[len] = '\0';
    
    if (len > 0 && buffer[0] >= 'a' && buffer[0] <= 'z') {
        buffer[0] = buffer[0] - 'a' + 'A';  // Convert to uppercase
    }
    
    return buffer;
}

// Forward declaration
static int levenshtein_distance(const char* s1, const char* s2);

// Common word patterns for ultra-fast checking
static const char* common_patterns[] = {
    "the", "and", "for", "are", "but", "not", "you", "all", "can", "had", "her", "was", "one", "our", "out", "day", "get", "has", "him", "his", "how", "man", "new", "now", "old", "see", "two", "way", "who", "boy", "did", "its", "let", "put", "say", "she", "too", "use"
};
#define COMMON_PATTERNS_SIZE (sizeof(common_patterns) / sizeof(common_patterns[0]))

// Fast check for common words (O(1) for most common typos)
static const char* check_common_patterns(const char* word) {
    int word_len = strlen(word);
    
    for (int i = 0; i < COMMON_PATTERNS_SIZE; i++) {
        const char* pattern = common_patterns[i];
        int pattern_len = strlen(pattern);
        
        // Quick length check first
        if (abs(word_len - pattern_len) <= MAX_EDIT_DISTANCE) {
            int distance = levenshtein_distance(word, pattern);
            if (distance <= MAX_EDIT_DISTANCE) {
                return pattern;
            }
        }
    }
    return NULL;
}
#endif

// Levenshtein distance calculation
static int min3(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

static int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // Quick optimization: if length difference is too large, skip
    if (abs(len1 - len2) > MAX_EDIT_DISTANCE) {
        return MAX_EDIT_DISTANCE + 1;
    }
    
    #if FAST_TYPER_MODE
    // Ultra-fast path: exact match
    if (len1 == len2 && strcmp(s1, s2) == 0) return 0;
    
    // Fast path: check for single character differences first
    if (len1 == len2) {
        int diff_count = 0;
        for (int i = 0; i < len1; i++) {
            if (s1[i] != s2[i]) {
                diff_count++;
                if (diff_count > MAX_EDIT_DISTANCE) return MAX_EDIT_DISTANCE + 1;
            }
        }
        return diff_count;
    }
    
    // Fast path: check for single insertion/deletion
    if (abs(len1 - len2) == 1) {
        const char* shorter = (len1 < len2) ? s1 : s2;
        const char* longer = (len1 < len2) ? s2 : s1;
        int short_len = (len1 < len2) ? len1 : len2;
        
        int i = 0, j = 0, diff_count = 0;
        while (i < short_len && j < short_len + 1) {
            if (shorter[i] != longer[j]) {
                diff_count++;
                if (diff_count > MAX_EDIT_DISTANCE) return MAX_EDIT_DISTANCE + 1;
                if (i == j) j++; // Skip character in longer string
                else i++; // Skip character in shorter string
            } else {
                i++; j++;
            }
        }
        return diff_count;
    }
    
    // Fast path: check common transposition errors (swap adjacent characters)
    if (len1 == len2 && len1 >= 2) {
        for (int i = 0; i < len1 - 1; i++) {
            if (s1[i] == s2[i + 1] && s1[i + 1] == s2[i]) {
                // Found transposition, check if rest matches
                bool rest_matches = true;
                for (int j = 0; j < len1; j++) {
                    if (j == i || j == i + 1) continue;
                    if (s1[j] != s2[j]) {
                        rest_matches = false;
                        break;
                    }
                }
                if (rest_matches) return 1; // Single transposition
            }
        }
    }
    #endif
    
    // Use static array to avoid dynamic allocation
    static int matrix[MAX_WORD_LEN + 1][MAX_WORD_LEN + 1];
    
    // Initialize first row and column
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    // Fill the matrix with early termination
    for (int i = 1; i <= len1; i++) {
        #if FAST_TYPER_MODE
        // Early termination: if minimum possible distance exceeds threshold
        int min_in_row = MAX_EDIT_DISTANCE + 1;
        #endif
        
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = min3(
                matrix[i-1][j] + 1,     // deletion
                matrix[i][j-1] + 1,     // insertion
                matrix[i-1][j-1] + cost // substitution
            );
            
            #if FAST_TYPER_MODE
            if (matrix[i][j] < min_in_row) min_in_row = matrix[i][j];
            #endif
        }
        
        #if FAST_TYPER_MODE
        // Early termination: if all values in this row exceed threshold
        if (min_in_row > MAX_EDIT_DISTANCE) {
            return MAX_EDIT_DISTANCE + 1;
        }
        #endif
    }
    
    return matrix[len1][len2];
}

// Find best match in dictionary
static const char* find_best_match(const char* word) {
    #if FAST_TYPER_MODE
    // Check cache first for fast repeat lookups
    const char* cached_result = check_cache(word);
    if (cached_result != NULL) {
        return cached_result;
    }
    
    // Ultra-fast capitalization correction (highest priority)
    const char* cap_result = check_capitalization(word);
    if (cap_result != NULL) {
        add_to_cache(word, cap_result);
        return cap_result;
    }
    
    // Ultra-fast check for common patterns
    const char* common_result = check_common_patterns(word);
    if (common_result != NULL) {
        add_to_cache(word, common_result);
        return common_result;
    }
    #endif
    
    int best_distance = MAX_EDIT_DISTANCE + 1;
    const char* best_match = NULL;
    int word_len = strlen(word);
    
    // Don't correct very short words
    if (word_len < MIN_WORD_LENGTH) {
        return NULL;
    }
    
    // Get dictionary for the first letter of the word
    const dictionary_entry_t* dict_entry = get_dictionary_for_letter(word[0]);
    
    if (dict_entry && dict_entry->words) {
        #if FAST_TYPER_MODE
        // First pass: exact matches only (fastest possible)
        for (size_t i = 0; i < dict_entry->size; i++) {
            const char* candidate = dict_entry->words[i];
            if (strlen(candidate) == word_len && strcmp(word, candidate) == 0) {
                add_to_cache(word, candidate);
                return candidate; // Exact match found
            }
        }
        
        // Second pass: same length with 1 error (very fast)
        for (size_t i = 0; i < dict_entry->size; i++) {
            const char* candidate = dict_entry->words[i];
            int candidate_len = strlen(candidate);
            
            if (candidate_len == word_len) {
                // Quick single-difference check for same length
                int diff_count = 0;
                for (int j = 0; j < word_len; j++) {
                    if (word[j] != candidate[j]) {
                        diff_count++;
                        if (diff_count > 1) break; // More than 1 difference
                    }
                }
                
                if (diff_count == 1) {
                    best_distance = 1;
                    best_match = candidate;
                    break; // Single character difference is good enough
                }
            }
        }
        
        // Third pass: check length +/- 1 with edit distance (if no single-char match)
        if (best_match == NULL) {
            for (size_t i = 0; i < dict_entry->size; i++) {
                const char* candidate = dict_entry->words[i];
                int candidate_len = strlen(candidate);
                
                // Only check words with length difference of 1
                if (abs(candidate_len - word_len) == 1) {
                    int distance = levenshtein_distance(word, candidate);
                    if (distance <= MAX_EDIT_DISTANCE && distance < best_distance) {
                        best_distance = distance;
                        best_match = candidate;
                        
                        // Single insertion/deletion is good enough
                        if (distance == 1) break;
                    }
                }
            }
        }
        
        // Fourth pass: full edit distance only if necessary (slowest)
        if (best_match == NULL) {
        #endif
            for (size_t i = 0; i < dict_entry->size; i++) {
                const char* candidate = dict_entry->words[i];
                int candidate_len = strlen(candidate);
                
                #if FAST_TYPER_MODE
                // Skip if already checked in previous passes
                if (abs(candidate_len - word_len) <= 1) continue;
                #endif
                
                // Skip if length difference exceeds edit distance
                if (abs(candidate_len - word_len) > MAX_EDIT_DISTANCE) continue;
                
                int distance = levenshtein_distance(word, candidate);
                if (distance <= MAX_EDIT_DISTANCE && distance < best_distance) {
                    best_distance = distance;
                    best_match = candidate;
                }
            }
        #if FAST_TYPER_MODE
        }
        
        // Cache the result for future lookups
        if (best_match != NULL) {
            add_to_cache(word, best_match);
        }
        #endif
    }
    
    return best_match;
}

// Check if word exists in dictionary (exact match)
static bool is_valid_word(const char* word) {
    // Get dictionary for the first letter of the word
    const dictionary_entry_t* dict_entry = get_dictionary_for_letter(word[0]);
    
    if (dict_entry && dict_entry->words) {
        int word_len = strlen(word);
        
        #if FAST_TYPER_MODE
        // Optimized search: first check length then compare
        for (size_t i = 0; i < dict_entry->size; i++) {
            const char* candidate = dict_entry->words[i];
            
            // Quick length check first (faster than full strcmp)
            if (strlen(candidate) != word_len) continue;
            
            // Only do full comparison if lengths match
            if (strcmp(word, candidate) == 0) {
                return true;
            }
        }
        #else
        // Original implementation
        for (size_t i = 0; i < dict_entry->size; i++) {
            if (strcmp(word, dict_entry->words[i]) == 0) {
                return true;
            }
        }
        #endif
    }
    return false;
}

// Convert key to char
static char key_to_char(zmk_key_t key) {
    if (key >= HID_USAGE_KEY_KEYBOARD_A && key <= HID_USAGE_KEY_KEYBOARD_Z) {
        return 'a' + (key - HID_USAGE_KEY_KEYBOARD_A);
    }
    if (key == HID_USAGE_KEY_KEYBOARD_SPACEBAR) return ' ';
    if (key == HID_USAGE_KEY_KEYBOARD_PERIOD_AND_GREATER_THAN) return '.';
    if (key == HID_USAGE_KEY_KEYBOARD_COMMA_AND_LESS_THAN) return ',';
    if (key == HID_USAGE_KEY_KEYBOARD_RETURN_ENTER) return '\n';
    if (key == HID_USAGE_KEY_KEYBOARD_TAB) return '\t';
    if (key == HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION) return '!';  // Shift+1 for !
    if (key == HID_USAGE_KEY_KEYBOARD_SLASH_AND_QUESTION_MARK) return '?';  // Shift+/ for ?
    if (key == HID_USAGE_KEY_KEYBOARD_APOSTROPHE_AND_QUOTE) return '\'';  // Apostrophe for contractions
    return 0;
}

// Send key event
static void send_key_event(zmk_key_t key, bool pressed) {
    raise_zmk_keycode_state_changed_from_encoded(key, pressed, k_uptime_get());
}

// Type a string
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
            #if FAST_TYPER_MODE
            k_msleep(2);  // Reduced delay for faster correction
            #else
            k_msleep(5);  // Original delay
            #endif
        }
    }
}

// Perform spell correction
static void correct_word(const char* correct_word, int typo_len) {
    if (correcting) return;
    correcting = true;
    
    #if FAST_TYPER_MODE
    // Faster deletion for quick typers
    for (int i = 0; i < typo_len; i++) {
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true);
        k_msleep(1);  // Minimal delay
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false);
        k_msleep(1);  // Minimal delay
    }
    #else
    // Original deletion speed
    for (int i = 0; i < typo_len; i++) {
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true);
        k_msleep(5);
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false);
        k_msleep(5);
    }
    #endif
    
    // Type the correction
    type_string(correct_word);
    
    correcting = false;
}

// Process completed word
static void process_word() {
    if (word_pos == 0) return;  // Empty word
    
    current_word[word_pos] = '\0';  // Null terminate
    
    // Special case: always correct standalone "i" to "I" (before length check)
    if (word_pos == 1 && current_word[0] == 'i') {
        correct_word("I", 1);  // Correcting 1 character: "i" -> "I"
        word_pos = 0;  // Reset word buffer after correction
        return;
    }
    
    #if FAST_TYPER_MODE
    // For fast typers: don't correct very short words or incomplete fragments  
    if (word_pos < MIN_WORD_LENGTH || word_pos > MAX_WORD_LEN - 1) {
        word_pos = 0;
        return;
    }
    #else
    // Original behavior: skip short and long words
    if (word_pos < 3 || word_pos > MAX_WORD_LEN - 1) {
        word_pos = 0;
        return;
    }
    #endif
    
    // Handle sentence capitalization
    const char* final_correction = NULL;
    
    if (should_capitalize_sentence_start) {
        // Check if we need to capitalize the first letter
        static char capitalized_buffer[MAX_WORD_LEN];
        char* capitalized = capitalize_first_letter(current_word, capitalized_buffer, MAX_WORD_LEN);
        
        if (capitalized && strcmp(current_word, capitalized) != 0) {
            // Check if the capitalized version is valid or if we should correct it
            if (is_valid_word(capitalized)) {
                final_correction = capitalized;
            } else {
                // Try to find correction for the capitalized version
                const char* cap_correction = find_best_match(capitalized);
                if (cap_correction) {
                    final_correction = cap_correction;
                }
            }
        }
        should_capitalize_sentence_start = false;  // Reset after first word
    }
    
    // If no sentence capitalization needed, check normal correction
    if (final_correction == NULL) {
        // Check if word is already correct (fast path)
        if (is_valid_word(current_word)) {
            word_pos = 0;
            return;
        }
        
        // Find best correction
        final_correction = find_best_match(current_word);
    }
    
    // Apply correction if found
    if (final_correction != NULL && strcmp(current_word, final_correction) != 0) {
        correct_word(final_correction, word_pos);
    }
    
    word_pos = 0;  // Reset for next word
}

#if FAST_TYPER_MODE
// Timeout-based word processing for fast typers
static void timeout_process_word(struct k_work *work) {
    if (word_pos > 0) {
        int64_t time_since_last = k_uptime_get() - last_keypress_time;
        
        // If enough time has passed, process the current word
        if (time_since_last >= TYPING_TIMEOUT_MS) {
            process_word();
        }
    }
}

// Define work item for timeout processing
K_WORK_DELAYABLE_DEFINE(timeout_work, timeout_process_word);
#endif

// Toggle function for enabling/disabling spell checker
void zmk_spell_checker_toggle(void) {
    spell_checker_enabled = !spell_checker_enabled;
}

// Get current state
bool zmk_spell_checker_is_enabled(void) {
    return spell_checker_enabled;
}

// Main keystroke handler
int zmk_autocorrect_keyboard_press(zmk_key_t key) {
    // Check for F24 key (our toggle trigger)
    if (key == HID_USAGE_KEY_KEYBOARD_F24) {
        spell_checker_enabled = !spell_checker_enabled;
        return 0;
    }
    
    if (correcting || !spell_checker_enabled) return 0;
    
    #if FAST_TYPER_MODE
    // Update timing for potential future features (keeping for compatibility)
    last_keypress_time = k_uptime_get();
    #endif
    
    char c = key_to_char(key);
    if (c == 0) return 0;  // Unsupported key
    
    // Word boundary characters
    if (c == ' ' || c == '.' || c == ',' || c == '\n' || c == '\t' || c == '!' || c == '?') {
        // Check if this is a standalone "i" before processing
        bool was_standalone_i = (word_pos == 1 && current_word[0] == 'i');
        
        process_word();  // Check and correct the completed word immediately
        
        // Check for sentence boundaries (next word should be capitalized)
        if (c == '.' || c == '!' || c == '?' || c == '\n') {
            should_capitalize_sentence_start = true;
        }
        
        // If we corrected "i" to "I", we need to add the space manually since correction consumed it
        if (was_standalone_i && c == ' ') {
            send_key_event(HID_USAGE_KEY_KEYBOARD_SPACEBAR, true);
            send_key_event(HID_USAGE_KEY_KEYBOARD_SPACEBAR, false);
        }
        
        return 0;
    }
    
    // Add letter or apostrophe to current word
    if ((c >= 'a' && c <= 'z') || c == '\'' && word_pos < MAX_WORD_LEN - 1) {
        current_word[word_pos++] = c;
        
        // Note: Timeout-based correction removed to prevent interrupting typing flow
        // Only correct when explicit word boundaries (space, punctuation) are hit
    }
    
    return 0;
}

// Initialize spell checker
static int spell_checker_init(void) {
    memset(current_word, 0, sizeof(current_word));
    word_pos = 0;
    correcting = false;
    spell_checker_enabled = true;
    last_keypress_time = 0;
    
    #if FAST_TYPER_MODE
    // Initialize cache
    memset(lookup_cache, 0, sizeof(lookup_cache));
    cache_index = 0;
    cache_access_counter = 0;
    #endif
    
    return 0;
}

SYS_INIT(spell_checker_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
