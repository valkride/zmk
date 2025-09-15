#ifndef DICT_N_H
#define DICT_N_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_N_MAX_WORDS 16174
#define DICT_N_TXT_PATH "Dictionary/dict_n.txt"

// Global arrays for loaded words
static char* dict_n_words[DICT_N_MAX_WORDS];
static size_t dict_n_count = 0;

// Function to load words from txt file
static int load_dict_n_from_file(void) {
    FILE* file = fopen(DICT_N_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_n_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_n_count < DICT_N_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_n_words[dict_n_count] = malloc(strlen(buffer) + 1);
            if (dict_n_words[dict_n_count]) {
                strcpy(dict_n_words[dict_n_count], buffer);
                dict_n_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_n_words(void) {
    for (size_t i = 0; i < dict_n_count; i++) {
        if (dict_n_words[i]) {
            free(dict_n_words[i]);
            dict_n_words[i] = NULL;
        }
    }
    dict_n_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_n = (const char* const*)dict_n_words;
static const size_t DICT_N_SIZE = DICT_N_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_n_size(void) {
    return dict_n_count;
}

#endif // DICT_N_H
