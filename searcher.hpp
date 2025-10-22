#include"include.hpp"

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

            }
    };

}