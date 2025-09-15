#ifndef DICT_S_H
#define DICT_S_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_S_MAX_WORDS 50571
#define DICT_S_TXT_PATH "Dictionary/dict_s.txt"

// Global arrays for loaded words
static char* dict_s_words[DICT_S_MAX_WORDS];
static size_t dict_s_count = 0;

// Function to load words from txt file
static int load_dict_s_from_file(void) {
    FILE* file = fopen(DICT_S_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_s_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_s_count < DICT_S_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_s_words[dict_s_count] = malloc(strlen(buffer) + 1);
            if (dict_s_words[dict_s_count]) {
                strcpy(dict_s_words[dict_s_count], buffer);
                dict_s_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_s_words(void) {
    for (size_t i = 0; i < dict_s_count; i++) {
        if (dict_s_words[i]) {
            free(dict_s_words[i]);
            dict_s_words[i] = NULL;
        }
    }
    dict_s_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_s = (const char* const*)dict_s_words;
static const size_t DICT_S_SIZE = DICT_S_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_s_size(void) {
    return dict_s_count;
}

#endif // DICT_S_H
