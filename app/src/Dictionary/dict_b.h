#ifndef DICT_B_H
#define DICT_B_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_B_MAX_WORDS 24332
#define DICT_B_TXT_PATH "Dictionary/dict_b.txt"

// Global arrays for loaded words
static char* dict_b_words[DICT_B_MAX_WORDS];
static size_t dict_b_count = 0;

// Function to load words from txt file
static int load_dict_b_from_file(void) {
    FILE* file = fopen(DICT_B_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_b_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_b_count < DICT_B_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_b_words[dict_b_count] = malloc(strlen(buffer) + 1);
            if (dict_b_words[dict_b_count]) {
                strcpy(dict_b_words[dict_b_count], buffer);
                dict_b_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_b_words(void) {
    for (size_t i = 0; i < dict_b_count; i++) {
        if (dict_b_words[i]) {
            free(dict_b_words[i]);
            dict_b_words[i] = NULL;
        }
    }
    dict_b_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_b = (const char* const*)dict_b_words;
static const size_t DICT_B_SIZE = DICT_B_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_b_size(void) {
    return dict_b_count;
}

#endif // DICT_B_H
