#ifndef DICT_P_H
#define DICT_P_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_P_MAX_WORDS 40956
#define DICT_P_TXT_PATH "Dictionary/dict_p.txt"

// Global arrays for loaded words
static char* dict_p_words[DICT_P_MAX_WORDS];
static size_t dict_p_count = 0;

// Function to load words from txt file
static int load_dict_p_from_file(void) {
    FILE* file = fopen(DICT_P_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_p_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_p_count < DICT_P_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_p_words[dict_p_count] = malloc(strlen(buffer) + 1);
            if (dict_p_words[dict_p_count]) {
                strcpy(dict_p_words[dict_p_count], buffer);
                dict_p_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_p_words(void) {
    for (size_t i = 0; i < dict_p_count; i++) {
        if (dict_p_words[i]) {
            free(dict_p_words[i]);
            dict_p_words[i] = NULL;
        }
    }
    dict_p_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_p = (const char* const*)dict_p_words;
static const size_t DICT_P_SIZE = DICT_P_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_p_size(void) {
    return dict_p_count;
}

#endif // DICT_P_H
