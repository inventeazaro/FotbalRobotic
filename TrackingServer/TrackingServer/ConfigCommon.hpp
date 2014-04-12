
#ifndef CONFIG_COMMON_HPP
#define CONFIG_COMMON_HPP
#include <string>
#include <boost/variant.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <boost/any.hpp>

struct path_list
{
  std::vector<std::string> paths;
};

struct endpoint
{
  std::string    address;
  unsigned short port;
};

struct endpoint_list
{
  std::vector<endpoint> endpoints;
};

enum proto_type {
    FILE_PROTO=1,
    HTTP_PROTO=2,
    HTTPS_PROTO=3,
    INVALID_PROTO=0
};

struct url_string
{
    proto_type ptype;
    std::string url_path;
};


template<class T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);

std::ostream & operator<<(std::ostream&os,const endpoint& e);
std::ostream & operator<<(std::ostream&os,const endpoint_list& e);

std::ostream & operator<<(std::ostream&os,const path_list& e);
std::ostream & operator<<(std::ostream&os,const url_string& e);

//might be superfluous
void validate(boost::any& v,   const std::vector<std::string> & value,  std::vector<std::string>* target_type, int);

void validate(boost::any &v, const std::vector<std::string>& values, url_string *target_type, int);
void validate(boost::any &v, const std::vector<std::string>& values, endpoint* target_type, int);
void validate(boost::any &v, const std::vector<std::string>& values, path_list* target_type, int);
void validate(boost::any &v, const std::vector<std::string>& values, endpoint_list* target_type, int);

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    typename std::vector<T>::const_iterator i=v.begin();
    os<<"( ";
    for(;i!=v.end();++i)
    {
        os<<*i<<(i+1==v.end()?" )":", ");
    }
    return os;
}

class ConfValueType{
    boost::variant<bool,double,endpoint,endpoint_list,int,path_list,std::string,url_string> m_data;
    ConfValueType();
public:
    ConfValueType(bool v_bool):m_data(v_bool){};
    bool get_bool(){ return boost::get<bool>(m_data);};
    ConfValueType(double v_double):m_data(v_double){};
    double get_double(){ return boost::get<double>(m_data);};
    ConfValueType(endpoint v_endpoint):m_data(v_endpoint){};
    endpoint get_endpoint(){ return boost::get<endpoint>(m_data);};
    ConfValueType(endpoint_list v_endpoint_list):m_data(v_endpoint_list){};
    endpoint_list get_endpoint_list(){ return boost::get<endpoint_list>(m_data);};
    ConfValueType(int v_int):m_data(v_int){};
    int get_int(){ return boost::get<int>(m_data);};
    ConfValueType(path_list v_path_list):m_data(v_path_list){};
    path_list get_path_list(){ return boost::get<path_list>(m_data);};
    ConfValueType(std::string v_string):m_data(v_string){};
    std::string get_string(){ return boost::get<std::string>(m_data);};
    ConfValueType(url_string v_url_string):m_data(v_url_string){};
    url_string get_url_string(){ return boost::get<url_string>(m_data);};
    friend std::ostream & operator<<(std::ostream & os,const ConfValueType &T);
};
typedef std::map<std::string,ConfValueType> ConfValueTypeMap;

struct ConfigOptionsParser
{
    virtual bool doParse(std::string const& ConfigName,int argc, char *argv[], ConfValueTypeMap *pVM)=0;
};
#endif //double include
