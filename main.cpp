#include <iostream>
#include <sstream>
#include <climits>
#include "card.h"

using namespace std;

void func(card_vector_t& ck)
{
    ck[0] = 21;
}

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

    auto callback = [](const JHSearch& hu)->bool{
        bool d = false;
        for (auto i = hu.begin(); *i; ++i) {
            if (d) cout << ','; else d = true;
            cout << '[' << (int)*i;
            for (++i; *i; ++i) cout << ',' << (int)*i;
            cout << ']';
        }
        cout << endl;
        return false;
    };

    searchJH(vec, gui, jiang, callback);
    return 0;
}

