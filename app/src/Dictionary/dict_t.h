#ifndef DICT_T_H
#define DICT_T_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_T_MAX_WORDS 25221
#define DICT_T_TXT_PATH "Dictionary/dict_t.txt"

// Global arrays for loaded words
static char* dict_t_words[DICT_T_MAX_WORDS];
static size_t dict_t_count = 0;

// Function to load words from txt file
static int load_dict_t_from_file(void) {
    FILE* file = fopen(DICT_T_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_t_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_t_count < DICT_T_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_t_words[dict_t_count] = malloc(strlen(buffer) + 1);
            if (dict_t_words[dict_t_count]) {
                strcpy(dict_t_words[dict_t_count], buffer);
                dict_t_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_t_words(void) {
    for (size_t i = 0; i < dict_t_count; i++) {
        if (dict_t_words[i]) {
            free(dict_t_words[i]);
            dict_t_words[i] = NULL;
        }
    }
    dict_t_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_t = (const char* const*)dict_t_words;
static const size_t DICT_T_SIZE = DICT_T_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_t_size(void) {
    return dict_t_count;
}

#endif // DICT_T_H
