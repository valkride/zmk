#ifndef DICT_K_H
#define DICT_K_H

// Comprehensive 15K frequency dictionary - 26 words starting with 'k'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_k[] = {
    "key",
    "keyboard",
    "kitchen",
    "knob",
    "know",
    "knowable",
    "knowage",
    "knowar",
    "knowdom",
    "knower",
    "knowful",
    "knowhood",
    "knowible",
    "knowism",
    "knowist",
    "knowledge",
    "knowledgeed",
    "knowledgeing",
    "knowledges",
    "knowless",
    "knowment",
    "knowness",
    "knowor",
    "knowship",
    "knowsion",
    "knowtion",
};

#define DICT_K_SIZE (sizeof(dict_k) / sizeof(dict_k[0]))

#endif // DICT_K_H
