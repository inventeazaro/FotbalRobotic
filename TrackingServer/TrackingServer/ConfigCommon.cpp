
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>


#include "ConfigCommon.hpp"


std::ostream & operator<<(std::ostream & os,const endpoint& e)
{
    os<<"["<<e.address<<"]:["<<e.port<<"]";
    return os;
}
std::ostream & operator<<(std::ostream&os,const endpoint_list& e)
{
    os<<e.endpoints;
    return os;
}
std::ostream & operator<<(std::ostream&os,const path_list& e)
{
    os<<e.paths;
    return os;
}

static const char *proto_type_list[]=
{
    "INVALID_PROTO",
    "FILE_PROTO",
    "HTTP_PROTO",
    "HTTPS_PROTO"
};


std::ostream & operator<<(std::ostream& os,const url_string& e)
{
    os<<proto_type_list[e.ptype]<<":"<<e.url_path;
    return os;
}

void validate(boost::any& v,   const std::vector<std::string> & value,  std::vector<std::string>* target_type, int)
{

    boost::program_options::validators::check_first_occurrence(v);
    const std::string& s = boost::program_options::validators::get_single_string(value);
    std::vector< std::string > tokens;
    boost::split( tokens, s, boost::is_any_of( "," ) );
    v=boost::any(tokens) ;
}
void validate(boost::any &v, const std::vector<std::string> &values, url_string *target_type, int)
{
    url_string Url;
    const std::string& input_string = boost::program_options::validators::get_single_string(values);
    std::map<std::string,enum proto_type> proto_map;
    proto_map["http"]=HTTP_PROTO;
    proto_map["https"]=HTTPS_PROTO;
    proto_map["file"]=FILE_PROTO;

    boost::regex exrp( "^(https|http|file)://(.*)$") ;
    boost::match_results<std::string::const_iterator> what;
    if( regex_search( input_string, what, exrp ) ) {
        Url.ptype=proto_map[std::string( what[1].first, what[1].second )];
        Url.url_path=std::string( what[2].first, what[2].second );
    }
    else
    {
        throw boost::program_options::validation_error(         boost::program_options::validation_error::invalid_option_value,
         "url_string",
         input_string);
    }
    v=boost::any(Url);
}
void validate(boost::any& v,
              const std::vector<std::string>& values,
              endpoint* target_type, int)
{
    endpoint E;
    boost::program_options::validators::check_first_occurrence(v);
    const std::string& s = boost::program_options::validators::get_single_string(values);
    std::vector< std::string > tokens;
    boost::split( tokens, s, boost::is_any_of( "/:-" ) );
    if ( 2 != tokens.size() )
        throw boost::program_options::validation_error(
        boost::program_options::validation_error::invalid_option_value,  "endpoint",s );
    E.address=tokens[0];
    try{ E.port=boost::lexical_cast<unsigned short>(tokens[1]);}
    catch(boost::bad_lexical_cast &e){
         throw boost::program_options::validation_error(
         boost::program_options::validation_error::invalid_option_value,"endpoint",s);
    }
    v=boost::any(E);
}

void validate(boost::any& v, const std::vector<std::string>& values, path_list* target_type, int)
{
    path_list p;
    boost::program_options::validators::check_first_occurrence(v);
    const std::string& s = boost::program_options::validators::get_single_string(values);
    std::vector< std::string > tokens;
    boost::split( p.paths, s, boost::is_any_of( "," ) );
    v=boost::any(p);
}

void validate(boost::any& v, const std::vector<std::string>& values, endpoint_list* target_type, int)
{
    std::vector<std::string> list_elements;
    endpoint_list p;

    boost::program_options::validators::check_first_occurrence(v);
    const std::string& s = boost::program_options::validators::get_single_string(values);
    std::vector< std::string > tokens;
    boost::split( list_elements , s, boost::is_any_of( "," ) );
    for(std::vector<std::string>::iterator i=list_elements.begin();i!=list_elements.end();i++)
    {
        endpoint E;
        std::vector< std::string > tokens;
        boost::split( tokens, *i, boost::is_any_of( "/:-" ) );
        if ( 2 != tokens.size() )
            throw boost::program_options::validation_error(
            boost::program_options::validation_error::invalid_option_value,  "endpoint",s );
        E.address=tokens[0];
        try{ E.port=boost::lexical_cast<unsigned short>(tokens[1]);}
        catch(boost::bad_lexical_cast &e){
             throw boost::program_options::validation_error(
             boost::program_options::validation_error::invalid_option_value,"endpoint",s);
        }
        p.endpoints.push_back(E);
    }
    v=boost::any(p);
}




std::ostream & operator<<(std::ostream & os,const ConfValueType &T)
{
    os<<T.m_data;
    return os;
}

