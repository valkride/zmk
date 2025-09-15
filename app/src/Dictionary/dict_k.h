#ifndef DICT_K_H
#define DICT_K_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_K_MAX_WORDS 6167
#define DICT_K_TXT_PATH "Dictionary/dict_k.txt"

// Global arrays for loaded words
static char* dict_k_words[DICT_K_MAX_WORDS];
static size_t dict_k_count = 0;

// Function to load words from txt file
static int load_dict_k_from_file(void) {
    FILE* file = fopen(DICT_K_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_k_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_k_count < DICT_K_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_k_words[dict_k_count] = malloc(strlen(buffer) + 1);
            if (dict_k_words[dict_k_count]) {
                strcpy(dict_k_words[dict_k_count], buffer);
                dict_k_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_k_words(void) {
    for (size_t i = 0; i < dict_k_count; i++) {
        if (dict_k_words[i]) {
            free(dict_k_words[i]);
            dict_k_words[i] = NULL;
        }
    }
    dict_k_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_k = (const char* const*)dict_k_words;
static const size_t DICT_K_SIZE = DICT_K_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_k_size(void) {
    return dict_k_count;
}

#endif // DICT_K_H
