#ifndef DICT_F_H
#define DICT_F_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_F_MAX_WORDS 15870
#define DICT_F_TXT_PATH "Dictionary/dict_f.txt"

// Global arrays for loaded words
static char* dict_f_words[DICT_F_MAX_WORDS];
static size_t dict_f_count = 0;

// Function to load words from txt file
static int load_dict_f_from_file(void) {
    FILE* file = fopen(DICT_F_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_f_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_f_count < DICT_F_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_f_words[dict_f_count] = malloc(strlen(buffer) + 1);
            if (dict_f_words[dict_f_count]) {
                strcpy(dict_f_words[dict_f_count], buffer);
                dict_f_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_f_words(void) {
    for (size_t i = 0; i < dict_f_count; i++) {
        if (dict_f_words[i]) {
            free(dict_f_words[i]);
            dict_f_words[i] = NULL;
        }
    }
    dict_f_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_f = (const char* const*)dict_f_words;
static const size_t DICT_F_SIZE = DICT_F_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_f_size(void) {
    return dict_f_count;
}

#endif // DICT_F_H
