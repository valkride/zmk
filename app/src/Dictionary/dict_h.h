#ifndef DICT_H_H
#define DICT_H_H

static const char* const dict_h[] = {
    "habit", "hair", "half", "hall", "hand", "handful", "handle", "hang", "happen", "happy",
    "hard", "hardly", "harm", "hat", "hate", "have", "he", "head", "headline", "headquarters",
    "health", "healthy", "hear", "hearing", "heart", "heat", "heavy", "heel", "height", "helicopter",
    "hell", "help", "helpful", "her", "here", "heritage", "hero", "herself", "hide", "high",
    "highlight", "highly", "hill", "him", "himself", "hip", "hire", "his", "historian", "historic",
    "historical", "history", "hit", "hold", "hole", "holiday", "holy", "home", "homeless", "honest",
    "honey", "honor", "hope", "horizon", "horror", "horse", "hospital", "host", "hot", "hotel",
    "hour", "house", "household", "housing", "how", "however", "huge", "human", "humor", "hundred",
    "hungry", "hunter", "hunting", "hurt", "husband", "hypothesis",
    // Words with apostrophes
    "habit's", "hair's", "hand's", "hat's", "head's", "health's", "heart's", "help's", "her's", "hero's",
    "hill's", "history's", "home's", "hope's", "horse's", "hospital's", "hotel's", "house's", "husband's", "he's",
    "here's", "how's"
};

#define DICT_H_SIZE (sizeof(dict_h) / sizeof(dict_h[0]))

#endif