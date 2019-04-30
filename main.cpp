#include <iostream>
#include <sstream>
#include "card.h"

using namespace std;

#define MAXCARD 256

int main (int argc, char *argv[])
{
    JHSearch::Alloc<MAXCARD> _searchData;
    JHSearch& jh = _searchData;
    card_size_t guiCount;
    if (argc > 1) {
        istringstream iss(argv[1]);
        for (string card; getline(iss, card, ',');) {
            auto i = ctoi(stoi(card));
            if (~i) ++jh.vec[i];
        }
        if (argc > 2) {
            guiCount = stoi(argv[2]);
            jh.gui = guiCount;
            if (argc > 3) {
                int jiang = stoi(argv[3]);
                jh.jiang1 = jiang;
                jh.jiang2 = jiang;
            }
        }
    }
    JHSearch::callback_t callback = [&](const JHSearch& hu)->bool{
        cout << (int)(guiCount - hu.gui) << '=';
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
            for (string card; getline(iss, card, ',');) {
                auto i = ctoi(stoi(card));
                if (~i) ++jh.vec[i];
            }
            jh.vecpipe(cout);
            cout << endl;
        }
        if (argc < 3) {
            cerr << "(2/3) guiCount: ";
            string line;
            cin >> line;
            if (line.length() == 0) break;

            guiCount = stoi(line);
            jh.gui = guiCount;
        }
        if (argc < 4) {
            cerr << "(3/3) jiang: ";
            string line;
            cin >> line;
            if (line.length() == 0) break;
            int jiang = stoi(line);
            jh.jiang1 = static_cast<card_t>(jiang);
            jh.jiang2 = static_cast<card_t>(jiang);
        }
        jh.search(&callback);
        if (argc > 3) {
            break;
        }
        if (argc < 2) {
            jh.vec.fill(0);
        }
    }
    return 0;
}

