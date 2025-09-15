#ifndef DICT_U_H
#define DICT_U_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_U_MAX_WORDS 23790
#define DICT_U_TXT_PATH "Dictionary/dict_u.txt"

// Global arrays for loaded words
static char* dict_u_words[DICT_U_MAX_WORDS];
static size_t dict_u_count = 0;

// Function to load words from txt file
static int load_dict_u_from_file(void) {
    FILE* file = fopen(DICT_U_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_u_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_u_count < DICT_U_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_u_words[dict_u_count] = malloc(strlen(buffer) + 1);
            if (dict_u_words[dict_u_count]) {
                strcpy(dict_u_words[dict_u_count], buffer);
                dict_u_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_u_words(void) {
    for (size_t i = 0; i < dict_u_count; i++) {
        if (dict_u_words[i]) {
            free(dict_u_words[i]);
            dict_u_words[i] = NULL;
        }
    }
    dict_u_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_u = (const char* const*)dict_u_words;
static const size_t DICT_U_SIZE = DICT_U_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_u_size(void) {
    return dict_u_count;
}

#endif // DICT_U_H
