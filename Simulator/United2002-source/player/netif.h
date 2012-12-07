/* -*- Mode: C++ -*- */

/*
 * United2002 (soccer client for Robocup2002)
 * Peter Stone <pstone@cs.utexas.edu>
 * Department of Computer Sciences
 * The University of Texas at Austin
 * Copyright (C) 2002 Peter Stone
 *
 * United-2002 was created by Peter Stone.
 *
 * You may copy and distribute this program freely as long as you
 * retain this notice.  If you make any changes or have any comments
 * we would appreciate a message.  For more information, please see
 * http://www.cs.utexas.edu/~pstone/RoboCup/United2002-sim.html

 * United-2002 was based closely on CMUnited-99:

 * CMUnited99 (soccer client for Robocup99)
 * Peter Stone <pstone@cs.cmu.edu>
 * Computer Science Department
 * Carnegie Mellon University
 * Copyright (C) 1999 Peter Stone
 *
 * CMUnited-99 was created by Peter Stone, Patrick Riley, and Manuela Veloso
 *
 * You may copy and distribute this program freely as long as you retain this notice.
 * If you make any changes or have any comments we would appreciate a message.
 * For more information, please see http://www.cs.cmu.edu/~robosoccer/
 */


/*
 * Definitions for UDP c/s program.
 */
#ifndef __netif_h
#define __netif_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#define DEFAULT_PORT_NUMBER	6000
#define MAXMESG			4096
#define MAXARG			200

#ifdef MEMSET
#define bzero(a, b) memset(a, 0, b)
#endif

#if 0
#ifndef Linux
extern "C" void usleep(unsigned long);
#endif
#endif

#ifdef __cplusplus
class Socket {
 public:
  int			socketfd  ;
  struct sockaddr_in	serv_addr ;
};
#else
typedef struct socket{
    int			socketfd  ;
    struct sockaddr_in	serv_addr ;
}Socket ;
#endif

extern Socket init_connection(char* host, int port) ;
extern int    send_message(char* buf, Socket *sock) ;
extern int    receive_message(char *buf, Socket *sock) ;
extern void   close_connection(Socket *sock) ;

int wait_message(char *buf, Socket *sock);

#endif
