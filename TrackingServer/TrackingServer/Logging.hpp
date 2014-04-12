#ifndef LOGGING_HPP
#define LOGGING_HPP
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h> 
#include <string.h>
#if 0
#define log_debug   "["<<__FUNCTION__<<" "<<__FILE__<<" "<<__LINE__<<"]["<<getpid()<<":"<<syscall(SYS_gettid)<<"]"
#define log_syserr  "["<<__FUNCTION__<<" "<<__FILE__<<" "<<__LINE__<<"]["<<getpid()<<":"<<syscall(SYS_gettid)<<"][errno:"<<errno<<" "<<strerror(errno))<<"]"
#endif

#define log_debug   "["<<__FUNCTION__<<" "<<__LINE__<<"]["<<getpid()<<":"<<syscall(SYS_gettid)<<"]"
#define log_syserr  "["<<__FUNCTION__<<" "<<__LINE__<<"]["<<getpid()<<":"<<syscall(SYS_gettid)<<"][errno:"<<errno<<" "<<strerror(errno)<<"]"

#include <iostream>
#define HAVE_SYSLOG_H
#include <syslogpp.h>

bool InitLog();
bool RedirectLog(std::ostream&,bool sync=false);
bool EndLog();

#endif // LOGGING_HPP
