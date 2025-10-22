#include <iostream>
#include <vector>
#include "util.hpp"
using namespace std;

int main() {


    string s = "我来到北京清华大学";
    vector<string> words;
    ns_util::JiebaUtil::Cut(s, &words);

    for(const auto& word : words){
        cout << word << "|";
    }
    cout << endl;

    return 0;
}