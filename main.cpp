#include <iostream>
#include <sstream>
#include <climits>
#include "card.h"

using namespace std;


int main (int argc, char *argv[])
{
    card_vector_t vec = {0};
    if (argc > 1) {
        istringstream iss(argv[1]);
        for (string card; getline(iss, card, ',');) {
            auto i = ctoi(stoi(card));
            if (~i) ++vec[i];
        }
    } else {
        cerr << "(1/3) cards: ";
        string line;
        cin >> line;
        istringstream iss(line);
        for (string card; getline(iss, card, ',');) {
            auto i = ctoi(stoi(card));
            if (~i) ++vec[i];
        }
        auto i = vec.begin();
        cerr << "[" << (int)*i;
        for (++i; i != vec.end(); ++i) {
            cerr << ',' << (int)*i;
        }
        cerr << ']' << endl;
    }

    int num;
    if (argc > 2) {
        num = stoi(argv[2]);
    } else {
        cerr << "(2/3) guiCount: ";
        cin >> num;
    }
    card_count_t gui = num;

    if (argc > 3) {
        num = stoi(argv[3]);
    } else {
        cerr << "(3/3) jiang: ";
        cin >> num;
    }
    card_t jiang = num;

    JHSearch::callback_t callback = [](const JHSearch& hu)->bool{
        hu.ckpipe(cout);
        cout << endl;
        return false;
    };
    // cout << "sizeof(JHSearch::callback_t) = " << sizeof(JHSearch::callback_t) << endl;
    JHSearch::Alloc<64> _hu(vec, gui, jiang);
    JHSearch& hu = _hu;
    hu.search(&callback);
    return 0;
}

