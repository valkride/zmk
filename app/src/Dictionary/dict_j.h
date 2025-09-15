#ifndef DICT_J_H
#define DICT_J_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_J_MAX_WORDS 4158
#define DICT_J_TXT_PATH "Dictionary/dict_j.txt"

// Global arrays for loaded words
static char* dict_j_words[DICT_J_MAX_WORDS];
static size_t dict_j_count = 0;

// Function to load words from txt file
static int load_dict_j_from_file(void) {
    FILE* file = fopen(DICT_J_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_j_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_j_count < DICT_J_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_j_words[dict_j_count] = malloc(strlen(buffer) + 1);
            if (dict_j_words[dict_j_count]) {
                strcpy(dict_j_words[dict_j_count], buffer);
                dict_j_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_j_words(void) {
    for (size_t i = 0; i < dict_j_count; i++) {
        if (dict_j_words[i]) {
            free(dict_j_words[i]);
            dict_j_words[i] = NULL;
        }
    }
    dict_j_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_j = (const char* const*)dict_j_words;
static const size_t DICT_J_SIZE = DICT_J_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_j_size(void) {
    return dict_j_count;
}

#endif // DICT_J_H
