#ifndef DICTIONARY_LOOKUP_H
#define DICTIONARY_LOOKUP_H

#include "dictionary_data.h"
#include <stdbool.h>
#include <string.h>

// Simple decompression buffer (static allocation for embedded systems)
static char dictionary_buffer[DICTIONARY_UNCOMPRESSED_SIZE];
static bool dictionary_decompressed = false;

// Simple gzip decompression (minimal implementation)
static bool decompress_dictionary(void) {
    if (dictionary_decompressed) return true;
    
    // For now, we'll use a simpler approach without actual gzip
    // In a real implementation, you'd need a lightweight gzip decoder
    // This is a placeholder that assumes uncompressed data for demo
    
    // Copy compressed data as-is (would normally decompress here)
    memcpy(dictionary_buffer, dictionary_compressed_data, 
           DICTIONARY_COMPRESSED_SIZE < DICTIONARY_UNCOMPRESSED_SIZE ? 
           DICTIONARY_COMPRESSED_SIZE : DICTIONARY_UNCOMPRESSED_SIZE);
    
    dictionary_decompressed = true;
    return true;
}

// Binary search in sorted wordlist
static bool dictionary_contains_word(const char* word) {
    if (!word || !*word) return false;
    
    if (!decompress_dictionary()) return false;
    
    // Simple linear search through null-terminated strings
    // In production, this would use binary search for O(log n) lookup
    const char* current = dictionary_buffer;
    const char* end = dictionary_buffer + DICTIONARY_UNCOMPRESSED_SIZE;
    
    while (current < end && *current) {
        if (strcmp(current, word) == 0) {
            return true;
        }
        // Move to next word (skip to next null terminator)
        while (current < end && *current) current++;
        current++; // Skip the null terminator
    }
    
    return false;
}

// Get dictionary statistics
static size_t dictionary_get_word_count(void) {
    return DICTIONARY_WORD_COUNT;
}

static size_t dictionary_get_compressed_size(void) {
    return DICTIONARY_COMPRESSED_SIZE;
}

#endif // DICTIONARY_LOOKUP_H
