#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"
using namespace std;

namespace ns_util{
    class FileUtil{
    public:
        //读取文件内容到字符串
        static bool ReadFile(const std::string& file_path, std::string* content){
            ifstream file(file_path,ios::in);
            if(!file.is_open()){
                cout<<"open file error! file: "<<file_path<<endl;
                return false;
            }
            string line;
            while(getline(file,line)){
                *content += line;
            }
            file.close();
            return true;
        }
    };

    class StringUtil{
        public:

        //字符串分割函数通过\3分割
        static void Split(const string &s, string sep, vector<string> *elems){
            boost::split(*elems, s, boost::is_any_of(sep), boost::token_compress_on);
        }
    };

    const string dict_path = "./dict/jieba.dict.utf8";
    const string hmm_path = "./dict/hmm_model.utf8";
    const string user_dict_path = "./dict/user.dict.utf8";
    const string idf_path = "./dict/idf.utf8";
    const string stop_words_path = "./dict/stop_words.utf8";

    class JiebaUtil{
        private:
        public:
            static cppjieba::Jieba jieba;

            //jieba分词接口
            static void Cut(const string &sentence, vector<string> *words){
                jieba.Cut(sentence, *words);
            }
    };

    
    cppjieba::Jieba JiebaUtil::jieba(dict_path, hmm_path, user_dict_path, idf_path, stop_words_path);
}