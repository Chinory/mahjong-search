#ifndef CARD_INCLUDED
#define CARD_INCLUDED

#include <array>
#include <vector>
#include <string>
#include <cstddef>
#include <cstring>
#include <functional>

typedef signed char card_t;
typedef size_t card_size_t;

typedef uint_least32_t card_index_t;
typedef unsigned char  card_count_t;
typedef std::array<card_count_t, 34> card_vector_t;

#define ANYGUI -1
#define NOCARD 0
#define card_next(c) ((c)+1)
#define card_prev(c) ((c)-1)
#define card_gui(c) (-(c))

#define cksizeof(n) ((n)<<2)

inline card_index_t ctoi (const card_t card)
{
    if (card < 11) return -1;
    else if (card < 20) return card - 11;
    else if (card < 30) return card - 21;
    else if (card < 40) return card - 31;
    else if (card < 48) return card - 41;
    else return -1;
}

inline card_t itoc (const card_index_t index)
{
    if (index < 9) return index + (11 - 0);
    else if (index < 18) return index + (21 - 9);
    else if (index < 27) return index + (31 - 18);
    else if (index < 34) return index + (41 - 27);
    else return NOCARD;
}

class JHSearch
{
public:
    typedef std::function<bool(const JHSearch&)> callback_t;

    card_vector_t vec;
    card_t jiang;
    card_size_t gui;
    callback_t callback;
    card_t *ckend;
    card_t ckbegin[];

    template <card_size_t CKSIZE>
    class Fixed
    {
    public:
        card_vector_t vec;
        card_t jiang;
        card_size_t gui;
        callback_t callback;
        card_t *ckend;
        card_t ckbegin[CKSIZE];

        Fixed(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, callback_t _callback)
        : vec(_vec), gui(_gui), jiang(_jiang), callback(_callback), ckend(&ckbegin[0]) { ckbegin[0] = NOCARD; };

        inline void entry () { ((JHSearch*)this)->entry(); }
    };

    JHSearch(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, callback_t _callback)
    : vec(_vec), gui(_gui), jiang(_jiang), callback(_callback), ckend(&ckbegin[0]) { ckbegin[0] = NOCARD; };

    void* operator new(size_t size, card_size_t cksize) {
        return ::operator new(size + sizeof(card_t[cksize]));
    }
    void operator delete(void* pointee) {
        ::operator delete(pointee);
    }

    void entry();

    void ckpipe(std::ostream& os) const;

    inline void ckpush_shun_fine(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_next(c);
        ckend[2] = card_next(card_next(c));
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_shun_fix_front(const card_t c) {
        ckend[0] = card_gui(card_prev(c));
        ckend[1] = c;
        ckend[2] = card_next(c);
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_shun_fix_middle(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_gui(card_next(c));
        ckend[2] = card_next(card_next(c));
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_shun_fix_back(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_next(c);
        ckend[2] = card_gui(card_next(card_next(c)));
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_kezi_fine(const card_t c) {
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = c;
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_kezi_fix1(const card_t c) {
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = card_gui(c);
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_kezi_fix2(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_gui(c);
        ckend[2] = card_gui(c);
        ckend[3] = NOCARD;
        ckend += 4;
    }
    inline void ckpush_jiang_fine(const card_t c) {
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = NOCARD;
        ckend += 3;
    }
    inline void ckpush_jiang_fix(const card_t c) {
        ckend[0] = c;
        ckend[1] = card_gui(c);
        ckend[2] = NOCARD;
        ckend += 3;
    }
    inline void shun_use_fine(const card_t c, const card_index_t i) {
        vec[i + 0] -= 1;
        vec[i + 1] -= 1;
        vec[i + 2] -= 1;
        ckpush_shun_fine(c);
    }
    inline void shun_use_fix(const card_t c, const card_index_t i, const card_index_t j) {
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
        ckend -= 4;
        vec[i + 0] += 1;
        vec[i + 1] += 1;
        vec[i + 2] += 1;
    }
    inline void shun_drop_fine_fine(const card_index_t i) {
        ckend -= 8;
        vec[i + 0] += 2;
        vec[i + 1] += 2;
        vec[i + 2] += 2;
    }
    inline void shun_drop_fix(const card_index_t i, const card_index_t j) {
        ckend -= 4;
        vec[i + 0] += 1;
        vec[i + j] += 1;
        gui += 1;
    }
    inline void shun_turn_fix(const card_t c, const card_index_t i, const card_index_t j) {
        if (j & 1) {
            vec[i + 2] += 1;
            gui += 1;
            ckend[-2] = card_next(c);
            ckend[-1] = card_gui(card_next(card_next(c)));
        } else {
            vec[i + 1] += 1;
            gui += 1;
            ckend[-2] = card_gui(card_next(c));
            ckend[-1] = card_next(card_next(c));
        }
    }

protected:
    bool shunx (card_index_t i);
    bool kanx (card_index_t i);
    bool zix (card_index_t i);
    bool hux ();
};


void searchJH(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, JHSearch::callback_t _callback);


#endif // CARD_INCLUDED
