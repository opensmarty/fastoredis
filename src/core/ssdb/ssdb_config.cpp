#include "core/ssdb/ssdb_config.h"

#include "common/utils.h"

#include "core/logger.h"

namespace fastoredis
{
    namespace
    {
        int parseOptions(int argc, char **argv, ssdbConfig& cfg) {
            int i;

            for (i = 0; i < argc; i++) {
                int lastarg = i==argc-1;

                if (!strcmp(argv[i],"-h") && !lastarg) {
                    free(cfg.hostip);
                    cfg.hostip = strdup(argv[++i]);
                }
                else if (!strcmp(argv[i],"-p") && !lastarg) {
                    cfg.hostport = atoi(argv[++i]);
                }
                else if (!strcmp(argv[i],"-u") && !lastarg) {
                    cfg.user_ = strdup(argv[++i]);
                }
                else if (!strcmp(argv[i],"-a") && !lastarg) {
                    cfg.password_ = strdup(argv[++i]);
                }
                else if (!strcmp(argv[i],"-d") && !lastarg) {
                    free(cfg.mb_delim);
                    cfg.mb_delim = strdup(argv[++i]);
                }
                else {
                    if (argv[i][0] == '-') {
                        const uint16_t size_buff = 256;
                        char buff[size_buff] = {0};
                        sprintf(buff, "Unrecognized option or bad number of args for: '%s'", argv[i]);
                        LOG_MSG(buff, common::logging::L_WARNING, true);
                        break;
                    } else {
                        /* Likely the command name, stop here. */
                        break;
                    }
                }
            }
            return i;
        }
    }

    ssdbConfig::ssdbConfig()
    {
        init();
    }

    ssdbConfig::ssdbConfig(const ssdbConfig &other)
    {
        init();
        copy(other);
    }

    ssdbConfig& ssdbConfig::operator=(const ssdbConfig &other)
    {
        copy(other);
        return *this;
    }

    void ssdbConfig::copy(const ssdbConfig& other)
    {
        using namespace common::utils;
        freeifnotnull(hostip);
        hostip = strdupornull(other.hostip); //

        hostport = other.hostport;

        freeifnotnull(user_);
        user_ = strdupornull(other.user_); //
        freeifnotnull(password_);
        password_ = strdupornull(other.password_); //

        freeifnotnull(mb_delim);
        mb_delim = strdupornull(other.mb_delim); //
        shutdown = other.shutdown;
    }

    void ssdbConfig::init()
    {
        hostip = strdup("127.0.0.1");
        hostport = 8888;

        user_ = NULL;
        password_ = NULL;

        mb_delim = strdup("\n");
        shutdown = 0;
    }

    ssdbConfig::~ssdbConfig()
    {
        using namespace common::utils;
        freeifnotnull(hostip);
        freeifnotnull(mb_delim);
        freeifnotnull(user_);
        freeifnotnull(password_);
    }
}

namespace common
{
    std::string convertToString(const fastoredis::ssdbConfig &conf)
    {
        std::vector<std::string> argv;

        if(conf.hostip){
            argv.push_back("-h");
            argv.push_back(conf.hostip);
        }

        if(conf.hostport){
            argv.push_back("-p");
            argv.push_back(convertToString(conf.hostport));
        }

        if(conf.user_){
            argv.push_back("-u");
            argv.push_back(conf.user_);
        }

        if(conf.password_){
            argv.push_back("-a");
            argv.push_back(conf.password_);
        }

        if (conf.mb_delim) {
            argv.push_back("-d");
            argv.push_back(conf.mb_delim);
        }

        std::string result;
        for(int i = 0; i < argv.size(); ++i){
            result+= argv[i];
            if(i != argv.size()-1){
                result+=" ";
            }
        }

        return result;
    }

    template<>
    fastoredis::ssdbConfig convertFromString(const std::string& line)
    {
        fastoredis::ssdbConfig cfg;
        enum { kMaxArgs = 64 };
        int argc = 0;
        char *argv[kMaxArgs] = {0};

        char* p2 = strtok((char*)line.c_str(), " ");
        while(p2){
            argv[argc++] = p2;
            p2 = strtok(0, " ");
        }

        fastoredis::parseOptions(argc, argv, cfg);
        return cfg;
    }
}
