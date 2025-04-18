#include "card.h"
#include <iostream>

void JHSearch::ckpipe(std::ostream& os) const
{
    for (const card_t *i = ckend - cksize; i < ckend; i += 3) {
        os << '[' << static_cast<int>(i[0]) << ',' << static_cast<int>(i[1]) << ',' << static_cast<int>(i[2]) << ']' << ',';
    }
    os << '[' << static_cast<int>(jiang1) << ',' << static_cast<int>(jiang2) << ']';
}

void JHSearch::vecpipe(std::ostream& os) const
{
    const card_count_t *i = vec.begin();
    os << '[' << static_cast<int>(*i);
    for (++i; i != vec.end(); ++i) {
        os << ',' << static_cast<int>(*i);
    }
    os << ']';
}

bool JHSearch::search (callback_t *_callback) noexcept
{
    if (!callback && ckend) {
        callback = _callback;
        if (jiang1 != NOCARD) search_zi_nojiang();
        else if (!search_zi_getjiang()) search_zi_nojiang();
        callback = nullptr;
        return true;
    } else {
        return false;
    }
}

bool JHSearch::search (callback_t *_callback, card_t *_ckend) noexcept
{
    if (!callback) {
        callback = _callback;
        card_t *const __ckend = ckend;
        ckend = _ckend;
        if (jiang1 != NOCARD) search_zi_nojiang();
        else if (!search_zi_getjiang()) search_zi_nojiang();
        ckend = __ckend;
        callback = nullptr;
        return true;
    } else {
        return false;
    }
}

bool JHSearch::search_zi_nojiang() noexcept
{
    card_size_t __gui = gui;
    card_size_t __cksize = cksize;
    for (card_index_t i = 27; i < 34; ++i) {
        card_count_t rest = vec[i];
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
        gui = __gui;
        ckend -= (cksize - __cksize);
        cksize = __cksize;
        return true;
    }
    bool ok = search_shun(0);
    gui = __gui;
    ckend -= (cksize - __cksize);
    cksize = __cksize;
    return ok;
}

bool JHSearch::search_zi_getjiang() noexcept
{
    card_size_t __gui = gui;
    card_size_t __cksize = cksize;
    for (card_index_t i = 27; i < 34; ++i) {
        card_count_t rest = vec[i];
        if (rest < 1) continue;
        card_t c = itoc(i);
        for (; rest > 2; rest -= 3)
            ckpush_kezi_fine(c);
        switch (rest) {
        case 0: continue;
        case 1:
            if (gui < 1) {
                break;
            } else if (jiang1 == NOCARD) {
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
        gui = __gui;
        ckend -= (cksize - __cksize);
        cksize = __cksize;
        return true;
    }
    bool ok;
    if (jiang1 != NOCARD) {
        ok = search_shun(0);
    } else if (gui > 1) {
        gui -= 2;
        jiang1 = ANYGUI;
        jiang2 = ANYGUI;
        ok = search_shun(0);
    } else {
        search_shun(0);
        ok = true;
    }
    jiang1 = NOCARD;
    jiang2 = NOCARD;
    gui = __gui;
    ckend -= (cksize - __cksize);
    cksize = __cksize;
    return ok;
}

bool JHSearch::search_shun (card_index_t i) noexcept
{
    for (;;) {
        if (1 << i & 0x1fcfe7f) {
            if (vec[i] > 0) break; else ++i;
        } else if (search_kezi(i)) {
            return true;
        } else if (gui > 0 && vec[i] > 0 && vec[i + 1] > 0) {
            shun_use_fix_front(itoc(i), i);
            bool ok = search_kezi(i);
            shun_drop_fix(i, 1);
            return ok;
        } else {
            return false;
        }
    }
    if (search_kezi(i)) {
        return true;
    }
    card_count_t fine = vec[i];
    if (fine > vec[i + 1]) fine = vec[i + 1];
    if (fine > vec[i + 2]) fine = vec[i + 2];
    unsigned j = vec[i] > fine && gui > 0
        ? vec[i + 1] > fine ? 1
        : vec[i + 2] > fine ? 2 : 0 : 0;
    card_t c = itoc(i);
    if (fine > 0) {
        shun_use_fine(c, i);
        if (search_kezi(i)) {
            shun_drop_fine(i);
            return true;
        }
        if (fine > 1) {
            shun_use_fine(c, i);
            if (search_kezi(i)) {
                shun_drop_fine_fine(i);
                return true;
            }
            shun_drop_fine(i);
        }
        if (j) {
            shun_turn_fix(i, j);
            if (search_kezi(i)) {
                shun_drop_fix(i, j);
                return true;
            }
            shun_use_fine(c, i);
            if (search_kezi(i)) {
                shun_drop_fix(i, j);
                shun_drop_fine(i);
                return true;
            }
            shun_drop_fix(i, j);
        }
        shun_drop_fine(i);
    } else if (j) {
        shun_use_fix(c, i, j);
        if (search_kezi(i)) {
            shun_drop_fix(i, j);
            return true;
        }
        shun_drop_fix(i, j);
    }
    return false;
}

bool JHSearch::search_kezi (card_index_t i) noexcept
{
    if (vec[i] < 1) for (++i;;) {
        if (i > 26) return search_summary();
        else if (vec[i] < 1) ++i;
        else if (1 << i & 0x3fdfeff) return search_shun(i);
        else break;
    }
    card_count_t rest = vec[i];
    card_t c = itoc(i);
    for (; rest > 2; rest -= 3)
        ckpush_kezi_fine(c);
    bool ok;
    switch (rest) {
    case 1: {
        if (gui < 1) {
            ok = false;
        } else if (jiang1 == NOCARD) {
            gui -= 1;
            jiang1 = c;
            jiang2 = card_gui(c);
            ok = 1 << i & 0x1feff7f ? search_shun(i + 1)
               : 1 << i & 0x2010080 ? search_kezi(i + 1)
                                    : search_summary();
            jiang1 = NOCARD;
            gui += 1;
        } else if (gui > 1) {
            gui -= 2;
            ckpush_kezi_fix2(c);
            ok = 1 << i & 0x1feff7f ? search_shun(i + 1)
               : 1 << i & 0x2010080 ? search_kezi(i + 1)
                                    : search_summary();
            ckend -= 3;
            cksize -= 3;
            gui += 2;
        } else {
            ok = false;
        }
        break;
    }
    case 2: {
        if (jiang1 == NOCARD) {
            jiang1 = c;
            jiang2 = c;
            ok = 1 << i & 0x1feff7f ? search_shun(i + 1)
               : 1 << i & 0x2010080 ? search_kezi(i + 1)
                                    : search_summary();
            jiang1 = NOCARD;
        } else if (gui > 0) {
            gui -= 1;
            ckpush_kezi_fix1(c);
            ok = 1 << i & 0x1feff7f ? search_shun(i + 1)
               : 1 << i & 0x2010080 ? search_kezi(i + 1)
                                    : search_summary();
            ckend -= 3;
            cksize -= 3;
            gui += 1;
        } else {
            ok = false;
        }
        break;
    }
    default:
        ok = 1 << i & 0x1feff7f ? search_shun(i + 1)
           : 1 << i & 0x2010080 ? search_kezi(i + 1)
                                : search_summary();
    }
    rest = vec[i] - rest;
    ckend -= rest;
    cksize -= rest;
    return ok;
}


bool JHSearch::search_summary () noexcept
{
    if (jiang1 != NOCARD) {
        return (*callback)(*this);
    } else {
        return false;
    }
}
