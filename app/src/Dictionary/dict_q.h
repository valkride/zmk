#ifndef DICT_Q_H
#define DICT_Q_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of words estimated from txt file
#define DICT_Q_MAX_WORDS 3218
#define DICT_Q_TXT_PATH "Dictionary/dict_q.txt"

// Global arrays for loaded words
static char* dict_q_words[DICT_Q_MAX_WORDS];
static size_t dict_q_count = 0;

// Function to load words from txt file
static int load_dict_q_from_file(void) {
    FILE* file = fopen(DICT_Q_TXT_PATH, "r");
    if (!file) {
        return -1; // Failed to open file
    }
    
    char buffer[256];
    dict_q_count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && dict_q_count < DICT_Q_MAX_WORDS) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        // Allocate and copy word
        if (strlen(buffer) > 0) {
            dict_q_words[dict_q_count] = malloc(strlen(buffer) + 1);
            if (dict_q_words[dict_q_count]) {
                strcpy(dict_q_words[dict_q_count], buffer);
                dict_q_count++;
            }
        }
    }
    
    fclose(file);
    return 0; // Success
}

// Function to free loaded words
static void free_dict_q_words(void) {
    for (size_t i = 0; i < dict_q_count; i++) {
        if (dict_q_words[i]) {
            free(dict_q_words[i]);
            dict_q_words[i] = NULL;
        }
    }
    dict_q_count = 0;
}

// Compatibility array pointer (points to loaded words)
static const char* const* dict_q = (const char* const*)dict_q_words;
static const size_t DICT_Q_SIZE = DICT_Q_MAX_WORDS; // Will be updated to actual count after loading

// Getter function for actual count
static size_t get_dict_q_size(void) {
    return dict_q_count;
}

#endif // DICT_Q_H
