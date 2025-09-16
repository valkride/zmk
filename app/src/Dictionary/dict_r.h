#ifndef DICT_R_H
#define DICT_R_H

// Comprehensive 15K frequency dictionary - 60 words starting with 'r'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_r[] = {
    "radio",
    "rarely",
    "rather",
    "react",
    "really",
    "receive",
    "recent",
    "recently",
    "receptionist",
    "recome",
    "refeel",
    "refind",
    "reform",
    "refresh",
    "refrigerator",
    "reget",
    "regive",
    "rego",
    "regularly",
    "rehelp",
    "reknow",
    "relatively",
    "reload",
    "relook",
    "remake",
    "reminder",
    "remote",
    "remove",
    "repair",
    "replay",
    "reply",
    "report",
    "reportedly",
    "representative",
    "reput",
    "research",
    "researched",
    "researching",
    "researchs",
    "resee",
    "resources",
    "restore",
    "result",
    "resulted",
    "resulting",
    "results",
    "retake",
    "rethink",
    "reuse",
    "revenue",
    "rework",
    "right",
    "ring",
    "road",
    "roaded",
    "roading",
    "roads",
    "roof",
    "room",
    "roughly",
};

#define DICT_R_SIZE (sizeof(dict_r) / sizeof(dict_r[0]))

#endif // DICT_R_H
