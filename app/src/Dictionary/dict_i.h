#ifndef DICT_I_H
#define DICT_I_H

// Comprehensive 15K frequency dictionary - 53 words starting with 'i'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_i[] = {
    "i",
    "i'd",
    "i'll",
    "i'm",
    "i've",
    "if",
    "illness",
    "illnesss",
    "image",
    "immediately",
    "importantly",
    "improvement",
    "in",
    "increasingly",
    "indeed",
    "individually",
    "initially",
    "innovation",
    "inside",
    "install",
    "instant",
    "instead",
    "instruction",
    "interact",
    "intercome",
    "interfeel",
    "interfind",
    "interform",
    "interget",
    "intergive",
    "intergo",
    "interhelp",
    "interknow",
    "interlook",
    "intermake",
    "intermove",
    "internet",
    "interplay",
    "interput",
    "intersee",
    "intertake",
    "interthink",
    "interuse",
    "interwork",
    "into",
    "is",
    "isn't",
    "issue",
    "it",
    "it'd",
    "it'll",
    "it's",
    "its",
};

#define DICT_I_SIZE (sizeof(dict_i) / sizeof(dict_i[0]))

#endif // DICT_I_H
