#ifndef APPLICATIONCONFIG_HPP
#define APPLICATIONCONFIG_HPP

#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "Logging.hpp"
#include "ConfigCommon.hpp"


class ApplicationConfig
{
    boost::mutex m_ConfMutex;
    boost::shared_ptr<ConfValueTypeMap> m_PtrMap;
    std::string m_ConfigFileName;
    int m_argc;
    char **m_argv;
    boost::shared_ptr<ConfigOptionsParser> m_ConfParser;
    ApplicationConfig();
public:

    ApplicationConfig(ConfigOptionsParser *ConfParser, std::string const &NewConfigFile,int argc,char *argv[]);
    bool ReloadConfig(std::string const &NewConfigFile);
    bool ReloadConfig();
    ConfValueType operator [](std::string const & key);
    ConfValueType operator [](const char *key);
    size_t size(){
        boost::mutex::scoped_lock lock(m_ConfMutex);
        return m_PtrMap->size();
    }
    boost::mutex & GetLock(){ return m_ConfMutex;};
    typedef ConfValueTypeMap::iterator iterator;
    typedef ConfValueTypeMap::const_iterator const_iterator;
    iterator begin() {
        return m_PtrMap->begin();
    }
    iterator end()   {
        return m_PtrMap->end();
    }
    virtual ~ApplicationConfig();
};



#endif // APPLICATIONCONFIG_HPP
