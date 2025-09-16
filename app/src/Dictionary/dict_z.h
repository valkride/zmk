#ifndef DICT_Z_H
#define DICT_Z_H

// Comprehensive 15K frequency dictionary - 1 words starting with 'z'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_z[] = {
    "zero",
};

#define DICT_Z_SIZE (sizeof(dict_z) / sizeof(dict_z[0]))

#endif // DICT_Z_H
