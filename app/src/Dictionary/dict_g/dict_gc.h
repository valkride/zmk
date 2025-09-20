#ifndef DICT_GC_H
#define DICT_GC_H

static const char* const dict_gc[] = {
    "gc",
    "gcc",
    "gcse",
};

#define DICT_GC_SIZE (sizeof(dict_gc) / sizeof(dict_gc[0]))

#endif // DICT_GC_H
