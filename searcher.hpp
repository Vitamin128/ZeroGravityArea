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

                for(const auto& word : words){
                    boost::to_lower(word);
                    ns_index::InvertedList* inverted_list=index->GetInvertedList(word);
                    if(inverted_list == nullptr){
                        continue;
                    }
                }
            }
    };

}