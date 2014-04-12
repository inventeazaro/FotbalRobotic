#include <iostream>
#include <syslog.h>
#include <syslogpp.h>


bool InitLog()
{
    syslog.open( NULL, LOG_PID, LOG_USER );
    syslog( LOG_DEBUG );
    return true;
}
bool RedirectLog(std::ostream&, bool sync)
{
    if(!sync)std::cerr << std::nounitbuf;
    std::cerr.rdbuf( syslog.rdbuf() );
    return true;
}
bool EndLog()
{
    syslog.close();
    return true;
}

