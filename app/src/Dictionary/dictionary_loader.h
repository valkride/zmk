#ifndef DICTIONARY_LOADER_H
#define DICTIONARY_LOADER_H

// Include all dictionary headers
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

// Function to initialize all dictionaries
static int load_all_dictionaries(void) {
    int result = 0;
    if (load_dict_a_from_file() != 0) result = -1;
    if (load_dict_b_from_file() != 0) result = -1;
    if (load_dict_c_from_file() != 0) result = -1;
    if (load_dict_d_from_file() != 0) result = -1;
    if (load_dict_e_from_file() != 0) result = -1;
    if (load_dict_f_from_file() != 0) result = -1;
    if (load_dict_g_from_file() != 0) result = -1;
    if (load_dict_h_from_file() != 0) result = -1;
    if (load_dict_i_from_file() != 0) result = -1;
    if (load_dict_j_from_file() != 0) result = -1;
    if (load_dict_k_from_file() != 0) result = -1;
    if (load_dict_l_from_file() != 0) result = -1;
    if (load_dict_m_from_file() != 0) result = -1;
    if (load_dict_n_from_file() != 0) result = -1;
    if (load_dict_o_from_file() != 0) result = -1;
    if (load_dict_p_from_file() != 0) result = -1;
    if (load_dict_q_from_file() != 0) result = -1;
    if (load_dict_r_from_file() != 0) result = -1;
    if (load_dict_s_from_file() != 0) result = -1;
    if (load_dict_t_from_file() != 0) result = -1;
    if (load_dict_u_from_file() != 0) result = -1;
    if (load_dict_v_from_file() != 0) result = -1;
    if (load_dict_w_from_file() != 0) result = -1;
    if (load_dict_x_from_file() != 0) result = -1;
    if (load_dict_y_from_file() != 0) result = -1;
    if (load_dict_z_from_file() != 0) result = -1;
    return result;
}

// Function to free all dictionaries
static void free_all_dictionaries(void) {
    free_dict_a_words();
    free_dict_b_words();
    free_dict_c_words();
    free_dict_d_words();
    free_dict_e_words();
    free_dict_f_words();
    free_dict_g_words();
    free_dict_h_words();
    free_dict_i_words();
    free_dict_j_words();
    free_dict_k_words();
    free_dict_l_words();
    free_dict_m_words();
    free_dict_n_words();
    free_dict_o_words();
    free_dict_p_words();
    free_dict_q_words();
    free_dict_r_words();
    free_dict_s_words();
    free_dict_t_words();
    free_dict_u_words();
    free_dict_v_words();
    free_dict_w_words();
    free_dict_x_words();
    free_dict_y_words();
    free_dict_z_words();
}

#endif // DICTIONARY_LOADER_H
