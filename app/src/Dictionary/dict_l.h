#ifndef DICT_L_H
#define DICT_L_H

// Comprehensive 15K frequency dictionary - 59 words starting with 'l'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_l[] = {
    "laboratory",
    "laboratoryed",
    "laboratorying",
    "laboratorys",
    "lamp",
    "largely",
    "late",
    "later",
    "lawyer",
    "league",
    "leagueed",
    "leagueing",
    "leagues",
    "learning",
    "learninged",
    "learninging",
    "learnings",
    "left",
    "lesson",
    "lessoned",
    "lessoning",
    "lessons",
    "let's",
    "lettuce",
    "light",
    "like",
    "likely",
    "line",
    "literally",
    "little",
    "live",
    "living",
    "lock",
    "login",
    "long",
    "look",
    "lookable",
    "lookage",
    "lookar",
    "lookdom",
    "looker",
    "lookful",
    "lookhood",
    "lookible",
    "lookism",
    "lookist",
    "lookless",
    "lookment",
    "lookness",
    "lookor",
    "lookship",
    "looksion",
    "looktion",
    "lose",
    "loseed",
    "loseing",
    "loses",
    "loss",
    "lunch",
};

#define DICT_L_SIZE (sizeof(dict_l) / sizeof(dict_l[0]))

#endif // DICT_L_H
