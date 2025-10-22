#include<iostream>
#include<string>
#include<vector>
#include<boost/filesystem.hpp>
#include<fstream>
#include"util.hpp"
using namespace std;
const string src_path = "data/input/";
const string output = "data/raw_html/raw.txt";

typedef struct DocInfo{
    string title;
    string content;
    string url;
} DocInfo_t;

bool EnumFile(const string& src_path, vector<string>* files_list)
{
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);
    if(!fs::exists(root_path)){
        cout <<root_path<< " path not exist!" << endl;///1215152121
        return false;
    }
    fs::recursive_directory_iterator end;
    for(fs::recursive_directory_iterator iter(root_path); iter != end; iter++){
        if(!fs::is_regular_file(*iter)){
            continue;
        }
        if(iter->path().extension() != ".html"){
            continue;
        }
        // cout<<"debug: "<<iter->path().string()<<endl;
        files_list->push_back(iter->path().string());
    }
    return true;
}
static bool ParseTitle(const string& html, string* title)
{
    int begin = html.find("<title>");
    if(begin == string::npos){
        return false;
    }
    begin += strlen("<title>");
    int end = html.find("</title>");
    if(end == string::npos){
        return false;
    }
    if(end < begin){
        return false;
    }
    *title = html.substr(begin, end - begin);
    // cout<<"debug: title= "<<*title<<endl;
    return true;
}
static bool ParseContent(const string& html, string* content)
{
    enum status{
        LABLE,
        CONTENT
    };
    enum status state = LABLE;
    for(char c : html){
        switch(state){
            case LABLE:
                if(c == '>'){
                    state = CONTENT;
                }
                break;
            case CONTENT:
                if(c == '<'){
                    state = LABLE;
                }else{
                    if(c =='\n')
                    {
                        content->push_back(' ');
                    }
                    content->push_back(c);
                }
                break;
            default:
                break;
        }
    }
    return true;
}
static bool ParseUrl(const string& html, string* url)
{
    string url_head="https://www.boost.org/doc/libs/1_78_0/doc/html/";
    string url_tail=html.substr(src_path.size());
    *url=url_head+url_tail;
    return true;
}
void ShowDoc(const DocInfo_t& doc)
{
    cout << "title: " << doc.title << endl;
    cout << "url: " << doc.url << endl;
    cout << "content: " << doc.content << endl;
}
bool ParseHtml(const vector<string>& files_list, vector<DocInfo_t>* results)
{
    for(const string& file : files_list){
        string result;

        //讲file对应目录文件下的内容写入到string result字符串中
        if(!ns_util::FileUtil::ReadFile(file, &result)){
            cout << "read file error! file: " << file << endl;
            continue;
        }
        // cout<<"debug result: "<<result<<endl;
        DocInfo_t doc;
        if(!ParseTitle(result, &doc.title)){
            cout << "parse title error! file: " << file << endl;
            continue;
        }
        if(!ParseContent(result, &doc.content)){
            cout << "parse content error! file: " << file << endl;
            continue;
        }
        if(!ParseUrl(file, &doc.url)){
            cout << "parse url error! file: " << file << endl;
            continue;
        }
        // ShowDoc(doc);
        results->push_back(move(doc)); //move会将doc的内容搬运到results中，避免拷贝，提高效率
        // cout<<result<<endl;
        // break;
    }
    return true;

}
bool SaveHtml(const vector<DocInfo_t>& results, const string& output)
{
    #define SEP '\3'
    ofstream out(output,ios::out | ios::binary);
    if(!out.is_open()){
        cerr<<"open "<<output<<" failed!"<<endl;
        return false;
    }
    for(auto& item : results){
        string out_string;
        out_string += item.title;
        out_string += SEP;
        out_string += item.content;
        out_string += SEP;
        out_string += item.url;
        out_string += '\n';
        out.write(out_string.c_str(), out_string.size());
    }
    out.close();
    return true;
}

int main()
{
    vector<string> files_list;
    if(EnumFile(src_path, &files_list) < 0){
        cout << "enum file name error!" << endl;
        return 1;
    }

    vector<DocInfo_t> results;
    if(!ParseHtml(files_list, &results)){
        cout << "parse html error!" << endl;
        return 2;
    }

    if(!SaveHtml(results,output)){
        cout << "Save html error!" << endl;
        return 3;
    }
}