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


#ifndef _BEHAVE_TEST_H_
#define _BEHAVE_TEST_H_

#include "geometry.h"
#include "types.h"

void test_scan_with_body();
void test_random_movement_in_rectangle(Rectangle *rect);
void test_1v1();
void test_volley();
void test_go_to_ball(AngleDeg kick_angle);
void test_go_to_ball();
void test_dribble_to_goal();
void test_go_to_point(Vector p, float buffer, float dash_power = 100);
void test_face_ball();
void test_random_movement();
void test_straight_to_ball();
void test_run_straight();
void test_turn_and_dash_slow();
void test_print_ball();
void test_print_positions();

void test_turnball();
void test_turnball2();
void test_hard_kick(KickMode km);
void test_hetero();
void test_intercept();
void test_go_to_static_ball();
void test_pred_cycles_to_point();
void test_log_action();

#endif
