#ifndef DICT_A_H
#define DICT_A_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_A_MAX_WORDS 30849
#define DICT_A_TXT_PATH "Dictionary/dict_a.txt"

// Global arrays for loaded words
static char* dict_a_words[DICT_A_MAX_WORDS];
static size_t dict_a_count = 0;

// Function to load words from txt file
static int load_dict_a_from_file(void) {
    FILE* file = fopen(DICT_A_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_a_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_a_count < DICT_A_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_a_words[dict_a_count] = malloc(strlen(buffer) + 1);
            if (dict_a_words[dict_a_count]) {
                strcpy(dict_a_words[dict_a_count], buffer);
                dict_a_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_a_words(void) {
    for (size_t i = 0; i < dict_a_count; i++) {
        if (dict_a_words[i]) {
            free(dict_a_words[i]);
            dict_a_words[i] = NULL;
        }
    }
    dict_a_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_a = (const char* const*)dict_a_words;
static const size_t DICT_A_SIZE = DICT_A_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_a_size(void) {
    return dict_a_count;
}

#endif // DICT_A_H
