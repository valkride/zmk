#ifndef DICT_R_H
#define DICT_R_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_R_MAX_WORDS 21286
#define DICT_R_TXT_PATH "Dictionary/dict_r.txt"

// Global arrays for loaded words
static char* dict_r_words[DICT_R_MAX_WORDS];
static size_t dict_r_count = 0;

// Function to load words from txt file
static int load_dict_r_from_file(void) {
    FILE* file = fopen(DICT_R_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_r_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_r_count < DICT_R_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_r_words[dict_r_count] = malloc(strlen(buffer) + 1);
            if (dict_r_words[dict_r_count]) {
                strcpy(dict_r_words[dict_r_count], buffer);
                dict_r_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_r_words(void) {
    for (size_t i = 0; i < dict_r_count; i++) {
        if (dict_r_words[i]) {
            free(dict_r_words[i]);
            dict_r_words[i] = NULL;
        }
    }
    dict_r_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_r = (const char* const*)dict_r_words;
static const size_t DICT_R_SIZE = DICT_R_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_r_size(void) {
    return dict_r_count;
}

#endif // DICT_R_H
