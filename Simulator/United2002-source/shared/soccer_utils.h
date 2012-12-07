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


/* these are some utility functions which are specfic to soccer */

#ifndef _SOCCER_UTILS_H_
#define _SOCCER_UTILS_H_

#include "utils.h"
#include "geometry.h"
#include "types_all.h"

//the _sh is for shared
float VelAtPt2VelAtFoot_sh(Vector pt, float targ_vel_at_pt, 
			   Vector mypos, float ball_decay);

int CyclesForBallToPoint(Vector pt, float targ_vel_at_pt, float max_kick_speed, 
			 Vector mypos, float ball_decay);

int UpperBForPlayerToPoint(Vector start, Vector end);


int kick_time_helper(Vector start, Vector end, float targ_vel_at_pt,
		     float max_kick_speed, int cycles_to_kick);
int UpperBForKickToPoint(Vector start, Vector end, float targ_vel_at_pt);
int AvgTimeForKickToPoint(Vector start, Vector end, float targ_vel_at_pt);
int MinTimeForKickToPoint(Vector start, Vector end, float targ_vel_at_pt);


SetPlayMode GetSetPlayMode(char* str);
const char* GetSetPlayModeString(SetPlayMode spm);
bool IsSetPlayDefensive(SetPlayMode spm);
bool IsSetPlayOffensive(SetPlayMode spm);

SpecSetPlayMode GetSpecSetPlayMode(char* str);
const char* GetSpecSetPlayModeString(SpecSetPlayMode spm);
SetPlayMode GetSPMForSpecSPM(SpecSetPlayMode spm);

//cycles left in game
int CyclesRemaining(int curr_cycle);
//cycles left in current half
int CyclesRemainingHalf(int curr_cycle);

  /* You can specify a flag at compile time so that all the LogAction calls
     disappear
     If you add a LogAction, make sure to add it in both places */
  /* The # at the end of the function name is the total # of args */
#ifdef NO_ACTION_LOG
  inline void nothing_func(void) {}
#define LogAction1(x1) nothing_func()	
#define LogAction2(x1,x2) nothing_func()	
#define LogAction3(x1,x2,x3) nothing_func()	
#define LogAction4(x1,x2,x3,x4) nothing_func()	
#define LogAction5(x1,x2,x3,x4,x5) nothing_func()	
#define LogAction6(x1,x2,x3,x4,x5,x6) nothing_func()	
#define LogAction7(x1,x2,x3,x4,x5,x6,x7) nothing_func()	
#define LogAction8(x1,x2,x3,x4,x5,x6,x7,x8) nothing_func()	
#define LogAction9(x1,x2,x3,x4,x5,x6,x7,x8,x9) nothing_func()	
#define LogAction10(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10) nothing_func()	
#else	
  void LogAction2(int level, const char* str); /* will be timestamped automatically */

  void LogAction3(int level, const char* str, const char* param);
  void LogAction3(int level, const char* str, char c1);
  void LogAction3(int level, const char* str, float f1);
  void LogAction3(int level, const char* str, int d1);

  void LogAction4(int level, const char* str, const char* param1, const char* param2);
  void LogAction4(int level, const char* str, float f1, int d1);
  void LogAction4(int level, const char* str, float f1, float f2);
  void LogAction4(int level, const char* str, int d1, int d2);
  void LogAction4(int level, const char* str, int d1, float f1);
  void LogAction4(int level, const char* str, int d1, const char* s1);
  void LogAction4(int level, const char* str, void* p1, void* p2);

  void LogAction5(int level, const char* str, float f1, float f2, float f3);
  void LogAction5(int level, const char* str, int d1, float f1, float f2);
  void LogAction5(int level, const char* str, int d1, int d2, float f1);
  void LogAction5(int level, const char* str, int d1, int d2, int d3);
  void LogAction5(int level, const char* str, float f1, int d1, int d2);
  void LogAction5(int level, const char* str, float f1, float f2, int d1);
  void LogAction5(int level, const char* str, void* p1, void* p2, void* p3);
  void LogAction5(int level, const char* str, int d1, const char* s1, int d2);
  void LogAction5(int level, const char* str, int d1, const char* s1, const char* s2);

  void LogAction6(int level, const char* str, float f1, float f2, float f3, float f4);
  void LogAction6(int level, const char* str, int d1, float f1, float f2, float f3);
  void LogAction6(int level, const char* str, int d1, int d2, float f1, float f2);
  void LogAction6(int level, const char* str, int d1, int d2, int d3, float f1);
  void LogAction6(int level, const char* str, int d1, int d2, int d3, int d4);
  void LogAction6(int level, const char* str, float f1, int d1, int d2, int d3);
  void LogAction6(int level, const char* str, float f1, float f2, int d1, int d2);
  void LogAction6(int level, const char* str, float f1, float f2, float f3, int d1);
  void LogAction6(int level, const char* str, int d1, const char* s1, int d2, int d3);

  void LogAction7(int level, const char* str, float f1, float f2, float f3, float f4, float f5);
  void LogAction7(int level, const char* str, int d1, float f1, float f2, float f3, float f4);
  void LogAction7(int level, const char* str, int d1, int d2, float f1, float f2, float f3);
  void LogAction7(int level, const char* str, int d1, int d2, int d3, float f1, float f2);
  void LogAction7(int level, const char* str, int d1, int d2, int d3, int d4, float f1);
  void LogAction7(int level, const char* str, int d1, int d2, int d3, int d4, int d5);

  void LogAction8(int level, const char* str, float f1, float f2, float f3, float f4, float f5, float f6);
  void LogAction8(int level, const char* str, int d1, float f1, float f2, float f3, float f4, float f5);
  void LogAction8(int level, const char* str, int d1, int d2, float f1, float f2, float f3, float f4);
  void LogAction8(int level, const char* str, int d1, int d2, int d3, float f1, float f2, float f3);
  void LogAction8(int level, const char* str, int d1, int d2, int d3, int d4, float f1, float f2);
  void LogAction8(int level, const char* str, int d1, int d2, int d3, int d4, int d5, float f1);
  void LogAction8(int level, const char* str, int d1, int d2, int d3, int d4, int d5, int d6);

#endif	 //NO_ACTION_LOG



#endif
