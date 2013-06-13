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


#ifndef _KICK_H_
#define _KICK_H_

#include "client.h"

/* header file for kick.C */
/*
#define Debug(x) printf(x) 
#define Debug2(x,y) printf(x,y) 
#define Debug3(x,y,z) printf(x,y,z) 
#define Debug4(w,x,y,z) printf(w,x,y,z) 
*/



void PatsTest_static();
void PatsTest_turn(void);
void PatsTest_kick(void);
void PatsTest_kick2(void);
void PatsTest_conv(void);

int DoTurnKickCommand(TurnKickCommand kickcom);


/* says whether one striaght kick will get the ball to angle dir,
   distance CP_opt_ctrl_dist for current spot */
int is_straight_kick(float dir, float EndDist, float closeMarg);

TurnKickCommand dokick(float ddir, float ddist, float distFactor=1.0,
		       Bool* pCanKickToTraj = NULL); 

TurnDir RotToAvoidOpponent(float abs_dir);
TurnDir RotClosest(float abs_dir);

//relative angle!
KickToRes turnball_kick(AngleDeg target_dir, TurnDir rotate, Bool StopBall,
			TurnKickCommand* pCom,
			float EndDist, float closeMarg, float kickFac);
inline
KickToRes turnball_kick(AngleDeg target_dir, TurnDir rotate, Bool StopBall,
			TurnKickCommand* pCom)
{ return turnball_kick(target_dir, rotate, StopBall, pCom,
		       Mem->CP_opt_ctrl_dist,
		       Mem->CP_closest_margin, Mem->CP_dokick_factor); }
  


//relative angle!
KickToRes TurnballTo(AngleDeg target_dir, TurnDir rotate = TURN_AVOID);

#ifdef NOT_USED
int kick_hard_straight(float dir, int step);

int kick_hard_turning(float dir, TurnDir rot, int step);
#endif

TurnDir KickRotationDirectionAbs(AngleDeg abs_ang, TurnDir rot = TURN_AVOID);
inline TurnDir KickRotationDirection(AngleDeg rel_to_body_ang, TurnDir rot = TURN_AVOID)
{ return KickRotationDirectionAbs(rel_to_body_ang + Mem->MyBodyAng(), rot); }

TurnKickCommand kick_hard_moderate(AngleDeg abs_dir, float targ_vel);

//int kick_hard_2step(float abs_dir, TurnDir rot, int step, TurnKickCommand* pCom);

int smart_kick_hard_abs(float abs_dir, KickMode mode, float targ_vel,
			TurnDir rot = TURN_AVOID);
inline int smart_kick_hard_abs(float abs_dir, KickMode mode,
			       TurnDir rot = TURN_AVOID)
{
  return smart_kick_hard_abs(abs_dir, mode,
			     2*Mem->SP_ball_speed_max, rot);  
}

inline int smart_kick_hard(float rel_to_body_dir, KickMode mode, float targ_vel,
			   TurnDir rot = TURN_AVOID)
{
  return smart_kick_hard_abs(rel_to_body_dir + Mem->MyBodyAng(), mode, targ_vel, rot);
}
inline int smart_kick_hard(float rel_to_body_dir, KickMode mode,
			   TurnDir rot = TURN_AVOID)
{
  return smart_kick_hard_abs(rel_to_body_dir + Mem->MyBodyAng(),
			     mode, 2*Mem->SP_ball_speed_max, rot);
}


#ifdef NOT_USED
/* takes a point and aims to get ball moving at targ_vel at that point
   uses the quickest mode that will get to that speed */
int smart_pass(Vector pt, float targ_vel_at_pt = 1.0, KickMode mode = KM_Moderate, TurnDir rot = TURN_AVOID);
#endif

Vector get_static_ball_pos(Vector ball_pos, float kick_ang);
inline Vector get_static_ball_pos(float kick_ang)
{ return get_static_ball_pos(Mem->BallAbsolutePosition(), kick_ang); }
//returns whether or not we are there
Bool go_to_static_ball(float kick_ang, float dash_pow);
inline Bool go_to_static_ball(Vector pt, float dash_pow)
{ return go_to_static_ball( (pt - Mem->BallAbsolutePosition()).dir(), dash_pow); }
inline Bool go_to_static_ball(float kick_ang)
{ return go_to_static_ball(kick_ang, Mem->My_stamina_inc()); }
inline Bool go_to_static_ball(Vector pt)
{ return go_to_static_ball( pt, Mem->My_stamina_inc()); }


  



#endif
