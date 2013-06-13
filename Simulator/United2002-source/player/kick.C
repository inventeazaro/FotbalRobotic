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
#include "test.h"
#include "behave.h"

#ifdef DEBUG_OUTPUT
#define DebugKick(x) 
#define DebugKick2(x)
#else
#define DebugKick(x) 
#define DebugKick2(x) 
#endif

/* these PatsTest_* functions are just that.
   They are temporary function that can be used to see some specific kicking
   behavior.
   What they do isn't documented anywhere but the source code below */

void PatsTest_static()
{
  DebugKick(printf("\nTime: %d\n", Mem->CurrentTime.t));  
  if (!Mem->MyConf())
    scan_field_with_body();
  else if (!Mem->BallPositionValid())
    face_neck_to_ball();
  else {
    if (Mem->CurrentTime.t % 100 < 10 && Mem->BallKickable())
      smart_kick_hard_abs(45, KM_Moderate);
    else {
      Bool res = go_to_static_ball(45);
      cout << "result: " << res << endl;
    }    
  }  
}


void PatsTest_conv(void)
{
  Vector g, r, g2;
  for (float i=-10.0; i<= 10.0; i+= 1.0) {
    for (float j=-10.0; j<= 10.0; j+= 1.0) {
      g = Vector(i,j);
      r = g.Global2Relative(-3.0, 37);
      g2 = r.Relative2Global(-3.0, 37);
      if (fabs(g.x - g2.x) > FLOAT_EPS ||
	  fabs(g.y - g2.y) > FLOAT_EPS)
	printf("Error i:%f\tj: %f\n", i, j);
    }
  }
  exit(1); 
}


void PatsTest_turn(void)
{
  KickToRes res;
  static int ang=180;
  static int wait_count = 0;

  /* SMURF - there is something wrong with Time class!
  if (Mem->CurrentTime - Mem->LastActionTime < Mem->CP_kick_time_space)
    return;
    */
  //TurnKickCommand com;
  if (Mem->CurrentTime.t % 100 < 20)
    return;
  
  res = KT_None;
  if (Mem->BallKickable()) {
    
    if (wait_count <= 0) {      
      DebugKick(printf("About to call turnball_kick\n"));
      res = TurnballTo((AngleDeg)ang - Mem->MyBodyAng(), TURN_CLOSEST);
    } else {
      DebugKick(printf("waiting...\n"));
      wait_count--;
    } 
    
  } else {
  } 

    
  if (res == KT_LostBall) 
    return;
  else if (res == KT_Success) {
    DebugKick(printf("SUCESS-play_with_ball!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
    DebugKick(printf("ang: %d", ang));
    ang = (90 + ang) % 360;
    DebugKick(printf("new ang: %d\n", ang));
  } else if (res == KT_DidNothing)
    return;
  return;

} 

void PatsTest_kick()
{
  if (!strcmp(Mem->MyTeamName, "CMUnited")) {
    //float targ_vel;
    Vector pass_targ;
  
    fflush(stdout);

    change_view(VW_Narrow);
    
    if (!Mem->BallKickable()) {
      //scan_field();
      return;
    } 

    /*    if((int)(Mem->CurrentTime.t) % 50 < 5 ) {
      return; 
    }*/

  DebugKick(printf("\nTime: %d\n", Mem->CurrentTime.t));  

  /*
  targ_vel = 1.0 + (Mem->CurrentTime.t / 50)*.1;
  printf("Target vel is: %f\n", targ_vel);*/
    pass_targ = Vector(-20, -20 + (Mem->CurrentTime.t / 50)*5);
    //cout << "Pass Target: " << pass_targ << endl;
  
  //DebugKick(printf("the (simulated) ball velocity is: %g\n",) 
	   //	 Mem->BallAbsoluteVelocity().mod());
  /*
  DebugKick(printf("My angle: %f\n", Mem->MyAng()));
  if (fabs(Mem->MyAng() - 90) > 5) {
    DebugKick(printf("Turning to 90\n"));
    //turn(90-Mem->MyAng());
    turn(Mem->MarkerAngle(Mem->RM_RC_Flag));
    return;
  }
  */
    if (Mem->BallKickable()) {
      /*if (step == 4)
	if (step == 3) {
	step = 0; 
	turn(90 - Mem->MyAng());
	} 	*/
      smart_kick_hard_abs(180, KM_Moderate);
      //smart_kick_hard_abs(180, KM_HardestKick);
      //smart_kick_hard_abs(180, KM_Moderate, targ_vel);
      //smart_pass(pass_targ);
    } 
  } else {
    static int FirstTime = TRUE;
    const int ydist = 2;
    //    if (Mem->MyNumber == 1)
    //      test_go_to_point(Vector(-.2, -1), .5, 50);
    if (!FirstTime)
      return;
    switch (Mem->MyNumber) {
    case 1: move(-.2, -1); break;
    case 2: move(-20,  ydist); break;
    case 3: move(-30, -ydist); break;
    case 4: move(-40,  ydist); break;
    case 5: move(-50, -ydist); break;
    }
    FirstTime = FALSE;
  }
  
  return;
}

void PatsTest_kick2()
{
  fflush(stdout);

  if (Mem->ViewWidth != VW_Wide)
    change_view(VW_Wide);
	  
  static int WasBallKickable;
  DebugKick(printf("Time: %d\n", Mem->CurrentTime.t));
  //DebugKick(printf(" Ball Distance: %f\n", Mem->BallDistance() ));
  //DebugKick(cout << " MyPos: " << Mem->MyPos() << endl));
  if (!Mem->BallPositionValid() || !Mem->BallKickable()) {
    WasBallKickable = FALSE;
    
    DebugKick(printf("chasing ball\n"));
    if (Mem->BallPositionValid()) {
      if (fabs(Mem->BallAngleFromBody()) > Mem->CP_KickTo_err) {
	DebugKick(printf("turning to face ball\n"));
	DebugKick(printf("the angle is: %f\n", Mem->BallAngle()));
	turn(Mem->BallAngleFromBody());
      } else {
	DebugKick(printf("dashing\n"));
	float power=Mem->BallDistance()*40;
	if (power>40) power = 40;
	dash(power);
      } 
    } else {
      DebugKick(printf("turning randomly\n"));
      turn(60);
    } 
    return;
  } 


  //DebugKick(printf("the (simulated) ball velocity is: %g\n",) 
	   //	 Mem->BallAbsoluteVelocity().mod());

  if (Mem->BallKickable()) {
    if (!smart_kick_hard_abs(0, KM_HardestKick))
      printf("test_kick_hard: UhOh, something bad happened\n");
    return;
  } 

  return;
}


int DoTurnKickCommand(TurnKickCommand com)
{
  if (com.time != Mem->CurrentTime) {
    my_error("DoTurnKickCommand- told to do command not set this cycle");
    return 0;
  }
    
  switch (com.type) {
  case CMD_none:
    break;
  case CMD_dash:
    DebugKick(printf("DoTurnKickCommand: dash\n"));
    dash(com.power);
    break;
  case CMD_turn:
    DebugKick(printf("DoTurnKickCommand: turn\n"));
    turn(com.angle);
    break;
  case CMD_kick:
    DebugKick(printf("DoTurnKickCommand: kick\n"));
    kick(com.power, com.angle);
    break;

  default:
    my_error("DoTurnKickCommand- unimplemented type!");
    return 0;
  }

  if (com.turn_neck) {
    turn_neck(com.turn_neck_angle);
  }
  return 1;  
}


/* decides if we can kick staright to the decired point around the player
   without a collision.
   (EndDist, dir) is a relative polar vector for the ball's final position
   closeMarg is what the radius of the player is considered to be */
int is_straight_kick(float dir, float EndDist, float closeMarg)
{
  Vector btraj = Polar2Vector(EndDist, dir) -  Mem->BallRelativeToBodyPosition();
  float ang;
  float dist;
  int res;

  DebugKick(printf("    isStriaght ball abs pos mod: %f\t dir: %f\n",
	 Mem->BallAbsolutePosition().mod(), Mem->BallAbsolutePosition().dir()));
  DebugKick(printf("    isStriaght ball rel pos mod: %f\t dir: %f\n",
	 Mem->BallRelativePosition().mod(), Mem->BallRelativePosition().dir()));
  DebugKick(printf("    isStriaght btraj mod: %f\t dir: %f\n", btraj.mod(), btraj.dir()));
  /* Apply the law of cosines to the anle formed by the player's center(A),
     the ball's current position(B), and the ball target position(C).
     The angle calculated is ABC */
  ang = ACos( (Sqr(EndDist) - Sqr(btraj.mod()) -
	       Sqr(Mem->BallDistance()))/
	      (-2 * Mem->BallDistance() * btraj.mod()) );
  DebugKick(printf("   isStraight ang: %f\n", ang));
  if (fabs(ang) > 90) {
    DebugKick(printf("    isStraight: Obtuse!\n"));
    LogAction2(120, "is_straight_kick: obtuse angle");
    return 1; /* obtuse angle implies definately straight */
  }
  /* get the height of the triangle, ie how close to the player the ball will
     go */
  dist = Sin(ang) * Mem->BallDistance();
  DebugKick(printf("   isStraight dist: %f\n", dist));
  LogAction3(120, "is_straight_kick: %f", dist);
  res = (fabs(dist) > closeMarg);
  return ( res );  
} 

/* picks a rotation (CW or CCW) to turnball based on the nearest opponent or
   teamless player */
TurnDir RotToAvoidOpponent(float abs_dir)
{
  TurnDir rot;
  float dist = HUGE;
  AngleDeg opp_ang;
  Unum opp = Mem->ClosestOpponent();
  AngleDeg ball_ang = Mem->BallAngleFromBody();
  if (opp != Unum_Unknown) {
    dist = Mem->OpponentDistance(opp);
    opp_ang = Mem->OpponentAngleFromBody(opp);
  }
  if (Mem->NumTeamlessPlayers() > 0) {
    Vector pos = Mem->ClosestTeamlessPlayerPosition();
    float d = Mem->DistanceTo(pos);
    if (d < dist) {
      dist = d;
      opp_ang = Mem->AngleToFromBody(pos);
    }
  }
  if (dist < Mem->CP_turnball_opp_worry_dist) {
    /* there is an opponent near enough to worry about */
    DebugKick(printf("In RotToAvoidOpponent, avoiding opponent\n"));
    AngleDeg ball_to_targ = GetNormalizeAngleDeg((abs_dir - Mem->MyBodyAng()) - ball_ang);
    AngleDeg opp_to_targ = GetNormalizeAngleDeg((abs_dir - Mem->MyBodyAng()) - opp_ang);

    if ( ball_to_targ * opp_to_targ < 0 ){ /* they're on opposite sides of the target */
      LogAction2(120, "RotToAvoidOpponent: CLOSEST");
      rot = TURN_CLOSEST;
    }
    else if ( ball_to_targ < opp_to_targ ){
      LogAction2(120, "RotToAvoidOpponent: CW");
      rot = TURN_CW;
    }
    else{
      LogAction2(120, "RotToAvoidOpponent: CCW");
      rot = TURN_CCW;
    }
  } else {
    LogAction2(120, "RotToAvoidOpponent: no opponents close enough");
    rot = TURN_CLOSEST;
  }
  
  return rot;
}

/* Picks the rotation direction (CW or CCW) such that the ball has to travel
   the least distance */
TurnDir RotClosest(float abs_dir)
{
  AngleDeg cw_ang = (abs_dir - Mem->MyBodyAng()) - Mem->BallAngleFromBody();
  AngleDeg ccw_ang = Mem->BallAngleFromBody() - (abs_dir - Mem->MyBodyAng());
  DebugKick(printf("Test1: cw_ang: %f\tccw_ang: %f\n", cw_ang, ccw_ang));
  if (cw_ang < 0) cw_ang += 360;
  if (ccw_ang < 0) ccw_ang += 360;
  DebugKick(printf("Test2: cw_ang: %f\tccw_ang: %f\n", cw_ang, ccw_ang));
  if (cw_ang < ccw_ang)
    return TURN_CW;
  else
    return TURN_CCW;  
}

  

/* Kick with direction ddir and power such that the ball moves distance ddist
   If ddist is too big, kick it as hard as possible.
   corrects for ball velocity
   Returns a command object partially filled out
   The distance is multiplied by the distFactor */
/* returns KickCommand.time = -1 for error */
TurnKickCommand dokick(AngleDeg ddir, float ddist, float distFactor,
		       Bool* pCanKickToTraj) 
{
  float v0;
  float power;
  float kick_dir = ddir;
  TurnKickCommand com;
  com.time = -1;
  com.type = CMD_kick;
  com.turn_neck = FALSE;
  if (pCanKickToTraj)
    *pCanKickToTraj = TRUE;

  LogAction6(110, "dokick: %.2f (%.2f) at %.2f, rate %.5f",
		  ddist, distFactor, ddir, Mem->BallKickRate());
  
  v0 = ddist * distFactor;

  NormalizeAngleDeg(&ddir);

  DebugKick(printf(" dokick: ddir: %f\tv0: %f\n", ddir, v0));
  DebugKick(printf(" kickrate: %f\tmyang: %f\n", Mem->BallKickRate(), Mem->MyAng() ));

  if (Mem->BallKickRate() == 0.0)
    my_error("dokick: Huh? BallKickRate is 0!");

  if (!Mem->BallVelocityValid()) {
    DebugKick(printf("In dokick with velocity not valid. Assuming it's 0."));
    LogAction2(130, "dokick: assuming vel 0");
  }
  
  if (Mem->BallVelocityValid() &&
      Mem->BallAbsoluteVelocity() != 0) { /* correct for ball velocity */
    Vector tmp = Polar2Vector(v0, ddir) - Mem->BallRelativeToBodyVelocity();
    kick_dir = tmp.dir();
    power = tmp.mod() / Mem->BallKickRate();
    DebugKick(printf(" Correcting for ball velocity# vel.x: %f\tvel.y:%f\n",
	  Mem->BallRelativeVelocity().x, Mem->BallRelativeVelocity().y));
    DebugKick(printf(" New stuff# power: %f\t ddir:%f\n", power, kick_dir));
    LogAction4(130, "dokick: vel corr: %f at %f", power, kick_dir);
  } else {
    LogAction2(130, "dokick: vel is 0");
    power = v0  / Mem->BallKickRate();
  }
  

  if (power > Mem->SP_max_power) {
    DebugKick(printf("Trying to kick over SP_max_power! Correcting...\n"));
    //LogAction2(130, "dokick: trying to kick too hard, correcting");
    if (!Mem->BallVelocityValid() || Mem->BallAbsoluteVelocity() == 0) {
      power = Mem->SP_max_power;
      LogAction4(130, "dokick: max_pow corr: (stopped ball) %f at %f",
		      power, kick_dir);
    } else if (ddist == 0) {
      /* this is probably a stop_ball kick, but ddir is meaningless for
	 the RayCircleIntersection below.
	 Therefore, we just kick against the ball's velocity */
      kick_dir = GetNormalizeAngleDeg(Mem->BallRelativeToBodyHeading() + 180);
      power = Mem->SP_max_power;
      LogAction4(130, "dokick: max_pow corr: (ddist = 0) %f at %f",
		      power, kick_dir);
    } else {
      /* this is a ray circle intersection problem
         We want to kick in the right direction, but not as hard as desired */
      Vector sol1, sol2;
      int numSol;
      Vector vNewTraj;
      numSol =
	RayCircleIntersect(Ray(Mem->BallRelativeToBodyPosition(), ddir),
			   Mem->SP_max_power * Mem->BallKickRate(),
			   Mem->Global2RelativeToMyBody(Mem->BallPredictedPosition()),
			   &sol1, &sol2);
      /* we want the solution that's furthest along the ray - that's sol2
	 if there are two solution */
      if (numSol == 0) {
	/* we can't kick ball to desired trajectory, so let's just slow it
	   down by kicking directly against velocity
	   It might be better to return an error and have the caller decide
	   what to do, but this works pretty well */
	DebugKick(printf("Can't kick ball to right trajectory!\n"));
	power = Mem->SP_max_power;
	kick_dir = Mem->BallRelativeToBodyHeading() + 180;
	if (pCanKickToTraj)
	  *pCanKickToTraj = FALSE;
	LogAction4(130, "dokick: max_pow corr: can't get to trajectory! %.2f at %.2f",
			power, kick_dir);
      } else {
	if (numSol == 1) {
	  vNewTraj = sol1;
	} else if (numSol == 2) {
	  /* we want the solution closer to the target point we wanted */
	  Vector targ = Mem->BallRelativeToBodyPosition() + Polar2Vector(v0, ddir);
	  LogAction8(130, "dokick: 2 solutions (%.2f, %.2f) (%.2f, %.2f) targ (%.2f, %.2f)",
			 sol1.x, sol1.y, sol2.x, sol2.y, targ.x, targ.y);
	  if (sol1.dist2(targ) < sol2.dist2(targ)) {
	    LogAction4(140, "Picked sol1: %.2f < %.2f",
			    sol1.dist2(targ), sol2.dist2(targ));
	    vNewTraj = sol1;
	  } else {
	    LogAction4(140, "Picked sol2: %.2f > %.2f",
			    sol1.dist2(targ), sol2.dist2(targ));	    
	    vNewTraj = sol2;
	  }	  
	} else {
	  my_error("dokick: How many solutions to RayCircleIntersection? %d", numSol);
	}
	
	Vector vNewKick = vNewTraj -
	  Mem->Global2RelativeToMyBody(Mem->BallPredictedPosition());
	power = vNewKick.mod() / Mem->BallKickRate();
	kick_dir = vNewKick.dir();

	DebugKick(printf(" Correcting for ball velocity AND max power# vel.x: %f\tvel.y:%f\n",
	       Mem->BallRelativeVelocity().x, Mem->BallRelativeVelocity().y));
	DebugKick(printf("New stuff# power: %f\t dir:%f\n", power, kick_dir));
	LogAction4(130, "dokick: max_pow corr: %f at %f", power, kick_dir);
      }
    }    
  } 

  power = Min(Round(power, -2), Mem->SP_max_power);
  kick_dir = Round(kick_dir, -2);
  NormalizeAngleDeg(&kick_dir);
  DebugKick(printf("kicking with power: %f at dir: %f\n", power, kick_dir));
  com.time = Mem->CurrentTime;
  com.power = power;
  com.angle = kick_dir;
  return com;
}

/* all the turnball reasoning is done in relative (to body) coordinates
   call this before calling dokick to correct for movement of the player */
void PlayerMovementCorrection(AngleDeg* pdir, float* pdist)
{
  LogAction4(110, "Player movement correction: before dist: %f at dir: %f",
		 *pdist, *pdir);
  DebugKick(printf("Before corr- dir: %f\t dist: %f\n", *pdir, *pdist));
  Vector vRelBallTarg = Mem->BallRelativeToBodyPosition() +
    Polar2Vector(*pdist, *pdir);
  Vector vNewRelTarg = Mem->MyPredictedPosition() +
    vRelBallTarg - Mem->MyPos();
  Vector vNewRelTraj = vNewRelTarg - Mem->BallRelativeToBodyPosition();
  *pdir  = vNewRelTraj.dir();
  *pdist = vNewRelTraj.mod();
  DebugKick(printf("After corr- dir: %f\t dist: %f\n", *pdir, *pdist));
  LogAction4(110, "Player movement correction: after dist: %f at dir: %f",
		 *pdist, *pdir);
}


/* kick ball so that it is at angle ddir and dist EndDist
   If you have to kick around the player, kick rotway(clockwise or counter-)
   */
/* we always reason about the right trajectory for the ball leave velocity
   correction for dokick */
KickToRes turnball_kick(AngleDeg target_dir, TurnDir rotate, 
		  Bool StopBall, TurnKickCommand* pCom,
		  float EndDist, float closeMarg, float kickFac)
{
  float dir;
  float dist;
  Vector btraj;

  pCom->time = -1;
  pCom->turn_neck = FALSE;
  
  DebugKick(printf("\nat turnball_kick: target_dir: %f\n", target_dir));
  LogAction4(60, "Turnball_kick: targ_dir: %.1f  dir: %d", target_dir, (int)rotate);
  
  NormalizeAngleDeg(&target_dir);
  
  //DebugKick(printf("HERE Time: %d\n", Mem->CurrentTime.t));
  /* the pos valid is not really right - if we are turning the ball and didn't
     actually see it last time, then there's a problem */
  if ( !Mem->BallPositionValid() || !Mem->BallKickable() ) {
    LogAction2(90, "turnball_kick: lost the ball");
    return KT_LostBall;
  }

  /* if the velocity isn's valid, turn to face ball */
  if ( !Mem->BallVelocityValid() ) {
    float ball_ang_from_body = Mem->BallAngleFromBody(); /* for efficiency */
    LogAction2(90, "turnball_kick: vel is not valid, looking at it");
    DebugKick(printf("turning to face ball\n"));
    if (Mem->CanSeeBallWithNeck()) {
      pCom->time = Mem->CurrentTime;
      pCom->type = CMD_kick;
      pCom->angle = ball_ang_from_body + 180;
      pCom->power = Mem->CP_stop_ball_power;

      pCom->turn_neck = TRUE;
      pCom->turn_neck_angle = Mem->LimitTurnNeckAngle(Mem->BallAngleFromNeck());
    } else {
      /* turn body to face ball, and turn neck to straight ahead */
      pCom->time = Mem->CurrentTime;
      pCom->type = CMD_turn;
	pCom->turn_neck = TRUE;
      if (fabs(ball_ang_from_body) > Mem->MaxEffectiveTurn()) {
	/* out body can't get to where we want to go */
	pCom->angle = 180; /* get our maximum effective turn */
	pCom->turn_neck_angle = ball_ang_from_body -
	  signf(ball_ang_from_body)*Mem->MaxEffectiveTurn();
      } else {
	pCom->angle = ball_ang_from_body;
	pCom->turn_neck_angle = -Mem->MyNeckRelAng();
      }
      
    }
    
    return KT_TurnedToBall;
  } 
  
  DebugKick(printf(" ball.dist: %f\t.dir: %f\n",
	 Mem->BallDistance(), Mem->BallAngle()));
  DebugKick(printf(" HERE ball.vel.x: %f\t.y: %f\tmod: %f\n",
	 Mem->BallRelativeVelocity().x, Mem->BallRelativeVelocity().y,
	 Mem->BallSpeed()));
  DebugKick(printf(" ball.rpos.x: %f\t.y: %f\n",
	 Mem->BallRelativePosition().x, Mem->BallRelativePosition().y));
  DebugKick(printf(" target_dir: %f\n", target_dir));

  if ( fabs(GetNormalizeAngleDeg(target_dir - Mem->BallAngleFromBody())) < Mem->CP_KickTo_err) {
    /* Do something to indicate we are done */
    if (!StopBall || Mem->BallSpeed() < Mem->CP_max_ignore_vel)
      return KT_DidNothing;
    LogAction2(90, "turnball_kick: we're there, stopping the ball");
    DebugKick(printf("  Stop ball kick\n"));
    dir = 0;
    dist = 0;    
    PlayerMovementCorrection(&dir, &dist);
    *pCom = dokick(dir, dist, 1.0);
    pCom->turn_neck = FALSE;
    return KT_Success;
  }

  if (rotate == TURN_AVOID) {
    rotate = RotToAvoidOpponent(target_dir + Mem->MyBodyAng());
  }
  
  if (rotate == TURN_CLOSEST) {
    rotate = RotClosest(target_dir + Mem->MyBodyAng());
  }
  
  
  if (is_straight_kick(target_dir, EndDist, closeMarg)) {
    float pow;
      
    btraj = Polar2Vector(EndDist, target_dir) - Mem->BallRelativeToBodyPosition();
    dir = btraj.dir();
    dist = btraj.mod();
    
    /* now we're goign to do some distance twiddling to get the ball to
       get to the right angle and stop */
    pow = dist / Mem->BallKickRate();
    pow = Min(pow, Mem->CP_max_turn_kick_pow);
    dist = pow * Mem->BallKickRate();
      
    LogAction4(90, "turnball_kick: striaght kick: dist %f at %f", dist, dir);
    DebugKick(printf("  Straight kick# dir: %f dist: %f\n", dir, dist));
    PlayerMovementCorrection(&dir, &dist);
    *pCom = dokick(dir, dist, 1.0);
    pCom->turn_neck = FALSE;

  } else if (Mem->BallDistance() < closeMarg) {

    /* ball is too close to do a tangent kick, so do a kick at 90 degrees */
    dir = ((int)rotate)*(-90) + Mem->BallAngleFromBody();
    dist = 2.0*sqrt(Sqr(Mem->CP_opt_ctrl_dist) - Sqr(Mem->BallDistance()));
    LogAction2(90, "turnball_kick: 90 deg kick");
    DebugKick(printf("  Close kick# dir: %f dist: %f\n", dir, dist));
    PlayerMovementCorrection(&dir, &dist);
    *pCom = dokick(dir, dist, kickFac);
    pCom->turn_neck = FALSE;

  } else {

    /* do a turning kick */
    /* we make a circle around the player of radius closeMarg
       and calculate the trajectory that goes in the right direction and is
       tangent to the circle */
    dir = 180 + Mem->BallAngleFromBody() + ((int)rotate)*ASin(closeMarg / Mem->BallDistance());
    DebugKick(printf(" ball dist: %f\tclosest_margin: %f\n",
	   Mem->BallDistance(), closeMarg));
    dist = sqrt(Sqr(Mem->BallDistance()) - Sqr(closeMarg));
    dist +=
      sqrt(Sqr(Mem->CP_opt_ctrl_dist) - Sqr(closeMarg));
    DebugKick(printf("  Turning ball# dir: %f dist: %f\n", dir, dist));
    LogAction2(90, "turnball_kick: turning kick");
    PlayerMovementCorrection(&dir, &dist);
    *pCom = dokick(dir, dist, kickFac);
    pCom->turn_neck = FALSE;

  }

  return KT_DidKick;
}


KickToRes TurnballTo(AngleDeg rel_dir, TurnDir rotate)
{
  TurnKickCommand com;
  KickToRes res = turnball_kick(rel_dir, rotate, TRUE, &com);
  if (res == KT_Success || res == KT_DidKick || res == KT_TurnedToBall)
    DoTurnKickCommand(com);

  return res;
}

/*******************************************************************************************/

/* if we kick the ball as hard as possible in the right direction, will it
   be a collision with the player? */
/* all we have to do is look at the predicted ball position with that kick and
   see if it is within the player's radius of the player */
int is_hard_kick_coll(float abs_dir, TurnKickCommand* pcom,
		      Vector* pvPredBall, float targ_vel, Bool* pCanKickToTraj = NULL)
{
  DebugKick2(cout << "Is hard_kick_coll here" << endl);
  
  /* we used to use targ_vel, but we want to kick as hard as possible to get it through 
     the player if we can. If we want to kick it slower then we'll slow it down on the other
     side of us */
  /* However, that change caused bad behavior in KM_Quickly and KM_QuickestRelease, 
     so we undid it */
  *pcom = dokick(abs_dir - Mem->MyBodyAng(), targ_vel, 1.0, pCanKickToTraj);
  //*pcom = dokick(abs_dir - Mem->MyBodyAng(), Mem->SP_ball_speed_max, 1.0, pCanKickToTraj);
  *pvPredBall = Mem->BallPredictedPosition(1, pcom->power, pcom->angle);
  DebugKick(cout << "IsColl: PredBall: " << *pvPredBall
	    << "\tMyPos: " << Mem->MyPredictedPosition() << endl);
  DebugKick(cout << "diff: " << (*pvPredBall - Mem->MyPredictedPosition()).mod()
	    << "\tmarg: " << Mem->CP_hard_kick_margin << endl);
  return (*pvPredBall - Mem->MyPredictedPosition()).mod() <=
    Mem->My_size() + Mem->CP_hard_kick_dist_buffer;
}

/* Used when we decide a kick in the right direction woudl be a collision,
   so we need to turnball to kick the ball more to the side of us,
   OR, when in KM_HardestKick, moving the ball back for a kick */
TurnKickCommand hard_kick_turnball(float abs_dir, TurnDir rot, Bool StopBall = FALSE)
{
  TurnKickCommand com;
  //TurnDir rot = RotToAvoidOpponent(abs_dir);
  /* SMURF - should this have a larger dokick_factor? */
  LogAction3(70, "hard_kick_turnball: %f", abs_dir);
  DebugKick2(cout << "Doing a hard_kick_turnball" << endl);
  KickToRes res = turnball_kick(abs_dir - Mem->MyBodyAng(), rot, StopBall,
				&com, Mem->CP_hardest_kick_ball_dist,
				Mem->CP_closest_margin, Mem->CP_dokick_factor);
  if (res == KT_DidNothing || res == KT_LostBall)
    my_error("	hard_kick_turnball: Something weird happened: %d", res);
  return com;
}

TurnKickCommand kick_hard_moderate(AngleDeg abs_dir, float targ_vel, TurnDir rot)
{
  /* Here's the idea:
       See if one strong kick will be a collision (if it is, turnball)
       o.w. manipulate our kick so that we get another one next cycle
       (but make sure the final velocity is higher)
       it that makes it a collision, just do the strong kick */
    /* there is no reasning about turning the ball backward to get max power.
       See KM_HardestKick for that */
  LogAction4(60, "kick_hard_moderate, ang %f with targvel %f", abs_dir, targ_vel);
  DebugKick2(cout << endl << "Time: " << Mem->CurrentTime.t
	     << "\tkick_hard_moderate called" << endl);
  TurnKickCommand kickCom;
  TurnKickCommand HKCommand;
  Vector vPredBall;
  Bool CanKickToTraj;
  if (is_hard_kick_coll(abs_dir, &kickCom, &vPredBall, targ_vel, &CanKickToTraj)) {
    DebugKick2(cout << " Moderate: collision, still thinking about it" << endl);
    if (Mem->BallDistance() < Mem->My_size() + Mem->CP_hard_kick_dist_buffer) {
      LogAction2(70, "kick_hard_moderate: ball is too close to plan for collision ");
      HKCommand = hard_kick_turnball(abs_dir, rot);
      if (HKCommand.time == Mem->CurrentTime)
	return HKCommand;
      else {
	LogAction2(70, "kick_hard_moderate: I'm using the original kick commands, even though it's a collision");
	return kickCom;
      }
    } else {
      if (CanKickToTraj) {
	LogAction2(70, "kick_hard_moderate: planning for collision ");

	/* figure out what kick it would be to just go to the edge of the player */
	Vector sol1, sol2;
	int numSol =
	  RayCircleIntersect(Ray(Mem->BallAbsolutePosition(),
				 vPredBall - Mem->BallAbsolutePosition()),
			     Mem->My_size() + Mem->CP_hard_kick_dist_buffer,
			     Mem->MyPredictedPosition(),
			     &sol1, &sol2);
	if (numSol == 0) {
	  my_error("Woah! No solution to kicking a ball less hard!");
	  return hard_kick_turnball(abs_dir, rot);      
	}
	Vector vBallTarg = sol1;
	/* SMURF: is there any case where we wouldn't want to do this kick? */
	DebugKick2(cout << "Trying to kick to edge of player" << endl);
	Vector vBallPath = vBallTarg.Global2Relative(Mem->BallAbsolutePosition(), Mem->MyBodyAng());
	HKCommand = dokick(vBallPath.dir(), vBallPath.mod());
	return HKCommand;
      } else {
	/* can't kick to trajectory and it's a collision.
	   That's fine! Just do the kick */
	LogAction2(70, "kick_hard_moderate: collision, but that's okay!");
	return kickCom;
      }
    }
  } else {
    DebugKick(cout << " MyPos: " << Mem->MyPos() << endl);
    DebugKick(cout << " vPredBall: " << vPredBall << endl);
    Vector vPredBallRel =
      vPredBall.Global2Relative(Mem->MyPredictedPosition(), Mem->MyBodyAng());
    DebugKick(cout << " vPredBallRel: " << vPredBallRel << endl);
    if (vPredBallRel.mod() < Mem->My_kickable_area()-Mem->CP_hard_kick_dist_buffer ||
	!Mem->BallWillBeKickable()) {
      /* we're going to get another kick next time or this will be our last
	   kick anyway - do the strong one! */
      LogAction2(70, "kick_hard_moderate: strong with another or last kick");
      DebugKick2(cout << " Moderate: strong with another kick or last kick!" << endl);
      return kickCom;
    }
    DebugKick2(cout << " Moderate: deciding whether to do one or two kicks" << endl);
    /* we're goign to set vBall to be the relative vector (to new pos)
	 to the position that will give us another kick next time */
    float oneKickVel = Min(Mem->SP_ball_speed_max,
			   (vPredBall - Mem->BallAbsolutePosition()).mod());
    float twoKickVel = -10.0;
    Vector sol1, sol2;
    int numSol;
    Vector vBallTarg;
    numSol =
      RayCircleIntersect(Ray(Mem->BallAbsolutePosition(),
			     vPredBall - Mem->BallAbsolutePosition()),
			 Mem->My_kickable_area() - Mem->CP_hard_kick_dist_buffer,
			 Mem->MyPredictedPosition(),
			 &sol1, &sol2);
    /* we want the solution that's furthest along the ray - that's sol2
	 if there are two solution */
    if (numSol != 0) {
	
      if (numSol == 2)
	vBallTarg = sol2;
      else
	vBallTarg = sol1;

      /* see if the first of the two kicks is a coll */
      if ((vBallTarg - Mem->MyPredictedPosition()).mod() >=
	  Mem->My_size() + Mem->CP_hard_kick_dist_buffer) {
	/* we can do it without collision */
	/* now see if this is actually goign to be better */
	vBallTarg =
	  vBallTarg.Global2Relative(Mem->BallAbsolutePosition(), Mem->MyBodyAng());
	float kickrate =
	  Mem->GetBall()->calc_kick_rate(vBallTarg.mod(), vBallTarg.dir());
	/* the first kick */
	//DebugKick( cout << "vBallTarg: " << vBallTarg << "\tBallAbsPos: " <<
	//	 Mem->BallAbsolutePosition() << endl );
	twoKickVel = (vBallTarg - Mem->BallRelativeToBodyPosition()).mod() *
	  Mem->SP_ball_decay;
	DebugKick(printf("  twoKickVel: first kick: %f\n", twoKickVel));
	/* the second kick */
	twoKickVel = Min(Mem->SP_ball_speed_max,
			 twoKickVel + kickrate * Mem->SP_max_power);
	DebugKick(printf("  oneKickVel: %f\ttwoKickVel: %f\n", oneKickVel, twoKickVel));
      } else
	my_error("kick_hard_moderate- no ray intersection?");	
    }
      
    /* remember if twoKick is a collision, then it's velocity will be < 0 */
    if (numSol == 0 || 
	twoKickVel < 0.0 ||
	(oneKickVel >= twoKickVel && oneKickVel <= targ_vel)) {
      /* do the one hard kick */
      LogAction2(70, "kick_hard_moderate: doing one hard kick");
      DebugKick2(cout << " Moderate- Doing one hard kick" << endl);
      return kickCom;
    } else {
      /* do the weaker kick */
      LogAction2(70, "kick_hard_moderate: doing first of two kicks");
      DebugKick2(cout << " Moderate- doing first of two kicks" << endl);
      //DebugKick(printf(" Predicted distance: %f\n",
      //		 (vBallTarg - Mem->MyPredictedPosition()).mod() ));
      //DebugKick(cout << " BallCurrPos: " << Mem->BallAbsolutePosition() << endl);
      HKCommand = dokick(vBallTarg.dir(), vBallTarg.mod());
      DebugKick(cout << " KickTraj: " << vBallTarg << endl);
      DebugKick(cout << " PredPos: " << Mem->MyPredictedPosition() << endl);
      return HKCommand;
    }
      
  }    
}


TurnDir KickRotationDirectionAbs(AngleDeg abs_ang, TurnDir rot)
{
  if (rot == TURN_AVOID) 
    rot = RotToAvoidOpponent(abs_ang);    
  if (rot == TURN_CLOSEST) 
    rot = RotClosest(abs_ang);
  if (rot != TURN_CW && rot != TURN_CCW)
    my_error("KickRotationDirection: bad TurnDir");

  return rot;
}


/* see above for description of KM_Moderate */
/* KM_HardestKick: Moves the ball to the side of us (relative to the kick
   direction) then uses KM_Moderate */
/* KM_Quickly, KM_QuickestRelease: get rid of the ball as fast as possible,
   will turnball if neccesary. KM_Quickly will turn to face the ball if it can
   so that we get a harder kick */
/* returns 1 if a kick actually done */
int smart_kick_hard_abs(float abs_dir, KickMode mode, float targ_vel,
			TurnDir rot)
{
  TurnKickCommand HKCommand;
  HKCommand.time = -1;
  HKCommand.turn_neck = FALSE;
  
  DebugKick(printf("\nsmart_kick_hard: Time: %d\n", Mem->CurrentTime.t));

  LogAction5(50, "smart_kick_hard_abs: mode = %d, angle = %.1f, targ_vel %.4f ",
	     mode, abs_dir, targ_vel);

  if (!Mem->BallPositionValid()) {
    my_error("smart_kick_hard called with ball position not valid");
    return 0;
  }

  if (!Mem->BallKickable()) {
    my_error("smart_kick_hard called with ball not kickable!");
    return 0;
  }
  
  /* if the velocity isn's valid, turn to face ball */

  if ( !Mem->BallVelocityValid() ) {
    float ball_ang_from_body = Mem->BallAngleFromBody(); /* for efficiency */
    LogAction2(60, "smart_kick_hard_abs: turning to face ball");
    DebugKick2(printf("smart_kcik_hard: turning to face ball\n"));
    if (Mem->CanSeeBallWithNeck()) {
      if (Mem->PlayMode == PM_Play_On ||
	  (Mem->PlayMode == PM_My_Goal_Kick && Mem->LastAction->type == CMD_kick)) {
	LogAction2(70, "smart_kick_hard_abs: turning neck and stopping ball");
	HKCommand.time = Mem->CurrentTime;
	HKCommand.type = CMD_kick;
	HKCommand.angle = ball_ang_from_body + 180;
	HKCommand.power = Mem->CP_stop_ball_power;
	
	HKCommand.turn_neck = TRUE;
	HKCommand.turn_neck_angle = Mem->LimitTurnNeckAngle(Mem->BallAngleFromNeck());
      } else {
	LogAction2(70, "smart_kick_hard_abs: just turniong neck");
	HKCommand.time = Mem->CurrentTime;
	HKCommand.type = CMD_none;
	HKCommand.angle = 0;
	HKCommand.power = 0;
	
	HKCommand.turn_neck = TRUE;
	HKCommand.turn_neck_angle = Mem->LimitTurnNeckAngle(Mem->BallAngleFromNeck());
      }
    } else {
      /* turn body to face ball, and turn neck to straight ahead */
      LogAction2(70, "smart_kick_hard_abs: turning neck and body");
      HKCommand.time = Mem->CurrentTime;
      HKCommand.type = CMD_turn;
	HKCommand.turn_neck = TRUE;
      if (fabs(ball_ang_from_body) > Mem->MaxEffectiveTurn()) {
	/* out body can't get to where we want to go */
	HKCommand.angle = 180; /* get our maximum effective turn */
	HKCommand.turn_neck_angle = ball_ang_from_body -
	  signf(ball_ang_from_body)*Mem->MaxEffectiveTurn();
      } else {
	HKCommand.angle = ball_ang_from_body;
	HKCommand.turn_neck_angle = -Mem->MyNeckRelAng();
      }
    }
    return DoTurnKickCommand(HKCommand);
  } 

  LogAction2(210, "HardestKick: got past vel check");
  rot = KickRotationDirectionAbs(abs_dir, rot);
  LogAction2(210, "HardestKick: got past rot setting");

#ifdef NEVER
  /* With the new kick code, the ball goes through the player, 
     so we don't usually need to do a turn-ball as part of a shot */
  if (mode <= KM_Moderate &&
      Mem->IsPointBehind(Mem->BallAbsolutePosition(), abs_dir)) {
    /* see if we need to rotate one way */
    DebugKick(cout << "smart_kick_hard: decign if rotating " << rot << endl);
    /* now decide if ball is on the wrong side	 */
    if (Mem->IsPointBehind(Mem->BallAbsolutePosition(), abs_dir+((int)rot)*90)) {
      /* start rotating right way */
      DebugKick(cout << "smart_kick_hard: special turnball to avoid opp" << endl);
      TurnKickCommand com;
      KickToRes res = turnball_kick( abs_dir - Mem->MyAng(),
				     //abs_dir + ((int)rot)*90 - Mem->MyAng(),
				     rot, FALSE, &com);
      if (res == KT_DidNothing || res == KT_LostBall)
	my_error("smart_kick_hard: special turnball; turnkick failed, res: %d", res);
      else
	return DoTurnKickCommand(com);
    }
  }
#endif  
  
  
  switch (mode) {
  case KM_None:
    my_error("KM_None is not a valid kick mode for smart_kick_hard!");
    break;
    
    // HKStep meanings: 0 turning player, 1 moving ball, 2 using moderate
  case KM_HardestKick: {      
    if (Mem->CurrentTime - 1 != Mem->HKTime) {
      DebugKick(printf("First in a chain of calls\n"));
      LogAction2(210, "HardestKick: Setting up for first call");
      /* this is the first in a chain of calls */      
      Mem->HKStep = 0;
      /* decide which way to rotate ball */
      Mem->HKrot = rot;
      /*AngleDeg ang = Mem->BallAngle() + Mem->MyAng() - abs_dir;
      NormalizeAngleDeg(&ang);
      if (ang >= 0)
	Mem->HKrot = TURN_CW;
      else
	Mem->HKrot = TURN_CCW;*/
    } else {
      LogAction4(210, "HardestKick: Setting new step %d (old %d)", Mem->HKStepNext, Mem->HKStep);
      Mem->HKStep = Mem->HKStepNext;
    }
  

    if (Mem->HKStep == 0) {
      /* see if we need to turn */
      LogAction2(210, "HardestKick: processing for step 0");
      AngleDeg target_dir = abs_dir - Mem->MyBodyAng() + 
	((int)Mem->HKrot)*Mem->CP_hardest_kick_player_ang;
      NormalizeAngleDeg(&target_dir);
      LogAction2(210, "HardestKick: (0) norm angle");
      if (fabs(target_dir) > Mem->CP_max_hard_kick_angle_err) {
	LogAction2(210, "HardestKick: (0) need to turn!");
	LogAction3(210, "HardestKick: (0) BallVelValid: %f", Mem->BallVelocityValid());
	LogAction4(210, "HardestKick: (0) speed %f, %f!", 
		   Mem->BallSpeed(), Mem->CP_hard_kick_dist_buffer);
	LogAction3(210, "HardestKick: (0) will be kick %d", 
		   Mem->BallWillBeKickable(1,0,Mem->CP_hard_kick_dist_buffer));
	if (!Mem->BallWillBeKickable(1, 0, Mem->CP_hard_kick_dist_buffer) &&
	    Mem->BallVelocityValid() && 
	    Mem->BallSpeed() > Mem->CP_hard_kick_dist_buffer) {
	  LogAction2(70, "smart_kick_hard_abs: would turn for power, but must stop ball");
	  if (Mem->BallVelocityValid()) {
	    LogAction2(70, "smart_kick_hard_abs: would turn for power, but stopping ball");
	    HKCommand = dokick(-180, 0, 1.0);
	    if (HKCommand.time == -1) {
	      my_error("Stopping ball for dokick failed!");
	    }
	  } else {
	    LogAction2(70, "smart_kick_hard_abs: would turn for power, but stopping unknown vel ball");
	    HKCommand.time = Mem->CurrentTime;
	    HKCommand.type = CMD_kick;
	    HKCommand.angle = Mem->BallAngleFromBody() + 180;
	    HKCommand.power = Mem->CP_stop_ball_power;

	    HKCommand.turn_neck = TRUE;
	    HKCommand.turn_neck_angle = Mem->LimitTurnNeckAngle(Mem->BallAngleFromNeck());
	  }
	  //Mem->HKTime = Mem->CurrentTime;
	  LogAction2(210, "HardestKick: (0) setting step next to 0");
	  Mem->HKStepNext = 0; //stay in turn, we'll drop through next times
	} else {
	  LogAction4(70, "smart_kick_hard_abs: hardest turning for power %f %d", 
		     target_dir, Mem->CP_max_hard_kick_angle_err);
	  DebugKick(printf("turning to get hard kick\n"));
	  //Mem->HKTime = Mem->CurrentTime;
	  Mem->HKStepNext = 0; //stay in turn, we'll drop through next times
	  HKCommand.type = CMD_turn;
	  HKCommand.angle = target_dir;
	  HKCommand.time = Mem->CurrentTime;
	  HKCommand.turn_neck = FALSE;
	  //break; //used to be: return DoTurnKickCommand(HKCommand);
	}
      }
    }

    LogAction2(210, "HardestKick: after 0 processing");
    
    /* AngleDeg turn_target = abs_dir + 180 +
      ((int)Mem->HKrot)*Mem->CP_hardest_kick_angle_disp; */
    AngleDeg turn_target = abs_dir + 
      ((int)Mem->HKrot)*(Mem->CP_hardest_kick_ball_ang);
    NormalizeAngleDeg(&turn_target);
    AngleDeg targ_err = turn_target-Mem->MyBodyAng()-Mem->BallAngleFromBody();
    NormalizeAngleDeg(&targ_err);
    if ( (Mem->HKStep == 1 ||
	  (Mem->HKStep < 1 && HKCommand.time != Mem->CurrentTime)) &&
	 fabs(targ_err) > Mem->CP_KickTo_err) {
      /* on step 1, we turn ball to back of us */      
      LogAction4(70, "smart_kick_hard_abs: hardest turning ball backwards %f %f", 
		 turn_target, targ_err);
      HKCommand = hard_kick_turnball(turn_target, TURN_CLOSEST, TRUE);//TRUE to stop ball
      if (HKCommand.time != Mem->CurrentTime) {
	LogAction2(70, "kick_hardest: turnball won't work, so stopping ball");
	HKCommand = dokick(-180, 0, 1.0);
      }
      Mem->HKStepNext = 1;
    }
    LogAction2(210, "HardestKick: after 1 processing");
    if (Mem->HKStep == 2 ||
	(Mem->HKStep < 2 && HKCommand.time != Mem->CurrentTime)) {
      /* on step 2, we call the moderate code */
      /* or if step 0 had turnball problems, we'll go ahead and drop
	 to this code */
      LogAction2(70, "smart_kick_hard_abs: calling kick_hard_moderate");
      HKCommand = kick_hard_moderate(abs_dir, targ_vel, rot);
      Mem->HKStepNext = 2;
    }
    LogAction2(210, "HardestKick: after 2 processing");
    if (Mem->HKStep < 0 && Mem->HKStep > 2)
      my_error("HKstep is not in a valid state: %d", Mem->HKStep);
    
    Mem->HKTime = Mem->CurrentTime;
    LogAction2(210, "HardestKick: after all processing");
    break; //used to be: return DoTurnKickCommand(HKCommand);
  }
  
  case KM_Hard:
    /* Actually, this is implemented as a "dash helping kick" at a higher level 
       in behave */
    my_error("KM_Hard not implemented yet!");
    //dump_core("blah");
    break;

  case KM_ModerateWTurn: {
    /* This mode is *not* garunteed to be harder than moderate
       It's really intended for dead ball situations */
    AngleDeg target_dir = abs_dir - Mem->MyBodyAng() + rot*Mem->CP_hardest_kick_player_ang;
    NormalizeAngleDeg(&target_dir);
    LogAction3(70, "ModerateWTurn: norm angle %.2f", target_dir);
    if (fabs(target_dir) > Mem->CP_max_hard_kick_angle_err) {
      LogAction2(70, "ModerateWTurn: (0) need to turn!");
      if (Mem->BallWillBeKickable()) {
	HKCommand.type = CMD_turn;
	HKCommand.angle = target_dir;
	HKCommand.time = Mem->CurrentTime;
	HKCommand.turn_neck = FALSE;
	break;
      } else {
	LogAction2(70, "ModerateWTurn: need to turn, but ball won't be kickable, so call moderate");
      }
    }
    HKCommand = kick_hard_moderate(abs_dir, targ_vel, rot);
  } break;
    
  case KM_Moderate:
    HKCommand = kick_hard_moderate(abs_dir, targ_vel, rot);
    break;
    
  case KM_Quickly:
  case KM_QuickestRelease: {
      Mem->HKStep = Mem->HKStepNext = -1;
      /* see if the hardest kick in the direction we want will be collision */
      /* use our dokick function to correct for vel */
      TurnKickCommand kickCom;
      Vector vPredBall;
      Bool   CanKickToTraj;
      if (!is_hard_kick_coll(abs_dir, &kickCom, &vPredBall, targ_vel, &CanKickToTraj)) {
	/* there is no collsion! */
	if (mode == KM_Quickly &&
	    Mem->BallWillBeKickable() &&
	    fabs(Mem->BallAngleFromBody()) > Mem->CP_max_hard_kick_angle_err) {
	  /* In KM_Quickly mode, we can turn to ball and get it next time */
	  LogAction2(70, "smart_kick_hard_abs: km_quickly: turning for power");
	  HKCommand.time = Mem->CurrentTime;
	  HKCommand.type = CMD_turn;
	  HKCommand.angle = Mem->AngleToFromBody(Mem->BallPredictedPosition());
	} else
	  HKCommand = kickCom;
	break; //used to be:return DoTurnKickCommand(HKCommand);
      } else {
	/* do turnball */
	if (!CanKickToTraj) {
	  /* can't kick to trajectory, so just do the collision kick */
	  LogAction2(70, "smart_kick_hard_abs: km_quick: can't kick to traj, taking the collision");
	  HKCommand = kickCom;
	} else {
	  LogAction2(70, "smart_kick_hard_abs: km_quick: turnball");
	  HKCommand = hard_kick_turnball(abs_dir, rot);
	  if (HKCommand.time != Mem->CurrentTime) {
	    LogAction2(70, "km_quick: I'm using the original kick commands, even though it's a collision");
	    HKCommand = kickCom;
	  }
	}
	break; //used to be:return DoTurnKickCommand(hard_kick_turnball(abs_dir, rot));
      }
    
    }
    my_error("How did I get to end of KM_Quickly/KM_Quickest_Release");
    break;
    
  default:
    my_error("Invalid/Unimplemented kick mode passed to smart_kick_hard");
    break;
  }

  /* now we'll add a turn_neck into this to look at where the ball will be
     If a turn_neck is already there, we won't do this */
  LogAction2(210, "KickHard: checking for turn_neck");
  if (!HKCommand.turn_neck) {
    if (HKCommand.time != Mem->CurrentTime)
      my_error("Adding a turn_neck to an invalid HKCommand");
    LogAction2(70, "smart_kick_hard_abs: doing a turn_neck");
    Vector pred_ball_pos;
    if (HKCommand.type == CMD_kick)
      pred_ball_pos = Mem->BallPredictedPosition(1, HKCommand.power, HKCommand.angle);
    else
      pred_ball_pos = Mem->BallPredictedPosition(1);
    float pred_ball_dir = (pred_ball_pos - Mem->MyPredictedPosition()).dir();
    pred_ball_dir -= Mem->MyNeckGlobalAng();
    pred_ball_dir -= (HKCommand.type == CMD_turn ? HKCommand.angle : 0);

    HKCommand.turn_neck = TRUE;
    HKCommand.turn_neck_angle = Mem->LimitTurnNeckAngle(GetNormalizeAngleDeg(pred_ball_dir));
  }
  
  LogAction2(210, "KickHard: returning");
  return DoTurnKickCommand(HKCommand);
}

#ifdef NOT_USED
/* good passes require that the ball is not moving too quickly when it reaches
   the intended recipient, so this cover function helps acheive that */
int smart_pass(Vector pt, float targ_vel_at_pt, KickMode mode, TurnDir rot)
{
  LogAction2(60, "smart_pass: doing a turn_neck");
  return smart_kick_hard(Mem->AngleToFromBody(pt), mode,
			 Mem->VelAtPt2VelAtFoot(pt, targ_vel_at_pt),
			 rot);
}
#endif

Vector get_static_ball_pos(Vector ball_pos, float kick_ang)
{
  return ball_pos + Polar2Vector(Mem->CP_hardest_kick_ball_dist, kick_ang);
}


/* should be used when we have free kicks and stuff */
/* kick_ang: absolute angle for the direction that the ball will be kicked */
/* returns whether we are in the right place or not */
Bool go_to_static_ball(float kick_ang, float dash_pow)
{
  Line l;

  LogAction6(50, "go_to_static_ball: ball at (%.1f, %.1f) for kick angle %.2f, dash_pow %.2f",
		  Mem->BallX(), Mem->BallY(), kick_ang, dash_pow);
  
  if (!Mem->BallPositionValid())
    my_error("go_to_static_ball: lost ball");

  /* we can be pretty tolerant of angle errors, but distance errors start to matter
     real quickly */

  Vector targ_pt = get_static_ball_pos(Mem->BallAbsolutePosition(), kick_ang);

  /* we want to try and face the ball at all times */
  turn_neck(Mem->LimitTurnNeckAngle(Mem->BallAngleFromNeck()));
  
  if (Mem->MySpeed() == 0 &&
      Mem->DistanceTo(targ_pt) <= Mem->CP_static_kick_dist_err) {
    LogAction3(60, "go_to_static_ball: I am done %.2f", Mem->DistanceTo(targ_pt));
    return TRUE;
  }

  /* if we are real far from the ball, just use the regular go_to_point */
  if (Mem->BallDistance() > 2 * Mem->My_kickable_area()) {
    LogAction2(60, "go_to_static_ball: far away, using go_to_point");
    if (go_to_point(targ_pt, 0 /* no buffer */, dash_pow)
	!= AQ_ActionQueued)
      my_error("go_to_static_ball: far away, why didn't go_to_point do anything?");
    return FALSE;
  }

  /* make sure that we go around the ball */
  l.LineFromTwoPoints(Mem->MyPos(), targ_pt);
  Vector proj_ball_pt = l.ProjectPoint(Mem->BallAbsolutePosition());
  if (proj_ball_pt.dist(Mem->BallAbsolutePosition()) <=
      Mem->My_size() + Mem->SP_ball_size + Mem->CP_collision_buffer &&
      l.InBetween(proj_ball_pt, Mem->MyPos(), targ_pt)) {
    /* we'll do a 90 degree dodge -we always go right */
    Vector dodge_pt = Mem->MyPos() +
      Polar2Vector(Mem->My_size(), Mem->BallAngleFromBody() + Mem->MyBodyAng() + 90);
    LogAction2(60, "go_to_static_ball: dodging the ball");
    if (go_to_point(dodge_pt, 0 /* no buffer */, dash_pow)
	!= AQ_ActionQueued)
      my_error("go_to_static_ball: dodging, why didn't go_to_point do anything?");
    return FALSE;
  }

  /* now we need to get to the target_point */
  /* first see if we need to turn */
  l.LineFromRay(Mem->MyPos(), Mem->MyBodyAng());
  float ang = Mem->AngleToFromBody(targ_pt);
  if (fabs(ang) > 90 ||
      (l.dist(targ_pt) > Mem->CP_static_kick_dist_err &&
       ang > Mem->CP_static_kick_ang_err)) {
    LogAction2(60, "go_to_static_ball: turning to target_point");
    turn(Mem->AngleToFromBody(targ_pt));
    return FALSE;
  }
  
  /* now calculate the speed we should be going to land right on the point */
  float targ_speed =
    SolveForFirstTermInfGeomSeries(Mem->My_decay(), Mem->DistanceTo(targ_pt));
  float dash_pow_to_use =
    MinMax(-dash_pow / 2,
	   (targ_speed - Mem->MySpeed()) / Mem->My_dash_power_rate(),
	   dash_pow);
  LogAction5(60, "go_to_static_ball: targ_speed: %.2f\tMySpeed: %.2f\tdash_pow: %.2f",
	    targ_speed, Mem->MySpeed(), dash_pow_to_use);
  if (fabs(dash_pow_to_use) > 1) {
    LogAction3(70, "go_to_static_ball: dash power is too small %.2f", dash_pow_to_use);
    dash(Mem->CorrectDashPowerForStamina(dash_pow_to_use));
    return FALSE;
  }

  return TRUE;
}



 
