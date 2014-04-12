
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <cerrno>

#include "ConfigTrackerOptions.hpp"



bool ConfigTrackerOptionsParser::doParse(std::string const& ConfigName,int argc, char *argv[], ConfValueTypeMap * pVM)
{ 
    int  camera_left;
    int  camera_right;
    int  left_hueLow;
    int  left_satLow;
    int  left_valLow;
    int  left_hueHi;
    int  left_satHi;
    int  left_valHi;
    int  right_hueLow;
    int  right_satLow;
    int  right_valLow;
    int  right_hueHi;
    int  right_satHi;
    int  right_valHi;


    boost::program_options::options_description desc("TRACKER options");
    
    desc.add_options()
        ("help", "produce help message")
        ("debug", "run in foreground")
    
	 ("camera.left",boost::program_options::value< int >(&camera_left) ->required(),"camera left: int")
	 ("camera.right",boost::program_options::value< int >(&camera_right) ->required(),"camera right: int")
	 ("left.hueLow",boost::program_options::value< int >(&left_hueLow) ->required(),"left hueLow: int")
	 ("left.satLow",boost::program_options::value< int >(&left_satLow) ->required(),"left satLow: int")
	 ("left.valLow",boost::program_options::value< int >(&left_valLow) ->required(),"left valLow: int")
	 ("left.hueHi",boost::program_options::value< int >(&left_hueHi) ->required(),"left hueHi: int")
	 ("left.satHi",boost::program_options::value< int >(&left_satHi) ->required(),"left satHi: int")
	 ("left.valHi",boost::program_options::value< int >(&left_valHi) ->required(),"left valHi: int")
	 ("right.hueLow",boost::program_options::value< int >(&right_hueLow) ->required(),"right hueLow: int")
	 ("right.satLow",boost::program_options::value< int >(&right_satLow) ->required(),"right satLow: int")
	 ("right.valLow",boost::program_options::value< int >(&right_valLow) ->required(),"right valLow: int")
	 ("right.hueHi",boost::program_options::value< int >(&right_hueHi) ->required(),"right hueHi: int")
	 ("right.satHi",boost::program_options::value< int >(&right_satHi) ->required(),"right satHi: int")
	 ("right.valHi",boost::program_options::value< int >(&right_valHi) ->required(),"right valHi: int");
    boost::program_options::variables_map vm;
    std::ifstream conf(ConfigName.c_str());
    boost::program_options::store(boost::program_options::parse_command_line(argc,argv,desc),vm);
    if(vm.count("help"))
    {
       std::cout<<desc<<std::endl;
    }
    if(vm.count("debug"))
    {
       std::cout<<"debug mode"<<std::endl;
    }
    if(conf.is_open())
    {
        boost::program_options::store(boost::program_options::parse_config_file(conf,desc),vm);
    }
    else 
    {
        throw std::runtime_error(ConfigName+":"+std::strerror( errno ));
    }

    boost::program_options::store(boost::program_options::parse_command_line(argc,argv,desc),vm);


    boost::program_options::notify(vm);
  
    pVM->insert(std::make_pair("camera.left", ConfValueType(camera_left)));
    pVM->insert(std::make_pair("camera.right", ConfValueType(camera_right)));
    pVM->insert(std::make_pair("left.hueLow", ConfValueType(left_hueLow)));
    pVM->insert(std::make_pair("left.satLow", ConfValueType(left_satLow)));
    pVM->insert(std::make_pair("left.valLow", ConfValueType(left_valLow)));
    pVM->insert(std::make_pair("left.hueHi", ConfValueType(left_hueHi)));
    pVM->insert(std::make_pair("left.satHi", ConfValueType(left_satHi)));
    pVM->insert(std::make_pair("left.valHi", ConfValueType(left_valHi)));
    pVM->insert(std::make_pair("right.hueLow", ConfValueType(right_hueLow)));
    pVM->insert(std::make_pair("right.satLow", ConfValueType(right_satLow)));
    pVM->insert(std::make_pair("right.valLow", ConfValueType(right_valLow)));
    pVM->insert(std::make_pair("right.hueHi", ConfValueType(right_hueHi)));
    pVM->insert(std::make_pair("right.satHi", ConfValueType(right_satHi)));
    pVM->insert(std::make_pair("right.valHi", ConfValueType(right_valHi)));
    if(vm.count("debug"))pVM->insert(std::make_pair("debug",ConfValueType(1)));
;
 return true;
 }
