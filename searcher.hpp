#include"index.hpp"
#include"util.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <cctype>
#include<algorithm>
namespace boost_searcher{

    class Searcher{
        private:
            ns_index::Index* index;
        public:
            Searcher(){}
            ~Searcher(){}

            //加载索引容器,正序和倒序,并且初始化index指针
            void InitSearcher(std::string index_path)
            {
                index = ns_index::Index::GetInstance();
                if(index == nullptr){
                    cout << "GetInstance failed" << endl;
                    return;
                }
                index->BuildIndex(index_path);
            }

            //搜索接口
            void Search(const std::string& query, std::string* json_result)
            {
                std::vector<std::string> words;
                ns_util::JiebaUtil::Cut(query, &words);
                ns_index::InvertedList inverted_list_all;

                std::vector<std::string> wordSame;

                //便利查询的每个词语，获取对应的倒排结果
                for(auto& word : words){
                    if(std::find(wordSame.begin(), wordSame.end(), word)!=wordSame.end())
                    {
                        continue;
                    }
                    boost::to_lower(word);
                    ns_index::InvertedList* inverted_list=index->GetInvertedList(word);

                    if(inverted_list == nullptr){
                        cout<<"inverted_list == nullptr"<<endl;
                        continue;
                    }
                    
                    wordSame.push_back(word);
                    inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end());
                }

                //将所有的倒排结果根据权重进行排序
                std::sort(inverted_list_all.begin(), inverted_list_all.end(),
                          [](const ns_index::InvertedElem &e1, const ns_index::InvertedElem &e2) {
                              return e1.weight > e2.weight;
                });



                //将结果word对应的文档排好转换成json格式
                Json::Value root;
                for(const auto& item : inverted_list_all){
                    ns_index::DocInfo* doc_info = index->GetForwardIndex(item.doc_id);
                    if(doc_info == nullptr){
                        continue;
                    }
                    Json::Value elem;
                    elem["title"] = doc_info->title;
                    elem["url"] = doc_info->url;
                    elem["weight"] = item.weight;
                    elem["content"] = GetDesc(doc_info->content,item.word);
                    root.append(elem);
                }
                Json::StyledWriter writer;
                *json_result = writer.write(root);
            }
            // std::string GetDesc(const std::string &content, const std::string &word)
            // {
            //     int pos = content.find(word);


            // }
            std::string GetDesc(const std::string &content, const std::string &word)
            {
                // size_t pos = content.find(word);
                // cout<<"word: "<<word<<endl;
                auto iter = std::search(content.begin(), content.end(), 
                                        word.begin(), word.end(),
                                        [](char ch1, char ch2){
                                            return std::tolower(ch1) == std::tolower(ch2);
                                        });
                size_t pos = (iter != content.end()) ? std::distance(content.begin(), iter) : std::string::npos;

                if(pos == std::string::npos){
                    if(content.size() <= 160){
                        return content;
                    }else{
                        return content.substr(0, 160) + "...";
                    }
                }else{
                    size_t begin = (pos >= 80) ? (pos - 80) : 0;
                    size_t end = ((content.size() - pos) >= 80) ? (pos + 80) : content.size();
                    std::string desc = content.substr(begin, end - begin);
                    if(begin != 0){
                        desc = "..." + desc;
                    }
                    if(end != content.size()){
                        desc = desc + "...";
                    }
                    return desc;
                }
            }
    };

}