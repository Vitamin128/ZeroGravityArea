#include "searcher.hpp"
#include <iostream>
#include <string>

const std::string index_path = "data/raw_html/raw_index.txt";
int main()
{
    boost_searcher::Searcher searcher;
    searcher.InitSearcher(index_path);
    std::string query;
    std::cout << "Please enter your search query: ";
    while (std::getline(std::cin, query)) {
        if (query.empty()) {
            std::cout << "Please enter a non-empty search query: ";
            continue;
        }
        std::string json_result;
        searcher.Search(query, &json_result);
        std::cout << "Search results in JSON format:\n" << json_result << std::endl;
        std::cout << "Please enter your search query: ";
    }
    return 0;
}