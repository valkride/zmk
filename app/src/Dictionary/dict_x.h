#ifndef DICT_X_H
#define DICT_X_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_X_MAX_WORDS 609
#define DICT_X_TXT_PATH "Dictionary/dict_x.txt"

// Global arrays for loaded words
static char* dict_x_words[DICT_X_MAX_WORDS];
static size_t dict_x_count = 0;

// Function to load words from txt file
static int load_dict_x_from_file(void) {
    FILE* file = fopen(DICT_X_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_x_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_x_count < DICT_X_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_x_words[dict_x_count] = malloc(strlen(buffer) + 1);
            if (dict_x_words[dict_x_count]) {
                strcpy(dict_x_words[dict_x_count], buffer);
                dict_x_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_x_words(void) {
    for (size_t i = 0; i < dict_x_count; i++) {
        if (dict_x_words[i]) {
            free(dict_x_words[i]);
            dict_x_words[i] = NULL;
        }
    }
    dict_x_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_x = (const char* const*)dict_x_words;
static const size_t DICT_X_SIZE = DICT_X_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_x_size(void) {
    return dict_x_count;
}

#endif // DICT_X_H
