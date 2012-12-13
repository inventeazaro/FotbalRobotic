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


#ifndef _MEMACTION_H_
#define _MEMACTION_H_

#include "MemPosition.h"
#include "soccer_utils.h"

/****************************************************************************************/

typedef struct PLAYERINTERCEPTINFO 
{
  Time time;
  float dash_pow;
  int lookahead;
  InterceptRes res;
  int numCyc;
  Vector pos;
  float dash_pow_to_use;
} PlayerInterceptInfo;

typedef struct TURNKICKCOMMAND 
{
  CMDType type;
  Time time;
  float angle;
  float power;  
  Bool turn_neck;
  float turn_neck_angle;
} TurnKickCommand;

const int LA_Default = -2;
const int LA_BestSoFar = -1;




/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class ActionInfo : public PositionInfo {
public:

  void Initialize();

  void UpdateActionMode();
  ActionMode MyActionMode;

  // these are for hard kicking 
  Time HKTime;
  int HKStep;
  int HKStepNext;
  TurnDir HKrot;

  float VelAtPt2VelAtFoot(Vector pt, float targ_vel_at_pt)
  { return VelAtPt2VelAtFoot_sh(pt, targ_vel_at_pt, MyPos(), SP_ball_decay); }
  
  /* these are for ball interception */
  InterceptRes PlayerInterceptionResult(char side, Unum num, float dash_pow);
  InterceptRes PlayerInterceptionResult(char side, Unum num)
    { return PlayerInterceptionResult(side, num, 
				      (side==MySide && TeammateTired(num)) ? Teammate_stamina_inc(num) : SP_max_power); }
  Bool PlayerInterceptionAble(char side, Unum num, float dash_pow);
  Bool PlayerInterceptionAble(char side, Unum num)
    { return PlayerInterceptionAble(side, num, 
				    (side==MySide && TeammateTired(num)) ? Teammate_stamina_inc(num) : SP_max_power); }
  int PlayerInterceptionNumberCycles(char side, Unum num, float dash_pow);
  int PlayerInterceptionNumberCycles(char side, Unum num)
    { return PlayerInterceptionNumberCycles(side, num, 
					    (side==MySide&&TeammateTired(num)) ? Teammate_stamina_inc(num):SP_max_power); }
  Vector PlayerInterceptionPoint(char side, Unum num, float dash_pow);
  Vector PlayerInterceptionPoint(char side, Unum num)
    { return PlayerInterceptionPoint(side, num, 
				     (side==MySide && TeammateTired(num)) ? Teammate_stamina_inc(num) : SP_max_power); }
  float PlayerInterceptionDashPower(char side, Unum num, float dash_pow);
  float PlayerInterceptionDashPower(char side, Unum num)
    { return PlayerInterceptionDashPower(side, num, 
					 (side==MySide && TeammateTired(num)) ? Teammate_stamina_inc(num) : SP_max_power); }
  
  InterceptRes TeammateInterceptionResult(Unum num, float dash_pow)
    { return PlayerInterceptionResult(MySide, num, dash_pow); }
  InterceptRes TeammateInterceptionResult(Unum num)
    { return (num == MyNumber) ?
	MyInterceptionResult() :
      TeammateInterceptionResult(num, TeammateTired(num) ? Teammate_stamina_inc(num) : SP_max_power);
    }
  Bool TeammateInterceptionAble(Unum num, float dash_pow)
    { return PlayerInterceptionAble(MySide, num, dash_pow); }
  Bool TeammateInterceptionAble(Unum num)
    { return (num == MyNumber) ?
	MyInterceptionAble() :
      TeammateInterceptionAble(num, TeammateTired(num) ? Teammate_stamina_inc(num) : SP_max_power);
    }
  int TeammateInterceptionNumberCycles(Unum num, float dash_pow)
    { return PlayerInterceptionNumberCycles(MySide, num, dash_pow); }
  int TeammateInterceptionNumberCycles(Unum num)
    { return  (num == MyNumber) ?
	MyInterceptionNumberCycles() :
      TeammateInterceptionNumberCycles(num, TeammateTired(num) ? Teammate_stamina_inc(num) : SP_max_power);
    }
  Vector TeammateInterceptionPoint(Unum num, float dash_pow)
    { return PlayerInterceptionPoint(MySide, num, dash_pow); }
  Vector TeammateInterceptionPoint(Unum num)
    { return  (num == MyNumber) ?
	MyInterceptionPoint() :
      TeammateInterceptionPoint(num, TeammateTired(num) ? Teammate_stamina_inc(num) : SP_max_power); }
  float TeammateInterceptionDashPower(Unum num, float dash_pow)
    { return PlayerInterceptionDashPower(MySide, num, dash_pow); }
  float TeammateInterceptionDashPower(Unum num)
    { return  (num == MyNumber) ?
	MyInterceptionDashPower() :
      TeammateInterceptionDashPower(num, TeammateTired(num) ? Teammate_stamina_inc(num) : SP_max_power);
    }

  InterceptRes OpponentInterceptionResult(Unum num, float dash_pow)
    { return PlayerInterceptionResult(TheirSide, num, dash_pow); }
  InterceptRes OpponentInterceptionResult(Unum num)
    { return OpponentInterceptionResult(num, SP_max_power); }
  Bool OpponentInterceptionAble(Unum num, float dash_pow)
    { return PlayerInterceptionAble(TheirSide, num, dash_pow); }
  Bool OpponentInterceptionAble(Unum num)
    { return OpponentInterceptionAble(num, SP_max_power); }
  int OpponentInterceptionNumberCycles(Unum num, float dash_pow)
    { return PlayerInterceptionNumberCycles(TheirSide, num, dash_pow); }
  int OpponentInterceptionNumberCycles(Unum num)
    { return OpponentInterceptionNumberCycles(num, SP_max_power); }
  Vector OpponentInterceptionPoint(Unum num, float dash_pow)
    { return PlayerInterceptionPoint(TheirSide, num, dash_pow); }
  Vector OpponentInterceptionPoint(Unum num)
    { return OpponentInterceptionPoint(num, SP_max_power); }
  float OpponentInterceptionDashPower(Unum num, float dash_pow)
    { return PlayerInterceptionDashPower(TheirSide, num, dash_pow); }
  float OpponentInterceptionDashPower(Unum num)
    { return OpponentInterceptionDashPower(num, SP_max_power); }

  InterceptRes MyInterceptionResult(float dash_pow)
    { return PlayerInterceptionResult(MySide, MyNumber, dash_pow); }
  InterceptRes MyInterceptionResult()
    { return MyInterceptionResult(CorrectDashPowerForStamina(SP_max_power)); }
  Bool MyInterceptionAble(float dash_pow)
    { return PlayerInterceptionAble(MySide, MyNumber, dash_pow); }
  Bool MyInterceptionAble()
    { return MyInterceptionAble(CorrectDashPowerForStamina(SP_max_power)); }
  int MyInterceptionNumberCycles(float dash_pow)
    { return PlayerInterceptionNumberCycles(MySide, MyNumber, dash_pow); }
  int MyInterceptionNumberCycles()
    { return MyInterceptionNumberCycles(CorrectDashPowerForStamina(SP_max_power)); }
  Vector MyInterceptionPoint(float dash_pow)
    { return PlayerInterceptionPoint(MySide, MyNumber, dash_pow); }
  Vector MyInterceptionPoint()
    { return MyInterceptionPoint(CorrectDashPowerForStamina(SP_max_power)); }
  float MyInterceptionDashPower(float dash_pow)
    { return PlayerInterceptionDashPower(MySide, MyNumber, dash_pow); }
  float MyInterceptionDashPower()
    { return MyInterceptionDashPower(CorrectDashPowerForStamina(SP_max_power)); }

  /* just min of what's been done so far - returns -1 if nothing done */
  int GetInterceptionMinCyc();
  inline void ResetInterceptionMinCyc()  { IntMinCycTime -= 1; }
  inline int  GetInterceptionLookahead() { return InterceptLookahead; }
  void SetInterceptionLookahead(int newval);

  Bool BallPathInterceptValid();
  Vector BallPathInterceptPoint();
  Bool BallPathInterceptAmIThere(float buffer);
  Bool BallPathInterceptAmIThere()
    { return BallPathInterceptAmIThere(CP_at_point_buffer); }
  float BallPathInterceptDistance();
  /* careful! if ball is kickable, next func returns 0 */
  int BallPathInterceptCyclesForBall();
  Bool BallPathInterceptCanIGetThere(float max_pow = 100.0);

  KickMode BestKickModeAbs(AngleDeg abs_ang);
  KickMode BestKickMode(AngleDeg rel_ang)  /* Angle relative to body */
    { return BestKickModeAbs(GetNormalizeAngleDeg(rel_ang + MyBodyAng())); }

  int EstimatedCyclesToSteal(Unum opp, Vector ball_pos);  
  inline int EstimatedCyclesToSteal(Unum opp, AngleDeg ball_ang) //absolute angle
    { return EstimatedCyclesToSteal(opp, MyPos() +
				    Polar2Vector(CP_opt_ctrl_dist, ball_ang));
    }
  inline int EstimatedCyclesToSteal(Unum opp)
    {
      if (!BallPositionValid())
	my_error("EstimateCyclesToSteal: don;t know where ball is");
      return EstimatedCyclesToSteal(opp, BallAbsolutePosition());
    }


  Bool WillDashHelpKick(Vector pt, float dash_pow);
  Bool WillDashHelpKick(Vector pt)
    { return WillDashHelpKick(pt, SP_max_power); }

  
  Bool KickInProgress(); 
  void StartKick(AngleDeg target_angle, KickMode mode, float target_vel, TurnDir rot=TURN_AVOID);
  void StartShot(AngleDeg target_angle, KickMode mode, TurnDir rot=TURN_AVOID);
  void StartPass(Unum target, float target_vel, TurnDir rot=TURN_AVOID);  
  void AbortKickInProgress() { kick_in_progress = FALSE; }
  
  AngleDeg       kick_in_progress_abs_angle;
  float          kick_in_progress_target_vel;
  KickMode       kick_in_progress_mode;
  TurnDir        kick_in_progress_rotation;

  Unum team_receiver;
  Vector pass_to_point;
  Unum team_passer; 
  Time team_pass_time;

  Bool goalie_after_catch;
  Bool goalie_moved_after_catch;
  Time goalie_last_catch_time;
  Time GoalieWarnTime;

  Unum FastestTeammateToBall();
  Unum FastestOpponentToBall();
  Unum BallPossessor();   /* possessor means can get there quickest */
  char TeamInPossession();
  
  Unum LastBallPossessor;
  Time LastBallPossessorTime;

  inline void SetDribbleTime(Time t) { dribble_time = t;     }
  inline Time GetDribbleTime()       { return dribble_time;  }
  inline Bool DribbleInProgress() { return (dribble_time == CurrentTime-1 ? TRUE : FALSE); }
  inline void SetDribbling(Vector target, float dash_power, float buffer) 
    { SetDribbleTime(CurrentTime); dribble_in_progress_target = target;
    dribble_in_progress_dash_power = dash_power; dribble_in_progress_buffer = buffer; }

  Vector dribble_in_progress_target;
  float  dribble_in_progress_dash_power;
  float  dribble_in_progress_buffer;

  Time ChangeViewForHandleBallTime;

  PlayerInterceptInfo  ActiveCanGetThere(float max_pow, int max_lookahead,
					 Vector vBallPos, Vector vBallVel,
					 char side, Unum num,
					 Vector vPlayerPos, Vector vPlayerVel,
					 float vPlayerAng, int PlayerAngValid,
					 bool IsThisMe);

private:
  Bool           kick_in_progress;
  Time           start_kick_time;
  Time           kick_in_progress_time;

  Time           dribble_time; /* Last time we called DribbleTo */

  Time           DribbleTargetTime;
  Vector         DribbleTargetVector;
  Bool           DribbleTargetAvailable;

  PlayerInterceptInfo* TeamIntInfo[MAX_PLAYERS];
  PlayerInterceptInfo* OppIntInfo[MAX_PLAYERS];

  Time Stored_Fastest_Teammate_Time;
  Unum Stored_Fastest_Teammate;
  Time Stored_Fastest_Opponent_Time;
  Unum Stored_Fastest_Opponent;

  int InterceptLookahead; /* can either be a positve number or a LA_constant
			     from above */
  int IntMinCyc;
  Time IntMinCycTime;
  void SetIntMinCyc(int newval);

  void BallIntercept_active(float max_pow_to_use, int max_lookahead,
			    char PlayerSide, Unum PlayerNum,
			    PlayerInterceptInfo* pInfo);
  PlayerInterceptInfo CloseBallInterception(float max_pow, int max_lookahead,
					    Vector vBallPos, Vector vBallVel);

  PlayerInterceptInfo* GetPlayerIntInfo(char side, Unum num);
  PlayerInterceptInfo* VerifyIntInfo(char side, Unum num, float dash_pow);

  bool IsSuccessRes(InterceptRes res)
    { return (res == BI_CanChase || res == BI_ReadyToKick); }

  Time BPItime;
  Bool BPIvalid;
  Bool BPIable; 
  Bool BPIballable;
  float BPIdist;
  Vector BPIpoint;
  float BPIballcyc;

  void VerifyBPIInfo();
  int GetClosestPointToBallPath(Vector* pvPt, float* pNumCycles,
				Bool *pBallAble,
				Vector PlayerPos, Vector BallPos,
				Vector BallVel);
  void BallIntercept_passive(float max_pow_to_use, 
			     PlayerInterceptInfo* pInfo);      
};

inline Bool ActionInfo::BallPathInterceptValid()
{
  VerifyBPIInfo();
  return BPIvalid;
}

#endif
