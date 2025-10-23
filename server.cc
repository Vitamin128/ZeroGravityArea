#include"cpp-httplib/httplib.h"

const std::string root_path = "./wwwroot";
int main()
{
    using namespace httplib;
    Server svr;
    svr.set_base_dir(root_path.c_str());
    svr.Get("/hi", [](const Request& req, Response& res) {
        res.set_content("的VS发的不舒服", "text/plain;charset=utf-8");
    }); 
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}
