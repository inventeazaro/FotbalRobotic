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


#include <client.h>

#define MY_ERROR_LOG_LEVEL 5

void my_error(char *msg)
{
  fprintf(stderr,"PETER's ERROR (player %d%c, time %d.%d): %s\n",
	 Mem->MyNumber,Mem->MySide,Mem->CurrentTime.t,Mem->CurrentTime.s,msg);
  LogAction3(MY_ERROR_LOG_LEVEL, "MyError: %s", msg);
  //msg[1000000]=0; /* to segfault */
  if (Mem->CP_stop_on_error) {
    int tmp;
    scanf("%d",&tmp);
  }
}

//get the shared version
#include <utils.C>



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

Time Time::operator -(const int &a)
{ 
  if ( s==0 && t-a > Mem->LastStartClockTime.t ) /* default case */
    return Time(t-a,0);

  if ( s>0 ){ 
    if ( a <= s ) 
      return Time(t,s-a); /* Just take off from stopped time */
    else{ /* a > s */
      Time new_time = Time( t-(a-s),0 );  /* take off from stopped time, then server time */
      if ( new_time < Mem->LastStartClockTime ) 
	my_error ("can't be sure of this subtraction (1)");
      return new_time;
    }
  }
  else{ /* t-a <= Mem->LastStartClockTime.t */
    int stopped_time =  a - (t - Mem->LastStartClockTime.t);  
    if ( stopped_time > Mem->LastStartClockTime.s ) {
      if ( !Mem->LastStartClockTime.t ) /* if ==0, OK---account for players starting at different times */
	return Time(0,0);
      int tmp = Mem->LastStartClockTime.t - (stopped_time - Mem->LastStartClockTime.s);
      if ( tmp <= Mem->SecondLastStartClockTime.t ) 
	my_error("can't be sure of this subtraction (2) %d %d %d  %d",t,s,a,Mem->LastStartClockTime.s);
      return Time( tmp, 0 );
    }
    return Time( Mem->LastStartClockTime.t, Mem->LastStartClockTime.s - stopped_time );
  }
}

int  Time::operator -(const Time &a)
{
  if ( s==0 ){
    if ( a.t < Mem->LastStartClockTime.t ){
      if ( a.t <= Mem->SecondLastStartClockTime.t )
	my_error("Can't be sure of this subtraction (3) %d %d  -  %d %d",t,s,a.t,a.s);
      return (t - a.t) + Mem->LastStartClockTime.s;
    }
    if ( a.t > Mem->LastStartClockTime.t )
      return (t - a.t);
    if ( a.t == Mem->LastStartClockTime.t )
      return (t - a.t) + (Mem->LastStartClockTime.s - a.s);
  }
  else if ( s > 0 ){
    if ( a.t <= Mem->SecondLastStartClockTime.t )   /* If they're equal, it's not OK */
      my_error("Can't be sure of this subtraction (4) %d %d  %d %d",t,s,a.t,a.s);
    if ( a.t < Mem->LastStartClockTime.t )
      return Mem->LastStartClockTime.s + (t - a.t);
    else if ( a.t == Mem->LastStartClockTime.t && a.t != t ) /* a is during the last stopped interval */
      return ( s + ( Mem->LastStartClockTime.s - a.s ) ) + (t - a.t);
    return (s - a.s) + (t - a.t);
  }
  else /* s<0 */
    my_error("s shouldn't be <0");
  return 0;
}

Time Time::operator +(const int &a)
{
  if ( s==0 && t > Mem->LastStartClockTime.t && t+a < Mem->CurrentTime.t ) /* default case */
    return Time(t+a,0);

  Time new_time;
  
  if ( s==0 ){
    if ( Mem->LastStartClockTime.t > t ){ /* Could've missed one already */
      my_error("Can't be sure of this addition (1) %d %d",Mem->LastStartClockTime.t,t);
      new_time = Time(t+a,0);
    }
    if ( t+a < Mem->CurrentTime.t )
      new_time = Time( t+a,0 );
    else  /* t+a >= Mem->CurrentTime.t */
      new_time = Time( Mem->CurrentTime.t, a-(Mem->CurrentTime.t-t) );
  }
  else if ( s>0 ){
    if ( t == Mem->CurrentTime.t ) /* clock still stopped */
      new_time = Time( t,s+a );
    else{
      if ( Mem->LastStartClockTime.t != t ) /* Could've missed one already */
	my_error("Can't be sure of this addition (2)");
      new_time = Time ( t+(a-(Mem->LastStartClockTime.s - s)),0 );
    }
  }
  else /* s<0 */
    my_error("s shouldn't be <0");

  if ( new_time > Mem->CurrentTime ) /* clock might stop */
    my_error("Can't be sure of this addition (3)");

  return new_time;
}

Time Time::operator +(const Time &a)
{
  if (a.t == 0) {
    //just add the .s part
    return Time(t, s + a.s);
  } else {
    return Time(t+a.t, a.s);    
  }
}


Bool Time::CanISubtract(const Time &a)
{
  if ( s==0 ){
    if ( a.t < Mem->LastStartClockTime.t ){
      if ( a.t <= Mem->SecondLastStartClockTime.t )
	return FALSE;
      return TRUE;
    }
    return TRUE;
  }
  else if ( s > 0 ){
    if ( a.t <= Mem->SecondLastStartClockTime.t )   /* If they're equal, it's not OK */
      return FALSE;
    return TRUE;
  }
  else /* s<0 */
    my_error("s shouldn't be <0");
  return FALSE;
}






