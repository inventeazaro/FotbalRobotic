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


#ifndef _BEHAVE_H_
#define _BEHAVE_H_

#include "geometry.h"
#include "types.h"

void behave();

ActionQueueRes scan_field_with_body();
void turn_neck_to_relative_angle(AngleDeg ang);
void scan_field_with_neck();

ActionQueueRes face_only_body_to_point(Vector point);
void           face_only_neck_to_point(Vector point);
ActionQueueRes face_neck_to_point(Vector point);
ActionQueueRes face_neck_and_body_to_point(Vector point);

ActionQueueRes face_only_body_to_player(char side, Unum num);
void           face_only_neck_to_player(char side, Unum num);
ActionQueueRes face_neck_to_player(char side, Unum num);
ActionQueueRes face_neck_and_body_to_player(char side, Unum num);

ActionQueueRes face_only_body_to_opponent(Unum opponent);
void           face_only_neck_to_opponent(Unum opponent);
ActionQueueRes face_neck_to_opponent(Unum opponent);
ActionQueueRes face_neck_and_body_to_opponent(Unum opponent);

ActionQueueRes face_only_body_to_teammate(Unum teammate);
void           face_only_neck_to_teammate(Unum teammate);
ActionQueueRes face_neck_to_teammate(Unum teammate);
ActionQueueRes face_neck_and_body_to_teammate(Unum teammate);

ActionQueueRes face_only_body_to_ball();
void           face_only_neck_to_ball();
ActionQueueRes face_neck_to_ball();
ActionQueueRes face_neck_and_body_to_ball();

void get_ball();
void stop_ball();
void hold_ball();
void pass_ball(Unum teammate, float target_vel=1.0);
void kick_ball(AngleDeg target_angle, KickMode mode, float targ_vel,
	       TurnDir rotation = TURN_NONE);
void kick_ball(Vector point, KickMode mode, float targ_vel,
	       TurnDir rotation = TURN_NONE);
void kick_ball(AngleDeg target_angle, KickMode mode = KM_Moderate,
	       TurnDir rotation = TURN_NONE);
void kick_ball(Vector point, KickMode mode = KM_Moderate,
	       TurnDir rotation = TURN_NONE);

ActionQueueRes go_to_point(Vector p, float buffer = 0, float dash_power = 100, DodgeType dodge = DT_all);


#endif
