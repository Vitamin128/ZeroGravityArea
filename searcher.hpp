#include"include.hpp"
#include"util.hpp"

namespace boost_searcher{

    class Searcher{
        private:
            ns_index::Index* index;
        public:
            Searcher(){}
            ~Searcher(){}
            void InitSearcher(std::string index_path)
            {
                index = ns_index::Index::GetInstance();
                index->BuildIndex(index_path);
            }
            void Search(const std::string& query, std::string* json_result)
            {
                std::vector<std::string> words;
                ns_util::JsonUtil::Cut(query, &words);
                ns_index::InvertedList inverted_list_all;
                std::vector<std::string> wordSame;
                for(const auto& word : words){
                    if(wordSame.find(word)!=wordSame.end())
                    {
                        continue;
                    }
                    boost::to_lower(word);
                    ns_index::InvertedList* inverted_list=index->GetInvertedList(word);
                    if(inverted_list == nullptr){
                        continue;
                    }
                    
                    wordSame.push_back(word);
                    inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end());
                }
                std::sort(inverted_list_all.begin(), inverted_list_all.end(),
                          [](const ns_index::InvertedElem &e1, const ns_index::InvertedElem &e2) {
                              return e1.weight > e2.weight;
                });
            }
    };

}