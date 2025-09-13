/*
 * ZMK Spell Checker with Levenshtein Distance
 * Comprehensive autocorrect for all words using edit distance algorithm
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <string.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <dt-bindings/zmk/keys.h>

// Common English words dictionary (top 500 most used words)
static const char* dictionary[] = {
    "the", "be", "to", "of", "and", "a", "in", "that", "have", "i",
    "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
    "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
    "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
    "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
    "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
    "people", "into", "year", "your", "good", "some", "could", "them", "see", "other",
    "than", "then", "now", "look", "only", "come", "its", "over", "think", "also",
    "back", "after", "use", "two", "how", "our", "work", "first", "well", "way",
    "even", "new", "want", "because", "any", "these", "give", "day", "most", "us",
    "is", "was", "are", "been", "has", "had", "were", "said", "each", "which",
    "do", "does", "did", "will", "would", "could", "should", "may", "might", "must",
    "shall", "can", "cannot", "could", "should", "would", "might", "may", "need", "dare",
    "ought", "used", "am", "is", "are", "was", "were", "be", "being", "been",
    "have", "has", "had", "having", "do", "does", "did", "doing", "done", "get",
    "got", "gotten", "go", "goes", "went", "going", "gone", "come", "comes", "came",
    "coming", "take", "takes", "took", "taking", "taken", "make", "makes", "made", "making",
    "see", "sees", "saw", "seeing", "seen", "know", "knows", "knew", "knowing", "known",
    "think", "thinks", "thought", "thinking", "say", "says", "said", "saying", "tell",
    "tells", "told", "telling", "ask", "asks", "asked", "asking", "work", "works",
    "worked", "working", "try", "tries", "tried", "trying", "use", "uses", "used",
    "using", "find", "finds", "found", "finding", "give", "gives", "gave", "giving",
    "given", "play", "plays", "played", "playing", "run", "runs", "ran", "running",
    "move", "moves", "moved", "moving", "live", "lives", "lived", "living", "believe",
    "believes", "believed", "believing", "hold", "holds", "held", "holding", "bring",
    "brings", "brought", "bringing", "happen", "happens", "happened", "happening", "write",
    "writes", "wrote", "writing", "written", "sit", "sits", "sat", "sitting", "stand",
    "stands", "stood", "standing", "lose", "loses", "lost", "losing", "pay", "pays",
    "paid", "paying", "meet", "meets", "met", "meeting", "include", "includes", "included",
    "including", "continue", "continues", "continued", "continuing", "set", "sets", "setting",
    "learn", "learns", "learned", "learning", "change", "changes", "changed", "changing",
    "lead", "leads", "led", "leading", "understand", "understands", "understood", "understanding",
    "watch", "watches", "watched", "watching", "follow", "follows", "followed", "following",
    "stop", "stops", "stopped", "stopping", "create", "creates", "created", "creating",
    "speak", "speaks", "spoke", "speaking", "spoken", "read", "reads", "reading", "add",
    "adds", "added", "adding", "spend", "spends", "spent", "spending", "grow", "grows",
    "grew", "growing", "grown", "open", "opens", "opened", "opening", "walk", "walks",
    "walked", "walking", "win", "wins", "won", "winning", "offer", "offers", "offered",
    "offering", "remember", "remembers", "remembered", "remembering", "love", "loves", "loved",
    "loving", "consider", "considers", "considered", "considering", "appear", "appears", "appeared",
    "appearing", "buy", "buys", "bought", "buying", "wait", "waits", "waited", "waiting",
    "serve", "serves", "served", "serving", "die", "dies", "died", "dying", "send",
    "sends", "sent", "sending", "expect", "expects", "expected", "expecting", "build",
    "builds", "built", "building", "stay", "stays", "stayed", "staying", "fall", "falls",
    "fell", "falling", "fallen", "cut", "cuts", "cutting", "reach", "reaches", "reached",
    "reaching", "kill", "kills", "killed", "killing", "remain", "remains", "remained",
    "remaining", "suggest", "suggests", "suggested", "suggesting", "raise", "raises", "raised",
    "raising", "pass", "passes", "passed", "passing", "sell", "sells", "sold", "selling",
    "require", "requires", "required", "requiring", "report", "reports", "reported", "reporting",
    "decide", "decides", "decided", "deciding", "pull", "pulls", "pulled", "pulling"
};

#define DICT_SIZE (sizeof(dictionary) / sizeof(dictionary[0]))
#define MAX_WORD_LEN 15
#define MAX_EDIT_DISTANCE 2  // Allow up to 2 character errors

// Buffer for word extraction
static char current_word[MAX_WORD_LEN] = {0};
static int word_pos = 0;
static bool correcting = false;

// Levenshtein distance calculation
static int min3(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

static int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // Quick optimization: if length difference is too large, skip
    if (abs(len1 - len2) > MAX_EDIT_DISTANCE) {
        return MAX_EDIT_DISTANCE + 1;
    }
    
    // Use static array to avoid dynamic allocation
    static int matrix[MAX_WORD_LEN + 1][MAX_WORD_LEN + 1];
    
    // Initialize first row and column
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    // Fill the matrix
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = min3(
                matrix[i-1][j] + 1,     // deletion
                matrix[i][j-1] + 1,     // insertion
                matrix[i-1][j-1] + cost // substitution
            );
        }
    }
    
    return matrix[len1][len2];
}

// Find best match in dictionary
static const char* find_best_match(const char* word) {
    int best_distance = MAX_EDIT_DISTANCE + 1;
    const char* best_match = NULL;
    
    for (int i = 0; i < DICT_SIZE; i++) {
        int distance = levenshtein_distance(word, dictionary[i]);
        if (distance <= MAX_EDIT_DISTANCE && distance < best_distance) {
            best_distance = distance;
            best_match = dictionary[i];
            
            // Perfect match found
            if (distance == 0) break;
        }
    }
    
    return best_match;
}

// Check if word exists in dictionary (exact match)
static bool is_valid_word(const char* word) {
    for (int i = 0; i < DICT_SIZE; i++) {
        if (strcmp(word, dictionary[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Convert key to char
static char key_to_char(zmk_key_t key) {
    if (key >= HID_USAGE_KEY_KEYBOARD_A && key <= HID_USAGE_KEY_KEYBOARD_Z) {
        return 'a' + (key - HID_USAGE_KEY_KEYBOARD_A);
    }
    if (key == HID_USAGE_KEY_KEYBOARD_SPACEBAR) return ' ';
    if (key == HID_USAGE_KEY_KEYBOARD_PERIOD_AND_GREATER_THAN) return '.';
    if (key == HID_USAGE_KEY_KEYBOARD_COMMA_AND_LESS_THAN) return ',';
    return 0;
}

// Send key event
static void send_key_event(zmk_key_t key, bool pressed) {
    raise_zmk_keycode_state_changed_from_encoded(key, pressed, k_uptime_get());
}

// Type a string
static void type_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        zmk_key_t key = 0;
        
        if (c >= 'a' && c <= 'z') {
            key = HID_USAGE_KEY_KEYBOARD_A + (c - 'a');
        }
        
        if (key != 0) {
            send_key_event(key, true);
            send_key_event(key, false);
            k_msleep(5);  // Faster typing
        }
    }
}

// Perform spell correction
static void correct_word(const char* correct_word, int typo_len) {
    if (correcting) return;
    correcting = true;
    
    // Delete the incorrect word
    for (int i = 0; i < typo_len; i++) {
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, true);
        k_msleep(5);
        send_key_event(HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE, false);
        k_msleep(5);
    }
    
    // Type the correction
    type_string(correct_word);
    
    correcting = false;
}

// Process completed word
static void process_word() {
    if (word_pos == 0) return;  // Empty word
    
    current_word[word_pos] = '\0';  // Null terminate
    
    // Skip very short words (1-2 characters) and very long words
    if (word_pos < 3 || word_pos > MAX_WORD_LEN - 1) {
        word_pos = 0;
        return;
    }
    
    // Check if word is already correct
    if (is_valid_word(current_word)) {
        word_pos = 0;
        return;
    }
    
    // Find best correction
    const char* correction = find_best_match(current_word);
    if (correction != NULL) {
        correct_word(correction, word_pos);
    }
    
    word_pos = 0;  // Reset for next word
}

// Main keystroke handler
int zmk_autocorrect_keyboard_press(zmk_key_t key) {
    if (correcting) return 0;
    
    char c = key_to_char(key);
    if (c == 0) return 0;  // Unsupported key
    
    // Word boundary characters
    if (c == ' ' || c == '.' || c == ',' || c == '\n' || c == '\t') {
        process_word();  // Check and correct the completed word
        return 0;
    }
    
    // Add letter to current word
    if (c >= 'a' && c <= 'z' && word_pos < MAX_WORD_LEN - 1) {
        current_word[word_pos++] = c;
    }
    
    return 0;
}

// Initialize spell checker
static int spell_checker_init(const struct device *dev) {
    memset(current_word, 0, sizeof(current_word));
    word_pos = 0;
    correcting = false;
    return 0;
}

SYS_INIT(spell_checker_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
