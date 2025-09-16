#ifndef SPELL_DICTIONARY_MAP_H
#define SPELL_DICTIONARY_MAP_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// Include all letter-based dictionaries
#include "dict_a.h"
#include "dict_b.h"
#include "dict_c.h"
#include "dict_d.h"
#include "dict_e.h"
#include "dict_f.h"
#include "dict_g.h"
#include "dict_h.h"
#include "dict_i.h"
#include "dict_j.h"
#include "dict_k.h"
#include "dict_l.h"
#include "dict_m.h"
#include "dict_n.h"
#include "dict_o.h"
#include "dict_p.h"
#include "dict_q.h"
#include "dict_r.h"
#include "dict_s.h"
#include "dict_t.h"
#include "dict_u.h"
#include "dict_v.h"
#include "dict_w.h"
#include "dict_x.h"
#include "dict_y.h"
#include "dict_z.h"

// Dictionary entry structure
typedef struct {
    const char* const* words;
    size_t size;
} dictionary_entry_t;

// Dictionary mapping for each letter
static const dictionary_entry_t letter_dictionaries[26] = {
    {dict_a, DICT_A_SIZE}, // a: 28646 words
    {dict_b, DICT_B_SIZE}, // b: 21584 words
    {dict_c, DICT_C_SIZE}, // c: 35664 words
    {dict_d, DICT_D_SIZE}, // d: 20867 words
    {dict_e, DICT_E_SIZE}, // e: 15903 words
    {dict_f, DICT_F_SIZE}, // f: 13399 words
    {dict_g, DICT_G_SIZE}, // g: 13061 words
    {dict_h, DICT_H_SIZE}, // h: 16011 words
    {dict_i, DICT_I_SIZE}, // i: 14277 words
    {dict_j, DICT_J_SIZE}, // j: 3832 words
    {dict_k, DICT_K_SIZE}, // k: 5841 words
    {dict_l, DICT_L_SIZE}, // l: 12306 words
    {dict_m, DICT_M_SIZE}, // m: 23464 words
    {dict_n, DICT_N_SIZE}, // n: 14936 words
    {dict_o, DICT_O_SIZE}, // o: 13911 words
    {dict_p, DICT_P_SIZE}, // p: 37356 words
    {dict_q, DICT_Q_SIZE}, // q: 1951 words
    {dict_r, DICT_R_SIZE}, // r: 18894 words
    {dict_s, DICT_S_SIZE}, // s: 42412 words
    {dict_t, DICT_T_SIZE}, // t: 20839 words
    {dict_u, DICT_U_SIZE}, // u: 23225 words
    {dict_v, DICT_V_SIZE}, // v: 6223 words
    {dict_w, DICT_W_SIZE}, // w: 7936 words
    {dict_x, DICT_X_SIZE}, // x: 574 words
    {dict_y, DICT_Y_SIZE}, // y: 1420 words
    {dict_z, DICT_Z_SIZE}, // z: 1760 words
};

// Get dictionary for a specific letter
static inline const dictionary_entry_t* get_dictionary_for_letter(char letter) {
    char lower_letter = tolower(letter);
    if (lower_letter >= 'a' && lower_letter <= 'z') {
        return &letter_dictionaries[lower_letter - 'a'];
    }
    return NULL;
}

// Binary search in a sorted dictionary
static bool binary_search_dictionary(const char* const* dict, size_t size, const char* word) {
    if (!dict || !word || size == 0) return false;
    
    int left = 0;
    int right = size - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, dict[mid]);
        
        if (cmp == 0) {
            return true;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return false;
}

// Main dictionary lookup function
static bool hash_dictionary_lookup(const char* word) {
    if (!word || !*word) return false;
    
    // Convert to lowercase
    char lower_word[64];
    int i = 0;
    while (word[i] && i < 63) {
        lower_word[i] = tolower(word[i]);
        i++;
    }
    lower_word[i] = '\0';
    
    // Get dictionary for first letter
    const dictionary_entry_t* dict = get_dictionary_for_letter(lower_word[0]);
    if (!dict) return false;
    
    // Binary search in the appropriate dictionary
    return binary_search_dictionary(dict->words, dict->size, lower_word);
}

// Forward declaration for levenshtein_distance
static int levenshtein_distance(const char* s1, const char* s2);

// Find similar word in the same letter dictionary
static const char* find_similar_word(const char* word, int max_edit_distance) {
    if (!word || !*word) return NULL;
    
    char lower_word[64];
    int i = 0;
    while (word[i] && i < 63) {
        lower_word[i] = tolower(word[i]);
        i++;
    }
    lower_word[i] = '\0';
    
    const dictionary_entry_t* dict = get_dictionary_for_letter(lower_word[0]);
    if (!dict) return NULL;
    
    const char* best_match = NULL;
    int best_distance = max_edit_distance + 1;
    
    // Check words in the same letter dictionary
    for (size_t j = 0; j < dict->size; j++) {
        int distance = levenshtein_distance(lower_word, dict->words[j]);
        if (distance <= max_edit_distance && distance < best_distance) {
            best_distance = distance;
            best_match = dict->words[j];
        }
    }
    
    return best_match;
}

// Print dictionary statistics
static inline void print_dictionary_stats() {
    size_t total_words = 0;
    for (int i = 0; i < 26; i++) {
        total_words += letter_dictionaries[i].size;
    }
    // Total words: varies by actual content
    // Dictionary approach: Letter-based (a-z)
    // Lookup method: Binary search per letter
}

#endif // SPELL_DICTIONARY_MAP_H
