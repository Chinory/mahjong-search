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
    if (card < 11) return static_cast<card_index_t>(-1);
    else if (card < 20) return static_cast<card_index_t>(card) - (11 - 0);
    else if (card < 30) return static_cast<card_index_t>(card) - (21 - 9);
    else if (card < 40) return static_cast<card_index_t>(card) - (31 - 18);
    else if (card < 48) return static_cast<card_index_t>(card) - (41 - 27);
    else return static_cast<card_index_t>(-1);
}
constexpr card_t itoc (const card_index_t index)
{
    if (index < 18) {
        if (index < 9) {
            return static_cast<card_t>(index + (11 - 0));
        } else {
            return static_cast<card_t>(index + (21 - 9));
        }
    } else if (index < 27) {
        return static_cast<card_t>(index + (31 - 18));
    } else if (index < 34) {
        return static_cast<card_t>(index + (41 - 27));
    } else {
        return NOCARD;
    }
}
#endif // CARD_DEC

#ifdef CARD_HEX
constexpr card_index_t ctoi (const card_t card)
{
    card_index_t point = card & 15;
    if (point) {
        switch (card >> 4) {
            case 1: return point < 10 ? point - 1 : static_cast<card_index_t>(-1);
            case 2: return point < 10 ? point + 8 : static_cast<card_index_t>(-1);
            case 3: return point < 10 ? point + 17 : static_cast<card_index_t>(-1);
            case 4: return point < 8 ? point + 26 : static_cast<card_index_t>(-1);
        }
    }
    return static_cast<card_index_t>(-1);
}
constexpr card_t itoc (const card_index_t index)
{
    if (index < 18) {
        if (index < 9) {
            return static_cast<card_t>(index + 1) | 0x10;
        } else {
            return static_cast<card_t>(index - 8) | 0x20;
        }
    } else if (index < 27) {
        return static_cast<card_t>(index - 17) | 0x30;
    } else if (index < 34) {
        return static_cast<card_t>(index - 26) | 0x40;
    } else {
        return NOCARD;
    }
}
#endif // CARD_HEX

#ifdef CARD_CONTI
constexpr card_index_t ctoi (const card_t card)
{
    if (card > 0 && card < 36) {
        return static_cast<card_index_t>(card) - 1;
    } else {
        return static_cast<card_index_t>(-1);
    }
}
constexpr card_t itoc (const card_index_t index)
{
    if (index < 34) {
        return static_cast<card_t>(index + 1);
    } else {
        return NOCARD;
    }
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
    callback_t *callback;
    card_t ckdata[];

    template <card_size_t MAXCARD>
    class Fixed
    {
    public:
        card_vector_t vec;
        card_t jiang1;
        card_t jiang2;
        card_size_t gui;
        card_size_t cksize;
        callback_t *callback;
        card_t ckdata[MAXCARD];

        Fixed(card_size_t _gui = 0, card_t _jiang = NOCARD)
        : vec(), jiang1(_jiang), jiang2(_jiang), gui(_gui), cksize(0), callback(nullptr) {}

        Fixed(const card_vector_t& _vec, card_size_t _gui = 0, card_t _jiang = NOCARD)
        : vec(_vec), jiang1(_jiang), jiang2(_jiang), gui(_gui), cksize(0), callback(nullptr) {}

        operator JHSearch& () { return *reinterpret_cast<JHSearch*>(this); }
    };

    JHSearch(card_size_t _gui = 0, card_t _jiang = NOCARD)
    : vec(), jiang1(_jiang), jiang2(_jiang), gui(_gui), cksize(0), callback(nullptr) {}

    JHSearch(const card_vector_t& _vec, card_size_t _gui = 0, card_t _jiang = NOCARD)
    : vec(_vec), jiang1(_jiang), jiang2(_jiang), gui(_gui), cksize(0), callback(nullptr) {}

    static constexpr size_t size(const card_size_t maxcard) {
        return sizeof(JHSearch) + sizeof(card_t) * maxcard;
    }
    void* operator new (size_t, void* p) {
        return p;
    }
    void* operator new (size_t count, card_size_t maxcard) {
        return ::operator new(count + sizeof(card_t) * maxcard);
    }
    void* operator new[] (size_t count, card_size_t maxcard) {
        return ::operator new(count + sizeof(card_t) * maxcard);
    }
    void operator delete (void* ptr) {
        ::operator delete(ptr);
    }

    void ckpipe(std::ostream& os) const;
    void vecpipe(std::ostream& os) const;

    bool search(callback_t* _callback);

    inline void ckpush_shun_fine(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = c;
        ckend[1] = card_next(c);
        ckend[2] = card_next(card_next(c));
        cksize += 3;
    }
    inline void ckpush_shun_fix_front(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = card_gui(card_prev(c));
        ckend[1] = c;
        ckend[2] = card_next(c);
        cksize += 3;
    }
    inline void ckpush_shun_fix_middle(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = c;
        ckend[1] = card_gui(card_next(c));
        ckend[2] = card_next(card_next(c));
        cksize += 3;
    }
    inline void ckpush_shun_fix_back(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = c;
        ckend[1] = card_next(c);
        ckend[2] = card_gui(card_next(card_next(c)));
        cksize += 3;
    }
    inline void ckpush_kezi_fine(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = c;
        cksize += 3;
    }
    inline void ckpush_kezi_fix1(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = c;
        ckend[1] = c;
        ckend[2] = card_gui(c);
        cksize += 3;
    }
    inline void ckpush_kezi_fix2(const card_t c) {
        card_t* ckend = ckdata + cksize;
        ckend[0] = c;
        ckend[1] = card_gui(c);
        ckend[2] = card_gui(c);
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
        cksize -= 3;
        vec[i + 0] += 1;
        vec[i + 1] += 1;
        vec[i + 2] += 1;
    }
    inline void shun_drop_fine_fine(const card_index_t i) {
        cksize -= 6;
        vec[i + 0] += 2;
        vec[i + 1] += 2;
        vec[i + 2] += 2;
    }
    inline void shun_drop_fix(const card_index_t i, const uint8_t j) {
        cksize -= 3;
        vec[i + 0] += 1;
        vec[i + j] += 1;
        gui += 1;
    }
    inline void shun_turn_fix(const card_index_t i, const uint8_t j) {
        card_t* ckend = ckdata + cksize;
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
