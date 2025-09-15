#ifndef DICT_O_H
#define DICT_O_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_O_MAX_WORDS 15061
#define DICT_O_TXT_PATH "Dictionary/dict_o.txt"

// Global arrays for loaded words
static char* dict_o_words[DICT_O_MAX_WORDS];
static size_t dict_o_count = 0;

// Function to load words from txt file
static int load_dict_o_from_file(void) {
    FILE* file = fopen(DICT_O_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_o_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_o_count < DICT_O_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_o_words[dict_o_count] = malloc(strlen(buffer) + 1);
            if (dict_o_words[dict_o_count]) {
                strcpy(dict_o_words[dict_o_count], buffer);
                dict_o_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_o_words(void) {
    for (size_t i = 0; i < dict_o_count; i++) {
        if (dict_o_words[i]) {
            free(dict_o_words[i]);
            dict_o_words[i] = NULL;
        }
    }
    dict_o_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_o = (const char* const*)dict_o_words;
static const size_t DICT_O_SIZE = DICT_O_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_o_size(void) {
    return dict_o_count;
}

#endif // DICT_O_H
