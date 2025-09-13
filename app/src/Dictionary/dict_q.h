#ifndef DICT_Q_H
#define DICT_Q_H

static const char* const dict_q[] = {
    "qualify", "quality", "quantity", "quarter", "quarterly", "queen", "quest", "question", "questionnaire", "quick",
    "quickly", "quiet", "quietly", "quit", "quite", "quiz", "quota", "quote",
    // Words with apostrophes
    "quality's", "quarter's", "queen's", "question's", "quiz's", "quote's"
};

#define DICT_Q_SIZE (sizeof(dict_q) / sizeof(dict_q[0]))

#endif
