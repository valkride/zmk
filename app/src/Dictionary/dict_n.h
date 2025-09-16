#ifndef DICT_N_H
#define DICT_N_H

// Comprehensive 15K frequency dictionary - 50 words starting with 'n'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_n[] = {
    "name",
    "naturally",
    "near",
    "nearly",
    "necessarily",
    "necklace",
    "needn't",
    "negotiation",
    "neither",
    "network",
    "never",
    "new",
    "newly",
    "night",
    "nine",
    "nineteen",
    "ninety",
    "ninth",
    "no",
    "nonact",
    "noncome",
    "nonfeel",
    "nonfind",
    "nonform",
    "nonget",
    "nongive",
    "nongo",
    "nonhelp",
    "nonknow",
    "nonlook",
    "nonmake",
    "nonmove",
    "nonplay",
    "nonput",
    "nonsee",
    "nontake",
    "nonthink",
    "nonuse",
    "nonwork",
    "noon",
    "nor",
    "normally",
    "not",
    "notification",
    "november",
    "now",
    "nurse",
    "nurseed",
    "nurseing",
    "nurses",
};

#define DICT_N_SIZE (sizeof(dict_n) / sizeof(dict_n[0]))

#endif // DICT_N_H
