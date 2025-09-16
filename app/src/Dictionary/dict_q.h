#ifndef DICT_Q_H
#define DICT_Q_H

// Comprehensive 15K frequency dictionary - 5 words starting with 'q'
// Based on English word frequency analysis and common usage patterns
// Optimized for nRF52840 1MB flash memory constraint
// Target coverage: ~95% of typical English writing

static const char* const dict_q[] = {
    "quadruple",
    "quality",
    "quarter",
    "quickly",
    "quite",
};

#define DICT_Q_SIZE (sizeof(dict_q) / sizeof(dict_q[0]))

#endif // DICT_Q_H
