/*
 * Performance: handles 120+ WPM typing
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <zmk/spell_checker.h>

#include "Dictionary/spell_dictionary_two_letter_map.h"

// ARM NEON support detection
#ifdef CONFIG_ARM
#ifdef __ARM_NEON
#include <arm_neon.h>
#define NEON_AVAILABLE 1
#else
#define NEON_AVAILABLE 0
#endif
#else
#define NEON_AVAILABLE 0
#endif

#define MAX_WORD_LEN 15
#define MAX_EDIT_DISTANCE 2  // Allow up to 2 character errors (balanced)

// Global enable/disable flag
static bool spell_checker_enabled = true;

// Buffer for word extraction
static char current_word[MAX_WORD_LEN] = {0};
static int word_pos = 0;
static bool correcting = false;
static uint32_t correction_start_time = 0;
static uint32_t last_keystroke_time = 0;

// Context-aware correction system
#define CONTEXT_HISTORY_SIZE 3
static struct {
    char words[CONTEXT_HISTORY_SIZE][MAX_WORD_LEN];
    int count;
    int index;
} word_context = {0};

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

// Function declarations
static int levenshtein_distance(const char* s1, const char* s2);
static bool are_keyboard_adjacent(char c1, char c2);

// Enhanced keyboard layout awareness for Keyball44
// Physical key distance matrix for QWERTY layout (approximate distances)
static float get_key_distance(char c1, char c2) {
    // Convert to lowercase for comparison
    c1 = (c1 >= 'A' && c1 <= 'Z') ? c1 + 32 : c1;
    c2 = (c2 >= 'A' && c2 <= 'Z') ? c2 + 32 : c2;
    
    if (c1 == c2) return 0.0f;
    
    // Define key positions (row, column) for QWERTY layout
    static const struct { char key; float row; float col; } key_positions[] = {
        {'q', 0, 0}, {'w', 0, 1}, {'e', 0, 2}, {'r', 0, 3}, {'t', 0, 4}, 
        {'y', 0, 5}, {'u', 0, 6}, {'i', 0, 7}, {'o', 0, 8}, {'p', 0, 9},
        {'a', 1, 0.5}, {'s', 1, 1.5}, {'d', 1, 2.5}, {'f', 1, 3.5}, {'g', 1, 4.5},
        {'h', 1, 5.5}, {'j', 1, 6.5}, {'k', 1, 7.5}, {'l', 1, 8.5},
        {'z', 2, 1}, {'x', 2, 2}, {'c', 2, 3}, {'v', 2, 4}, {'b', 2, 5},
        {'n', 2, 6}, {'m', 2, 7},
        {'\0', 0, 0} // Terminator
    };
    
    float pos1_row = -1, pos1_col = -1, pos2_row = -1, pos2_col = -1;
    
    // Find positions of both characters
    for (int i = 0; key_positions[i].key != '\0'; i++) {
        if (key_positions[i].key == c1) {
            pos1_row = key_positions[i].row;
            pos1_col = key_positions[i].col;
        }
        if (key_positions[i].key == c2) {
            pos2_row = key_positions[i].row;
            pos2_col = key_positions[i].col;
        }
    }
    
    // If either character not found in layout, return high distance  
    if (pos1_row < 0 || pos2_row < 0) return 10.0f;
    
    // Calculate Euclidean distance
    float row_diff = pos1_row - pos2_row;
    float col_diff = pos1_col - pos2_col;
    return sqrtf(row_diff * row_diff + col_diff * col_diff);
}

// Returns true if two characters are adjacent on the keyboard (distance <= 1.5)
static bool are_keyboard_adjacent(char c1, char c2) {
    return get_key_distance(c1, c2) <= 1.5f;
}

// Calculate keyboard-aware correction score (lower is better)
static float get_keyboard_aware_score(const char* word, const char* candidate) {
    int word_len = strlen(word);
    int candidate_len = strlen(candidate);
    int min_len = (word_len < candidate_len) ? word_len : candidate_len;
    
    float score = 0.0f;
    float position_weight = 1.0f; // Give more weight to early characters
    
    // Compare character by character
    for (int i = 0; i < min_len; i++) {
        if (word[i] == candidate[i]) {
            // Perfect match - no penalty
            continue;
        } else {
            // Add penalty based on keyboard distance
            float distance = get_key_distance(word[i], candidate[i]);
            score += distance * position_weight;
        }
        
        // Reduce position weight for later characters (typos early in word are more disruptive)
        position_weight *= 0.8f;
    }
    
    // Add penalty for length differences
    int length_diff = abs(word_len - candidate_len);
    score += length_diff * 2.0f; // Length differences are expensive
    
    return score;
}

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

// Capitalization and contraction correction rules only
static const struct {
    const char* incorrect;
    const char* correct;
} correction_rules[] = {
    {"i", "I"},          // Standalone lowercase "i"
    {"i'm", "I'm"},
    {"i'll", "I'll"},
    {"i've", "I've"},
    {"i'd", "I'd"},
    {"im", "I'm"},       // Common contraction without apostrophe
};
#define CORRECTION_RULES_SIZE (sizeof(correction_rules) / sizeof(correction_rules[0]))

// Check for common typo and capitalization corrections
static const char* check_corrections(const char* word) {
    for (int i = 0; i < CORRECTION_RULES_SIZE; i++) {
        if (strcmp(word, correction_rules[i].incorrect) == 0) {
            return correction_rules[i].correct;
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

// Context-aware correction functions
static void add_to_context(const char* word) {
    if (strlen(word) == 0 || strlen(word) >= MAX_WORD_LEN) return;
    
    // Add word to circular buffer
    strncpy(word_context.words[word_context.index], word, MAX_WORD_LEN - 1);
    word_context.words[word_context.index][MAX_WORD_LEN - 1] = '\0';
    
    word_context.index = (word_context.index + 1) % CONTEXT_HISTORY_SIZE;
    if (word_context.count < CONTEXT_HISTORY_SIZE) {
        word_context.count++;
    }
}

static const char* get_context_aware_correction(const char* word, const char* primary_correction) {
    if (word_context.count == 0 || !primary_correction) return primary_correction;
    
    // Get the most recent word for context
    int last_index = (word_context.index - 1 + CONTEXT_HISTORY_SIZE) % CONTEXT_HISTORY_SIZE;
    const char* prev_word = word_context.words[last_index];
    
    // Context-specific corrections based on common patterns
    if (strlen(prev_word) > 0) {
        // "I wan" -> "want" (not "wan")
        if (strcmp(prev_word, "I") == 0 && strcmp(word, "wan") == 0) {
            return "want";
        }
        
        // "to go" context
        if (strcmp(prev_word, "to") == 0) {
            if (strcmp(word, "go") == 0 || strcmp(word, "og") == 0) return "go";
            if (strcmp(word, "be") == 0 || strcmp(word, "eb") == 0) return "be";
            if (strcmp(word, "do") == 0 || strcmp(word, "od") == 0) return "do";
        }
        
        // "the" context - common article patterns
        if (strcmp(prev_word, "the") == 0) {
            if (strcmp(word, "tiem") == 0 || strcmp(word, "itme") == 0) return "time";
            if (strcmp(word, "palce") == 0 || strcmp(word, "plca") == 0) return "place";
        }
        
        // "and" context
        if (strcmp(prev_word, "and") == 0) {
            if (strcmp(word, "hte") == 0 || strcmp(word, "teh") == 0) return "the";
            if (strcmp(word, "I") == 0 || strcmp(word, "i") == 0) return "I";
        }
        
        // Question patterns
        if (strcmp(prev_word, "what") == 0) {
            if (strcmp(word, "si") == 0 || strcmp(word, "is") == 0) return "is";
        }
        
        if (strcmp(prev_word, "how") == 0) {
            if (strcmp(word, "era") == 0 || strcmp(word, "aer") == 0) return "are";
        }
    }
    
    // Check two-word context for more complex patterns
    if (word_context.count >= 2) {
        int second_last = (word_context.index - 2 + CONTEXT_HISTORY_SIZE) % CONTEXT_HISTORY_SIZE;
        const char* prev_prev_word = word_context.words[second_last];
        
        // "I want to" -> common verb patterns
        if (strcmp(prev_prev_word, "I") == 0 && strcmp(prev_word, "want") == 0) {
            if (strcmp(word, "og") == 0 || strcmp(word, "go") == 0) return "go";
            if (strcmp(word, "eb") == 0 || strcmp(word, "be") == 0) return "be";
        }
        
        // "are you" patterns
        if (strcmp(prev_prev_word, "are") == 0 && strcmp(prev_word, "you") == 0) {
            if (strcmp(word, "ko") == 0 || strcmp(word, "ok") == 0) return "ok";
        }
    }
    
    return primary_correction; // Return original correction if no context match
}

// Levenshtein distance calculation
static int min3(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

// Forward declaration for NEON version
#if NEON_AVAILABLE
static int levenshtein_distance_neon(const char* s1, const char* s2);
#endif

static int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // Quick optimization: if length difference is too large, skip
    if (abs(len1 - len2) > MAX_EDIT_DISTANCE) {
        return MAX_EDIT_DISTANCE + 1;
    }
    
    // Simple exact match check
    if (len1 == len2 && strcmp(s1, s2) == 0) return 0;
    
    // Use static array to avoid dynamic allocation
    static int matrix[MAX_WORD_LEN + 1][MAX_WORD_LEN + 1];
    
    // Initialize first row and column
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    // Fill the matrix using optimized Levenshtein algorithm with early termination
    for (int i = 1; i <= len1; i++) {
        int row_min = MAX_EDIT_DISTANCE + 1; // Track minimum in current row for early exit
        
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = min3(
                matrix[i-1][j] + 1,     // deletion
                matrix[i][j-1] + 1,     // insertion
                matrix[i-1][j-1] + cost // substitution
            );
            
            // Track minimum value in this row for early termination
            if (matrix[i][j] < row_min) {
                row_min = matrix[i][j];
            }
        }
        
        // Early termination: if minimum in this row exceeds threshold, abort
        if (row_min > MAX_EDIT_DISTANCE) {
            return MAX_EDIT_DISTANCE + 1;
        }
    }
    
    return matrix[len1][len2];
}

#if NEON_AVAILABLE
// SIMD-optimized Levenshtein distance using ARM NEON
static int levenshtein_distance_neon(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // Quick optimization: if length difference is too large, skip
    if (abs(len1 - len2) > MAX_EDIT_DISTANCE) {
        return MAX_EDIT_DISTANCE + 1;
    }
    
    // Simple exact match check
    if (len1 == len2 && strcmp(s1, s2) == 0) return 0;
    
    // For very short strings, use standard algorithm
    if (len1 < 4 || len2 < 4) {
        // Use static array to avoid dynamic allocation
        static int matrix[MAX_WORD_LEN + 1][MAX_WORD_LEN + 1];
        
        // Initialize first row and column
        for (int i = 0; i <= len1; i++) matrix[i][0] = i;
        for (int j = 0; j <= len2; j++) matrix[0][j] = j;
        
        // Standard algorithm for short strings
        for (int i = 1; i <= len1; i++) {
            int row_min = MAX_EDIT_DISTANCE + 1;
            
            for (int j = 1; j <= len2; j++) {
                int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
                matrix[i][j] = min3(
                    matrix[i-1][j] + 1,     // deletion
                    matrix[i][j-1] + 1,     // insertion
                    matrix[i-1][j-1] + cost // substitution
                );
                
                if (matrix[i][j] < row_min) {
                    row_min = matrix[i][j];
                }
            }
            
            // Early termination
            if (row_min > MAX_EDIT_DISTANCE) {
                return MAX_EDIT_DISTANCE + 1;
            }
        }
        
        return matrix[len1][len2];
    }
    
    // NEON-optimized version for longer strings
    static int matrix[MAX_WORD_LEN + 1][MAX_WORD_LEN + 1];
    
    // Initialize first row and column
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    // NEON-optimized inner loop
    for (int i = 1; i <= len1; i++) {
        int row_min = MAX_EDIT_DISTANCE + 1;
        uint8_t s1_char = (uint8_t)s1[i-1];
        
        // Process characters in groups of 8 using NEON
        int j = 1;
        for (; j <= len2 - 7; j += 8) {
            // Load 8 characters from s2
            uint8x8_t s2_chars = vld1_u8((uint8_t*)&s2[j-1]);
            
            // Broadcast current s1 character
            uint8x8_t s1_chars = vdup_n_u8(s1_char);
            
            // Compare characters (result is 0xFF for match, 0x00 for mismatch)
            uint8x8_t cmp_result = vceq_u8(s1_chars, s2_chars);
            
            // Extract comparison results and compute matrix values
            uint8_t results[8];
            vst1_u8(results, cmp_result);
            
            for (int k = 0; k < 8 && j + k <= len2; k++) {
                int cost = (results[k] == 0xFF) ? 0 : 1;
                matrix[i][j + k] = min3(
                    matrix[i-1][j + k] + 1,     // deletion
                    matrix[i][j + k - 1] + 1,   // insertion
                    matrix[i-1][j + k - 1] + cost // substitution
                );
                
                if (matrix[i][j + k] < row_min) {
                    row_min = matrix[i][j + k];
                }
            }
        }
        
        // Handle remaining characters
        for (; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = min3(
                matrix[i-1][j] + 1,     // deletion
                matrix[i][j-1] + 1,     // insertion
                matrix[i-1][j-1] + cost // substitution
            );
            
            if (matrix[i][j] < row_min) {
                row_min = matrix[i][j];
            }
        }
        
        // Early termination
        if (row_min > MAX_EDIT_DISTANCE) {
            return MAX_EDIT_DISTANCE + 1;
        }
    }
    
    return matrix[len1][len2];
}

// Wrapper function that chooses between NEON and standard implementation
static inline int levenshtein_distance_optimized(const char* s1, const char* s2) {
    return levenshtein_distance_neon(s1, s2);
}
#else
// Standard implementation when NEON is not available
static inline int levenshtein_distance_optimized(const char* s1, const char* s2) {
    return levenshtein_distance(s1, s2);
}
#endif

// Check if word needs common corrections (typos, capitalization, contractions)
static const char* get_correction(const char* word) {
    // Always check common corrections first, even for valid dictionary words
    const char* correction_result = check_corrections(word);
    if (correction_result != NULL) {
        return correction_result;
    }
    return NULL;
}

// Find best match in dictionary
static const char* find_best_match(const char* word) {
    // Check cache first for fast repeat lookups
    const char* cached_result = check_cache(word);
    if (cached_result != NULL) {
        return cached_result;
    }
    
    // Ultra-fast capitalization correction (highest priority) - always check this first
    const char* correction_result = get_correction(word);
    if (correction_result != NULL) {
        add_to_cache(word, correction_result);
        return correction_result;
    }
    
    const char* best_match = NULL;
    int word_len = strlen(word);
    
    // Balanced approach: be careful with longer words but still correct obvious typos
    if (word_len > 8) {
        return NULL;  // Don't correct very long words
    }
    
    // Don't correct words that start with uppercase (might be proper nouns)
    // But allow corrections for obvious typos in capitalized words at sentence start
    if (word[0] >= 'A' && word[0] <= 'Z' && !should_capitalize_sentence_start) {
        return NULL;
    }
    
    // Look for similar words using two-letter dictionary approach with keyboard awareness
    int best_distance = MAX_EDIT_DISTANCE + 1;
    float best_keyboard_score = 1000.0f; // Lower is better
    
    // Helper macro to search a specific dictionary with aggressive pre-filtering and keyboard awareness
    #define SEARCH_DICTIONARY(dict_entry) do { \
        if ((dict_entry) && (dict_entry)->words) { \
            for (size_t i = 0; i < (dict_entry)->size; i++) { \
                const char* candidate = (dict_entry)->words[i]; \
                int candidate_len = strlen(candidate); \
                \
                /* Aggressive length-based pre-filtering */ \
                int max_length_diff; \
                if (word_len <= 3) { \
                    max_length_diff = 1; /* Very short words: max 1 char difference */ \
                } else if (word_len <= 6) { \
                    max_length_diff = 2; /* Medium words: max 2 char difference */ \
                } else { \
                    max_length_diff = 2; /* Long words: still max 2 for safety */ \
                } \
                if (abs(candidate_len - word_len) > max_length_diff) continue; \
                \
                /* Keyboard-aware pre-filtering: first character proximity check */ \
                if (word_len >= 2 && candidate_len >= 2) { \
                    float first_char_distance = get_key_distance(word[0], candidate[0]); \
                    if (first_char_distance > 3.0f) continue; /* Skip very distant first chars */ \
                } \
                \
                int distance = levenshtein_distance_optimized(word, candidate); \
                \
                bool should_correct = false; \
                if (distance == 1) { \
                    should_correct = true; \
                } else if (distance == 2 && word_len >= 5) { \
                    int matching_chars = 0; \
                    int min_len = (word_len < candidate_len) ? word_len : candidate_len; \
                    for (int j = 0; j < min_len; j++) { \
                        if (j < word_len && j < candidate_len && word[j] == candidate[j]) { \
                            matching_chars++; \
                        } \
                    } \
                    if (matching_chars >= (min_len * 6 / 10)) { \
                        should_correct = true; \
                    } \
                } \
                \
                if (should_correct) { \
                    /* Use keyboard-aware scoring for better candidate selection */ \
                    float keyboard_score = get_keyboard_aware_score(word, candidate); \
                    \
                    /* Prefer candidates with better keyboard score, or lower edit distance */ \
                    if (distance < best_distance || \
                        (distance == best_distance && keyboard_score < best_keyboard_score)) { \
                        best_distance = distance; \
                        best_keyboard_score = keyboard_score; \
                        best_match = candidate; \
                    } \
                } \
            } \
        } \
    } while(0)
    
    // First, search the primary dictionary (first two letters of the word)
    const two_letter_dict_entry_t* primary_dict = get_two_letter_dict(word);
    SEARCH_DICTIONARY(primary_dict);
    
    // If no good match found (distance > 1), search additional dictionaries
    // This handles common typo patterns like "teh" -> "the"
    if (best_distance > 1 && word_len >= 3) {
        // Generate alternative prefixes for common typo patterns
        char alt_prefix1[3] = {0}; // Swap first two letters
        char alt_prefix2[3] = {0}; // First + third letter
        char alt_prefix3[3] = {0}; // Second + third letter
        
        // Pattern 1: Swap first two characters (handles "teh" -> "th")
        if (word[0] != word[1]) {
            alt_prefix1[0] = word[1];
            alt_prefix1[1] = word[0];
        }
        
        // Pattern 2: First + third character (handles "tge" -> "th")  
        if (word_len > 2 && word[0] != word[2]) {
            alt_prefix2[0] = word[0];
            alt_prefix2[1] = word[2];
        }
        
        // Pattern 3: Second + third character (handles "eth" -> "th")
        if (word_len > 2 && word[1] != word[2]) {
            alt_prefix3[0] = word[1];
            alt_prefix3[1] = word[2];
        }
        
        // Search alternative dictionaries
        if (alt_prefix1[0] && alt_prefix1[1]) {
            const two_letter_dict_entry_t* alt_dict1 = get_two_letter_dict(alt_prefix1);
            SEARCH_DICTIONARY(alt_dict1);
        }
        
        if (alt_prefix2[0] && alt_prefix2[1]) {
            const two_letter_dict_entry_t* alt_dict2 = get_two_letter_dict(alt_prefix2);
            SEARCH_DICTIONARY(alt_dict2);
        }
        
        if (alt_prefix3[0] && alt_prefix3[1]) {
            const two_letter_dict_entry_t* alt_dict3 = get_two_letter_dict(alt_prefix3);
            SEARCH_DICTIONARY(alt_dict3);
        }
    }
    
    #undef SEARCH_DICTIONARY
    
    // Cache the result for future lookups
    if (best_match != NULL) {
        add_to_cache(word, best_match);
    }
    
    return best_match;
}

// Check if word exists in dictionary (exact match)
static bool is_valid_word(const char* word) {
    // Use the two-letter dictionary lookup with binary search
    return two_letter_dictionary_lookup(word);
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
        bool need_shift = false;
        
        if (c >= 'a' && c <= 'z') {
            key = HID_USAGE_KEY_KEYBOARD_A + (c - 'a');
        } else if (c >= 'A' && c <= 'Z') {
            key = HID_USAGE_KEY_KEYBOARD_A + (c - 'A');
            need_shift = true;
        }
        
        if (key != 0) {
            // Press shift if needed for uppercase
            if (need_shift) {
                send_key_event(HID_USAGE_KEY_KEYBOARD_LEFTSHIFT, true);
            }
            
            send_key_event(key, true);
            send_key_event(key, false);
            
            // Release shift if used
            if (need_shift) {
                send_key_event(HID_USAGE_KEY_KEYBOARD_LEFTSHIFT, false);
            }
            
            k_msleep(2);  // Reduced delay for faster correction
        }
    }
}

// Perform spell correction
static void correct_word(const char* correct_word, int typo_len) {
    if (correcting) return;
    correcting = true;
    correction_start_time = k_uptime_get_32();
    
    // Fast deletion for quick typers
    for (int i = 0; i < typo_len; i++) {
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true);
        k_msleep(1);  // Minimal delay
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false);
        k_msleep(1);  // Minimal delay
    }
    
    // Type the correction
    type_string(correct_word);
    
    // Add corrected word to context for future corrections
    add_to_context(correct_word);
    
    correcting = false;
    correction_start_time = 0;
}



// Process completed word
static void process_word() {
    if (word_pos == 0) return;  // Empty word
    
    current_word[word_pos] = '\0';  // Null terminate
    
    // Skip very long words
    if (word_pos > MAX_WORD_LEN - 1) {
        word_pos = 0;
        return;
    }
    
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
        // Always check for common corrections first (typos, capitalization, contractions)
        final_correction = get_correction(current_word);
        
        // If no capitalization correction and word is valid, add to context and don't correct
        if (final_correction == NULL && is_valid_word(current_word)) {
            add_to_context(current_word);  // Track valid words for context
            word_pos = 0;
            return;
        }
        
        // If still no correction found, attempt dictionary-based correction
        if (final_correction == NULL) {
            final_correction = find_best_match(current_word);
        }
        
        // Apply context-aware improvement to the correction
        if (final_correction != NULL) {
            final_correction = get_context_aware_correction(current_word, final_correction);
        }
    }
    
    // Apply correction if found, but be extra conservative with final checks
    if (final_correction != NULL && strcmp(current_word, final_correction) != 0) {
        // Check if this is a common correction (always allow these)
        const char* common_correction = get_correction(current_word);
        bool is_common_correction = (common_correction != NULL && strcmp(final_correction, common_correction) == 0);
        
        if (is_common_correction) {
            // Always apply common corrections (typos, contractions, capitalization)
            correct_word(final_correction, word_pos);
        } else if (strlen(final_correction) > 0 && !is_valid_word(current_word)) {
            // Multiple safety checks before correcting other words
            int correction_distance = levenshtein_distance_optimized(current_word, final_correction);
            int word_length = strlen(current_word);
            int correction_length = strlen(final_correction);
            
            // Only correct if:
            // 1. Single character difference, OR
            // 2. Two character difference but words are similar length and reasonably long
            bool should_apply_correction = false;
            if (correction_distance == 1) {
                should_apply_correction = true;  // Single typo corrections are usually safe
            } else if (correction_distance == 2 && word_length >= 4 && abs(word_length - correction_length) <= 1) {
                // Double typo only for longer words with similar length
                should_apply_correction = true;
            }
            
            if (should_apply_correction) {
                correct_word(final_correction, word_pos);
            }
        }
    }
    
    word_pos = 0;  // Reset for next word
}



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
    // Check for F24 key (toggle functionality)
    if (key == HID_USAGE_KEY_KEYBOARD_F24) {
        spell_checker_enabled = !spell_checker_enabled;
        return 0;
    }
    
    uint32_t current_time = k_uptime_get_32();
    
    // Safety check: if correcting flag is stuck, reset it after timeout (5 seconds)
    if (correcting && correction_start_time > 0) {
        if (current_time - correction_start_time > 5000) {
            correcting = false;
            correction_start_time = 0;
            word_pos = 0;  // Reset word buffer as well
            memset(current_word, 0, sizeof(current_word));
        }
    }
    
    // Buffer timeout: reset word buffer after 2 seconds of inactivity
    if (word_pos > 0 && last_keystroke_time > 0) {
        if (current_time - last_keystroke_time > 2000) {
            word_pos = 0;
            memset(current_word, 0, sizeof(current_word));
        }
    }
    
    if (correcting || !spell_checker_enabled) return 0;
    
    // Update last keystroke time for any key press
    last_keystroke_time = current_time;
    
    char c = key_to_char(key);
    if (c == 0) return 0;  // Unsupported key
    
    // Word boundary characters
    if (c == ' ' || c == '.' || c == ',' || c == '\n' || c == '\t' || c == '!' || c == '?') {
        process_word();  // Check and correct the completed word immediately
        
        // Check for sentence boundaries (next word should be capitalized)
        if (c == '.' || c == '!' || c == '?' || c == '\n') {
            should_capitalize_sentence_start = true;
        }
        
        return 0;
    }
    
    // Add letter or apostrophe to current word
    if (((c >= 'a' && c <= 'z') || c == '\'') && word_pos < MAX_WORD_LEN - 1) {
        current_word[word_pos++] = c;
    }
    
    return 0;
}

// Initialize spell checker
static int spell_checker_init(void) {
    memset(current_word, 0, sizeof(current_word));
    word_pos = 0;
    correcting = false;
    correction_start_time = 0;
    last_keystroke_time = 0;
    spell_checker_enabled = true;
    
    // Initialize cache
    memset(lookup_cache, 0, sizeof(lookup_cache));
    cache_index = 0;
    cache_access_counter = 0;
    
    return 0;
}

SYS_INIT(spell_checker_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
