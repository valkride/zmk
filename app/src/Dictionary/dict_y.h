#ifndef DICT_Y_H
#define DICT_Y_H

static const char* const dict_y[] = {
    "yard", "yeah", "year", "yellow", "yes", "yesterday", "yet", "yield", "yoga", "you",
    "young", "younger", "youngest", "your", "yours", "yourself", "youth",
    // Words with apostrophes
    "year's", "youth's", "you're", "you've", "you'll", "you'd"
};

#define DICT_Y_SIZE (sizeof(dict_y) / sizeof(dict_y[0]))

#endif
