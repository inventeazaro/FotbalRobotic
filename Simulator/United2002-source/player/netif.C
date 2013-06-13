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
 *  udp client program.
 */

//  escape prototype of recvfrom 
//  for escape conversion of signed-unsigned ints 
#define recvfrom _escape_recvfrom
//

#include <errno.h>
#include "netif.h"
#include "client.h"

// [2000/01/14:I.Noda]
//  escape prototype of recvfrom 
//  for escape conversion of signed-unsigned ints 
#undef recvfrom
//  prototype of recvfrom for escape conversion of signed-unsigned ints 
//
extern "C" {
   int recvfrom(int s, void *buf, int len, unsigned int flags,
		struct sockaddr *from, size_t *fromlen);
} ;
//

int wait_message(char *buf, Socket *sock)
{
  if (receive_message(buf, sock) == 1) {
    while (receive_message(buf, sock) == 1);
    return 1;
  } 
  else for (int i = 0; i < 100; i++)  {
    if (receive_message(buf, sock) == 1) return 1;
    //my_error("sleeping, waiting for message");
    usleep(50000);
  }
  return 0;
}


Socket init_connection(char *host, int port)
{
  struct hostent 	*host_ent ;
  struct in_addr      *addr_ptr ;
  struct sockaddr_in	cli_addr ;
  int			sockfd, val ;
  Socket		sock ;

  sock.socketfd = -1 ;

  if((host_ent = (struct hostent *)gethostbyname(host)) == NULL) {
    /* Check if a numeric address */
    if((int)inet_addr(host) == -1){
      return sock ;
    }
  }
  else{
    addr_ptr = (struct in_addr *) *host_ent->h_addr_list ;
    host = inet_ntoa(*addr_ptr) ;
  }

  sigset_t sigmask;

  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGIO);      
  sigprocmask(SIG_BLOCK, &sigmask, NULL);
  
  /*
   *  Open UDP socket.
   */
  if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    return sock ;  	/* Can't open socket. */
  }
  
  if (fcntl(sockfd, F_SETOWN, getpid()) == -1) return sock;

  val = fcntl(sockfd, F_GETFL, 0) ;

/* was "defined NewsOS || defined IRIX" */
#if 1 
  val |= O_NDELAY ;
#else
  val |= O_NONBLOCK ;
#endif
  val |= FASYNC;
  fcntl(sockfd, F_SETFL, val) ;

  /*
   *  Bind any local address.
   */
  bzero((char *) &cli_addr, sizeof(cli_addr)) ;
  cli_addr.sin_family		= AF_INET ;
  cli_addr.sin_addr.s_addr	= htonl(INADDR_ANY) ;
  cli_addr.sin_port		= htons(0) ;

  if(bind(sockfd, (struct sockaddr *) &cli_addr,
	  sizeof(cli_addr)) < 0){
    return sock ;  /* Can't bind local address */
  }

  /*
   *  Fill in the structure with the address of the server.
   */
  sock.socketfd = sockfd ;

  bzero((char *) &sock.serv_addr, sizeof(sock.serv_addr)) ;
  sock.serv_addr.sin_family		= AF_INET ;
  sock.serv_addr.sin_addr.s_addr	= inet_addr(host) ;
  sock.serv_addr.sin_port		= htons(port) ;

  return sock ;
}

int send_message(char *buf, Socket *sock)
{
  //struct timeval tv_new;
  int n ;

  if (buf[0]==0) return 0;
  n = strlen(buf)+1; // +1 to null terminate everything

  if (Mem->CP_save_log && Mem->Initialized)
    fprintf(Mem->SaveLogFile, ">> %s\n\n",buf); 

  /* not needed after server 5.23
  gettimeofday(&tv_new, NULL); // no time zone info;
  Mem->SetRealTimeOfLastSend(tv_new);
  */
  
  if( sendto(sock->socketfd, buf, n, 0,
	     (struct sockaddr *)&sock->serv_addr, sizeof(sock->serv_addr))
      != n )
    return (-1) ;
  else {
    return 0;
  }
  
}
	   
int receive_message(char *buf, Socket *sock)
{
  int			n;
  size_t                servlen ;
  struct sockaddr_in	serv_addr ;
    
  servlen = sizeof(serv_addr) ;
  n = recvfrom(sock->socketfd, buf, MAXMESG, 0,
	       (struct sockaddr *)&serv_addr, &servlen);

  if(n < 0)
    if( n == -1 && errno == EWOULDBLOCK){
      /* buf[0] = '\0' ; by Pat- don't want to erase previous messages */
      /* my_error("Receive would block: %d", sock->socketfd); */
      return 0 ;
    }
    else {
      LogAction4(5, "Receive error: %d on %d", errno, sock->socketfd);
      //perror("");
      //fflush(stderr);
      return (-1) ;
    }
  else{
    buf[n] = '\0' ;

    if (Mem->CP_save_log && Mem->Initialized){ 
      fprintf(Mem->SaveLogFile, "%s\n", buf);
      if (Mem->SaveLogCounter++ % Mem->CP_save_freq == 0){
	fclose(Mem->SaveLogFile);
	Mem->SaveLogFile = fopen(Mem->SaveLogFileName,"a");
      }
    }
    if (Mem->CP_save_sound_log && Mem->Initialized && buf[1] == 'h'){ /* Hear message */
      fprintf(Mem->SaveSoundLogFile, "%s\n", buf);
      if (Mem->SaveSoundLogCounter++ % Mem->CP_save_sound_freq == 0){
	fclose(Mem->SaveSoundLogFile);
	Mem->SaveSoundLogFile = fopen(Mem->SaveSoundLogFileName,"a");
      }
    }

    sock->serv_addr.sin_port = serv_addr.sin_port ;

    if(n == 0) {
      my_error("Received null message");
      return 0 ;
    } else
      return 1 ;
  }
}


void close_connection(Socket *sock)
{
  close(sock->socketfd) ;
}

  

