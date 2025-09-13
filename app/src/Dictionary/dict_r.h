#ifndef DICT_R_H
#define DICT_R_H

static const char* const dict_r[] = {
    "race", "racial", "radio", "rail", "railroad", "rain", "raise", "range", "rank", "rapid",
    "rapidly", "rare", "rarely", "rate", "rather", "rating", "ratio", "raw", "reach", "react",
    "reaction", "read", "reading", "ready", "real", "reality", "realize", "really", "reason", "reasonable",
    "reasonably", "receive", "recent", "recently", "recognize", "recommend", "recommendation", "record", "recover", "recovery",
    "red", "reduce", "reduction", "refer", "reference", "reflect", "reflection", "reform", "regard", "regarding",
    "regardless", "region", "regional", "register", "regular", "regularly", "regulation", "reject", "relate", "relation",
    "relationship", "relative", "relatively", "relax", "release", "relevant", "reliable", "relief", "religion", "religious",
    "reluctant", "rely", "remain", "remaining", "remarkable", "remember", "remind", "remote", "remove", "repeat",
    "replace", "reply", "report", "represent", "representative", "reputation", "request", "require", "requirement", "research",
    "reserve", "resident", "resolution", "resolve", "resource", "respect", "respond", "response", "responsibility", "responsible",
    "rest", "restaurant", "restore", "restrict", "restriction", "result", "retain", "retire", "retirement", "return",
    "reveal", "revenue", "review", "revolution", "reward", "rhythm", "rice", "rich", "rid", "ride",
    "right", "ring", "rise", "risk", "river", "road", "rob", "rock", "role", "roll",
    "romantic", "roof", "room", "root", "rope", "rose", "rough", "round", "route", "routine",
    "row", "royal", "rub", "rule", "run", "running", "rural", "rush",
    // Words with apostrophes  
    "race's", "radio's", "rain's", "range's", "rate's", "reaction's", "reason's", "record's", "region's", "relationship's",
    "report's", "research's", "result's", "review's", "right's", "risk's", "river's", "road's", "role's", "room's",
    "rule's"
};

#define DICT_R_SIZE (sizeof(dict_r) / sizeof(dict_r[0]))

#endif
