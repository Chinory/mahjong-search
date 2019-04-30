#include <iostream>
#include <sstream>
#include <climits>
#include "card.h"

using namespace std;

#define MAXCARD 256

int main (int argc, char *argv[])
{
    JHSearch::Alloc<MAXCARD> _searchData;
    JHSearch& jh = _searchData;


    if (argc > 1) {
        istringstream iss(argv[1]);
        for (string card; getline(iss, card, ',');) {
            auto i = ctoi(stoi(card));
            if (~i) ++jh.vec[i];
        }
    } else {
        cerr << "(1/3) cards: ";
        string line;
        cin >> line;
        istringstream iss(line);
        for (string card; getline(iss, card, ',');) {
            auto i = ctoi(stoi(card));
            if (~i) ++jh.vec[i];
        }
        jh.vecpipe(cout);
        cout << endl;
    }

    if (argc > 2) {
        jh.gui = stoi(argv[2]);
    } else {
        cerr << "(2/3) guiCount: ";
        cin >> jh.gui;
    }

    int jiang;
    if (argc > 3) {
        jiang = stoi(argv[3]);
    } else {
        cerr << "(3/3) jiang: ";
        cin >> jiang;
    }
    jh.jiang1 = (card_t)jiang;
    jh.jiang2 = (card_t)jiang;

    JHSearch::callback_t callback = [](const JHSearch& hu)->bool{
        hu.ckpipe(cout);
        cout << endl;
        return false;
    };

    ((JHSearch&)jh).search(&callback);
    return 0;
}

