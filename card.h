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
    if (card > 0) {
        const auto color = card / 10;
        const auto point = card - color * 10;
        switch (color)
        {
            case 1: return point + (0 - 1);
            case 2: return point + (9 - 1);
            case 3: return point + (18 - 1);
            case 4: return point + (27 - 1);
        }
    }
    return -1;
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

    JHSearch(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, callback_t _callback)
    : vec(_vec), gui(_gui), jiang(_jiang), callback(_callback), ckend(&ckbegin[0]) { ckbegin[0] = NOCARD; };

    void* operator new(size_t size, card_size_t cksize) {
        return ::operator new(size + sizeof(card_t[cksize]));
    }

    void operator delete(void* pointee) {
        ::operator delete(pointee);
    }

    void entry();

    inline auto begin() { return ckbegin; }
    inline auto end() { return ckend; }

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

        inline void entry () {
            ((JHSearch*)this)->entry();
        }
    };

protected:
    bool shunx (card_index_t i);
    bool kanx (card_index_t i);
    bool zix (card_index_t i);
    bool hux ();

public:
    inline void ck_push_shun_fine(const card_t c) {
        *ckend++ = c;
        *ckend++ = card_next(c);
        *ckend++ = card_next(card_next(c));
        *ckend++ = NOCARD;
    }
    inline void ck_push_shun_fix(const card_t c, const card_index_t j) {
        *ckend++ = c;
        *ckend++ = j & 1 ? card_next(c) : card_gui(card_next(c));
        *ckend++ = j & 1 ? card_gui(card_next(card_next(c))) : card_next(card_next(c));
        *ckend++ = NOCARD;
    }
    inline void ck_push_shun_fix_front(const card_t c) {
        *ckend++ = card_gui(card_prev(c));
        *ckend++ = c;
        *ckend++ = card_next(c);
        *ckend++ = NOCARD;
    }
    inline void use_shun_fine(const card_t c, const card_index_t i) {
        --vec[i]; --vec[i + 1]; --vec[i + 2];
        ck_push_shun_fine(c);
    }
    inline void use_shun_fix(const card_t c, const card_index_t i, const card_index_t j) {
        --vec[i]; --vec[i + j]; --gui;
        ck_push_shun_fix(c, j);
    }
    inline void use_shun_fix_front(const card_t c, const card_index_t i) {
        --vec[i]; --vec[i + 1]; --gui;
        ck_push_shun_fix_front(c);
    }
    inline void drop_shun_fine(const card_index_t i) {
        ckend -= 4;
        ++vec[i]; ++vec[i + 1]; ++vec[i + 2];
    }
    inline void drop_shun_fine_fine(const card_index_t i) {
        ckend -= 8;
        vec[i] += 2; vec[i + 1] += 2; vec[i + 2] += 2;
    }
    inline void drop_shun_fix(const card_index_t i, const card_index_t j) {
        ckend -= 4;
        ++vec[i]; ++vec[i + j]; ++gui;
    }
    inline void turn_shun_fix(const card_t c, const card_index_t i, const card_index_t j) {
        ++vec[i + (j ^ 3)]; --gui;
        ckend -= 4;
        ck_push_shun_fix(c, j);
    }
};


void searchJH(const card_vector_t& _vec, card_size_t _gui, card_t _jiang, JHSearch::callback_t _callback);


#endif // CARD_INCLUDED
