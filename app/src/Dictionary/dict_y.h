#ifndef DICT_Y_H
#define DICT_Y_H

// Comprehensive 15K frequency dictionary - 10 words starting with 'y'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_y[] = {
    "yard",
    "year",
    "yesterday",
    "yet",
    "you",
    "you'd",
    "you'll",
    "you're",
    "you've",
    "your",
};

#define DICT_Y_SIZE (sizeof(dict_y) / sizeof(dict_y[0]))

#endif // DICT_Y_H
