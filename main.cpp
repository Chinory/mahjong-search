#include <iostream>
#include <sstream>
#include "card.h"
#include <chrono>

using namespace std;

#define MAXCARD 256
#define BENCH_COUNT 0xFFF
int benchmain ()
{
    auto start = chrono::system_clock::now();
    static card_t cards[] = {0x11,0x12,0x13,0x14,0x15,0x12,0x13,0x14,0x15,0x16,0x11,0x12,0x13,0x14,0x15,0x12,0x13,0x14,0x15,0x16,0x11,0x12,0x13,0x14,0x15,0x12,0x13,0x14,0x15,0x16};
    JHSearch::callback_t callback = [](const JHSearch&)->bool{
        return true;
    };
    for (size_t count = BENCH_COUNT; count; --count) {
        JHSearch jh(100);
        card_size_t cardCount = 0;
        for (auto i = cards, end = cards + sizeof(cards); i != end; ++i) {
            card_index_t idx = ctoi(*i);
            if (~idx) { ++jh.vec[idx]; ++cardCount; }
        }
        card_t ckdata[jh.cksize_for_this(cardCount)];
        jh.search(&callback, ckdata);
    }
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    auto ops = double(BENCH_COUNT) / double(duration.count()) / chrono::microseconds::period::num * chrono::microseconds::period::den;
    cout.imbue(std::locale("en_US.UTF8"));
    cout << size_t(ops) << endl;
    getchar();
    return 0;
}

int main (int argc, char *argv[])
{
    JHSearch jh;
    card_size_t guiCount = 0;
    card_size_t cardCount = 0;
    if (argc > 1) {
        cardCount = 0;
        istringstream iss(argv[1]);
        for (string card; getline(iss, card, ',');) {
            card_index_t i = ctoi(static_cast<card_t>(stoi(card)));
            if (~i) { ++jh.vec[i]; ++cardCount; }
        }
        if (argc > 2) {
            jh.gui = static_cast<card_size_t>(stoi(argv[2]));
            guiCount = jh.gui;
            if (argc > 3) {
                card_t jiang = static_cast<card_t>(stoi(argv[3]));
                jh.jiang1 = jiang;
                jh.jiang2 = jiang;
            }
        }
    }
    JHSearch::callback_t callback = [&](const JHSearch& hu)->bool{
        cout << static_cast<int>(guiCount - hu.gui) << '=';
        hu.ckpipe(cout);
        cout << endl;
        return false;
    };
    for (;;) {
        if (argc < 2) {
            cerr << "(1/3) cards: ";
            string line;
            cin >> line;
            if (line.length() == 0) break;
            istringstream iss(line);
            cardCount = 0;
            for (string card; getline(iss, card, ',');) {
                card_index_t i = ctoi(static_cast<card_t>(stoi(card)));
                if (~i) { ++jh.vec[i]; ++cardCount; }
            }
            jh.vecpipe(cout);
            cout << endl;
        }
        if (argc < 3) {
            cerr << "(2/3) guiCount: ";
            string line;
            cin >> line;
            if (line.length() == 0) break;
            jh.gui = static_cast<card_size_t>(stoi(line));
            guiCount = jh.gui;
        }
        if (argc < 4) {
            cerr << "(3/3) jiang: ";
            string line;
            cin >> line;
            if (line.length() == 0) break;
            card_t jiang = static_cast<card_t>(stoi(line));
            jh.jiang1 = jiang;
            jh.jiang2 = jiang;
        }
        card_t ckdata[cardCount + jh.cksize_for_gui()];
        jh.search(&callback, ckdata);
        if (argc > 3) break;
        if (argc < 2) jh.vec.fill(0);
    }
    return 0;
}

