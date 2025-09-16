#ifndef SPELL_DICTIONARY_MAP_H
#define SPELL_DICTIONARY_MAP_H

#include "simple_hash_dictionary.h"

// Check if word exists in hash dictionary
static bool hash_dictionary_lookup(const char* word) {
    // Use the hash_table_contains_word function from simple_hash_dictionary.h
    return hash_table_contains_word(word) ? true : false;
}

// Legacy structure for compatibility - will be refactored later
typedef struct {
    const char* const* words;
    size_t size;
} dictionary_entry_t;

// Helper function to get dictionary for a letter - compatibility wrapper
static inline const dictionary_entry_t* get_dictionary_for_letter(char letter) {
    // This is now just a stub - we don't use letter-based lookup anymore
    return NULL;
}

// Function declaration for levenshtein_distance
static int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // Quick optimization: if length difference is too large, skip
    if (abs(len1 - len2) > 2) {
        return 3; // More than max edit distance
    }
    
    // Ultra-fast path: exact match
    if (len1 == len2 && strcmp(s1, s2) == 0) return 0;
    
    // Fast path: check for single character differences first
    if (len1 == len2) {
        int diff_count = 0;
        for (int i = 0; i < len1; i++) {
            if (s1[i] != s2[i]) {
                diff_count++;
                if (diff_count > 2) return 3; // More than max edit distance
            }
        }
        return diff_count;
    }
    
    // Use static array to avoid dynamic allocation
    static int matrix[16][16]; // Maximum word length for edit distance is 15
    
    if (len1 >= 15 || len2 >= 15) {
        return 3; // Words too long, just return max+1
    }
    
    // Initialize first row and column
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    // Fill the matrix
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            int del = matrix[i-1][j] + 1;
            int ins = matrix[i][j-1] + 1;
            int sub = matrix[i-1][j-1] + cost;
            
            // Find minimum
            matrix[i][j] = del;
            if (ins < matrix[i][j]) matrix[i][j] = ins;
            if (sub < matrix[i][j]) matrix[i][j] = sub;
        }
    }
    
    return matrix[len1][len2];
}

// Find a word similar to the given word using our hash dictionary
// This is a simplified implementation that doesn't scan the whole dictionary
// but uses a sample of common words to find similar matches
static const char* find_similar_word(const char* word, int max_edit_distance) {
    if (!word) return NULL;
    
    // Common words for spell checking (simplified approach)
    // In a real implementation, we would scan the entire dictionary,
    // but for now we'll use a sample of common words as a demonstration
    static const char* common_words[] = {
        "the", "be", "to", "of", "and", "a", "in", "that", "have", "I", 
        "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
        "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
        "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
        "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
        "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
        "people", "into", "year", "your", "good", "some", "could", "them", "see", "other",
        "than", "then", "now", "look", "only", "come", "its", "over", "think", "also",
        "back", "after", "use", "two", "how", "our", "work", "first", "well", "way",
        "even", "new", "want", "because", "any", "these", "give", "day", "most", "us"
    };
    #define NUM_COMMON_WORDS (sizeof(common_words) / sizeof(common_words[0]))
    
    // Start with a lookup in our hash dictionary
    if (hash_dictionary_lookup(word)) {
        return word; // Word already exists
    }
    
    const char* best_match = NULL;
    int best_distance = max_edit_distance + 1;
    
    // First check for simple typos in common words
    for (int i = 0; i < NUM_COMMON_WORDS; i++) {
        const char* candidate = common_words[i];
        
        // Skip if length difference exceeds edit distance
        if (abs(strlen(candidate) - strlen(word)) > max_edit_distance) continue;
        
        int distance = levenshtein_distance(word, candidate);
        if (distance <= max_edit_distance && distance < best_distance) {
            best_distance = distance;
            best_match = candidate;
            
            // If we found an exact match or a very close match, return immediately
            if (distance <= 1) return best_match;
        }
    }
    
    // If no good match in common words, we could do a more exhaustive search
    // but that would be too slow for embedded systems.
    
    return best_match;
}

#endif // SPELL_DICTIONARY_MAP_H