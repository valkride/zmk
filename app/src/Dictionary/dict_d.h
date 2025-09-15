#ifndef DICT_D_H
#define DICT_D_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_D_MAX_WORDS 22777
#define DICT_D_TXT_PATH "Dictionary/dict_d.txt"

// Global arrays for loaded words
static char* dict_d_words[DICT_D_MAX_WORDS];
static size_t dict_d_count = 0;

// Function to load words from txt file
static int load_dict_d_from_file(void) {
    FILE* file = fopen(DICT_D_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_d_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_d_count < DICT_D_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_d_words[dict_d_count] = malloc(strlen(buffer) + 1);
            if (dict_d_words[dict_d_count]) {
                strcpy(dict_d_words[dict_d_count], buffer);
                dict_d_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_d_words(void) {
    for (size_t i = 0; i < dict_d_count; i++) {
        if (dict_d_words[i]) {
            free(dict_d_words[i]);
            dict_d_words[i] = NULL;
        }
    }
    dict_d_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_d = (const char* const*)dict_d_words;
static const size_t DICT_D_SIZE = DICT_D_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_d_size(void) {
    return dict_d_count;
}

#endif // DICT_D_H
