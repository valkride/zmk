#ifndef DICT_C_H
#define DICT_C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_C_MAX_WORDS 38934
#define DICT_C_TXT_PATH "Dictionary/dict_c.txt"

// Global arrays for loaded words
static char* dict_c_words[DICT_C_MAX_WORDS];
static size_t dict_c_count = 0;

// Function to load words from txt file
static int load_dict_c_from_file(void) {
    FILE* file = fopen(DICT_C_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_c_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_c_count < DICT_C_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_c_words[dict_c_count] = malloc(strlen(buffer) + 1);
            if (dict_c_words[dict_c_count]) {
                strcpy(dict_c_words[dict_c_count], buffer);
                dict_c_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_c_words(void) {
    for (size_t i = 0; i < dict_c_count; i++) {
        if (dict_c_words[i]) {
            free(dict_c_words[i]);
            dict_c_words[i] = NULL;
        }
    }
    dict_c_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_c = (const char* const*)dict_c_words;
static const size_t DICT_C_SIZE = DICT_C_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_c_size(void) {
    return dict_c_count;
}

#endif // DICT_C_H
