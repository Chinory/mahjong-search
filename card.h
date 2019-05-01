#ifndef CARD_INCLUDED
#define CARD_INCLUDED

#include <array>
#include <vector>
#include <functional>

typedef int8_t    card_t;
typedef uint8_t   card_count_t;
typedef uint16_t  card_size_t;
typedef uint_fast8_t  card_index_t;
typedef std::array<card_count_t, 34> card_vector_t;

#define ANYGUI -1
#define NOCARD 0
#define card_next(c) ((c)+1)
#define card_prev(c) ((c)-1)
#define card_gui(c) (-(c))

#define CARD_DEC

#ifdef CARD_DEC
constexpr card_index_t ctoi (const card_t card)
{
    return static_cast<card_index_t>(
        card < 11 ? -1
        : card < 20 ? card - (11 - 0)
        : card < 30 ? card - (21 - 9)
        : card < 40 ? card - (31 - 18)
        : card < 48 ? card - (41 - 27)
        : -1
    );
}
constexpr card_t itoc (const card_index_t index)
{
    return static_cast<card_t>(
        index < 18 ? index < 9 ? index + (11 - 0) : index + (21 - 9)
        : index < 27 ? index + (31 - 18)
        : index < 34 ? index + (41 - 27)
        : NOCARD
    );
}
#endif // CARD_DEC

#ifdef CARD_HEX
#if __cplusplus < 201402L
inline card_index_t ctoi (const card_t card)
#else
constexpr card_index_t ctoi (const card_t card)
#endif
{
    card_index_t index = static_cast<card_index_t>(-1);
    card_index_t point = card & 15;
    if (point) switch (card >> 4) {
        case 1: { if (point < 10) index = point - 1; break; }
        case 2: { if (point < 10) index = point + 8; break; }
        case 3: { if (point < 10) index = point + 17; break; }
        case 4: { if (point < 8) index = point + 26; break; }
    }
    return index;
}
constexpr card_t itoc (const card_index_t index)
{
    return static_cast<card_t>(
        index < 18 ? index < 9 ? (index + 1) | 0x10 : (index - 8) | 0x20
        : index < 27 ? (index - 17) | 0x30
        : index < 34 ? (index - 26) | 0x40
        : NOCARD
    );
}
#endif // CARD_HEX

#ifdef CARD_CONTI
constexpr card_index_t ctoi (const card_t card)
{
    return static_cast<card_index_t>(
        card > 0 && card < 36 ? card - 1 : -1
    );
}
constexpr card_t itoc (const card_index_t index)
{
    return static_cast<card_t>(
        index < 34 ? index + 1 : NOCARD
    );
}
#endif // CARD_CONTI

class JHSearch
{
public:
    typedef std::function<bool(const JHSearch&)> callback_t;
    card_vector_t vec;
    card_t jiang1;
    card_t jiang2;
    card_size_t gui;
    card_size_t cksize;
    card_t     *ckend;
    callback_t *callback;

    JHSearch(card_size_t _gui = 0, card_t _jiang = NOCARD)
    : vec(), jiang1(_jiang), jiang2(_jiang), gui(_gui), cksize(0), ckend(nullptr), callback(nullptr) {}

    JHSearch(const card_vector_t& _vec, card_size_t _gui = 0, card_t _jiang = NOCARD)
    : vec(_vec), jiang1(_jiang), jiang2(_jiang), gui(_gui), cksize(0), ckend(nullptr), callback(nullptr) {}

    void ckpipe(std::ostream& os) const;
    void vecpipe(std::ostream& os) const;

    card_size_t cksize_gui () { return gui < 34 ? gui * 2 : 34 * 2; }

    bool search(callback_t *_callback);
    bool search(callback_t *_callback, card_t *_ckend);

    inline void ckpush_shun_fine(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_next(c);
        ckend[2] = card_next(card_next(c));
        ckend += 3;
        cksize += 3;
    }
    inline void ckpush_shun_fix_front(const card_t c) {
        ckend[0] = card_gui(card_prev(c));
        ckend[1] = c;
        ckend[2] = card_next(c);
        ckend += 3;
        cksize += 3;
    }
    inline void ckpush_shun_fix_middle(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_gui(card_next(c));
        ckend[2] = card_next(card_next(c));
        ckend += 3;
        cksize += 3;
    }
    inline void ckpush_shun_fix_back(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_next(c);
        ckend[2] = card_gui(card_next(card_next(c)));
        ckend += 3;
        cksize += 3;
    }
    inline void ckpush_kezi_fine(const card_t c) {
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = c;
        ckend += 3;
        cksize += 3;
    }
    inline void ckpush_kezi_fix1(const card_t c) {
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = card_gui(c);
        ckend += 3;
        cksize += 3;
    }
    inline void ckpush_kezi_fix2(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_gui(c);
        ckend[2] = card_gui(c);
        ckend += 3;
        cksize += 3;
    }
    inline void shun_use_fine(const card_t c, const card_index_t i) {
        vec[i + 0] -= 1;
        vec[i + 1] -= 1;
        vec[i + 2] -= 1;
        ckpush_shun_fine(c);
    }
    inline void shun_use_fix(const card_t c, const card_index_t i, const uint8_t j) {
        if (j & 1) {
            vec[i + 0] -= 1;
            vec[i + 1] -= 1;
            gui -= 1;
            ckpush_shun_fix_back(c);
        } else {
            vec[i + 0] -= 1;
            vec[i + 2] -= 1;
            gui -= 1;
            ckpush_shun_fix_middle(c);
        }
    }
    inline void shun_use_fix_front(const card_t c, const card_index_t i) {
        vec[i + 0] -= 1;
        vec[i + 1] -= 1;
        gui -= 1;
        ckpush_shun_fix_front(c);
    }
    inline void shun_drop_fine(const card_index_t i) {
        ckend -= 3;
        cksize -= 3;
        vec[i + 0] += 1;
        vec[i + 1] += 1;
        vec[i + 2] += 1;
    }
    inline void shun_drop_fine_fine(const card_index_t i) {
        ckend -= 6;
        cksize -= 6;
        vec[i + 0] += 2;
        vec[i + 1] += 2;
        vec[i + 2] += 2;
    }
    inline void shun_drop_fix(const card_index_t i, const uint8_t j) {
        ckend -= 3;
        cksize -= 3;
        vec[i + 0] += 1;
        vec[i + j] += 1;
        gui += 1;
    }
    inline void shun_turn_fix(const card_index_t i, const uint8_t j) {
        if (j & 1) {
            vec[i + 2] += 1;
            gui -= 1;
            ckend[-1] = card_gui(ckend[-1]);
        } else {
            vec[i + 1] += 1;
            gui -= 1;
            ckend[-2] = card_gui(ckend[-2]);
        }
    }

protected:
    bool search_zi_getjiang ();
    bool search_zi_nojiang ();
    bool search_shun (card_index_t i);
    bool search_kezi (card_index_t i);
    bool search_summary ();
};


#endif // CARD_INCLUDED
