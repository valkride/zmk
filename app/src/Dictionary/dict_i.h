#ifndef DICT_I_H
#define DICT_I_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_I_MAX_WORDS 15220
#define DICT_I_TXT_PATH "Dictionary/dict_i.txt"

// Global arrays for loaded words
static char* dict_i_words[DICT_I_MAX_WORDS];
static size_t dict_i_count = 0;

// Function to load words from txt file
static int load_dict_i_from_file(void) {
    FILE* file = fopen(DICT_I_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_i_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_i_count < DICT_I_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_i_words[dict_i_count] = malloc(strlen(buffer) + 1);
            if (dict_i_words[dict_i_count]) {
                strcpy(dict_i_words[dict_i_count], buffer);
                dict_i_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_i_words(void) {
    for (size_t i = 0; i < dict_i_count; i++) {
        if (dict_i_words[i]) {
            free(dict_i_words[i]);
            dict_i_words[i] = NULL;
        }
    }
    dict_i_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_i = (const char* const*)dict_i_words;
static const size_t DICT_I_SIZE = DICT_I_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_i_size(void) {
    return dict_i_count;
}

#endif // DICT_I_H
