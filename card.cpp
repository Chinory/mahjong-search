#include "card.h"
#include <iostream>
#include <numeric>

void searchJH(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, JHSearch::callback_t _callback)
{
    card_size_t count = std::accumulate(_vec.begin(), _vec.end(), 0);
    if (count < 15) {
        // std::cout << "JHSearch::Fixed<cksizeof(14)>" << " count: " << count << ", size: " << sizeof(JHSearch::Fixed<cksizeof(14)>) << std::endl;
        JHSearch::Fixed<cksizeof(14)> hu(_vec, _gui, _jiang, _callback);
        hu.entry();
    } else if (count < 21) {
        // std::cout << "JHSearch::Fixed<cksizeof(20)>" << " count: " << count << ", size: " << sizeof(JHSearch::Fixed<cksizeof(20)>) << std::endl;
        JHSearch::Fixed<cksizeof(20)> hu(_vec, _gui, _jiang, _callback);
        hu.entry();
    } else {
        // std::cout << "JHSearch new(cksizeof(" << count << "))" << " count: " << count << ", size: " << sizeof(JHSearch) + cksizeof(count) << std::endl;
        JHSearch* hu = new(cksizeof(count)) JHSearch(_vec, _gui, _jiang, _callback);
        hu->entry();
        delete hu;
    }
}

void JHSearch::ckpipe(std::ostream& os) const
{
    auto i = ckbegin;
    os << '[' << (int)*i;
    for (++i; *i; ++i) os << ',' << (int)*i;
    os << ']';
    for (++i; *i; ++i) { os << ',';
        os << '[' << (int)*i;
        for (++i; *i; ++i) os << ',' << (int)*i;
        os << ']';
    }
}

void JHSearch::entry ()
{
    if (jiang == NOCARD) {
        shunx(0);
    } else {
        ckpush_jiang_fine(jiang);
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
    card_size_t fine = vec[i];
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
            shun_turn_fix(c, i, j);
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
        if (i > 26) return zix(i);
        else if (vec[i] < 1) ++i;
        else if (1 << i & 0x3fdfeff) return shunx(i);
        else break;
    }
    card_count_t rest = vec[i];
    card_count_t fine = 0;
    card_t c = itoc(i);
    while (rest > 2) {
        rest -= 3;
        fine += 1;
        ckpush_kezi_fine(c);
    }
    bool ok;
    switch (rest) {
    case 1: {
        if (gui < 1) {
            ok = false;
        } else if (jiang == NOCARD) {
            gui -= 1;
            jiang = c;
            ckpush_jiang_fix(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            ckend -= 3;
            jiang = NOCARD;
            gui += 1;
        } else if (gui > 1) {
            gui -= 2;
            ckpush_kezi_fix2(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            ckend -= 4;
            gui += 2;
        } else {
            ok = false;
        }
        break;
    }
    case 2: {
        if (jiang == NOCARD) {
            jiang = c;
            ckpush_jiang_fine(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            ckend -= 3;
            jiang = NOCARD;
        } else if (gui > 0) {
            gui -= 1;
            ckpush_kezi_fix1(c);
            ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
            ckend -= 4;
            gui += 1;
        } else {
            ok = false;
        }
        break;
    }
    default:
        ok = 1 << i & 0x1fcfe7f ? shunx(i + 1) : kanx(i + 1);
    }
    ckend -= fine * 4;
    return ok;
}

bool JHSearch::zix (card_index_t i)
{
    if (vec[i] < 1) for (++i;;) {
        if (i > 33) return hux();
        else if (vec[i] < 1) ++i;
        else break;
    }
    card_count_t rest = vec[i];
    card_count_t fine = 0;
    card_t c = itoc(i);
    while (rest > 2) {
        rest -= 3;
        fine += 1;
        ckpush_kezi_fine(c);
    }
    bool ok;
    switch (rest) {
    case 1: {
        if (gui < 1) {
            ok = false;
        } else if (jiang == NOCARD) {
            gui -= 1;
            jiang = c;
            ckpush_jiang_fix(c);
            ok = i < 33 ? zix(i + 1) : hux();
            ckend -= 3;
            jiang = NOCARD;
            gui += 1;
        } else if (gui > 1) {
            gui -= 2;
            ckpush_kezi_fix2(c);
            ok = i < 33 ? zix(i + 1) : hux();
            ckend -= 4;
            gui += 2;
        } else {
            ok = false;
        }
        break;
    }
    case 2: {
        if (jiang == NOCARD) {
            jiang = c;
            ckpush_jiang_fine(c);
            ok = i < 33 ? zix(i + 1) : hux();
            ckend -= 3;
            jiang = NOCARD;
        } else if (gui > 0) {
            gui -= 1;
            ckpush_kezi_fix1(c);
            ok = i < 33 ? zix(i + 1) : hux();
            ckend -= 4;
            gui += 1;
        } else {
            ok = false;
        }
        break;
    }
    default:
        ok = i < 33 ? zix(i + 1) : hux();
    }
    ckend -= fine * 4;
    return ok;
}

bool JHSearch::hux ()
{
    if (jiang != NOCARD) {
        ckend[0] = NOCARD;
        return callback(*this);
    } else if (gui > 1) {
        gui -= 2;
        jiang = ANYGUI;
        ckpush_jiang_fine(ANYGUI);
        ckend[0] = NOCARD;
        bool ok = callback(*this);
        ckend -= 3;
        jiang = NOCARD;
        gui += 2;
        return ok;
    } else {
        return false;
    }
}
