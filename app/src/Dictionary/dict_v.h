#ifndef DICT_V_H
#define DICT_V_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_V_MAX_WORDS 6816
#define DICT_V_TXT_PATH "Dictionary/dict_v.txt"

// Global arrays for loaded words
static char* dict_v_words[DICT_V_MAX_WORDS];
static size_t dict_v_count = 0;

// Function to load words from txt file
static int load_dict_v_from_file(void) {
    FILE* file = fopen(DICT_V_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_v_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_v_count < DICT_V_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_v_words[dict_v_count] = malloc(strlen(buffer) + 1);
            if (dict_v_words[dict_v_count]) {
                strcpy(dict_v_words[dict_v_count], buffer);
                dict_v_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_v_words(void) {
    for (size_t i = 0; i < dict_v_count; i++) {
        if (dict_v_words[i]) {
            free(dict_v_words[i]);
            dict_v_words[i] = NULL;
        }
    }
    dict_v_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_v = (const char* const*)dict_v_words;
static const size_t DICT_V_SIZE = DICT_V_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_v_size(void) {
    return dict_v_count;
}

#endif // DICT_V_H
