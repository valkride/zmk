#ifndef DICT_W_H
#define DICT_W_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_W_MAX_WORDS 11672
#define DICT_W_TXT_PATH "Dictionary/dict_w.txt"

// Global arrays for loaded words
static char* dict_w_words[DICT_W_MAX_WORDS];
static size_t dict_w_count = 0;

// Function to load words from txt file
static int load_dict_w_from_file(void) {
    FILE* file = fopen(DICT_W_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_w_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_w_count < DICT_W_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_w_words[dict_w_count] = malloc(strlen(buffer) + 1);
            if (dict_w_words[dict_w_count]) {
                strcpy(dict_w_words[dict_w_count], buffer);
                dict_w_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_w_words(void) {
    for (size_t i = 0; i < dict_w_count; i++) {
        if (dict_w_words[i]) {
            free(dict_w_words[i]);
            dict_w_words[i] = NULL;
        }
    }
    dict_w_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_w = (const char* const*)dict_w_words;
static const size_t DICT_W_SIZE = DICT_W_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_w_size(void) {
    return dict_w_count;
}

#endif // DICT_W_H
