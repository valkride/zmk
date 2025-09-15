#ifndef DICT_Y_H
#define DICT_Y_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_Y_MAX_WORDS 1740
#define DICT_Y_TXT_PATH "Dictionary/dict_y.txt"

// Global arrays for loaded words
static char* dict_y_words[DICT_Y_MAX_WORDS];
static size_t dict_y_count = 0;

// Function to load words from txt file
static int load_dict_y_from_file(void) {
    FILE* file = fopen(DICT_Y_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_y_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_y_count < DICT_Y_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_y_words[dict_y_count] = malloc(strlen(buffer) + 1);
            if (dict_y_words[dict_y_count]) {
                strcpy(dict_y_words[dict_y_count], buffer);
                dict_y_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_y_words(void) {
    for (size_t i = 0; i < dict_y_count; i++) {
        if (dict_y_words[i]) {
            free(dict_y_words[i]);
            dict_y_words[i] = NULL;
        }
    }
    dict_y_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_y = (const char* const*)dict_y_words;
static const size_t DICT_Y_SIZE = DICT_Y_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_y_size(void) {
    return dict_y_count;
}

#endif // DICT_Y_H
