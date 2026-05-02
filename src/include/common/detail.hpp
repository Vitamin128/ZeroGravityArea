#pragma once
#include<ctime>
#include<jsoncpp/json/json.h>
#include<random>
#include<iomanip>
#include<atomic>
namespace gchrpc{
    #define LDBG 0
    #define LINF 1
    #define LERR 2

    #define LDEFAULT LDBG

    //!定义了一个日志,不想看了,以后再来吧
    #define LOG(level,format,...){\
        if(level>=LDEFAULT){\
            time_t t=time(NULL);\
            struct tm* lt=localtime(&t);\
            char time_tmp[32]={0};\
            strftime(time_tmp,31,"%m-%d %T",lt);\
            fprintf(stdout,"[%s][%s:%d]" format "\n",time_tmp,__FILE__,__LINE__,##__VA_ARGS__);\
        }\
    }
    #define DLOG(format,...) LOG(LDBG,format,##__VA_ARGS__);
    #define ILOG(format,...) LOG(LINF,format,##__VA_ARGS__);
    #define ELOG(format,...) LOG(LERR,format,##__VA_ARGS__);
    

    //JSON类,定义了序列化和反序列化方法
    class JSON{
        public:

        //Json::Value----->String
        static bool serialize(const Json::Value& val,std::string& body)
        {
            
            std::stringstream ss;
            Json::StreamWriterBuilder swb;
            swb["emitUTF8"]=true;
            std::unique_ptr<Json::StreamWriter>sw(swb.newStreamWriter());
            int ret=sw->write(val,&ss);
            if(ret!=0)
            {
                ELOG("JSON UNSERIALIZE FAILED");
                return false;
            }
            body=ss.str();
            return true;
        }

        //String----->Json::value
        static bool unserialize(const std::string &body,Json::Value&val)
        {
            Json::CharReaderBuilder crb;
            std::string errs;
            std::unique_ptr<Json::CharReader>cr(crb.newCharReader());
            bool ret=cr->parse(body.c_str(),body.c_str()+body.size(),&val,&errs);
            if(ret==false)
            {
                ELOG("JSON UNSERIALIZE FAILED:%s",errs.c_str());
                return false;
            }
            return true;
        }

    };

    //!生成一个密钥比如:ad2af789-d53c-154f-0000-000000000001,也不想看,看不懂
    class UUID{
        public:
        static std::string uuid()
        {
            std::stringstream ss;
            
            std::random_device rd;

            std::mt19937 generator(rd());

            std::uniform_int_distribution<int>distribution(0,255);
            for(int i=0;i<8;i++)
            {
                if(i==4||i==6)
                {
                    ss<<"-";
                }
                ss<<std::setw(2)<<std::setfill('0')<<std::hex<<distribution(generator);
            }
            ss<<"-";
            static std::atomic<size_t>seq(1);
            size_t cur=seq.fetch_add(1);
            for(int i=7;i>=0;i--)
            {
                if(i==5)
                {
                    ss<<"-";
                }
                ss<<std::setw(2)<<std::setfill('0')<<std::hex<<((cur>>(i*8))&0xFF);
            }
            return ss.str();
        }
    };
}