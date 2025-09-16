#ifndef DICT_J_H
#define DICT_J_H

// Comprehensive 15K frequency dictionary - 7 words starting with 'j'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_j[] = {
    "jacket",
    "january",
    "jewelry",
    "juice",
    "july",
    "june",
    "just",
};

#define DICT_J_SIZE (sizeof(dict_j) / sizeof(dict_j[0]))

#endif // DICT_J_H
