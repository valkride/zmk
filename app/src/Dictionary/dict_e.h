#ifndef DICT_E_H
#define DICT_E_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_E_MAX_WORDS 16815
#define DICT_E_TXT_PATH "Dictionary/dict_e.txt"

// Global arrays for loaded words
static char* dict_e_words[DICT_E_MAX_WORDS];
static size_t dict_e_count = 0;

// Function to load words from txt file
static int load_dict_e_from_file(void) {
    FILE* file = fopen(DICT_E_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_e_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_e_count < DICT_E_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_e_words[dict_e_count] = malloc(strlen(buffer) + 1);
            if (dict_e_words[dict_e_count]) {
                strcpy(dict_e_words[dict_e_count], buffer);
                dict_e_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_e_words(void) {
    for (size_t i = 0; i < dict_e_count; i++) {
        if (dict_e_words[i]) {
            free(dict_e_words[i]);
            dict_e_words[i] = NULL;
        }
    }
    dict_e_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_e = (const char* const*)dict_e_words;
static const size_t DICT_E_SIZE = DICT_E_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_e_size(void) {
    return dict_e_count;
}

#endif // DICT_E_H
