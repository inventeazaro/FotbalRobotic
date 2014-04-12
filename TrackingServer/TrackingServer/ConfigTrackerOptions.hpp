#ifndef CONFIGTRACKEROPTIONSPARSER_HPP
#define CONFIGTRACKEROPTIONSPARSER_HPP
#include "ConfigCommon.hpp"
#include <boost/program_options.hpp>
struct ConfigTrackerOptionsParser:public ConfigOptionsParser
{
    bool doParse(std::string const& ConfigName,int argc, char *argv[], ConfValueTypeMap * pVM);
};
#endif //double include

