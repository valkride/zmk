#ifndef DICT_G_H
#define DICT_G_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_G_MAX_WORDS 14466
#define DICT_G_TXT_PATH "Dictionary/dict_g.txt"

// Global arrays for loaded words
static char* dict_g_words[DICT_G_MAX_WORDS];
static size_t dict_g_count = 0;

// Function to load words from txt file
static int load_dict_g_from_file(void) {
    FILE* file = fopen(DICT_G_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_g_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_g_count < DICT_G_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_g_words[dict_g_count] = malloc(strlen(buffer) + 1);
            if (dict_g_words[dict_g_count]) {
                strcpy(dict_g_words[dict_g_count], buffer);
                dict_g_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_g_words(void) {
    for (size_t i = 0; i < dict_g_count; i++) {
        if (dict_g_words[i]) {
            free(dict_g_words[i]);
            dict_g_words[i] = NULL;
        }
    }
    dict_g_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_g = (const char* const*)dict_g_words;
static const size_t DICT_G_SIZE = DICT_G_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_g_size(void) {
    return dict_g_count;
}

#endif // DICT_G_H
