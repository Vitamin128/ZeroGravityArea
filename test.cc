#include <iostream>
#include <vector>
#include "util.hpp"
#include <jsoncpp/json/json.h>
using namespace std;

int main() {
    Json::Value root;
    Json::Value item1;
    item1["name"] = "Boost C++ Libraries";
    item1["url"] = "https://www.boost.org/";
    item1["description"] = "A collection of peer-reviewed portable C++ source libraries.";

    Json::Value item2;
    item2["name"] = "Boost Test Library";
    item2["url"] = "https://www.boost.org/doc/libs/release/libs";
    item2["description"] = "A unit testing framework for C++.";

    root.append(item1);
    root.append(item2);

    Json::FastWriter writer;
    std::string output = writer.write(root);

    cout << output << endl;
    return 0;
}