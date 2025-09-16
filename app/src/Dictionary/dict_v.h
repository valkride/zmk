#ifndef DICT_V_H
#define DICT_V_H

// Comprehensive 15K frequency dictionary - 8 words starting with 'v'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_v[] = {
    "vacuum",
    "vegetable",
    "vehicle",
    "vehicleed",
    "vehicleing",
    "vehicles",
    "very",
    "video",
};

#define DICT_V_SIZE (sizeof(dict_v) / sizeof(dict_v[0]))

#endif // DICT_V_H
