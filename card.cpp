#include "card.h"
#include <iostream>
#include <numeric>

void searchJH(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, JHSearch::callback_t _callback)
{
    card_size_t count = std::accumulate(_vec.begin(), _vec.end(), 0);
    if (count <= cksizeof(14)) {
        JHSearch::Fixed<cksizeof(14)> hu(_vec, _gui, _jiang, _callback);
        hu.entry();
    } else if (count <= cksizeof(20)) {
        JHSearch::Fixed<cksizeof(20)> hu(_vec, _gui, _jiang, _callback);
        hu.entry();
    } else {
        JHSearch* hu = new(cksizeof(count)) JHSearch(_vec, _gui, _jiang, _callback);
        hu->entry();
        delete hu;
    }
}

void JHSearch::entry ()
{
    if (jiang == NOCARD) {
        shunx(0);
    } else {
        *ckend++ = jiang;
        *ckend++ = jiang;
        *ckend++ = NOCARD;
        shunx(0);
        ckend -= 3;
    }
}

bool JHSearch::shunx (card_index_t i)
{
    for (;;) {
        if (1 << i & 0x1fcfe7f) {
            if (vec[i] > 0) break; else ++i;
        } else if (kanx(i)) {
            return true;
        } else if (gui > 0 && vec[i] > 0 && vec[i + 1] > 0) {
            use_shun_fix_front(itoc(i), i);
            bool ok = kanx(i);
            drop_shun_fix(i, 1);
            return ok;
        } else {
            return false;
        }
    }
    if (kanx(i)) {
        return true;
    }
    card_size_t fine = vec[i];
    if (fine > vec[i + 1]) fine = vec[i + 1];
    if (fine > vec[i + 2]) fine = vec[i + 2];
    card_index_t j = vec[i] > fine && gui > 0
        ? vec[i + 1] > fine ? 1
        : vec[i + 2] > fine ? 2 : 0 : 0;
    card_t c = itoc(i);
    if (fine > 0) {
        use_shun_fine(c, i);
        if (kanx(i)) {
            drop_shun_fine(i);
            return true;
        }
        if (fine > 1) {
            use_shun_fine(c, i);
            if (kanx(i)) {
                drop_shun_fine_fine(i);
                return true;
            }
            drop_shun_fine(i);
        }
        if (j) {
            turn_shun_fix(c, i, j);
            if (kanx(i)) {
                drop_shun_fix(i, j);
                return true;
            }
            drop_shun_fix(i, j);
        } else {
            drop_shun_fine(i);
        }
    } else if (j) {
        use_shun_fix(c, i, j);
        if (kanx(i)) {
            drop_shun_fix(i, j);
            return true;
        }
        drop_shun_fix(i, j);
    }
    return false;
}

bool JHSearch::kanx (card_index_t i)
{
    if (vec[i] < 1) for (++i;;) {
        if (i > 26) return zix(i);
        else if (vec[i] < 1) ++i;
        else if (1 << i & 0x3fdfeff) return shunx(i);
        else break;
    }
    card_t c = itoc(i);
    card_size_t count;
    for (count = vec[i]; count > 2; count -= 3) { // fine kezi
        *ckend++ = c;
        *ckend++ = c;
        *ckend++ = c;
        *ckend++ = NOCARD;
    }
    bool ok;
    switch (count){
        case 1: {
            if (gui < 1) {
                ok = false;
            } else if (jiang == NOCARD) { // fix jiang
                gui -= 1;
                jiang = c;
                *ckend++ = c;
                *ckend++ = card_gui(c);
                *ckend++ = NOCARD;
                ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
                ckend -= 3;
                jiang = NOCARD;
                gui += 1;
            } else if (gui > 1) { // fix fix kezi
                gui -= 2;
                *ckend++ = c;
                *ckend++ = card_gui(c);
                *ckend++ = card_gui(c);
                *ckend++ = NOCARD;
                ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
                ckend -= 4;
                gui += 2;
            } else {
                ok = false;
            }
            break;
        }
        case 2: {
            if (jiang == NOCARD) { // fine jiang
                jiang = c;
                *ckend++ = c;
                *ckend++ = c;
                *ckend++ = NOCARD;
                ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
                ckend -= 3;
                jiang = NOCARD;
            } else if (gui > 0) { // fix kezi
                gui -= 1;
                *ckend++ = c;
                *ckend++ = c;
                *ckend++ = card_gui(c);
                *ckend++ = NOCARD;
                ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
                ckend -= 4;
                gui += 1;
            } else {
                ok = false;
            }
            break;
        }
        default: {
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
        }
    }
    for (count = vec[i]; count > 2; count -= 3) {
        ckend -= 4;
    }
    return ok;
}

bool JHSearch::zix (card_index_t i)
{
    if (vec[i] < 1) for (++i;;) {
        if (i > 33) return hux();
        else if (vec[i] < 1) ++i;
        else break;
    }
    card_t c = itoc(i);
    card_size_t count;
    for (count = vec[i]; count > 2; count -= 3) { // fine kezi
        *ckend++ = c;
        *ckend++ = c;
        *ckend++ = c;
        *ckend++ = NOCARD;
    }
    bool ok;
    switch (count){
        case 1: {
            if (gui < 1) {
                ok = false;
            } else if (jiang == NOCARD) { // fix jiang
                gui -= 1;
                jiang = c;
                *ckend++ = c;
                *ckend++ = card_gui(c);
                *ckend++ = NOCARD;
                ok = i < 33 ? zix(i + 1) : hux();
                ckend -= 3;
                jiang = NOCARD;
                gui += 1;
            } else if (gui > 1) { // fix fix kezi
                gui -= 2;
                *ckend++ = c;
                *ckend++ = card_gui(c);
                *ckend++ = card_gui(c);
                *ckend++ = NOCARD;
                ok = i < 33 ? zix(i + 1) : hux();
                ckend -= 4;
                gui += 2;
            } else {
                ok = false;
            }
            break;
        }
        case 2: {
            if (jiang == NOCARD) { // fine jiang
                jiang = c;
                *ckend++ = c;
                *ckend++ = c;
                *ckend++ = NOCARD;
                ok = i < 33 ? zix(i + 1) : hux();
                ckend -= 3;
                jiang = NOCARD;
            } else if (gui > 0) { // fix kezi
                gui -= 1;
                *ckend++ = c;
                *ckend++ = c;
                *ckend++ = card_gui(c);
                *ckend++ = NOCARD;
                ok = i < 33 ? zix(i + 1) : hux();
                ckend -= 4;
                gui += 1;
            } else {
                ok = false;
            }
            break;
        }
        default: {
            ok = i < 33 ? zix(i + 1) : hux();
        }
    }
    for (count = vec[i]; count > 2; count -= 3) {
        ckend -= 4;
    }
    return ok;
}

bool JHSearch::hux ()
{
    if (jiang != NOCARD) {
        *ckend = NOCARD;
        return callback(*this);
    } else if (gui > 1) {
        gui -= 2;
        jiang = ANYGUI;
        *ckend++ = ANYGUI;
        *ckend++ = ANYGUI;
        *ckend++ = NOCARD;
        *ckend = NOCARD;
        bool ok = callback(*this);
        ckend -= 3;
        jiang = NOCARD;
        gui += 2;
        return ok;
    } else {
        return false;
    }
}
