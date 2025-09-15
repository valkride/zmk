#ifndef DICT_Z_H
#define DICT_Z_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_Z_MAX_WORDS 1822
#define DICT_Z_TXT_PATH "Dictionary/dict_z.txt"

// Global arrays for loaded words
static char* dict_z_words[DICT_Z_MAX_WORDS];
static size_t dict_z_count = 0;

// Function to load words from txt file
static int load_dict_z_from_file(void) {
    FILE* file = fopen(DICT_Z_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_z_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_z_count < DICT_Z_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_z_words[dict_z_count] = malloc(strlen(buffer) + 1);
            if (dict_z_words[dict_z_count]) {
                strcpy(dict_z_words[dict_z_count], buffer);
                dict_z_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_z_words(void) {
    for (size_t i = 0; i < dict_z_count; i++) {
        if (dict_z_words[i]) {
            free(dict_z_words[i]);
            dict_z_words[i] = NULL;
        }
    }
    dict_z_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_z = (const char* const*)dict_z_words;
static const size_t DICT_Z_SIZE = DICT_Z_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_z_size(void) {
    return dict_z_count;
}

#endif // DICT_Z_H
