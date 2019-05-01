#include <iostream>
#include <sstream>
#include "card.h"

using namespace std;

#define MAXCARD 256

//a=[];for(c=1;c<5;++c)for(i=1;i<10;++i)for(n=1;n<5;++n)a.push(c*10+i);a.toString()

//11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18,19,19,19,19,21,21,21,21,22,22,22,22,23,23,23,23,24,24,24,24,25,25,25,25,26,26,26,26,27,27,27,27,28,28,28,28,29,29,29,29,31,31,31,31,32,32,32,32,33,33,33,33,34,34,34,34,35,35,35,35,36,36,36,36,37,37,37,37,38,38,38,38,39,39,39,39,41,41,41,41,42,42,42,42,43,43,43,43,44,44,44,44,45,45,45,45,46,46,46,46,47,47,47,47,48,48,48,48,49,49,49,49


int main (int argc, char *argv[])
{
    JHSearch::Fixed<MAXCARD> _searchData;
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

