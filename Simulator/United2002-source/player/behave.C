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


#include <fstream>
#include "behave.h"
#include "Memory.h"
#include "client.h"
#include "test.h"
#include "kick.h"
#include "dribble.h"

#define DebugMark(x) 

#ifdef DEBUG_OUTPUT
#define DebugBlock(x) x
#else
#define DebugBlock(x) 
#endif


void behave(){
  
  /* Put any one of these test functions in -- but only one at a time */

  /* Start one player -- move the ball around the field */
  //face_ball();

  /* Start one player, press kickoff -- it will kick to the goal */
  test_go_to_ball();

  /* Start one player, press kickoff -- it will dribble to the goal */
  //test_dribble_to_goal();

  /* Start 2 players on different teams, press 'drop ball' near one player */
  //test_1v1();

  /* Start 2 players on different teams, press 'drop ball' near one player */
  //test_volley();

  /* These are to print out the positions of objects */
  //test_print_ball();
  //test_print_positions();

  /* Start 3 players on each team and press 'drop ball' in the middle of the field */
  // Rectangle *rect = new Rectangle(Vector(0,0),Vector(30,30));
  // test_random_movement_in_rectangle(rect);
  // delete rect;

  /* make sure that the option save_action_log_level is >= 200
     The player will generate a file
     Logfiles/<team_name><num>-<side>-actions.log which records state
     information and action choices. This logfile is pure text. It can
     be viewed more easily using the modified logplayer which is
     available with the CMUnited99 support code. This is part of a
     system we call Layered Extropsection. Please see the web page
     indicated above for more details */
  //test_log_action();
  
  /* Look in test.[Ch] for more interesting beahviors to play with */
}




/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

ActionQueueRes scan_field_with_body()
{
  LogAction3(40,"scan_field_with_body (%d)",Mem->TimeToTurnForScan());
  if ( Mem->TimeToTurnForScan() ){
    turn(Mem->MyViewAngle() * 2 - Mem->CP_scan_overlap_angle);
    return AQ_ActionQueued;
  }
  else 
    return AQ_ActionNotQueued;
}

/*****************************************************************************************/

void turn_neck_to_relative_angle(AngleDeg ang)
{
  turn_neck(GetNormalizeAngleDeg(ang - Mem->MyNeckRelAng()));
}

/*****************************************************************************************/

void scan_field_with_neck()
{
  LogAction3(40,"scan_field_with_neck (%d)",Mem->TimeToTurnForScan());
  if ( Mem->TimeToTurnForScan() ){
    if ( Mem->MyNeckRelAng() >= Mem->SP_max_neck_angle-1 ) /* take into account reporting error */
      turn_neck_to_relative_angle(Mem->SP_min_neck_angle);
    else 
      turn_neck(Mem->LimitTurnNeckAngle(Mem->MyViewAngle()*2 - Mem->CP_scan_overlap_angle));
  }
}

/*****************************************************************************************/

ActionQueueRes face_only_body_to_point(Vector point)
{
  /* don't turn neck */
  LogAction4(30,"facing only body to point (%.1f %.1f)",point.x,point.y);

  /* shouldn't actually have queued actions at this point */
  AngleDeg target_rel_ang = Mem->PredictedPointRelAngFromBodyWithQueuedActions(point);

  if ( fabs(target_rel_ang) < 1 ){
    LogAction2(40,"Already close enough");
    return AQ_ActionNotQueued;
  }
  turn( target_rel_ang );
  return AQ_ActionQueued;
}

/*****************************************************************************************/

void face_only_neck_to_point(Vector point)
{
  /* don't turn body */
  LogAction4(30,"facing only neck to point (%.1f %.1f)",point.x,point.y);

  AngleDeg target_rel_ang = Mem->PredictedPointRelAngFromBodyWithQueuedActions(point);

  if ( fabs(GetNormalizeAngleDeg(Mem->MyNeckRelAng() - target_rel_ang)) < 1 ){
    LogAction2(40,"Already close enough");
    return;
  }

  if ( Mem->CanSeeAngleFromBodyWithNeck(target_rel_ang) ){
    turn_neck( Mem->LimitTurnNeckAngle(target_rel_ang - Mem->MyNeckRelAng()) );
  }
  else
    LogAction5(30,"can't face point (%.1f %.1f) with only neck (%.1f)",
		    point.x,point.y, target_rel_ang);
}

/*****************************************************************************************/

ActionQueueRes face_neck_to_point(Vector point)
{
  /* face_neck can turn body if needed */
  LogAction4(30,"facing neck to point (%.1f %.1f)",point.x,point.y);

  AngleDeg target_rel_ang = Mem->PredictedPointRelAngFromBodyWithQueuedActions(point);

  if ( fabs(GetNormalizeAngleDeg(Mem->MyNeckRelAng() - target_rel_ang)) < 1 ){
    LogAction2(40,"Already close enough");
    return AQ_ActionNotQueued;
  }

  if ( Mem->CanFaceAngleFromBodyWithNeck(target_rel_ang) ){
    LogAction2(35,"can face with neck");
    turn_neck_to_relative_angle(target_rel_ang);
    return AQ_ActionNotQueued;
  }

  /* If can't do it with just neck, turn body as much as needed to face directly */
  AngleDeg max_turn = Mem->MaxEffectiveTurn();
  if ( fabs(target_rel_ang) < max_turn ) {
    LogAction2(35,"can't face with neck, can with body");
    turn(target_rel_ang);
    turn_neck_to_relative_angle(0);
    return AQ_ActionQueued;
  }

  LogAction2(35,"can't face with neck or body alone, turning both");  
  turn(target_rel_ang);
  target_rel_ang -= max_turn;  /* The neck target_ang */

  if ( target_rel_ang < Mem->SP_min_neck_angle ){
    LogAction2(40,"couldn't face all the way");  
    turn_neck_to_relative_angle(Mem->SP_min_neck_angle);
  }
  else if ( target_rel_ang > Mem->SP_max_neck_angle ){
    LogAction2(40,"couldn't face all the way");  
    turn_neck_to_relative_angle(Mem->SP_max_neck_angle);
  }
  else 
    turn_neck_to_relative_angle(target_rel_ang);

  return AQ_ActionQueued;
}

/*****************************************************************************************/

ActionQueueRes face_neck_and_body_to_point(Vector point)
{
  /* face_neck_and_body will turn both as much as possible to the point */
  LogAction4(30,"facing neck and body to point (%.1f %.1f)",point.x,point.y);

  AngleDeg max_turn = Mem->MaxEffectiveTurn();
  AngleDeg target_rel_ang = Mem->PredictedPointRelAngFromBodyWithQueuedActions(point);

  if ( fabs(GetNormalizeAngleDeg(Mem->MyNeckRelAng() - target_rel_ang)) < 1 && fabs(target_rel_ang) < 1 ){
    LogAction2(40,"Already close enough");
    return AQ_ActionNotQueued;
  }

  if ( fabs(target_rel_ang) < max_turn ) {
    LogAction2(35,"Can get both neck and body there");
    /* Can get both neck and body there */
    face_only_body_to_point(point);
    turn_neck_to_relative_angle(0);
    return AQ_ActionQueued;
  }

  /* Turn body as much as possible and try to get neck there */
  return face_neck_to_point(point);
}

/*****************************************************************************************/

ActionQueueRes face_only_body_to_player(char side, Unum num)
{
  if ( Mem->PlayerPositionValid(side, num) ){
    return face_only_body_to_point(Mem->PlayerAbsolutePosition(side,num));
  }
  else
    return scan_field_with_body();
}

/*****************************************************************************************/

void face_only_neck_to_player(char side, Unum num)
{
  if ( Mem->PlayerPositionValid(side,num) ){
    face_only_neck_to_point(Mem->PlayerAbsolutePosition(side,num));
  }
  else
    scan_field_with_neck();
}

/*****************************************************************************************/

ActionQueueRes face_neck_to_player(char side, Unum num)
{
  if ( Mem->PlayerPositionValid(side, num) ){
    return face_neck_to_point(Mem->PlayerAbsolutePosition(side,num));
  }
  else
    return scan_field_with_body();
}

/*****************************************************************************************/

ActionQueueRes face_neck_and_body_to_player(char side, Unum num)
{
  if ( Mem->PlayerPositionValid(side, num) ){
    return face_neck_and_body_to_point(Mem->PlayerAbsolutePosition(side,num));
  }
  else
    return scan_field_with_body();
}

/*****************************************************************************************/

ActionQueueRes face_only_body_to_opponent(Unum opponent){ 
  LogAction3(30,"facing only body to opponent %d",opponent); 
  return face_only_body_to_player(Mem->TheirSide, opponent);
}

/*****************************************************************************************/

void           face_only_neck_to_opponent(Unum opponent){
  LogAction3(30,"facing only neck to opponent %d",opponent);
  face_only_neck_to_player(Mem->TheirSide, opponent);
}

/*****************************************************************************************/

ActionQueueRes face_neck_to_opponent(Unum opponent){
  LogAction3(30,"facing neck to opponent %d",opponent);
  return face_neck_to_player(Mem->TheirSide, opponent);
}

/*****************************************************************************************/

ActionQueueRes face_neck_and_body_to_opponent(Unum opponent){
  LogAction3(30,"facing neck and body to opponent %d",opponent);
  return face_neck_and_body_to_player(Mem->TheirSide, opponent);
}

/*****************************************************************************************/

ActionQueueRes face_only_body_to_teammate(Unum teammate){ 
  LogAction3(30,"facing only body to teammate %d",teammate); 
  return face_only_body_to_player(Mem->MySide, teammate);
}

/*****************************************************************************************/

void           face_only_neck_to_teammate(Unum teammate){
  LogAction3(30,"facing only neck to teammate %d",teammate);
  face_only_neck_to_player(Mem->MySide, teammate);
}

/*****************************************************************************************/

ActionQueueRes face_neck_to_teammate(Unum teammate){
  LogAction3(30,"facing neck to teammate %d",teammate);
  return face_neck_to_player(Mem->MySide, teammate);
}

/*****************************************************************************************/

ActionQueueRes face_neck_and_body_to_teammate(Unum teammate){
  LogAction3(30,"facing neck and body to teammate %d",teammate);
  return face_neck_and_body_to_player(Mem->MySide, teammate);
}

/*****************************************************************************************/

ActionQueueRes face_only_body_to_ball()
{
  LogAction2(30,"facing body to ball");
  if ( Mem->BallPositionValid() ) {
    return face_only_body_to_point(Mem->BallPredictedPositionWithQueuedActions());
  }
  else 
    return scan_field_with_body();
}

/*****************************************************************************************/

void face_only_neck_to_ball()
{
  LogAction2(30,"facing only neck to ball");
  if ( Mem->BallPositionValid() ) {
    face_only_neck_to_point(Mem->BallPredictedPositionWithQueuedActions());
  }
  else 
    scan_field_with_neck();
}

/*****************************************************************************************/

ActionQueueRes face_neck_to_ball()
{
  LogAction2(30,"facing neck to ball");
  if ( Mem->BallPositionValid() ) {
    return face_neck_to_point(Mem->BallPredictedPositionWithQueuedActions());
  }
  else 
    return scan_field_with_body();
}

/*****************************************************************************************/

ActionQueueRes face_neck_and_body_to_ball()
{
  LogAction2(30,"facing neck and body to ball");
  if ( Mem->BallPositionValid() ) {
    return face_neck_and_body_to_point(Mem->BallPredictedPositionWithQueuedActions());
  }
  else 
    return scan_field_with_body();
}


/*****************************************************************************************/
/* if the arg is DT_all, we always dodge, otherwise we only dodge if they don't have ball */
void get_ball()
{
  if ( !Mem->MyConf() || !Mem->BallPositionValid() ) my_error("not enough info to get ball");

  DodgeType dodge = DT_only_with_ball;

  if ( !Mem->BallMoving() ){
    LogAction2(30, "get_ball: ball not moving, going to it's pos");
    if ( go_to_point(Mem->BallAbsolutePosition(),0,100,dodge) == AQ_ActionNotQueued ){
      my_error("already there???");
      face_neck_and_body_to_ball();
    }
    face_only_neck_to_ball();
  }
  else {
    if ( !Mem->MyInterceptionAble() ){
      LogAction2(30, "get_ball: going to the moving ball, but can't?");
      my_error("Can't get to the ball");
      face_neck_and_body_to_ball();
    } else if (Mem->MyInterceptionNumberCycles() == 1) {
      /* we're just one dash away, so just do it */
      LogAction2(30, "get_ball: going to the moving ball, just dashing 1 cycle");
      dash(Mem->CorrectDashPowerForStamina(Mem->MyInterceptionDashPower()));
      face_only_neck_to_ball();
    } else if ( go_to_point(Mem->MyInterceptionPoint(),0,
			  Mem->MyInterceptionDashPower(),dodge) == AQ_ActionNotQueued ) {
      LogAction2(30, "get_ball: going to the moving ball, but already there?");
      my_error("already there (moving) ???");
      face_neck_and_body_to_ball();
    } else {
      LogAction4(30, "get_ball: going to the moving ball (%d) pow %.2f",
		     Mem->MyInterceptionNumberCycles(), Mem->MyInterceptionDashPower());
      face_only_neck_to_ball();
    }
  }

}

/*****************************************************************************************/

void stop_ball()
{
  if (!Mem->BallPositionValid())
    my_error("Need to know where ball is to stop it");

  if (Mem->BallVelocityValid()) {
    LogAction2(30, "stop_ball: velocity valid");
    DoTurnKickCommand(dokick(0,0));
  } else {
    LogAction2(30, "stop_ball: velocity not valid");
    //DebugDrib(printf("Stop kick; don't know velocity, so goign to kick to us"));
    kick(Mem->CP_stop_ball_power, GetNormalizeAngleDeg(Mem->BallAngleFromBody() + 180));
  }
}

/*****************************************************************************************/

void hold_ball()
{
  if (!Mem->BallPositionValid())
    my_error("Need to know where ball is to hold it");

  Unum opponent = Mem->ClosestOpponent();

  LogAction3(30, "hold_ball: closest opponent == %d",opponent);

  /* make sure that we keep control of the ball
     we need to make sure that ball's velocity is valid,
     and then scan_field only if we will keep control of ball */
  
  if ( opponent == Unum_Unknown) {
    if (!Mem->BallVelocityValid()){
      LogAction2(40, "hold_ball: velocity not valid");
      face_neck_to_ball();     
    }
    else if (Mem->TimeToTurnForScan() &&
	     (!Mem->BallMoving() || 
	      Mem->BallWillBeKickable(1,0,Mem->CP_holdball_kickable_buffer)) ){
      LogAction2(40, "hold_ball: ball not moving or will be kickable");
      scan_field_with_body();
    }
    else if (Mem->BallMoving() ||
	     !Mem->BallWillBeKickable(1,0,Mem->CP_holdball_kickable_buffer) ){
      LogAction2(40, "hold_ball: ball moving or won't be kickable");
      stop_ball();
    }
    else{
      LogAction2(40, "hold_ball: doing nothing");
      /* do nothing */
    }
  } else {
    /* closest opponent known */
    AngleDeg ang = GetNormalizeAngleDeg(Mem->OpponentAngleFromBody(opponent) + 180);

    Vector targ_pos = Mem->RelativeToMyBody2Global(Polar2Vector(Mem->CP_opt_ctrl_dist, ang));
    if (!Mem->FieldRectangle.IsWithin(targ_pos)) {
      /* Adjust the targ_pos and ang to be in bounds- ignore the corners! */
      Line lSide = Mem->FieldRectangle.nearestEdgeLine(Mem->MyPos());
      Vector sol1, sol2;
      int num_sol = LineCircleIntersect(lSide, Mem->CP_opt_ctrl_dist, Mem->MyPos(),
					&sol1, &sol2);
      if (num_sol == 0) {
	my_error("hold_ball: why didn't LineCircleIntersect work?");
      } else if (num_sol == 1) {
	LogAction6(40, "hold_ball: avoiding the sideline 1; old: (%.1f, %.1f)  new: (%.1f, %.1f)",
		   targ_pos.x, targ_pos.y, sol1.x, sol1.y);
	ang = Mem->AngleToFromBody(sol1);
      } else if (num_sol == 2) {
	if (targ_pos.dist2(sol1) < targ_pos.dist2(sol2)) {
	  LogAction6(40, "hold_ball: avoiding the sideline 2; old: (%.1f, %.1f)  new: (%.1f, %.1f)",
		     targ_pos.x, targ_pos.y, sol1.x, sol1.y);
	  ang = Mem->AngleToFromBody(sol1);
	} else {
	  LogAction6(40, "hold_ball: avoiding the sideline 3; old: (%.1f, %.1f)  new: (%.1f, %.1f)",
		     targ_pos.x, targ_pos.y, sol2.x, sol2.y);
	  ang = Mem->AngleToFromBody(sol2);
	}      
      }
    }
    
    if (!Mem->BallWillBeKickable(1,0,Mem->CP_holdball_kickable_buffer)) {
      LogAction2(40, "hold_ball: turning ball from opponent -- won't be kickable");
      if (TurnballTo(ang) == KT_DidNothing)
	my_error("hold_ball: why didn't turnball do something");
    } else if (Mem->OpponentPositionValid(opponent) < .9 &&
	       !Mem->InViewAngle(Mem->OpponentAngleFromNeck(opponent))){
      LogAction2(40, "hold_ball: looking at opponent");
      face_neck_to_opponent(opponent);
    }
    else if (Mem->TimeToTurnForScan() && Mem->EstimatedCyclesToSteal(opponent) > Mem->CP_time_for_full_rotation/3){
      /* do we want to scan when the ball may be near the opp? */
      /* yes, but only if the opponent isn't about to get the ball */
      LogAction2(40, "hold_ball: looking around");
      scan_field_with_body();
    }
    else{
      LogAction2(40, "hold_ball: turning ball from opponent");
      TurnballTo(ang);
    }
  }
}

/*****************************************************************************************/

void pass_ball(Unum teammate, float target_vel_at_dest)
{
  LogAction3(30, "passing to %d",teammate);    

  if ( teammate == Unum_Unknown ) my_error("Need to pass to a teammate");
  if ( !Mem->TeammatePositionValid(teammate) ) my_error("can't pass to invalid teammmate");

  if ( Mem->TeammatePositionValid(teammate) < .9 ){
    LogAction2(40, "pass_ball: looking for teammate");
    if ( face_neck_to_teammate(teammate) == AQ_ActionNotQueued )
      hold_ball();
    return;
  }

  Vector   target = Mem->TeammateAbsolutePosition(teammate);
  AngleDeg target_angle = Mem->AngleToFromBody(target);
  TurnDir  rotation = KickRotationDirection(target_angle);
  float target_vel = Mem->VelAtPt2VelAtFoot(target,target_vel_at_dest);
  KickMode mode = Mem->BestKickMode(target_angle); 
  if (mode == KM_HardestKick) 
    mode = KM_Hard; 

  Mem->team_passer = Mem->MyNumber;
  Mem->team_receiver = teammate;
  Mem->team_pass_time = Mem->CurrentTime;  

  LogAction2(40, "pass_ball: starting pass");
  kick_ball(target,mode,target_vel,rotation); 
}

/*****************************************************************************************/

/* extend this angle to the sideline and call the other kick_ball variant */
/* target_angle is relative to body */
void kick_ball(AngleDeg target_angle, KickMode mode, float target_vel, TurnDir rotation)
{
  Vector target_pt =
    Mem->FieldRectangle.RayIntersection(Ray(Mem->MyPos(), target_angle + Mem->MyBodyAng()));
  LogAction5(40, "starting kick to angle %.1f, translated to point (%.1f, %.1f)",
		  target_angle, target_pt.x, target_pt.y);
  kick_ball(target_pt, mode, target_vel, rotation);
  return;
}

void kick_ball(Vector point, KickMode mode, float target_vel, TurnDir rotation)
{
  if (rotation == TURN_NONE)
    rotation = KickRotationDirection(Mem->AngleToFromBody(point));

  /* look to see if a dash will help */
  if (mode == KM_Hard && Mem->WillDashHelpKick(point, Mem->SP_max_power)) {
    mode = KM_Moderate;
    float target_angle =
      (point - Mem->MyPredictedPosition(1, Mem->SP_max_power)).dir() - Mem->MyBodyAng();
    LogAction3(40, "kick_ball: dashing will help kick to angle: %1.f", target_angle);
    dash(Mem->SP_max_power);
    Mem->StartKick(target_angle, mode, target_vel, rotation);
  } else {
    if (mode == KM_Hard)
      mode = KM_Moderate;
    float target_angle = Mem->AngleToFromBody(point);
    LogAction3(40, "kick_ball: starting kick to angle %.1f",target_angle);
    Mem->StartKick(target_angle, mode, target_vel, rotation);
    smart_kick_hard(target_angle, mode, target_vel, rotation);
  }
  
  return;
}

void kick_ball(AngleDeg target_angle, KickMode mode, TurnDir rotation)
{
  kick_ball(target_angle,mode,2*Mem->SP_ball_speed_max,rotation);
}

void kick_ball(Vector point, KickMode mode, TurnDir rotation)
{
  kick_ball(point,mode,2*Mem->SP_ball_speed_max,rotation);
}


/*****************************************************************************************/

ActionQueueRes go_to_point( Vector p, float buffer, float dash_power, DodgeType dodge )
{
  LogAction5(30, "go_to_point %d (%.1f %.1f)",dodge, p.x,p.y);
  if ( !Mem->MyConf() ) my_error("Can't go to a point if not localized");

  if ( Mem->DistanceTo(p) < buffer ){
    if ( Mem->SP_use_offside && fabs(Mem->MyX() - Mem->my_offside_line) < 5 ){ /* hack */
      Unum opp = Mem->FurthestForwardOpponent();
      if ( opp != Unum_Unknown && Mem->OpponentPositionValid(opp) < .9 ){ /* hack */
	LogAction2(40, "go_to_point: looking for offsides line");
	return face_neck_to_opponent(opp);
      }
    }
    LogAction2(40, "go_to_point: already at the point");
    return AQ_ActionNotQueued;
  }

  if ( Mem->PlayMode == PM_Their_Goal_Kick && Mem->MyPos() != p ){
    /* if ( Mem->TheirPenaltyArea.IsWithin(p) ){
      my_error("Can't go into their penalty area on a goal kick!"); */
    Line l = LineFromTwoPoints(Mem->MyPos(),p);
    Vector intersection = AdjustPtToRectOnLine(Mem->MyPos(),Mem->TheirPenaltyArea,l);
    if ( intersection != Mem->MyPos() && l.InBetween(intersection,Mem->MyPos(),p) ){
      /* Need to go around the rectangle */
      LogAction2(40, "go_to_point: moving around penalty area");
      Vector target;
      if ( Mem->MyX() < Mem->TheirPenaltyArea.LeftX() )
	target = Vector(Mem->TheirPenaltyArea.LeftX()-3,p.y);
      else if ( Mem->MyY() > 0 )
	target = Vector(Mem->TheirPenaltyArea.LeftX()-3,Mem->TheirPenaltyArea.BottomY()+3 );
      else
	target = Vector(Mem->TheirPenaltyArea.LeftX()-3,Mem->TheirPenaltyArea.TopY()-3 );
      go_to_point( target, 0, dash_power, dodge );
      return AQ_ActionQueued;
    }
  }

  if ( Mem->PlayMode != PM_Play_On && Mem->TeamInPossession() == Mem->TheirSide &&
       !Mem->OffsidePosition(p,Mem->MySide) &&
       //p.dist(Mem->BallAbsolutePosition()) > Mem->SP_free_kick_buffer &&
       Mem->InOffsidePosition() && Mem->BallDistance() < Mem->SP_free_kick_buffer+1 ){
    LogAction2(40, "go_to_point: moving around free_kick area");
    if ( Mem->BallY() > Mem->MyY() )
      go_to_point(Vector(Mem->MyX(),Mem->BallY()-(Mem->SP_free_kick_buffer+1)));
    else
      go_to_point(Vector(Mem->MyX(),Mem->BallY()+(Mem->SP_free_kick_buffer+1)));
    return AQ_ActionQueued;
  }

  float target_ang  = GetNormalizeAngleDeg((p - Mem->MyPredictedPosition()).dir() - Mem->MyBodyAng());
  float target_dist = Mem->DistanceTo(p);

  if ( dodge != DT_none ){ /* dodge players */
    PlayerObject *player;
    float    dodge_dist = Min(Mem->CP_dodge_distance_buffer,target_dist);
    AngleDeg dodge_ang  = Mem->CP_dodge_angle_buffer;
    if ( (player = Mem->GetPlayerWithin( dodge_dist, dodge_ang, 0, target_ang - dodge_ang)) != NULL &&
	 (dodge!=DT_unless_with_ball || 
	  (Mem->BallPositionValid() && 
	   player->get_abs_pos().dist(Mem->BallAbsolutePosition()) > Mem->SP_kickable_area)) &&
	 (dodge!=DT_only_with_ball ||
	  (Mem->BallPositionValid() && 
	   player->get_abs_pos().dist(Mem->BallAbsolutePosition()) <= Mem->SP_kickable_area)) ){
      LogAction2(40, "go_to_point: dodging right");
      /*if ( Mem->NumPlayersWithin( dodge_dist, 2*dodge_ang) ){*/
	/* Target at dist player_size, so no players will be within in the next iteration ==> dash */
      Vector new_target = Mem->BodyPolar2Gpos(Mem->SP_player_size,player->get_ang_from_body() + Mem->CP_dodge_angle);
      if ( new_target == p )
	my_error("Dodging isn't changing the point!");
      go_to_point(new_target,0,Mem->CP_dodge_power,DT_none); 
      /*}
      else{
	dash(Mem->CorrectDashPowerForStamina(Min(dash_power,Mem->CP_dodge_power)));
      }*/
      return AQ_ActionQueued;
    }
    if ( (player = Mem->GetPlayerWithin( dodge_dist, dodge_ang, 0, target_ang + dodge_ang)) != NULL &&
	 (dodge!=DT_unless_with_ball || 
	  (Mem->BallPositionValid() && 
	   player->get_abs_pos().dist(Mem->BallAbsolutePosition()) > Mem->SP_kickable_area)) &&
	 (dodge!=DT_only_with_ball ||
	  (Mem->BallPositionValid() && 
	   player->get_abs_pos().dist(Mem->BallAbsolutePosition()) <= Mem->SP_kickable_area)) ){
      LogAction2(40, "go_to_point: dodging left");
      /*if ( Mem->NumPlayersWithin( dodge_dist, 2*dodge_ang) ){*/
	/* Target at dist player_size, so no players will be within in the next iteration ==> dash */
	Vector new_target = Mem->BodyPolar2Gpos(Mem->SP_player_size,player->get_ang_from_body() - Mem->CP_dodge_angle);
	if ( new_target == p )
	  my_error("Dodging isn't changing the point!");
	go_to_point(new_target,0,Mem->CP_dodge_power,DT_none);
      /*}
      else{
	dash(Mem->CorrectDashPowerForStamina(Min(dash_power,Mem->CP_dodge_power)));
      }*/
      return AQ_ActionQueued;
    }
  }

  if ( fabs(target_ang) > Mem->CP_max_go_to_point_angle_err || 
       (Mem->PlayMode == PM_Their_Goal_Kick && 
	Mem->TheirPenaltyArea.IsWithin(Mem->MyPredictedPosition(1,dash_power))) ){
    LogAction3(50, "go_to_point: turning %f", target_ang);
    turn(target_ang);
    return AQ_ActionQueued;
  }

  dash_power = Mem->CorrectDashPowerForStamina(dash_power);
  if ( dash_power > 0 ){
    LogAction3(50, "go_to_point: dashing %f", dash_power);
    dash( dash_power );
    return AQ_ActionQueued;
  }
  else
    {my_stamp; printf("recovering\n");} 

  LogAction2(50, "go_to_point: doing nothing?");
  return AQ_ActionNotQueued;
}

