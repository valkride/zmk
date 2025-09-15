#ifndef DICT_H_H
#define DICT_H_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_H_MAX_WORDS 18662
#define DICT_H_TXT_PATH "Dictionary/dict_h.txt"

// Global arrays for loaded words
static char* dict_h_words[DICT_H_MAX_WORDS];
static size_t dict_h_count = 0;

// Function to load words from txt file
static int load_dict_h_from_file(void) {
    FILE* file = fopen(DICT_H_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_h_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_h_count < DICT_H_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_h_words[dict_h_count] = malloc(strlen(buffer) + 1);
            if (dict_h_words[dict_h_count]) {
                strcpy(dict_h_words[dict_h_count], buffer);
                dict_h_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_h_words(void) {
    for (size_t i = 0; i < dict_h_count; i++) {
        if (dict_h_words[i]) {
            free(dict_h_words[i]);
            dict_h_words[i] = NULL;
        }
    }
    dict_h_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_h = (const char* const*)dict_h_words;
static const size_t DICT_H_SIZE = DICT_H_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_h_size(void) {
    return dict_h_count;
}

#endif // DICT_H_H
