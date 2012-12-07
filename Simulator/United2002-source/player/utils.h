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


#include "types.h"

//from shared directory
#include <utils.h>

#ifndef _UTILS_PLAYER_H_
#define _UTILS_PLAYER_H_

#define my_stamp printf("%d:%d.%d ",Mem->MyNumber,Mem->CurrentTime.t,Mem->CurrentTime.s);

class Time {
public:
  int t; /* time from the server */
  int s; /* stopped clock cycles */

  Time(int vt = 0, int vs = 0) { t = vt; s = vs; }
  Time operator - (const int &a);
  int  operator - (const Time &a);
  Time operator + (const int &a);
  Time operator + (const Time &a);
  int  operator % (const int &a) { return (t+s)%a; }
  void operator -=(const int &a) { *this = *this - a; }
  void operator -=(const Time &a){ *this = *this - a; }
  void operator +=(const int &a) { *this = *this + a; }
  void operator +=(const Time &a) { *this = *this + a; }
  void operator ++()             { *this += 1; }
  void operator --()             { *this -= 1; }
  Time operator = (const int &a) { t = a; s = 0; return *this; }
  bool operator ==(const Time &a) { return (s == a.s) && (t == a.t); }
  bool operator ==(const int &a)  { return t == a; }
  bool operator !=(const Time &a) { return (s != a.s) || (t != a.t); }
  bool operator !=(const int &a)  { return t != a; }
  bool operator < (const Time &a) { return ( t < a.t ) || ( t == a.t && s < a.s ); }
  bool operator < (const int &a)  { return t < a; }
  bool operator <=(const Time &a) { return ( t < a.t ) || ( t == a.t && s <= a.s ); }
  bool operator <=(const int &a)  { return t <= a; }
  bool operator > (const Time &a) { return ( t > a.t ) || ( t == a.t && s > a.s ); }
  bool operator > (const int &a)  { return t > a; }
  bool operator >=(const Time &a) { return ( t > a.t ) || ( t == a.t && s >= a.s ); }
  bool operator >=(const int &a)  { return t >= a; }
  bool operator !() { return (s == 0) && (t == 0); }

  Bool CanISubtract(const Time &a);
};


#endif






