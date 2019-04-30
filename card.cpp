#include "card.h"
#include <iostream>
#include <numeric>

void JHSearch::ckpipe(std::ostream& os) const
{
    auto ckend = ckdata + cksize;
    for (auto i = ckdata; i < ckend; i += 3) {
        os << '[' << (int)i[0] << ',' << (int)i[1] << ',' << (int)i[2] << ']' << ',';
    }
    os << '[' << (int)jiang1 << ',' << (int)jiang2 << ']';
}

bool JHSearch::search (callback_t* _callback)
{
    if (!callback) {
        callback = _callback;
        if (jiang1 != NOCARD) zixnj();
        else if (!zixj()) zixnj();
        callback = nullptr;
        return true;
    } else {
        return false;
    }
}

bool JHSearch::zixnj()
{
    auto _gui = gui;
    for (card_index_t i = 27; i < 34; ++i) {
        auto rest = vec[i];
        if (rest < 1) continue;
        card_t c = itoc(i);
        for (; rest > 2; rest -= 3)
            ckpush_kezi_fine(c);
        switch (rest) {
        case 0: continue;
        case 1:
            if (gui > 1) {
                gui -= 2;
                ckpush_kezi_fix2(c);
                continue;
            } else break;                
        case 2:
            if (gui > 0) {
                gui -= 1;
                ckpush_kezi_fix1(c);
                continue;
            } else break;
        }
        gui = _gui;
        cksize = 0;
        return true;
    }
    bool ok = shunx(0);
    gui = _gui;
    cksize = 0;
    return ok;
}

bool JHSearch::zixj()
{
    auto _gui = gui;
    for (card_index_t i = 27; i < 34; ++i) {
        auto rest = vec[i];
        if (rest < 1) continue;
        card_t c = itoc(i);
        for (; rest > 2; rest -= 3)
            ckpush_kezi_fine(c);
        switch (rest) {
        case 0: continue;
        case 1:
            if (gui < 1) break; else
            if (jiang1 == NOCARD) {
                gui -= 1;
                jiang1 = c;
                jiang2 = card_gui(c);
                continue;
            } else if (gui > 1) {
                gui -= 2;
                ckpush_kezi_fix2(c);
                continue;
            } else break;
        case 2:
            if (jiang1 == NOCARD) {
                jiang1 = c;
                jiang2 = c;
                continue;
            } else if (gui > 0) {
                gui -= 1;
                ckpush_kezi_fix1(c);
                continue;
            } else break;
        }
        jiang1 = NOCARD;
        jiang2 = NOCARD;
        gui = _gui;
        cksize = 0;
        return true; 
    }
    bool ok;
    if (jiang1 == NOCARD) {
        if (gui > 1) {
            gui -= 2;
            jiang1 = ANYGUI;
            jiang2 = ANYGUI; 
            ok = shunx(0);
        } else {
            ok = true;
        }
    } else {
        ok = shunx(0);
    }
    jiang1 = NOCARD;
    jiang2 = NOCARD;
    gui = _gui;
    cksize = 0;
    return ok;
}

bool JHSearch::shunx (card_index_t i)
{
    for (;;) {
        if (1 << i & 0x1fcfe7f) {
            if (vec[i] > 0) break; else ++i;
        } else if (kanx(i)) {
            return true;
        } else if (gui > 0 && vec[i] > 0 && vec[i + 1] > 0) {
            shun_use_fix_front(itoc(i), i);
            bool ok = kanx(i);
            shun_drop_fix(i, 1);
            return ok;
        } else {
            return false;
        }
    }
    if (kanx(i)) {
        return true;
    }
    auto fine = vec[i];
    if (fine > vec[i + 1]) fine = vec[i + 1];
    if (fine > vec[i + 2]) fine = vec[i + 2];
    card_index_t j = vec[i] > fine && gui > 0
        ? vec[i + 1] > fine ? 1
        : vec[i + 2] > fine ? 2 : 0 : 0;
    card_t c = itoc(i);
    if (fine > 0) {
        shun_use_fine(c, i);
        if (kanx(i)) {
            shun_drop_fine(i);
            return true;
        }
        if (fine > 1) {
            shun_use_fine(c, i);
            if (kanx(i)) {
                shun_drop_fine_fine(i);
                return true;
            }
            shun_drop_fine(i);
        }
        if (j) {
            shun_turn_fix(i, j);
            if (kanx(i)) {
                shun_drop_fix(i, j);
                return true;
            }
            shun_drop_fix(i, j);
        } else {
            shun_drop_fine(i);
        }
    } else if (j) {
        shun_use_fix(c, i, j);
        if (kanx(i)) {
            shun_drop_fix(i, j);
            return true;
        }
        shun_drop_fix(i, j);
    }
    return false;
}

bool JHSearch::kanx (card_index_t i)
{
    if (vec[i] < 1) for (++i;;) {
        if (i > 26) return hux();
        else if (vec[i] < 1) ++i;
        else if (1 << i & 0x3fdfeff) return shunx(i);
        else break;
    }
    auto rest = vec[i];
    card_t c = itoc(i);
    while (rest > 2) {
        rest -= 3;
        ckpush_kezi_fine(c);
    }
    bool ok;
    switch (rest) {
    case 1: {
        if (gui < 1) {
            ok = false;
        } else if (jiang1 == NOCARD) {
            gui -= 1;
            jiang1 = c; jiang2 = card_gui(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            jiang1 = NOCARD;
            gui += 1;
        } else if (gui > 1) {
            gui -= 2;
            ckpush_kezi_fix2(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            cksize -= 3;
            gui += 2;
        } else {
            ok = false;
        }
        break;
    }
    case 2: {
        if (jiang1 == NOCARD) {
            jiang1 = c; jiang2 = c;
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            jiang1 = NOCARD;
        } else if (gui > 0) {
            gui -= 1;
            ckpush_kezi_fix1(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            cksize -= 3;
            gui += 1;
        } else {
            ok = false;
        }
        break;
    }
    default:
        ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
    }
    cksize -= (vec[i] - rest);
    return ok;
}


bool JHSearch::hux ()
{
    if (jiang1 != NOCARD) {
        return (*callback)(*this);
    }
    return false;
}
