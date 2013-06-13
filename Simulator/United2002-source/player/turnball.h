/* header file for turnball.C */

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


#ifndef _h_TURNBALL
#define _h_TURNBALL


#define Debug(x) printf(x)
#define Debug2(x,y) printf(x,y)
#define Debug3(x,y,z) printf(x,y,z)


typedef enum TURNDIR 
{ TURN_CW = -1,
  TURN_CCW = 1
} TurnDir;

typedef enum KICKTORES
{ KT_NONE,
  KT_SUCCESS,
  KT_DID_KICK,
  KT_DID_NOTHING,
  KT_LOST_BALL
} KickToRes; 


/* given the location of the ball relativve to the player, what is max
     kick power */
float get_effkickpowrate();
int kickable();

/* says whether one striaght kick will get the ball to angle dir,
   distance CP_opt_ctrl_dist for current spot */
int is_straight_kick(float dir);

#ifdef NEVER
int ballstep(float power=0.0, float angle=0.0);
#endif

int dokick(float ddir, float ddist, int doit);

KickToRes kick_to(float target_dir, TurnDir rotway);

int kick_hard_straight(float dir, int step);



#endif /* _h_TURNBALL */
