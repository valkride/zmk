#ifndef DICT_L_H
#define DICT_L_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_L_MAX_WORDS 14093
#define DICT_L_TXT_PATH "Dictionary/dict_l.txt"

// Global arrays for loaded words
static char* dict_l_words[DICT_L_MAX_WORDS];
static size_t dict_l_count = 0;

// Function to load words from txt file
static int load_dict_l_from_file(void) {
    FILE* file = fopen(DICT_L_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_l_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_l_count < DICT_L_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_l_words[dict_l_count] = malloc(strlen(buffer) + 1);
            if (dict_l_words[dict_l_count]) {
                strcpy(dict_l_words[dict_l_count], buffer);
                dict_l_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_l_words(void) {
    for (size_t i = 0; i < dict_l_count; i++) {
        if (dict_l_words[i]) {
            free(dict_l_words[i]);
            dict_l_words[i] = NULL;
        }
    }
    dict_l_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_l = (const char* const*)dict_l_words;
static const size_t DICT_L_SIZE = DICT_L_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_l_size(void) {
    return dict_l_count;
}

#endif // DICT_L_H
