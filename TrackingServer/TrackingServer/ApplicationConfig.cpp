#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include "ApplicationConfig.hpp"
#include "Logging.hpp"
#include "ConfigCommon.hpp"




ApplicationConfig::~ApplicationConfig()
{
    //both will auto-delete
    //boost::mutex::scoped_lock lock(m_ConfMutex);
    //m_ConfParser.reset();
    //m_PtrMap.reset();
}

ApplicationConfig::ApplicationConfig(ConfigOptionsParser* ConfParser,const std::string &NewConfigFile,int argc,char *argv[]):
    m_ConfigFileName(NewConfigFile),
    m_argc(argc),
    m_argv(argv),
    m_PtrMap(new ConfValueTypeMap())
{
    boost::mutex::scoped_lock lock(m_ConfMutex);
    m_ConfParser.reset(ConfParser);
    m_ConfParser->doParse(NewConfigFile,argc,argv,m_PtrMap.get());

}

bool ApplicationConfig::ReloadConfig()
{
    boost::mutex::scoped_lock lock(m_ConfMutex);

    try{
        boost::shared_ptr<ConfValueTypeMap> PtrVariablesMap(new ConfValueTypeMap());
        m_ConfParser->doParse(m_ConfigFileName,m_argc,m_argv,PtrVariablesMap.get());
        m_PtrMap=PtrVariablesMap;
        return true;
    }
    catch(boost::program_options::validation_error &e)
    {
        syslog(LOG_ERR)<<log_debug<<"Parse Error["<<m_ConfigFileName<<"]:"<<e.what()<<std::endl;
    }
    catch(std::exception &e)
    {
        syslog(LOG_ERR)<<log_debug<<e.what()<<std::endl;
    }
    catch(...)
    {
        syslog(LOG_ERR)<<log_debug<<"Unknown Error["<<m_ConfigFileName<<"]"<<std::endl;
    }
    return false;
}
bool ApplicationConfig::ReloadConfig(const std::string &NewConfigFile)
{
    //boost::unique_lock<boost::mutex> lock(m_ConfMutex);
    boost::mutex::scoped_lock lock(m_ConfMutex);
    boost::shared_ptr<ConfValueTypeMap> PtrVariablesMap(new ConfValueTypeMap());
    try{
        m_ConfParser->doParse(NewConfigFile,m_argc,m_argv,PtrVariablesMap.get());
        m_PtrMap=PtrVariablesMap;
        m_ConfigFileName  = NewConfigFile;
        return true;
    }
    catch(boost::program_options::validation_error &e)
    {
        syslog(LOG_ERR)<<log_debug<<"Parse Error["<<NewConfigFile<<"]:"<<e.what()<<std::endl;
    }
    catch(std::exception &e)
    {
        syslog(LOG_ERR)<<log_syserr<<e.what()<<std::endl;
    }
    catch(...)
    {
        syslog(LOG_ERR)<<log_debug<<"Unknown Error["<<NewConfigFile<<"]"<<std::endl;
    }
    return false;
}

ConfValueType ApplicationConfig::operator [](std::string const  &key)
{
    return (*this)[key.c_str()];
}

ConfValueType ApplicationConfig::operator [](const char *key)
{
    //boost::unique_lock<boost::mutex> lock(m_ConfMutex);
    boost::mutex::scoped_lock lock(m_ConfMutex);

    ConfValueTypeMap::const_iterator i=m_PtrMap->find(key);
    if(i!=m_PtrMap->end())return i->second;
    throw std::runtime_error(std::string("key:")+key+" not found");
}

