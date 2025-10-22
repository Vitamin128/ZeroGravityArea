#include"index.hpp"
#include"util.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>
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
                if(index == nullptr){
                    cout << "GetInstance failed" << endl;
                    return;
                }
                index->BuildIndex(index_path);
            }
            void Search(const std::string& query, std::string* json_result)
            {
                std::vector<std::string> words;
                ns_util::JiebaUtil::Cut(query, &words);
                ns_index::InvertedList inverted_list_all;

                std::vector<std::string> wordSame;
                for(auto& word : words){
                    if(std::find(wordSame.begin(), wordSame.end(), word)!=wordSame.end())
                    {
                        continue;
                    }
                    boost::to_lower(word);
                    // std::cout<<"debug search word: "<<word<<std::endl;

                    //获取对应的倒排数组
                    ns_index::InvertedList* inverted_list=index->GetInvertedList(word);
                    // cout<<"inverted_list.size:"<<inverted_list->size()<<endl;

                    if(inverted_list == nullptr){
                        cout<<"inverted_list == nullptr"<<endl;
                        continue;
                    }
                    
                    wordSame.push_back(word);
                    inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end());
                }
                std::sort(inverted_list_all.begin(), inverted_list_all.end(),
                          [](const ns_index::InvertedElem &e1, const ns_index::InvertedElem &e2) {
                              return e1.weight > e2.weight;
                });

                // for(auto item:inverted_list_all)
                // {
                //     cout<<"debug search result doc_id: "<<item.doc_id<<" weight: "<<item.weight<<endl;
                // }

                Json::Value root;
                for(const auto& item : inverted_list_all){
                    ns_index::DocInfo* doc_info = index->GetForwardIndex(item.doc_id);
                    if(doc_info == nullptr){
                        continue;
                    }
                    Json::Value elem;
                    elem["title"] = doc_info->title;
                    elem["url"] = doc_info->url;
                    elem["content"] = doc_info->content;
                    root.append(elem);
                }
                Json::StyledWriter writer;
                *json_result = writer.write(root);
            }
    };

}