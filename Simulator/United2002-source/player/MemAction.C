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

#include "MemAction.h"
#include "soccer_utils.h"

#ifdef DEBUG_OUTPUT
#define DebugClear(x) 
#else
#define DebugClear(x) 
#endif

void ActionInfo::Initialize()
{
  MyActionMode = AM_Unknown;
  PullOffside = FALSE;

  Stored_Fastest_Teammate_Time = 0;
  Stored_Fastest_Opponent_Time = 0;

  for (int i = 1; i <= SP_team_size; i++) {
    TeamIntInfo[i] = new PlayerInterceptInfo;
    TeamIntInfo[i]->time = -1;
    OppIntInfo[i] = new PlayerInterceptInfo;
    OppIntInfo[i]->time = -1;
  }

  kick_in_progress = FALSE;
  goalie_last_catch_time = 0;
  GoalieWarnTime = 0;
  goalie_after_catch = FALSE;
  
  DribbleTargetAvailable = FALSE;
  DribbleTargetTime = -1;

  InterceptLookahead = LA_Default;
  IntMinCyc = -1;
  IntMinCycTime = -1;

  HKTime = -1;
  HKStep = -1;
  HKStepNext = -1;
  HKrot = TURN_CW;

  ChangeViewForHandleBallTime = 0;
  LastBallPossessorTime = 0;

  team_pass_time = -1;
  team_receiver = Unum_Unknown;
  pass_to_point = 0;
}


/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

#ifdef DEBUG_OUTPUT
#define DebugInt(x) 
#else
#define DebugInt(x) 
#endif

/* only used for this player */
PlayerInterceptInfo
ActionInfo::CloseBallInterception(float max_pow, int max_lookahead,
				  Vector vBallPos, Vector vBallVel)
{
  Vector vNewPos;
  PlayerInterceptInfo info;
  float dash_dist = max_pow * My_dash_power_rate();
  info.dash_pow = max_pow;
  info.lookahead = max_lookahead;
  info.res = BI_None;  
  
  vBallPos += vBallVel;
  vBallVel *= SP_ball_decay;
  /* we need to figure out the right dash power so that the ball ends up right in
     front of us. Like many things, this ends up as a LineCircleIntersect problem */
  Vector vMyPred = MyPredictedPosition();
  Ray    rDash(vMyPred, MyBodyAng());
  Line   lDash   = LineFromRay(rDash);
  Vector sol1, sol2;
  
  int num_sol = LineCircleIntersect(lDash, My_size() + SP_ball_size + CP_collision_buffer,
				    vBallPos, &sol1, &sol2);
  if (num_sol >= 1) {
    /* we'll make sure that the right answer is in sol1 */
    if (num_sol == 2) {
      /* we have to pick which point is better */
      if (fabs(GetNormalizeAngleDeg((vBallPos - sol2).dir() - MyBodyAng())) < 90) {
	sol1 = sol2; //sol2 is the right solution, put it in sol1
      } else if (!(fabs(GetNormalizeAngleDeg((vBallPos-sol1).dir() - MyBodyAng())) < 90)) {
	my_error("CloseBallInterception: 1 ahead, neither solution looks good %.2f %.2f",
		 GetNormalizeAngleDeg((vBallPos - sol2).dir() - MyBodyAng()),
		 GetNormalizeAngleDeg((vBallPos - sol1).dir() - MyBodyAng()));
      }
    }

    /* now figure out the dash power based on that point */
    float dash_pow = vMyPred.dist(sol1) / My_dash_power_rate();
    dash_pow = MinMax(SP_min_power, dash_pow, SP_max_power);
    if (!rDash.InRightDir(sol1))
      dash_pow = -dash_pow;

    if (vBallPos.dist(MyPredictedPosition(1, dash_pow)) < My_kickable_area()) {
      /* this works! */
      info.res = BI_CanChase;
      info.numCyc = 1;
      info.dash_pow_to_use = dash_pow;
      //this'll make go_to_point dash
      info.pos = vMyPred + Polar2Vector(signf(dash_pow)*dash_dist, MyBodyAng());
      LogAction5(70, "CloseBallInterception: One dash and we're there: %.2f to (%.2f, %.2f)",
		 dash_pow, sol1.x, sol1.y);
      return info;
    }
    
    
  }
  
  vBallPos += vBallVel;
  vBallVel *= SP_ball_decay;
  //now look two cycles ahead
  //try turn then dash
  float targ_ang = (vBallPos - MyPredictedPosition(2)).dir();
  if (fabs(targ_ang - MyBodyAng()) > CP_max_go_to_point_angle_err) {
    vNewPos = MyPredictedPositionWithTurn(targ_ang - MyBodyAng(), 2, max_pow);
    if (vNewPos.dist(vBallPos) < My_kickable_area()) {
      info.res = BI_CanChase;
      info.numCyc = 2;
      info.dash_pow_to_use = max_pow;
      //this'll make go_to_point turn
      //info.pos = MyPos() + Polar2Vector(dash_dist, targ_ang); 
      info.pos = MyPredictedPosition() + Polar2Vector(dash_dist, targ_ang); 
      LogAction2(70, "CloseBallInterception: Turn then dash and we're there");
      return info;
    }
  }
  //try two dashes
  vNewPos = MyPredictedPosition(2, max_pow);
  if (vNewPos.dist(vBallPos) < My_kickable_area()) {
    info.res = BI_CanChase;
    info.numCyc = 2;
    info.dash_pow_to_use = max_pow;
    //this'll make go_to_point dash
    //info.pos = MyPos() + Polar2Vector(2*dash_dist, MyBodyAng()); 
    info.pos = MyPredictedPosition() + Polar2Vector(dash_dist, MyBodyAng());
    LogAction2(70, "CloseBallInterception: Two dashes and we're there");
    return info;
  }
  return info;
}

/*****************************************************************************************/

/* does not set time field */
/* cyc inc is initially CP_intercept_step, but when an answer is found, we
   then bring cyc back a step, and go up by ones */
PlayerInterceptInfo
ActionInfo::ActiveCanGetThere(float max_pow, int max_lookahead,
			   Vector vBallPos, Vector vBallVel,
			   char side, Unum num,
			   Vector vPlayerPos, Vector vPlayerVel,
			   float fPlayerAng, int PlayerAngValid,
			   bool IsThisMe)
{
  float at_point_buffer = 1;
  PlayerInterceptInfo info;
  //Vector vPredPlayer = vPlayerPos + vPlayerVel;
  Vector vPredPlayer = vPlayerPos +
    vPlayerVel * (SumInfGeomSeries(vPlayerVel.mod(), Player_decay(side,num)));
  Vector vOldBallPos, vOldBallVel;
  float turn_angle;
  int cyc;
  int cyc_inc = (IsThisMe ? CP_my_intercept_step : CP_intercept_step); 
  int max_cyc = (max_lookahead + cyc_inc - 1);  
  max_cyc -= (max_cyc % cyc_inc); 
  /* max_cyc is so that we don't miss an interception if CP_intercept_step is not
     1. For example, if CP_intercept_step is 5, max_look is 14, and we can
     intercept in 13, we would return no intercept if we just used max_lookahead */
  
  DebugInt(printf(" ACGT: BallPos.vel.mod: %f\n", vBallVel.mod() ));

  info.dash_pow_to_use = max_pow;
  NormalizeAngleDeg(&fPlayerAng);

  /* we want to aim a little ahead of the ball, so advance it a little */
  for (int i=0; i < CP_intercept_aim_ahead; i++) {
    vBallPos += vBallVel;
    vBallVel *= SP_ball_decay;
  }

  //if (IsThisMe) LogAction4(140, "ActiveBallIntercept: %d %d", (int)max_pow, max_lookahead);
  
  for (cyc=0; cyc<=max_cyc; cyc += cyc_inc) {

    if (!IsPointInBounds(vBallPos,-3)) {  /* expand the field by 3 meters so we don't give up to soon */
      DebugInt(printf("The ball will go out of bounds before we can get it\n"));
      break;
    }
    
    /* decide if we need to turn to ball */
    float ball_ang = (vBallPos - vPredPlayer).dir();
    Vector vEndSpot;
    /* SMURF - we should probably aim for ball 1 cycle ahead or something
       like that */
    //DebugInt(printf(" angle to exp ball pos: %f\n", AngleTo(vBallPos)));
    turn_angle = ball_ang - fPlayerAng;
    if (fabs(turn_angle) < CP_max_go_to_point_angle_err)
      turn_angle = 0.0;      
    if (IsThisMe) {
      vEndSpot = MyPredictedPositionWithTurn(turn_angle, cyc, max_pow,(turn_angle != 0.0));
    } else {
      int run_cyc = cyc;
      if (PlayerAngValid) {
	if (turn_angle != 0.0)
	  run_cyc--;
	run_cyc = Max(0, run_cyc);
      }
      Vector PlayerDash =
	Polar2Vector(max_pow*Player_dash_power_rate(side,num), ball_ang);
      vEndSpot =
	PlayerPredictedPosition(side, num, run_cyc, PlayerDash);
    }
    

    float dist_to_ball_after = (vBallPos - vEndSpot).mod();
    /* if we can make it there */
    /* SMURF- is this too lenient? */
    if (dist_to_ball_after <= at_point_buffer || 
        (vEndSpot - vPredPlayer).mod() > (vBallPos - vPredPlayer).mod() + Player_kickable_area(side,num)) {
      /* we can get to the ball! */
      /* OR we travelled far enough, but somehow missed the ball,
	 return sucess */
      if (dist_to_ball_after <= at_point_buffer) {
	//if (IsThisMe) LogAction4(100, "Found a ball interception by being close (%.2f, %.2f)",
	//				vBallPos.x, vBallPos.y);
	info.numCyc = cyc;
      } else {	
	if (IsThisMe) LogAction4(100, "Found a ball interception by going far (%.2f, %.2f)",
				vBallPos.x, vBallPos.y);
	info.numCyc = cyc;
	//vBallPos += vBallVel; /* advance one spot for that turn*/
      }      
      
      if (cyc_inc > 1 && cyc != 0) {
	/* we want the best answer- go back and go up by ones */
	if (IsThisMe)
	  LogAction2(100, "Found a ball interception, but goign back for accuracy");	
	DebugInt(printf("Found answer, but going back for accuracy: %d\n", cyc));
	cyc -= cyc_inc;
	vBallPos = vOldBallPos;
	vBallVel = vOldBallVel;
	cyc_inc = 1;
	max_cyc = max_lookahead; // don;t need to go above this anymore
      } else {
	/* we want to try avoiding turning towards the ball for only a small savings
	   in time to intercept */
	if (IsThisMe && CP_no_turn_max_cyc_diff > -1 &&
	    turn_angle != 0.0 &&
	    (vBallVel.x >= FLOAT_EPS || vBallVel.y >= FLOAT_EPS)) {
	  Ray rBall(vBallPos, vBallVel);
	  Ray rPlayer(vPredPlayer, fPlayerAng);
	  Vector int_pt;
	  if (rBall.intersection(rPlayer, &int_pt)) {
	    float dist = vEndSpot.dist(int_pt);
	    float num_cyc; /* the number of cycles extra it takes the ball to get to
				this pos */
	    num_cyc = SolveForLengthGeomSeries(vBallVel.mod(), SP_ball_decay, dist);
	    LogAction3(90, "No turn interception: It takes %.2f extra cycles", num_cyc);
	    // if an answer less than 0 is given, the ball will never get there
	    if (num_cyc >= 0 &&
		num_cyc <= CP_no_turn_max_cyc_diff) {
	      /* use this target instead! */
	      LogAction4(70, "Using the new no turning interception point (%.2f, %.2f)",
			 int_pt.x, int_pt.y);
	      info.res = BI_CanChase;
	      info.pos = int_pt;
	      return info;
	    } /* using no turn interseption */
	  } /* there is an intersection */
	  
	} /* no turn interseption */
	

	if (info.numCyc > max_lookahead) {
	  info.res = BI_Failure;
	} else {
	  info.res = BI_CanChase;
	  info.pos = vBallPos;
	}	
	return info;
      }      
    }
    
    /* update ball position estimate */
    vOldBallPos = vBallPos;
    vOldBallVel = vBallVel;
    for (int i=0; i < cyc_inc; i++) {
      vBallPos += vBallVel;
      vBallVel *= SP_ball_decay;
    }
      
    
  } /* cycle loop */

  info.res = BI_Failure; // can't make it to ball before max_lookahead
  return info;
}

/*****************************************************************************************/

void ActionInfo::BallIntercept_active(float max_pow_to_use, int max_lookahead,
				   char PlayerSide, Unum PlayerNum,
				   PlayerInterceptInfo* pInfo)
{
  Vector PlayerPos;
  Vector PlayerVel;
  float PlayerAng;
  int AngValid = FALSE;
  Vector BallVel;

  pInfo->res = BI_None;

  if (!BallPositionValid()) {
    my_error("BallIntercept_active: Can't get to ball if I don;t know where it is");
    pInfo->res = BI_Invalid;
    return;
  }
  
  if (!PlayerPositionValid(PlayerSide, PlayerNum)) {
    my_error("BallIntercept_active: Can't give an answer if I don't know where player is");
    pInfo->res = BI_Invalid;
    return;
  }
  PlayerPos = PlayerAbsolutePosition(PlayerSide, PlayerNum);
  //DebugInt(cout << "PlayerPos: " << PlayerPos << endl);
  
  if (PlayerVelocityValid(PlayerSide, PlayerNum)) {
    PlayerVel = PlayerAbsoluteVelocity(PlayerSide, PlayerNum);
  } else {    
    PlayerVel = Vector(0,0);
  }
  
  if (PlayerBodyAngleValid(PlayerSide, PlayerNum)) {
    AngValid = TRUE;
    PlayerAng = PlayerAbsoluteBodyAngle(PlayerSide, PlayerNum);
  } else
    PlayerAng = 0;
    
  if ((PlayerPos - BallAbsolutePosition()).mod() <
      Player_kickable_area(PlayerSide, PlayerNum)) {
    pInfo->res = BI_ReadyToKick;
    pInfo->numCyc = 0;
    pInfo->pos = PlayerPos;
    return;
  }

  if (BallVelocityValid())
    BallVel = BallAbsoluteVelocity();
  else
    BallVel = Vector(0,0);
  
  DebugInt(printf("At BallIntercept_active  max_pow: %f, max_look: %d\n",
		  max_pow_to_use, max_lookahead));

  if (PlayerSide == MySide && PlayerNum == MyNumber)
    *pInfo = CloseBallInterception(max_pow_to_use, max_lookahead,
				   BallAbsolutePosition(), BallVel);
  
  if (pInfo->res == BI_None)
    *pInfo = 
      ActiveCanGetThere(max_pow_to_use, max_lookahead,
			BallAbsolutePosition(), BallVel,
			PlayerSide, PlayerNum,
			PlayerPos, PlayerVel, PlayerAng, AngValid,
			(PlayerSide == MySide && PlayerNum == MyNumber));
  else
    ;//{ printf("%d:%d Used Close Ball intercept\n",MyNumber,CurrentTime.t);}
} 

/*****************************************************************************************/

PlayerInterceptInfo* ActionInfo::GetPlayerIntInfo(char side, Unum num)
{
  if (side == MySide)
    return TeamIntInfo[num];
  else if (side == TheirSide)
    return OppIntInfo[num];
  else
    my_error("bad side passed to GetPlayerIntInfo");
  return NULL;
}

/*****************************************************************************************/

PlayerInterceptInfo* ActionInfo::VerifyIntInfo(char side, Unum num, float dash_pow)
{
  PlayerInterceptInfo* pInfo = GetPlayerIntInfo(side, num);
  if (pInfo == NULL) {
    my_error("Bad side or number passed to VerifyIntInfo");
    return NULL;
  }

  int lookahead;
  switch (InterceptLookahead) {
  case LA_Default: lookahead = CP_max_int_lookahead; break;
  case LA_BestSoFar:
    lookahead =
      (IntMinCycTime == CurrentTime) ? (IntMinCyc) : CP_max_int_lookahead;
    break;
  default: lookahead = InterceptLookahead; break;
    break;
  }
  if ( pInfo->time != CurrentTime || fabs((pInfo->dash_pow-dash_pow))>FLOAT_EPS ||
       (pInfo->lookahead < lookahead && !IsSuccessRes(pInfo->res)) ) {
    /* set the info struct */
    DebugInt(printf("%d %d Data not current. Calling interception code\n", MyNumber, num));

    if (pInfo->time == CurrentTime && (pInfo->dash_pow-dash_pow)<=FLOAT_EPS &&
	(side != MySide || num != MyNumber))
      my_error("Recomputing %c %d because lookahead got bigger; old: %d\tnew: %d",
	       side,num,pInfo->lookahead, lookahead);
    
    /* let's do a real quick estimate to see if the player can make it there
       if player dist to ball > max ball dist will travel + max_dist we'll
       travel, then there's no way to get there */
    if (!PlayerPositionValid(side, num)) {      
      my_error("VerifyIntInfo: Can't give an answer if I don't know where player is");
      pInfo->res = BI_Invalid;
      return pInfo;
    }
    DebugInt(printf("Lookahead: %d\n", lookahead));
    float ball_travel = SumGeomSeries((BallVelocityValid() ? BallSpeed() : 0),
				      SP_ball_decay, lookahead);
    float player_travel = Player_speed_max(side,num) * lookahead;
    float play_ball_dist = (PlayerAbsolutePosition(side, num) -
			    BallAbsolutePosition()).mod() ;
    if (play_ball_dist > player_travel + ball_travel) {
      pInfo->time = CurrentTime;
      pInfo->dash_pow = dash_pow;
      pInfo->dash_pow_to_use = dash_pow;
      pInfo->lookahead = lookahead;
      pInfo->res = BI_Failure;
      DebugInt(printf("Interception: %d, %d Took shortcut to decide failure\n", MyNumber, num));
    } else {
      DebugInt(printf("Interception: %d, %d About to do actual calculation\n", MyNumber, num));
      BallIntercept_active( dash_pow, lookahead, side, num, pInfo);
      if (IsSuccessRes(pInfo->res))
	SetIntMinCyc(pInfo->numCyc);
      pInfo->time = CurrentTime;
      pInfo->dash_pow = dash_pow;
      pInfo->lookahead = lookahead;
    }    
  }
  else if ( IsSuccessRes(pInfo->res) )
    SetIntMinCyc(pInfo->numCyc);

  return pInfo;
}

/*****************************************************************************************/


InterceptRes ActionInfo::PlayerInterceptionResult(char side, Unum num,
				      float dash_pow)
{
  return (VerifyIntInfo(side, num, dash_pow))->res;
}

/*****************************************************************************************/

Bool ActionInfo::PlayerInterceptionAble(char side, Unum num, float dash_pow)
{
  return IsSuccessRes((VerifyIntInfo(side, num, dash_pow))->res) ? TRUE : FALSE;
}

/*****************************************************************************************/

int ActionInfo::PlayerInterceptionNumberCycles(char side, Unum num,
				   float dash_pow)
{
  PlayerInterceptInfo* pInfo = VerifyIntInfo(side, num, dash_pow);
  if (!IsSuccessRes(pInfo->res))
    my_error("Trying to get number of cycles on invalid result: %c%d %d",
	     side, num, pInfo->res);
  return pInfo->numCyc;
}

/*****************************************************************************************/

Vector ActionInfo::PlayerInterceptionPoint(char side, Unum num,
			       float dash_pow)
{
  PlayerInterceptInfo* pInfo = VerifyIntInfo(side, num, dash_pow);
  if (!IsSuccessRes(pInfo->res))
    my_error("Trying to get interception point on invalid result: %c%d %d", 
	     side, num, pInfo->res);
  return pInfo->pos;  
}

/*****************************************************************************************/

float ActionInfo::PlayerInterceptionDashPower(char side, Unum num, float dash_pow)
{
  PlayerInterceptInfo* pInfo = VerifyIntInfo(side, num, dash_pow);
  if (!IsSuccessRes(pInfo->res))
    my_error("Trying to get interception dash power on invalid result: %c%d %d", 
	     side, num, pInfo->res);
  return pInfo->dash_pow_to_use;  
}


/*****************************************************************************************/

int ActionInfo::GetInterceptionMinCyc()
{
  if (IntMinCycTime != CurrentTime)
    return -1;
  else
    return IntMinCyc;
}

/*****************************************************************************************/

void ActionInfo::SetIntMinCyc(int newval)
{
  if (IntMinCycTime != CurrentTime) {
    IntMinCycTime = CurrentTime;
    IntMinCyc = newval;
  } else if (IntMinCyc > newval)
    IntMinCyc = newval;
}

/*****************************************************************************************/

void ActionInfo::SetInterceptionLookahead(int newval)
{
  if (newval > 0 || newval == LA_Default || newval == LA_BestSoFar) {
    if (IntMinCycTime == CurrentTime) 
      DebugInt(cout << "Changing lookahead mid way through computations. Could be bad" <<endl);
    InterceptLookahead = newval;
  } else {
    my_error("Trying to set InterceptLookahead to an invlaid value");
  }
  
}




/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/* Passive interception stuff */

int ActionInfo::GetClosestPointToBallPath(Vector* pPt, float* pNumCycles, 
					  Bool *pBallAble,
				       Vector PlayerPos, Vector BallPos,
				       Vector BallVel)
{
  if (fabs(BallVel.x) < FLOAT_EPS && fabs(BallVel.y) < FLOAT_EPS) {
    *pPt = BallPos;
    *pNumCycles = 0;
    *pBallAble = TRUE;
    return 1;
  }

  Ray rBallPath(BallPos, BallVel);;
  
  *pPt = rBallPath.GetClosestPoint(PlayerPos);
  
  /* adjust point for sidelines */
  Rectangle field(Vector(0,0), Vector(SP_pitch_length, SP_pitch_width));
  *pPt = AdjustPtToRectOnLine(*pPt, field, LineFromRay(rBallPath));

  /* Now let's reason about how far off we will be if we favor not turning */
  Vector no_turn_pt;
  if (rBallPath.intersection(Ray(MyPos(), MyBodyAng()), &no_turn_pt)) {
    if (no_turn_pt.dist(*pPt) < CP_no_turn_max_dist_diff) {
      LogAction6(110, "BPI: using no turn interception, old: (%.1f, %.1f) new: (%.1f, %.1f)",
		 pPt->x, pPt->y, no_turn_pt.x, no_turn_pt.y);
      *pPt = no_turn_pt;
    }
  }
  
  /* compute the number of cycles to get here */
  *pNumCycles = 0;
  *pBallAble = TRUE;

  /* now get the number of cycles */
  Vector traj = *pPt - BallPos;
  DebugInt(cout << "Pt: " << *pPt << "\tBallVel: " << BallVel
	   << "\tBallPos: " << BallPos << "\ttraj: " << traj << endl);
  /* first decide if the ball is actually coming towards us */
  if (signf(traj.x) != signf(BallVel.x) ||
      signf(traj.y) != signf(BallVel.y)) {
    DebugInt(printf("  GCPTBP: Ball is goign wrong way for closest intercept!\n"));
    return 0;
  }

  float trajDist = traj.mod();
  float velMod = BallVel.mod();
  float temp = trajDist / velMod * (SP_ball_decay - 1) + 1;
  if (temp < 0.0) {
    /* ball will never make it to closest point */
    /* SMURF - shoudl adjust for actual closest!!!! */
    /* But the only time this is used (in 12/99), is to see if the player 
       can get there first.  So if we set numcycles to a high number, 
       the player will be  predicted able to get there with plenty of time
       to spare (i.e. in fewer cycles) */
    /* One fix would be to not test BPICanIgetthere in the normal way in this case.
       i.e. store that the ball won't get there.  Then automatically set that 
       I CAN get there if the ball will never get there. */
    /* Did the above:  Peter 12/22/99 */
    DebugInt(printf("GCPTBP: Ball will never make it to closest point, adjusting\n"));
    *pPt = BallPos + traj * SumInfGeomSeries(velMod, SP_ball_decay) / traj.mod();
    *pNumCycles = 1000000; //just a big number
    *pBallAble = FALSE;
    //my_error("Should adjust for actual closest.");
    return 1; 
  } else
    *pNumCycles = log(temp) / log(SP_ball_decay);

  return 1;
}

/*****************************************************************************************/

void ActionInfo::VerifyBPIInfo()
{
  if (BPItime == CurrentTime)
    return;

  BPItime = CurrentTime;
  
  Vector BallVel;

  if (!MyConf()) {
    my_error("Can't intercept if I don't know where I am");
    BPIvalid = FALSE;
    return;
  }

  if (!BallPositionValid()) {
    my_error("Can't get to ball path if I don't know where it is");
    BPIvalid = FALSE;
    return;
  }
  
  if (BallKickable()) {
    BPIvalid = TRUE;
    BPIable = TRUE;
    BPIballable = TRUE;
    BPIdist = 0;
    BPIpoint = MyPos();
    BPIballcyc = 0;
    return;
  }

  if (BallVelocityValid())
    BallVel = BallAbsoluteVelocity();
  else {
    BPIvalid = TRUE;
    BPIable = TRUE;
    BPIballable = TRUE;
    BPIdist = BallDistance();
    BPIpoint = BallAbsolutePosition();
    BPIballcyc = 0;
    return;
  }
      
  DebugInt(printf("\nTime: %d\n", CurrentTime.t));
  DebugInt(printf("At BallIntercept_passive\n"));

  int passRet;
  passRet = GetClosestPointToBallPath(&BPIpoint, &BPIballcyc, &BPIballable, MyPos(),
				      BallAbsolutePosition(), BallVel);
  DebugInt(printf("Passive Method: ret: %d\tx: %f\ty:%f\tcyc: %f\n",
		  passRet, BPIpoint.x, BPIpoint.y, BPIballcyc));
  if (passRet) {
    BPIvalid = TRUE;
    BPIable = TRUE;
    BPIdist = (BPIpoint - MyPos()).mod();
  } else {
    BPIvalid = TRUE;
    BPIable = FALSE;
  }

  return;
  
}

/*****************************************************************************************/

Vector ActionInfo::BallPathInterceptPoint()
{
  VerifyBPIInfo();
  if (!BPIvalid)
    my_error("Calling BallPathInterceptionPoint when info not valid?");
  return BPIpoint;
}

/*****************************************************************************************/

Bool ActionInfo::BallPathInterceptAmIThere(float buffer)
{
  VerifyBPIInfo();
  if (!BPIvalid)
    my_error("Calling BallPathInterceptionAmIThere when info not valid");
  return (BPIable && (MyPos() - BPIpoint).mod() <= buffer) ? TRUE : FALSE;
}

/*****************************************************************************************/

float ActionInfo::BallPathInterceptDistance()
{
  VerifyBPIInfo();
  if (!BPIable)
    my_error("Calling BallPathInterceptionDistance when I can't get get there");
  return BPIdist;
}

/*****************************************************************************************/

int ActionInfo::BallPathInterceptCyclesForBall()
{
  VerifyBPIInfo();
  if (!BPIable)
    my_error("Calling BallPathInterceptionCyclesForBall when I can't get get there");
  return (int)ceil(BPIballcyc);
}

/*****************************************************************************************/

Bool ActionInfo::BallPathInterceptCanIGetThere(float max_pow)
{
  VerifyBPIInfo();
  if (!BPIable)
    return FALSE;

  if (!BPIballable) // The ball won't ever get to the closest point, 
    return TRUE;    // so I can definitely beat it there.

  AngleDeg targAng = AngleToFromBody(BPIpoint);
  Vector myEnd;
  if (fabs(GetNormalizeAngleDeg(MyBodyAng() - targAng)) >
      CP_max_go_to_point_angle_err) {
    myEnd = MyPredictedPosition((int)ceil(BPIballcyc), max_pow);
  } else {
    myEnd = MyPredictedPositionWithTurn(targAng - MyBodyAng(),
					(int)ceil(BPIballcyc), max_pow);    
  }

  return ( (myEnd - MyPos()).mod() >= (BPIpoint - MyPos()).mod() ) ? TRUE : FALSE;
}




/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

/* looks at closeest opponent or teamless player */
/* SMURF: the teamless part is a hack */
KickMode ActionInfo::BestKickModeAbs(AngleDeg abs_ang)
{
  if (PlayMode != PM_Play_On) {
    if (PlayMode == PM_My_Goal_Kick)//&& HasBallMovedFromStart())
      return KM_HardestKick; //this is the second player who is still in the box
    if (PlayMode == PM_My_Goalie_Free_Kick)
      return KM_HardestKick; //goalie should use hardest
    return KM_ModerateWTurn;
  }
  
  Unum closest = ClosestOpponent();
  if (NumTeamlessPlayers() > 0) {
    Vector teamless_pos = ClosestTeamlessPlayerPosition();
    if (closest == Unum_Unknown ||
	DistanceTo(teamless_pos) < OpponentDistance(closest))
      closest = Unum_Teamless;
  }

  if (closest == Unum_Unknown)
    return KM_HardestKick;
  int cyc_to_steal = EstimatedCyclesToSteal(closest);
  float targ_ang = abs_ang + signf(GetNormalizeAngleDeg(BallAngleFromBody()-abs_ang)) *
    (90 + CP_hardest_kick_ball_ang);
  float ang_diff = GetNormalizeAngleDeg(BallAngleFromBody() - targ_ang);
  NormalizeAngleDeg(&ang_diff);  
  if (cyc_to_steal > fabs(ang_diff)/CP_time_for_full_rotation + CP_cycles_to_kick)
    return KM_HardestKick;
  //if (OpponentWithBall() != Unum_Unknown)
  if (cyc_to_steal <= 1)
    return KM_QuickestRelease;
  if (cyc_to_steal < CP_cycles_to_kick)
    return KM_Quickly;
  if (cyc_to_steal < CP_cycles_to_kick + 1) // time for a dash in KM_Hard
    return KM_Moderate;
  return KM_Hard;
}

/*****************************************************************************************/


/* returns estimated cycles for opponent to get the ball into his kickable
   area */
/* can handle Unum_Teamless SMURF: it's kind of a hack though */
int ActionInfo::EstimatedCyclesToSteal(Unum opp, Vector ball_pos)
{
  if (!BallKickable())
    my_error("EstimatedCyclesToSteal: shouldn't use this if the ball is not kickable");

  if (BallKickableForOpponent(opp)) {
    LogAction2(110, "EstimatedCyclesToSteal: already kickable for opponent");
    return 0;
  }
  
  Vector targ = ball_pos;
  Vector pos;
  int cyc;
  if (opp == Unum_Teamless) {
    if (NumTeamlessPlayers() < 1)
      my_error("EstimatedCyclesToSteal: can't estimate teamless if there aren't any");
    pos = ClosestTeamlessPlayerPosition();
    targ -= (ball_pos - pos).SetLength(Opponent_kickable_area(opp));
    cyc = (int)ceil(targ.dist(pos) / Opponent_speed_max(opp));
  } else {
    if (!OpponentPositionValid(opp))
      my_error("EstimateCyclesToSteal: can't estimate if I don;t know where opponent is");
    pos = OpponentAbsolutePosition(opp);
    targ -= (ball_pos - pos).SetLength(Opponent_kickable_area(opp));
    cyc = OpponentPredictedCyclesToPoint(opp, targ);
  }  

  /* now decide if the player will have to dodge */
  if (!pos.ApproxEqual(targ)) {
    Line oppLine = LineFromTwoPoints(pos, targ);
    Vector dodge_pos = oppLine.ProjectPoint(MyPos());
    dodge_pos += (pos - dodge_pos).SetLength(My_size() + Opponent_size(opp));
    float dodge_dist = oppLine.dist(MyPos());
    if (dodge_dist < (My_size() + Opponent_size(opp)) &&
	oppLine.InBetween(dodge_pos, pos, targ)) {
      /* need to take into account a dodge */
      cyc += 2; //have to turn twice
      if (dodge_dist > (My_size() + Opponent_size(opp)) - Opponent_speed_max(opp))
	cyc += 1; // one dash will dodge us
      else
	cyc += 2; // it takes two dashes to dodge us
    }
  }
  
  return cyc;
}


/*****************************************************************************************/

/* this is not an exact function becuase we don't have a perfect mapping of
   ball speed/position to kick power.
   Basically, this function returns whether the ball will be further back but still
   kickable after a dash */
Bool ActionInfo::WillDashHelpKick(Vector pt, float dash_pow)
{
  if (!BallWillBeKickable(1, dash_pow, CP_kickable_buffer)) {
    LogAction2(130, "WillDashHelpKick: ball will not be kickable");
    return FALSE;
  }
  
  /* we're going to assume that a collision is bad.
     but depending on how the ball is actually moving that could be good */
  if (WillDashBeCollision(dash_pow, CP_collision_buffer)) {
    LogAction2(130, "WillDashHelpKick: collision");
    return FALSE;
  }

  /* if we're not facing genrally in the direction we want to kick it,
     dashing will probably not help */
  if (fabs(AngleToFromBody(pt)) > CP_max_dash_help_kick_angle) {
    LogAction2(130, "WillDashHelpKick: not facing");    
    return FALSE;
  }
  
  AngleDeg curr_ang = BallAngleFromBody() - AngleToFromBody(pt);
  NormalizeAngleDeg(&curr_ang);
  Vector my_pred_pos = MyPredictedPosition(1, dash_pow);
  AngleDeg pred_ang =
    (BallPredictedPosition() - my_pred_pos).dir() -
    (pt - my_pred_pos).dir();
  NormalizeAngleDeg(&pred_ang);

  LogAction4(130, "WillDashHelpKick: curr: %.1f  pred: %.1f", curr_ang, pred_ang);
  
  return (fabs(pred_ang) > fabs(curr_ang)) ? TRUE : FALSE;
}



/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

Bool ActionInfo::KickInProgress()
{
  /* need to have kicked last cycle.  Updates kick_in_progress_time */
  if ( BallKickable() && kick_in_progress && kick_in_progress_time == LastActionOpTime ){
    kick_in_progress_time = CurrentTime;
    return TRUE;
  }
  return FALSE;
}

/*****************************************************************************************/

void ActionInfo::StartKick(AngleDeg target_angle, KickMode mode, float target_vel, TurnDir rot)
{
  kick_in_progress = TRUE;
  start_kick_time = kick_in_progress_time = CurrentTime;
  kick_in_progress_abs_angle = GetNormalizeAngleDeg(target_angle + MyBodyAng());
  kick_in_progress_mode = mode;
  kick_in_progress_target_vel = target_vel;
  kick_in_progress_rotation = rot;
}

/*****************************************************************************************/

void ActionInfo::StartShot(AngleDeg target_angle, KickMode mode, TurnDir rot)
{
  StartKick(target_angle,mode,2*SP_ball_speed_max, rot);
}

/*****************************************************************************************/

void ActionInfo::StartPass(Unum target, float target_vel_at_dest, TurnDir rot)
{
  if ( target == Unum_Unknown || !TeammatePositionValid(target) ) my_error("can't start this pass");

  team_passer = MyNumber;
  team_receiver = target;
  team_pass_time = CurrentTime;
  pass_to_point = TeammateAbsolutePosition(target);
  
  float target_vel = VelAtPt2VelAtFoot(pass_to_point,target_vel_at_dest);
  StartKick(TeammateAngleFromBody(target),KM_Moderate,target_vel,rot);
}




/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

/* No reasoning about players being tired yet:  
   if so, need to add dash_pow to the interception calls */

/* These functions are very computationally intensive */

/* the stored value does not include the goalie */
Unum ActionInfo::FastestTeammateToBall()
{
  if ( !BallPositionValid() ) my_error("Need to know ball position to know fastest to it\n");

  Unum closest = ClosestTeammateToBall();
  if ( !BallMoving() && !TeammateTired(closest) ) return closest;

  if ( CurrentTime == Stored_Fastest_Teammate_Time ) return Stored_Fastest_Teammate;
  
  ResetInterceptionMinCyc();
  SetInterceptionLookahead(LA_BestSoFar);

  //Run ourselves first, just so we have a valid answer
  MyInterceptionAble();  

  Unum FastestPlayer = Unum_Unknown;
  int cycles, min_cycles = CP_max_int_lookahead+1;

  for (int i=1; i<=SP_team_size; i++){
    if ( TeammatePositionValid(i) && TeammateInterceptionAble(i) == TRUE &&
	 (cycles=TeammateInterceptionNumberCycles(i)) < min_cycles && 
	 (i != FP_goalie_number || CP_goalie) ){
      min_cycles = cycles;
      FastestPlayer = i;
    }
  }

  Stored_Fastest_Teammate = FastestPlayer;
  Stored_Fastest_Teammate_Time = CurrentTime;

  return Stored_Fastest_Teammate;
}

/*****************************************************************************************/

Unum ActionInfo::FastestOpponentToBall()
{
  if ( !BallPositionValid() ) my_error("Need to know ball position to know fastest to it\n");

  if ( !BallMoving() ) return ClosestOpponentToBall();

  if ( CurrentTime == Stored_Fastest_Opponent_Time ) return Stored_Fastest_Opponent;

  ResetInterceptionMinCyc();
  SetInterceptionLookahead(LA_BestSoFar);

  Unum FastestPlayer = Unum_Unknown;
  int cycles, min_cycles = CP_max_int_lookahead+1;
  for (int i=1; i<=SP_team_size; i++){
    if ( OpponentPositionValid(i) && OpponentInterceptionAble(i) == TRUE &&
	 (cycles=OpponentInterceptionNumberCycles(i)) < min_cycles ){
      min_cycles = cycles;
      FastestPlayer = i;
    }
  }

  Stored_Fastest_Opponent = FastestPlayer;
  Stored_Fastest_Opponent_Time = CurrentTime;

  return FastestPlayer;
}

/*****************************************************************************************/
Unum ActionInfo::BallPossessor(){

  if (!BallPositionValid()) {
    //my_error("BallPossesor: ball position not valid");
    return Unum_Unknown;
  }

  Unum num_with_ball = PlayerWithBall();
  if (num_with_ball != Unum_Unknown)
    return num_with_ball;

  Unum fastestTeammate, fastestOpponent;
  
  if (BallMoving()) {
    int teamCycles, oppCycles;
    fastestOpponent = FastestOpponentToBall();
    fastestTeammate = FastestTeammateToBall(); 
  
    if (fastestTeammate == Unum_Unknown ||
	fastestOpponent == Unum_Unknown) 
      return (fastestTeammate == Unum_Unknown ? -fastestOpponent : fastestTeammate);

    teamCycles = TeammateInterceptionNumberCycles(fastestTeammate); 
    oppCycles = OpponentInterceptionNumberCycles(fastestOpponent); 

    if (teamCycles + CP_possessor_intercept_space < oppCycles)
      return fastestTeammate;
    else if (oppCycles + CP_possessor_intercept_space < teamCycles)
      return -fastestOpponent;
  } else {
    fastestTeammate = ClosestTeammateToBall();
    fastestOpponent = ClosestOpponentToBall();

    if (fastestTeammate == Unum_Unknown ||
	fastestOpponent == Unum_Unknown) 
      return (fastestTeammate == Unum_Unknown ? -fastestOpponent : fastestTeammate);

    /* we'll just ignore facing angles because they probably aren't right anwyay */;
    if (TeammateAbsolutePosition(fastestTeammate).dist(BallAbsolutePosition()) <
	OpponentAbsolutePosition(fastestOpponent).dist(BallAbsolutePosition()))
      return fastestTeammate;
    else
      return -fastestOpponent;
  }

  return Unum_Unknown;
}

 
/*****************************************************************************************/

char ActionInfo::TeamInPossession()
{
  switch ( PlayMode ){
  case PM_Play_On: break;
  case PM_My_Kick_In:
  case PM_My_Corner_Kick:
  case PM_My_Kick_Off:
  case PM_My_Free_Kick:
  case PM_My_Goalie_Free_Kick:
  case PM_My_Offside_Kick:
  case PM_My_Goal_Kick: return MySide;
  case PM_Their_Kick_In:
  case PM_Their_Corner_Kick:
  case PM_Their_Goal_Kick:
  case PM_Their_Kick_Off:
  case PM_Their_Offside_Kick:
  case PM_Their_Free_Kick: 
  case PM_Their_Goalie_Free_Kick: return TheirSide;
  default: break;
  }

  Unum player = BallPossessor();
  if ( player > 0 ) return MySide;
  else if (player < 0) return TheirSide;
  else return '?';
}

