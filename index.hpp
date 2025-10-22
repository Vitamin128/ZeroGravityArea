#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>
#include<fstream>
#include<mutex>
#include"util.hpp"

using namespace std;

namespace ns_index{

    //正排索引结构体
    struct DocInfo{
        uint64_t doc_id;
        std::string title;
        std::string url;
        std::string content;
    };

    //倒排索引结构体
    struct InvertedElem{
        uint64_t doc_id;
        string word;
        int weight;
    };

    typedef vector<InvertedElem> InvertedList;

    class Index{
        private:
            vector<DocInfo> forward_index;
            unordered_map<string, InvertedList> inverted_index;
            Index(){}
            Index(const Index&) = delete;
            Index& operator=(const Index&) = delete;

            static Index* instance;
            static std::mutex mtx;
            ~Index(){}
        public:
            static Index* GetInstance()
            {
                mtx.lock();
                if(instance == nullptr){
                    instance = new Index();
                }
                mtx.unlock();
                return instance;
            }
            //正排查询
            DocInfo *GetForwardIndex(uint64_t doc_id)
            {
                if(doc_id >= forward_index.size()){
                    cerr << "doc_id out range,error" << endl;
                    return nullptr;
                }
                return &forward_index[doc_id];
            }
            //倒排查询1
            InvertedList* GetInvertedList(const string &word)
            {
                auto it = inverted_index.find(word);
                if (it != inverted_index.end()) {
                    return &it->second;
                }
                cout<<"word not found in inverted index: " << word << endl;
                return nullptr;
            }
            bool BuildIndex(const string &input)
            {
                fstream in(input, ios::in | ios::binary);
                if(!in.is_open()){
                    cerr << "open file error" << endl;
                    return false;
                }
                string file;
                while(getline(in, file)){
                    DocInfo* doc = BuildForwardIndex(file);
                    if(doc == nullptr){
                        cerr << "BuildForwardIndex error" << endl;
                        continue;
                    }
                    BuildInvertedIndex(*doc);
                }
                return true;
            }
        private:
            DocInfo *BuildForwardIndex(const string &line)
            {
                vector<string> results;
                string sep='\3';
                ns_util::StringUtil::Split(line, sep, &results);
                if(results.size() != 3){
                    cerr << "line format error" << endl;
                    return nullptr;
                }
                DocInfo doc;
                doc.doc_id = forward_index.size();
                doc.title = results[0];
                doc.content = results[1];
                doc.url = results[2];
                forward_index.push_back(move(doc));
                return &forward_index.back();

            }
            bool BuildInvertedIndex(const DocInfo &doc)
            {
                struct word_cnt{
                    int title_cnt;
                    int content_cnt;
                    word_cnt():title_cnt(0), content_cnt(0){}
                };
                std::unordered_map<string, word_cnt> word_map;

                std::vector<string> title_words;
                ns_util::StringUtil::CutWord(doc.title, &title_words);
                for(const auto &word : title_words){
                    word_map[word].title_cnt++;
                }

                std::vector<string> content_words;
                ns_util::StringUtil::CutWord(doc.content, &content_words);
                for(const auto &word : content_words){
                    word_map[word].content_cnt++;
                }
                for(const auto &word_pair : word_map){
                    InvertedElem elem;
                    elem.doc_id = doc.doc_id;
                    elem.word = word_pair.first;
                    elem.weight = word_pair.second.title_cnt * 10 + word_pair.second.content_cnt;
                    inverted_index[elem.word].push_back(move(elem));
                }
            }
            
    };
    static Index::instance = nullptr;
}