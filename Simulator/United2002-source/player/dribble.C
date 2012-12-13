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


#include <math.h>
#include "Memory.h"
#include "client.h"
#include "kick.h"
#include "dribble.h"
#include "behave.h"

#ifdef DEBUG_OUTPUT
#define DebugDrib(x) 
#define DebugDrib2(x) 
#else
#define DebugDrib(x)
#define DebugDrib2(x)
#endif

/* Here's the basic strategy for dribbling:
   Alternate kicks and dashs so that the ball stays close to us.
   Using predicted ball positions and velocity corrected kicks, then given
   an angle we want to dribble at, we can choose a dash or kick that is most
   appropraite */

/* Checks to see if a dash would keep us in control of ball or if it would
   help us catch up to ball. If not, it calls dribble_kick */
TurnKickCommand dribble_dash(Vector vEndPos, float max_pow,
			     AngleDeg drib_ang, DribbleMode mode)
{
  LogAction2(70, "dribble: trying to do a dash");
  DebugDrib(cout << "in dribble_dash" << endl);
  TurnKickCommand com;
  com.time = -1;
  com.turn_neck = FALSE;
  
  if (!Mem->BallWillBeKickable(1, max_pow, Mem->CP_kickable_buffer)) {    
    if (Mem->BallKickable()) {      
      DebugDrib(printf("Dribble: Ball is not goign to be kickable, so we'll kick it.\n"));
      LogAction2(80, "dribble: tried dash, but will lose ball, so kicking");
      return dribble_kick(vEndPos, max_pow, drib_ang, mode);
    }    
    if (!Mem->BallWillBeKickable(2, max_pow, Mem->CP_kickable_buffer) &&
	!Mem->BallWillBeKickable(3, max_pow, Mem->CP_kickable_buffer)) {
      DebugDrib(printf("Dribble: looks bad for dash, probably need to go to intercept"));
      LogAction2(70, "dribble: lost ball it seems");
      return com;
    } else
      DebugDrib(printf("Dribble: It's okay, ball will be kickable soon\n"));
  }

  if (Mem->WillDashBeCollision(max_pow, Mem->CP_collision_buffer)) {
    DebugDrib(printf("Dribble: dash would be a collision, so we'll kick instead\n"));
    if (Mem->BallKickable()) {
      LogAction2(70, "dribble: dash would collide, so we kick");
      return dribble_kick(vEndPos, max_pow, drib_ang, mode);
    } else {
      /* we'll fall through to a dash-
	 that'll at least get us close to the ball */
      DebugDrib(printf("Dribble: dash would be collision, but we can't kick the ball?"));
      LogAction2(70, "dribble: dash would collide, but can't kick!");
    }
  }
  
  //SMURF: need to plan for end point
  /* it might be a good idea to plan for the point where we want to stop
     dribbling so we don't overshoot and/or lose the ball when we stop */

  LogAction2(80, "dribble: doing a dash");
  DebugDrib(printf("Dribble: doing a basic dash\n"));
  com.time = Mem->CurrentTime;
  com.type = CMD_dash;
  com.power = max_pow;
  return com;
}

/* returns a kick that would put that would (assuming we dash next cycle)
   leave us with the ball at the correct angle after the next cycle */
TurnKickCommand GetDribbleKickCom(Vector vMyPred, AngleDeg drib_ang)
{
  Vector vBallTarg;

  LogAction2(90, "dribble: getting the kick command");
  
  if (!Mem->BallKickable())
    my_error("GetDribbleKickCom: Ball isn't kickable\n");
  
  vBallTarg = vMyPred +
    Polar2Vector(Mem->CP_dribble_ball_dist, drib_ang + Mem->MyBodyAng());

  vBallTarg = Mem->Global2RelativeToMyBody(vBallTarg);
  Vector vBallTraj = vBallTarg - Mem->BallRelativeToBodyPosition();
  /*vBallTraj = vBallTraj.rotate(-Mem->MyAng());*/

  return dokick(vBallTraj.dir(), vBallTraj.mod() / (1 + Mem->SP_ball_decay));
}

/* we may need to do a turnkick if the ball gets stuck behind us for some
   reason */
TurnKickCommand GetDribbleTurnKickCom(AngleDeg drib_ang)
{
  TurnKickCommand com;

  LogAction2(90, "dribble: getting the turnkick command");
  
  if (!Mem->BallKickable())
    my_error("GetDribbleTurnKickCom: Ball isn't kickable\n");
  
  DebugDrib(printf("Dribble: Having to do turnball\n"));
  TurnDir rot = RotToAvoidOpponent(drib_ang + Mem->MyBodyAng());
  DebugDrib(printf("Dribble: rot dir is %d\n", rot));
  KickToRes res = turnball_kick(drib_ang, rot, FALSE, &com);
  if (res == KT_LostBall)
    my_error("Dribble: turning lost the ball");
  else if (res == KT_Success || res == KT_DidNothing)
    {
      /* if we don;t know where we want to turn, just turn to in front of us
	 so that our next kick won;t be a collision */
      res = turnball_kick(0, TURN_CLOSEST, FALSE, &com);
      if (res == KT_Success || res == KT_DidNothing)
	my_error("Dribble: TurnKick finished in dribble, so we shouldn't have doen it");
    }  
  return com;
}


/* figures out a kick to do to keep dribbling
   (see the dribble_kick comment).
   Checks for collisions and calls turnkick if applicable */
/* the DribbleMode determines what to do when the kick we want to do is a
   collision. In strict mode, we always stop and turnball.
   In lazy mode, we try to get halfway to the desired angle and if that doesn't
   work, then we turnball */
/* lazy mode has been tested and works, but Strict mode has not. It seems to
   lead to many lost balls */
TurnKickCommand dribble_kick(Vector vEndPos, float max_pow,
			     AngleDeg drib_ang, DribbleMode mode)
{
  DebugDrib(cout << "in dribble_kick" << endl);
  if (!Mem->BallKickable())
    my_error("dribble_kick: Ball isn't kickable\n");
  
  TurnKickCommand com;
  /* figure out the point to kick the ball to */
  /* the 1 at the end is an idle cycle */
  Vector vMyPred = Mem->MyPredictedPosition(2, max_pow, 1);

  if ((vMyPred - Mem->MyPos()).mod() > (vEndPos - Mem->MyPos()).mod()) {
    /* if we are going to go far enough to get to our endpos, we may not want
       to dash as far */
    vMyPred = vEndPos;
  }
  
  com = GetDribbleKickCom(vMyPred, drib_ang);

  if (Mem->WillKickBeCollision(com.power, com.angle, Mem->CP_collision_buffer)) {
    if (mode == DM_Strict && fabs(Mem->BallAngleFromBody() - drib_ang) > 20 ) {      
      //	(mode == DM_Lazy && signf(Mem->BallAngle()) == signf(drib_ang))) {
      /* we'll turnball instead */
      DebugDrib(printf("In strict mode, doing turnball\n"));
      LogAction2(80, "dribble: strict mode, turnballing");
      com = GetDribbleTurnKickCom(drib_ang);
    } else if (mode == DM_Lazy) {
      //SMURF: should we average, or send to 0???
      //com = GetDribbleKickCom(vMyPred, 0);
      DebugDrib(printf("Dribble: In lazy mode, trying to interpolate changeover\n"));
      com = GetDribbleKickCom(vMyPred,
			      (Mem->BallAngleFromBody() + drib_ang)/2);
      if (Mem->WillKickBeCollision(com.power, com.angle, Mem->CP_collision_buffer)) {
	/* still didn't work, use turnball! */
	DebugDrib(printf("Dribble: In lazy mode, didn't work\n"));
	LogAction2(80, "dribble: lazy mode, but still have to turnball");
	com = GetDribbleTurnKickCom(drib_ang);
      } else {
	LogAction2(80, "dribble: lazy mode, averaging kick");
      }
    } else
      my_error("Dribble: mode is bad!");    
    
  } else {
    DebugDrib(printf("Dribble: doing a kick\n"));
    LogAction2(80, "dribble: doing a normal kick");
  }
  
  return com;
}


DribbleRes DribbleTo(Vector vEndPos, float max_dash_pow, float buffer,
		     AngleDeg drib_ang, DribbleMode mode,
		     Bool IsDodge, Vector DodgePoint)
{
  AngleDeg targAng;
  AngleDeg targAngErr = Mem->CP_max_go_to_point_angle_err;
  
  if (!Mem->BallPositionValid()) {
    my_error("must know where ball is to dribble");
    return DR_Error;
  }  

  /* first check velocity confidence */
  if (!Mem->BallVelocityValid()) {
    my_error("Shouldn't call dribble with ball velocity invalid");
    return DR_Error;
    /*LogAction2(60, "Dribble: ball velocity not valid");
    face_only_neck_to_ball();
    stop_ball();
    return DR_Going; */
  }

  DebugDrib(cout << "MyPos: " << Mem->MyPos() << "\tMyAng: " << Mem->MyAng() << endl);
  DebugDrib(cout << "BallPos: " << Mem->BallAbsolutePosition() << endl);
  DebugDrib(cout << "BallDist: " << Mem->BallDistance() << endl);
  DebugDrib(cout << "BallVel: " << Mem->BallAbsoluteVelocity() << endl);
  DebugDrib(cout << "vEndPos: " << vEndPos << "\tdrib_ang: " << drib_ang << endl);

  LogAction6(60, "DribbleTo: pow %.1f to (%.1f, %.1f) at ang %.2f",
		  max_dash_pow, vEndPos.x, vEndPos.y, drib_ang);
  
  if (mode == DM_None)
    my_error("Can't pass DM_None as a mode to DribbleTo");
  
  if ((Mem->MyPos() - vEndPos).mod() < buffer) {
    DebugDrib(printf("Got to target!\n"));
    LogAction2(80, "dribble: I got to target!");
    return DR_GotThere;
  }
  
  
  if (IsDodge) {
    /* there is something we need to dodge */
    /* we'll dodge whatever side of us the ball was supposed to go */
    Vector disp = vEndPos - Mem->MyPos();
    //DebugDrib(cout << "Disp1: " << di	sp << endl);
    disp *= Mem->CP_dodge_distance_buffer / disp.mod();
    //DebugDrib(cout << "Disp2: " <<	 disp << endl);
    disp = disp.rotate(signf(drib_ang) * 90);
    DebugDrib(cout << "Disp: " << disp << endl);
    if (Mem->DistanceTo(DodgePoint) < Mem->CP_dribble_dodge_close_dist) {
      DebugDrib(printf("Dribble: sharp dodge\n"));
      vEndPos = Mem->MyPos() + disp;
    } else {
      DebugDrib(printf("Dribble: regular dodge\n"));
      vEndPos = (DodgePoint + disp - Mem->MyPos())*2 + Mem->MyPos();      
    }
    DebugDrib(cout << "We're dodging a player: disp: " << disp
	      << "\tvEndPos: " << vEndPos << endl);
    LogAction2(80, "dribble: dodging a player");
    targAngErr = Mem->CP_dribble_dodge_angle_err;
  }    

  targAng = Mem->AngleToFromBody(vEndPos);
  DebugDrib(printf("targAng: %f\n", targAng));
  TurnKickCommand com;  

  /* if the mode is strict and then ball is way off,
     then go ahead and turnball */
  if (mode == DM_Strict &&
      fabs(Mem->BallAngleFromBody() - (targAng + drib_ang)) >
      Mem->CP_dribble_exp_angle_buffer &&
      Mem->BallKickable()) {
    com = GetDribbleTurnKickCom(drib_ang + targAng);
    if (DoTurnKickCommand(com))
      return DR_Going;
    else
      return DR_Error;
  }
  
  /* first turn the right way */
  if (fabs(targAng) > targAngErr) {
    DebugDrib(printf("Want to turn\n"));
    if (!Mem->BallWillBeKickable(1, 0, Mem->CP_kickable_buffer) &&
	!Mem->BallWillBeKickable(2, 0, 0))
      if (Mem->BallKickable()) {
	DebugDrib(printf("Ball will not be kickable, but it is now, so we're kicking\n"));
	LogAction2(80, "dribble: stopping the ball");
	com = dokick(0,0);
      } else {
	DebugDrib(printf("Ball will not be kickable, trying to catch up\n"));
	LogAction2(70, "dribble: trying to catch up to ball");
	com = dribble_dash(vEndPos, max_dash_pow, drib_ang, mode);
	if (com.time != Mem->CurrentTime)
	  return DR_LostBall;  
      }    
    else {      
      DebugDrib(printf("Dribble: turning to the right angle %f %f\n", targAng, Mem->MyAng()));
      LogAction2(80, "dribble: turning the right way");
      com.time = Mem->CurrentTime;
      com.type = CMD_turn;
      com.angle = targAng;
      com.turn_neck = FALSE;
    }    
    if (DoTurnKickCommand(com))
      return DR_Going;
    else
      return DR_Error;
  }

  if (Mem->BallKickable()) {
    AngleDeg expBallAngle;
    expBallAngle = (Mem->BallPredictedPosition() -
		    Mem->MyPredictedPosition(1, max_dash_pow)).dir();
    expBallAngle -= Mem->MyBodyAng();
    NormalizeAngleDeg(&expBallAngle);
    DebugDrib(printf("Expected ball angle: %f\n", expBallAngle));
    if (fabs(expBallAngle-drib_ang) > Mem->CP_dribble_exp_angle_buffer ||
	fabs(expBallAngle) > Mem->CP_dribble_behind_angle) {      
      /* kick if the ball will end up behind us */
      DebugDrib(printf("The ball will be too far off it's target, so kicking\n"));
      LogAction3(70, "dribble: kicking because ball will be behind us: %f", 
		 expBallAngle);
      com = dribble_kick(vEndPos, max_dash_pow, drib_ang, mode);
    } else {      
      /* otherwise dash */
      DebugDrib(printf("Going to try to do a dash\n"));
      LogAction2(70, "dribble: trying to dash");
      com = dribble_dash(vEndPos, max_dash_pow, drib_ang, mode);
    }    
  } else {
    DebugDrib(printf("Ball is not kickable, better dash\n"));
    LogAction2(70, "dribble: trying to dash because ball not kickable");
    com = dribble_dash(vEndPos, max_dash_pow, drib_ang, mode);
  }
  
  if (com.time != Mem->CurrentTime)
    return DR_LostBall;
  
  if (DoTurnKickCommand(com))
    return DR_Going;
  else
    return DR_Error;
}

DribbleRes SimpleDribbleTo(Vector vEndPos, float max_dash_pow, float buffer)
{
  return DribbleTo(vEndPos, max_dash_pow, buffer, Mem->CP_dribble_angle_norm,
		   DM_Lazy, FALSE, Vector(0,0));
  
}



AngleDeg GetNormalizeDribAng(AngleDeg ang)
{
  NormalizeAngleDeg(&ang);
  if (ang <= -90)
    ang = -90;
  if (ang >= 90)
    ang = 90;
  return ang;
}

AngleDeg GetNormalizeDribAngWithBuffer(AngleDeg ang)
{
  NormalizeAngleDeg(&ang);
  if (fabs(ang) > 180-Mem->CP_dribble_angle_ignore_buffer)
    /* we want the ball right in back, so use the side the ball is on */
    ang = signf(Mem->BallAngleFromBody())*90; 
  else 
    ang = GetNormalizeDribAng(ang);
  return ang;
}


