#include "util.hpp"

int main() {
    std::string src_path = "../data_1/input";
    std::string output_path = "../data_1/raw_html/raw.txt";

    std::vector<std::string> files_list;
    if (!ns_util::ParserUtil::EnumFile(src_path, &files_list)) {
        std::cout << "enum file name error!" << std::endl;
        return 1;
    }

    std::vector<ns_util::ParserUtil::DocInfo> results;
    if (!ns_util::ParserUtil::ParseHtml(files_list, &results, src_path)) {
        std::cout << "parse html error!" << std::endl;
        return 2;
    }

    if (!ns_util::ParserUtil::SaveHtml(results, output_path)) {
        std::cout << "Save html error!" << std::endl;
        return 3;
    }

    std::cout << "parse html success!" << std::endl;
    return 0;
}