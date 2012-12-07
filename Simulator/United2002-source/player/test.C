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


#include <stdio.h>
#include <fstream>
#include <iomanip.h>
#include "test.h"
#include "Memory.h"
#include "client.h"
#include "kick.h"
#include "dribble.h"
#include "behave.h"

/*****************************************************************************************/

void test_scan_with_body()
{
  if ( Mem->PlayMode == PM_Before_Kick_Off ){
    move(-1,0);
    my_stamp; printf("%.1f\n",Mem->MyBodyAng());
    return;
  }

  if ( Mem->TimeToTurnForScan() && Mem->CurrentTime < 50 ){
    my_stamp; printf("turning.  Last sight at %d\n",Mem->LastSightTime.t);
    turn(Mem->MyViewAngle() * 2);
  }
  my_stamp; printf("        %.1f\n",Mem->MyBodyAng());
}


/*****************************************************************************************/

void test_1v1()
{
  static int First_time = TRUE;
  if ( First_time ){
    if (Mem->PlayMode == PM_Before_Kick_Off)
      move(-10,0);
    First_time=FALSE;
    return;
  }

  if ( Mem->BallPositionValid() ){
    if ( Mem->BallKickable() ){
      hold_ball();
    }
    else {
      get_ball();
    }
  }
  else
    face_neck_and_body_to_ball();
}

/*****************************************************************************************/


/*****************************************************************************************/

void test_volley()
{
  static int First_time = TRUE;
  if ( First_time ){
    move(-20,0);
    First_time=FALSE;
    return;
  }

  if ( Mem->BallPositionValid() && Mem->BallX() < -1 )
    test_go_to_ball();
  else
    test_go_to_point(Vector(-20,0),3);
}

/*****************************************************************************************/

void test_go_to_ball(AngleDeg kick_angle)
{
  if ( Mem->BallKickable() ){
    if ( !Mem->TestVersion ){
      if (!smart_kick_hard(kick_angle, KM_QuickestRelease))
	my_error("test_go_to_ball: kick failed\n");
    }
    else
      kick(100,kick_angle);
  }
  else if ( Mem->MyConf() && Mem->BallPositionValid() ){
    if ( Mem->TestVersion )
      test_go_to_point(Mem->BallAbsolutePosition(),Mem->My_kickable_area());
    else {
      if (Mem->MyInterceptionAble())
	test_go_to_point(Mem->MyInterceptionPoint(),0);	
      else 
	test_face_ball();
    }
  }
  else 
    turn(90);
}

/*****************************************************************************************/

void test_go_to_ball()
{
  if ( Mem->MyConf() ) 
    test_go_to_ball(Mem->MarkerAngleFromBody(Mem->RM_Their_Goal));
  else
    turn(90);
}

/*****************************************************************************************/

void test_go_to_point(Vector p, float buffer, float dash_power )
{
  if ( !Mem->MyConf() ) {
    turn(Mem->MyViewAngle());
    return;
  }

  if ( Mem->DistanceTo(p) < buffer ){
    //turn(30);
    test_face_ball();
    return;
  }

  float target_ang  = Mem->AngleToFromBody(p);
  float target_dist = Mem->DistanceTo(p);

  if ( 1 && !Mem->ClockStopped ){ /* dodge players */
    PlayerObject *player;
    float    dodge_dist = Min(Mem->CP_dodge_distance_buffer,target_dist);
    AngleDeg dodge_ang  = Mem->CP_dodge_angle_buffer;
    if ( (player = Mem->GetPlayerWithin( dodge_dist, dodge_ang, 0, target_ang - dodge_ang))
	 != NULL ){
      if ( Mem->NumPlayersWithin( dodge_dist, 2*dodge_ang) ){
	/* Target close, so no players will be within in the next iteration ==> dash */
	Vector new_target = Mem->BodyPolar2Gpos(Mem->SP_player_min_size,player->get_ang_from_body() + 90);
	test_go_to_point(new_target,0);
      }
      else{
	dash(Mem->CP_dodge_power);
      }
      return;
    }
    if ( (player = Mem->GetPlayerWithin( dodge_dist, dodge_ang, 0, target_ang + dodge_ang))
	 != NULL ){
      if ( Mem->NumPlayersWithin( dodge_dist, 2*dodge_ang) ){
	/* Target close, so no players will be within in the next iteration ==> dash */
	Vector new_target = Mem->BodyPolar2Gpos(Mem->SP_player_min_size,player->get_ang_from_body() - 90);
	test_go_to_point(new_target,0);
      }
      else{
	dash(Mem->CP_dodge_power);
      }
      return;
    }
  }

  if ( fabs(target_ang) > Mem->CP_max_go_to_point_angle_err ){
    turn(target_ang);
    return;
  }

  if ( Mem->MyStamina() >= Mem->EffortDecThreshold )
    dash( Min( dash_power, Mem->MyStamina()-Mem->EffortDecThreshold ) );
  else
    {my_stamp; printf("recovering\n");} //turn(180);
}

/*****************************************************************************************/

void test_dribble_to_goal()
{
  if ( Mem->BallKickable() ){
    if ( Mem->KickInProgress() ){
      smart_kick_hard_abs(Mem->kick_in_progress_abs_angle,Mem->kick_in_progress_mode,
			  Mem->kick_in_progress_target_vel,Mem->kick_in_progress_rotation);
    }
    else
      SimpleDribbleTo(Mem->MarkerPosition(Mem->RM_Their_Goal));
    return;
  }
  else if ( Mem->MyConf() && Mem->BallPositionValid() ){
    if ( Mem->TestVersion )
      test_go_to_point(Mem->BallAbsolutePosition(),Mem->SP_kickable_area);
    else {
      if (Mem->MyInterceptionAble())
	test_go_to_point(Mem->MyInterceptionPoint(),0);	
      else 
	test_face_ball();
    }
  }
  else 
    turn(90);
}

/*****************************************************************************************/

void test_face_ball()
{
  if ( Mem->BallPositionValid() ) 
    turn( Mem->BallAngleFromNeck() );
  else 
    turn(Mem->MyViewAngle());
}


/*****************************************************************************************/

void test_random_movement()
{
  static int First_time = FALSE;
  if ( First_time ){
    move(range_random(-50,0),range_random(-Mem->SP_pitch_width/2,Mem->SP_pitch_width/2));
    First_time=FALSE;
    return;
  }

  /* if      ( !int_random(100) ) change_view(VW_Wide);
     else if ( !int_random(100) ) change_view(VW_Normal);
     else if ( !int_random(100) ) change_view(VW_Narrow); */

  if ( Mem->ClockStopped ) return;

  if ( Mem->BallKickable() ){
    kick(range_random(0,100),Mem->BallAngleFromBody());
    return;
  }

  if ( Mem->MyConf() && Mem->BallPositionValid() && Mem->BallDistance() < 9 ){
    if ( fabs(Mem->BallAngleFromBody()) > 1 ) 
      turn( Mem->BallAngleFromBody() );
    else
      dash(100);
    return;
  }

  if ( int_random(4) )
    dash(range_random(20,30));
  else 
    turn(range_random(-90,90));

  return;
}

/*****************************************************************************************/

void test_straight_to_ball()
{
  static int First_time = TRUE;
  if ( First_time ){
    if (Mem->MySide == 'l')
      turn(90);
    else
      turn(-90);
    First_time=FALSE;
    return;
  }

  if ( Mem->CurrentTime.t && Mem->CurrentTime.t < 10 && Mem->GetBall()->pos_valid() ){
    turn(Mem->GetBall()->get_ang_from_body());
  }
  else 
    dash(Mem->CurrentTime.t%100); 
  return;
}

/*****************************************************************************************/

void test_run_straight()
{
  static Bool GO = TRUE;
  
  if ( !(Mem->CurrentTime.t%200) ) GO = TRUE;

  if ( Mem->MyStamina() > 600 && GO ) 
    dash(60);
  else if ( Mem->CurrentTime.t ){
    GO = FALSE;
    printf("stamina = %f, effort = %f\n",Mem->MyStamina(),Mem->MyEffort());
  }
}

/*****************************************************************************************/

void test_turn_and_dash_slow()
{
  static int First_time = TRUE;
  if ( First_time ){
    if (Mem->MySide == 'l')
      turn(90);
    else
      turn(-90);
    First_time=FALSE;
    return;
  }

  if ( Mem->CurrentTime.t%5 || !Mem->CurrentTime.t ) return;

  if ( !(Mem->CurrentTime.t%50) )
    turn(40);
  else
    dash(100);

  return;
}


/*****************************************************************************************/

void test_print_ball()
{
  my_stamp;

  if ( Mem->BallPositionValid() ) {
    printf(" Ball at %.1f %.1f (%.2f)",Mem->BallX(), Mem->BallY(), Mem->BallPositionValid()); 
    if ( Mem->BallVelocityValid() )
          printf("     Speed %.2f dir %.1f (%.2f)",
		 Mem->BallSpeed(), Mem->BallAbsoluteHeading(), Mem->BallVelocityValid()); 
    else
      printf("    velocity not valid");
  }
  else 
    printf(" Ball not valid");

  printf("\n");
}

/*****************************************************************************************/

void test_print_positions()
{
  Bool me = TRUE;
  Bool ball = FALSE;
  Bool players = TRUE;

  if (me){
    my_stamp; printf("at %d I'm at (%.1f %.1f) facing %.1f with velocity (%f %.1f)\n",
		     Mem->CurrentTime.t,Mem->MyX(),Mem->MyY(),Mem->MyBodyAng(),Mem->MySpeed(),Mem->MyDir());
  }

  if (ball){
    if (Mem->GetBall()->pos_valid())
      printf("Ball: (%f %f)       ",Mem->GetBall()->get_x(),Mem->GetBall()->get_y());

    if (Mem->GetBall()->vel_valid())
      printf("velocity: (%f %f)\n",Mem->GetBall()->get_speed(),Mem->GetBall()->get_abs_heading());
    else if (Mem->GetBall()->pos_valid())
      printf("\n"); 
  }

  if (players){
    for (int i=0; i<Mem->NumTeammates(); i++){
      my_stamp;
      printf("T %d at %.1f %.1f facing %.1f\n",Mem->Teammates()[i]->unum,Mem->Teammates()[i]->get_x(),Mem->Teammates()[i]->get_y(),Mem->Teammates()[i]->get_abs_body_ang());
    }
    for (int i=0; i<Mem->NumOpponents(); i++){
      my_stamp;
      printf("O %d at %.1f %.1f facing %.1f\n",Mem->Opponents()[i]->unum,Mem->Opponents()[i]->get_x(),Mem->Opponents()[i]->get_y(),Mem->Opponents()[i]->get_abs_body_ang());
    }
    for (int i=0; i<Mem->NumTeamlessPlayers(); i++){
      my_stamp;
      printf("? %d at %.1f %.1f facing %.1f\n",Mem->TeamlessPlayers()[i]->unum,Mem->TeamlessPlayers()[i]->get_x(),Mem->TeamlessPlayers()[i]->get_y(),Mem->TeamlessPlayers()[i]->get_abs_body_ang());
    }
  }
}

/*****************************************************************************************/

void test_time()
{
  Time tmp = Mem->CurrentTime - 2;
  printf("time: %d %d         %d %d      (%d %d)",
	 Mem->CurrentTime.t,Mem->CurrentTime.s,tmp.t,tmp.s,Mem->LastStartClockTime.t,Mem->LastStartClockTime.s);
  if ( Mem->CurrentTime.t > 5 ){
    int tmp2 = Mem->CurrentTime - Time(5,0);
    printf(" --------  %d)",tmp2);
  }
  printf("\n");
}


/***************************************************************************/
/* Pat added for parameter tuning */
/**************************************************************************/
void test_turnball()
{
  if (!Mem->MyConf() || !Mem->BallPositionValid())
    scan_field_with_body();
  else if (Mem->BallKickable()) {
    TurnballTo(Mem->BallAngleFromBody() + 180, TURN_CCW);
  } else
    ; /* if we can't kick it, do nothing */
  
}

void test_turnball2()
{
  static TurnDir turn_dir = TURN_CW;
  if (!Mem->MyConf() || !Mem->BallPositionValid()) {
    /* switch the turn direction when we lose the ball,
       this effectively randomizes it */
    turn_dir = (turn_dir == TURN_CW) ? TURN_CCW : TURN_CW;
    scan_field_with_body();
  }  
  else if (Mem->BallKickable()) {
    TurnballTo(-Mem->MyBodyAng(), turn_dir);
  } else
    ; /* if we can't kick it, do nothing */
  
}

#ifdef DEBUG_OUTPUT
#define DEBUG_TEST_KICK(x) 
#else
#define DEBUG_TEST_KICK(x)
#endif
void test_hard_kick(KickMode km)
{
  static float bally;
  static Bool LastActionKick = FALSE;

  if (!Mem->MyConf()) {
    DEBUG_TEST_KICK(cout << "Time: " << Mem->CurrentTime.t << "\tScanning field" << endl);
    scan_field_with_body();
    LastActionKick = FALSE;
  } else if (Mem->PlayMode == PM_Before_Kick_Off) {
    move(-10,0);
  } else {
    DEBUG_TEST_KICK(cout << "Time: " << Mem->CurrentTime.t << "\tMyAng: " << Mem->MyAng() << endl);
    if (fabs(GetNormalizeAngleDeg(Mem->MyBodyAng() -
				  signf(bally)*
				  Mem->CP_hardest_kick_player_ang)) > 9 &&
	(!Mem->BallKickable() || Mem->BallVelocityValid()) &&
	!LastActionKick) {
      DEBUG_TEST_KICK(cout << " Turning: target: "
	   << signf(bally)*Mem->CP_hardest_kick_player_ang
	   << "\tturning: "
	   << signf(bally)*Mem->CP_hardest_kick_player_ang - Mem->MyAng() << endl);
      turn(signf(bally)*Mem->CP_hardest_kick_player_ang - Mem->MyBodyAng());
      LastActionKick = FALSE;
    } else if (Mem->BallKickable()) {
      DEBUG_TEST_KICK(cout << "calling smart_kick_hard" << endl);
      smart_kick_hard_abs(0.0, km);
      if (Mem->Action->type == CMD_kick)
	LastActionKick = TRUE;
      else
	LastActionKick = FALSE;
      bally = Mem->BallY();
    } else
      LastActionKick = FALSE; /* if we can't kick or turn, do nothing */
  }
  
}

void test_hetero()
{
  Vector target = Vector(15,10);

  if (!Mem->MyConf()) {
    scan_field_with_body();
  } 
  else if ( Mem->MyY() > target.y+1 ){
    if ( go_to_point(target) == AQ_ActionNotQueued )
      go_to_point(Vector(0,0));
  } 
  else if ( !Mem->BallKickable() ){
    if ( Mem->BallPositionValid() )
      go_to_point(Mem->BallAbsolutePosition());
    else
      go_to_point(Vector(0,0));
  }
  else if ( Mem->KickInProgress() ){
    /* Already kicking */
    smart_kick_hard_abs(Mem->kick_in_progress_abs_angle,Mem->kick_in_progress_mode,
			Mem->kick_in_progress_target_vel,Mem->kick_in_progress_rotation);
  }
  else {
    kick_ball(Mem->MarkerPosition(Mem->RM_Their_Goal));
  }
}


void test_intercept()
{
  if (!Mem->MyConf()) {
    LogAction2(10, "Lost myself, scanning field");
    scan_field_with_body();
  } else if (!Mem->BallPositionValid()) {
    LogAction2(10, "Lost the ball, scanning field");
    scan_field_with_body();
  } else if (Mem->BallKickable()) {
    LogAction2(10, "Have ball, holding onto it");
    hold_ball();
  } else {
    LogAction2(10, "Chasing the ball");
    get_ball();
  }
  
}

void test_go_to_static_ball()
{
 if (!Mem->MyConf() || !Mem->BallPositionValid())
    scan_field_with_body();
  else if (Mem->BallKickable() && Mem->LastActionType() == CMD_kick) {
    smart_kick_hard_abs((-Mem->BallAbsolutePosition()).dir(), KM_HardestKick);
  } else {
    if (go_to_static_ball((-Mem->BallAbsolutePosition()).dir()))
      smart_kick_hard_abs((-Mem->BallAbsolutePosition()).dir(), KM_HardestKick);
    //cout << Mem->CurrentTime.t << " I'm ready to kick" << endl;
  }
}

void test_pred_cycles_to_point()
{
  static Vector pt(0,0);
  static Rectangle r(Vector(0,0), Vector(20,20));
  
  if (!Mem->MyConf())
    scan_field_with_body();
  else {
    cout << "Time: " << setw(4) << Mem->CurrentTime.t << '\t'
	 << "predict " << setw(3) << Mem->PredictedCyclesToPoint(pt) << " cycles "
	 << "to point " << pt << endl;
    if (go_to_point(pt, Mem->CP_at_point_buffer) == AQ_ActionNotQueued) {
      cout << "I got there!" << endl;
      pt = r.random();
    }
  }
}




void test_log_action()
{
  if (!Mem->MyConf()) {
    LogAction2(10, "I'm lost! Looking around");
    scan_field_with_body();
  } else if ( !Mem->BallPositionValid() ){
    LogAction2(10, "I lost the ball! Looking around");
    scan_field_with_body();
  } else if ( Mem->BallKickable() ){
    float kick_angle;
    LogAction2(10, "Ball is kickable!");
    if (Mem->MyX() > 0) {
      LogAction2(30, "On offensive half, kicking to goal");
      kick_angle = Mem->MarkerAngleFromBody(Mem->RM_Their_Goal);
    } else {
      LogAction2(30, "On defensive half, kicking to corner");
      kick_angle = Mem->MarkerAngleFromBody(Mem->RM_RF_Flag);
    }
    
    if (!smart_kick_hard(kick_angle, KM_QuickestRelease))
      my_error("test_go_to_ball: kick failed\n");
  } else {
    LogAction2(10, "I see the ball! going for it");
    test_go_to_point(Mem->BallAbsolutePosition(),Mem->SP_kickable_area);
  }
}





