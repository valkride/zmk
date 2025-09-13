#ifndef SPELL_DICTIONARY_H
#define SPELL_DICTIONARY_H

// Common English words dictionary for spell checking
// Organized by frequency of use for better performance
static const char* const dictionary[] = {
    // Most common words (1-50)
    "the", "be", "to", "of", "and", "a", "in", "that", "have", "i",
    "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
    "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
    "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
    "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
    
    // Common words (51-150)
    "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
    "people", "into", "year", "your", "good", "some", "could", "them", "see", "other",
    "than", "then", "now", "look", "only", "come", "its", "over", "think", "also",
    "back", "after", "use", "two", "how", "our", "work", "first", "well", "way",
    "even", "new", "want", "because", "any", "these", "give", "day", "most", "us",
    "is", "was", "are", "been", "has", "had", "were", "said", "each", "which",
    "did", "very", "where", "much", "too", "right", "still", "should", "old", "here",
    "through", "before", "try", "may", "why", "come", "each", "still", "large", "must",
    "big", "group", "those", "often", "run", "important", "until", "children", "side", "feet",
    "car", "mile", "night", "walk", "white", "sea", "began", "grow", "took", "river",
    
    // Additional common words (151-300)
    "four", "carry", "state", "once", "book", "hear", "stop", "without", "second", "later",
    "miss", "idea", "enough", "eat", "face", "watch", "far", "indian", "really", "almost",
    "let", "above", "girl", "sometimes", "mountain", "cut", "young", "talk", "soon", "list",
    "song", "being", "leave", "family", "it's", "body", "music", "color", "stand", "sun",
    "questions", "fish", "area", "mark", "dog", "horse", "birds", "problem", "complete", "room",
    "knew", "since", "ever", "piece", "told", "usually", "didn't", "friends", "easy", "heard",
    "order", "red", "door", "sure", "become", "top", "ship", "across", "today", "during",
    "short", "better", "best", "however", "low", "hours", "black", "products", "happened", "whole",
    "measure", "remember", "early", "waves", "reached", "listen", "wind", "rock", "space", "covered",
    "fast", "several", "hold", "himself", "toward", "five", "step", "morning", "passed", "vowel",
    
    // More words (301-400)
    "true", "hundred", "against", "pattern", "numeral", "table", "north", "slowly", "money", "map",
    "farm", "pulled", "draw", "voice", "seen", "cold", "cried", "plan", "notice", "south",
    "sing", "war", "ground", "fall", "king", "town", "i'll", "unit", "figure", "certain",
    "field", "travel", "wood", "fire", "upon", "done", "english", "road", "half", "ten",
    "fly", "gave", "box", "finally", "wait", "correct", "oh", "quickly", "person", "became",
    "shown", "minutes", "strong", "verb", "stars", "eat", "test", "tree", "never", "pressed",
    "full", "letter", "typing", "point", "built", "object", "scale", "heat", "nothing", "course",
    "wheels", "full", "force", "blue", "object", "decide", "surface", "behind", "special", "clear",
    "tail", "produce", "fact", "street", "inch", "multiply", "nothing", "course", "stay", "wheel",
    "full", "force", "blue", "object", "decide", "surface", "behind", "special", "clear", "tail",
    
    // Final words (401-500)
    "produce", "fact", "street", "inch", "multiply", "nothing", "course", "stay", "wheel", "full",
    "basic", "smell", "valley", "nor", "double", "seat", "continue", "block", "chart", "hat",
    "sell", "success", "company", "subtract", "event", "particular", "deal", "swim", "term", "opposite",
    "wife", "shoe", "shoulder", "spread", "arrange", "camp", "invent", "cotton", "born", "determine",
    "quart", "nine", "truck", "noise", "level", "chance", "gather", "shop", "stretch", "throw",
    "shine", "property", "column", "molecule", "select", "wrong", "gray", "repeat", "require", "broad",
    "prepare", "salt", "nose", "plural", "anger", "claim", "continent", "oxygen", "sugar", "death",
    "pretty", "skill", "women", "season", "solution", "magnet", "silver", "thank", "branch", "match",
    "suffix", "especially", "fig", "afraid", "huge", "sister", "steel", "discuss", "forward", "similar",
    "guide", "experience", "score", "apple", "bought", "led", "pitch", "coat", "mass", "card",
    "band", "rope", "slip", "win", "dream", "evening", "condition", "feed", "tool", "total"
};

// Dictionary size
#define DICTIONARY_SIZE (sizeof(dictionary) / sizeof(dictionary[0]))

#endif // SPELL_DICTIONARY_H
