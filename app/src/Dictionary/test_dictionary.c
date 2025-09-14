#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "dictionary_simple.h"

// Test function to verify dictionary works
void test_dictionary() {
    const char* test_words[] = {
        "the", "and", "of", "to", "in", "is", "you", "that", "it", "he",
        "was", "for", "on", "are", "as", "with", "his", "they", "i", "at",
        "hello", "world", "test", "example", "programming", "keyboard", "spell", "check"
    };
    
    int num_tests = sizeof(test_words) / sizeof(test_words[0]);
    int found = 0;
    
    printf("Testing dictionary with %d words...\n", num_tests);
    
    for (int i = 0; i < num_tests; i++) {
        bool result = spell_check_word(test_words[i]);
        printf("'%s': %s\n", test_words[i], result ? "FOUND" : "NOT FOUND");
        if (result) found++;
    }
    
    printf("\nResults: %d/%d words found in dictionary\n", found, num_tests);
    printf("Dictionary statistics:\n");
    printf("- Word count: %zu\n", DICTIONARY_SIMPLE_WORD_COUNT);
    printf("- Data size: %zu bytes (%.1f KB)\n", DICTIONARY_WORDLIST_SIZE, DICTIONARY_WORDLIST_SIZE / 1024.0);
}

int main() {
    test_dictionary();
    return 0;
}
