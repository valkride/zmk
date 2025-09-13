#ifndef SPELL_DICTIONARY_MAP_H
#define SPELL_DICTIONARY_MAP_H

#include "dict_a.h"
#include "dict_b.h" 
#include "dict_c.h"
#include "dict_d.h"
#include "dict_e.h"
#include "dict_f.h"
#include "dict_g.h"
#include "dict_h.h"
#include "dict_i.h"
#include "dict_j.h"
#include "dict_k.h"
#include "dict_l.h"
#include "dict_m.h"
#include "dict_n.h"
#include "dict_o.h"
#include "dict_p.h"
#include "dict_q.h"
#include "dict_r.h"
#include "dict_s.h"
#include "dict_t.h"
#include "dict_u.h"
#include "dict_v.h"
#include "dict_w.h"
#include "dict_x.h"
#include "dict_y.h"
#include "dict_z.h"

// Dictionary map structure
typedef struct {
    const char* const* words;
    size_t size;
} dictionary_entry_t;

// Map each letter to its dictionary
static const dictionary_entry_t dictionary_map[26] = {
    [0] = {dict_a, DICT_A_SIZE},   // 'a'
    [1] = {dict_b, DICT_B_SIZE},   // 'b' 
    [2] = {dict_c, DICT_C_SIZE},   // 'c'
    [3] = {dict_d, DICT_D_SIZE},   // 'd'
    [4] = {dict_e, DICT_E_SIZE},   // 'e'
    [5] = {dict_f, DICT_F_SIZE},   // 'f'
    [6] = {dict_g, DICT_G_SIZE},   // 'g'
    [7] = {dict_h, DICT_H_SIZE},   // 'h'  
    [8] = {dict_i, DICT_I_SIZE},   // 'i'
    [9] = {dict_j, DICT_J_SIZE},   // 'j'
    [10] = {dict_k, DICT_K_SIZE},  // 'k'
    [11] = {dict_l, DICT_L_SIZE},  // 'l'
    [12] = {dict_m, DICT_M_SIZE},  // 'm'
    [13] = {dict_n, DICT_N_SIZE},  // 'n'
    [14] = {dict_o, DICT_O_SIZE},  // 'o'
    [15] = {dict_p, DICT_P_SIZE},  // 'p'
    [16] = {dict_q, DICT_Q_SIZE},  // 'q'
    [17] = {dict_r, DICT_R_SIZE},  // 'r'
    [18] = {dict_s, DICT_S_SIZE},  // 's' 
    [19] = {dict_t, DICT_T_SIZE},  // 't'
    [20] = {dict_u, DICT_U_SIZE},  // 'u'
    [21] = {dict_v, DICT_V_SIZE},  // 'v'
    [22] = {dict_w, DICT_W_SIZE},  // 'w'
    [23] = {dict_x, DICT_X_SIZE},  // 'x'
    [24] = {dict_y, DICT_Y_SIZE},  // 'y'
    [25] = {dict_z, DICT_Z_SIZE},  // 'z'
};

// Helper function to get dictionary for a letter
static inline const dictionary_entry_t* get_dictionary_for_letter(char letter) {
    if (letter >= 'a' && letter <= 'z') {
        return &dictionary_map[letter - 'a'];
    }
    return NULL;
}

#endif // SPELL_DICTIONARY_MAP_H
