#include"cpp-httplib/httplib.h"
#include"searcher.hpp"

const std::string _path = "./data/raw_html/raw.txt";
const std::string root_path = "./wwwroot/dist";
int main()
{
    using namespace httplib;
    using namespace boost_searcher;
    Searcher searcher;
    Server svr;
    searcher.InitSearcher(_path);
    svr.set_base_dir(root_path.c_str());
    svr.Get("/hi", [&searcher](const Request& req, Response& res) {
	res.set_header("Access-Control-Allow-Origin", "*");
    	res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    	res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if(!req.has_param("query")){
            res.set_content("参数错误", "text/plain; charset=utf-8");
            return;
        }
        std::string query = req.get_param_value("query");
        std::string json_result;
        searcher.Search(query, &json_result);
        res.set_content(json_result, "application/json;charset=utf-8");
    });

    svr.listen("0.0.0.0", 8080);
    return 0;
}
