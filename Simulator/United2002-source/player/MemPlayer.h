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


#ifndef __player_h
#define __player_h

#include "netif.h"
#include "types.h"
#include "MemOption.h"
#include "utils.h"
#include "geometry.h"

#define MAX_PLAYER_TYPES 25

class Command {
public:

  CMDType type;
  
  float power;
  float angle;
  Vqual qual;
  Vwidth width;
  char side;
  Unum num;
  Vector dest;

  char  command[MAXMESG];
  Time  time;

  Command() { type = CMD_none; }

  inline Bool valid() { return (Bool) (type != CMD_none); }
  inline Bool valid(Time t) { return (Bool) (type != CMD_none && t == time); }
};



/*****************************************/
/*****************************************/
/*****************************************/

class PlayerInfo : public OptionInfo{
public:

  PlayerInfo();
  virtual ~PlayerInfo();
  void Initialize();

  void FlushFiles();

  void SetPlayMode(Pmode mode);
  void sanitize_time(Time &tm);

  void EstimateMyPos();
  void EstimateMyVel(Time time);
  Vector NewVelFromDash(Vector old_vel, float dash_power);
  void UpdateFromMyAction(Time time);
  void update_self_estimate(Time time);
  void update_self_neck_rel_ang(Time time);
  void update_stamina(Time time);
  void reset_stamina();
  Time update_time(int time);

  virtual void VerifyDash(float *dash_power);

  //called by the stuff in soccer_utils
  void LogAction_worker(int level, const char* str);
  
  Socket           *sock;

  char             SaveLogFileName[MAX_FILE_LEN];
  FILE             *SaveLogFile;
  int              SaveLogCounter;
  char             SaveSoundLogFileName[MAX_FILE_LEN];
  FILE             *SaveSoundLogFile;
  int              SaveSoundLogCounter;
  char             SaveActionLogFileName[MAX_FILE_LEN];
  FILE             *SaveActionLogFile;
  int              SaveActionLogCounter;

  char             MySide;            // 'l' or 'r'                          
  char             TheirSide;         // 'l' or 'r'                          
  char             TheirTeamName[MAX_TEAMNAME_LEN];    // The name of their team                 
  int              MyTeamNameLen;     // strlen of MyTeamName
  Unum		   MyNumber;          // uniform number                      
  int              Initialized;					   
  Bool             ServerAlive;       // is the server going?
  Bool             TestVersion;       // is this the test version of the program?			   
  Bool             CoachActive;       // is there an on-line coach? 

  int              TimerInterval;
  Bool             ClockStopped;
  int              StoppedClockMSec;  // time elapsed in before_kick_off_mode (msec)

  Time             CurrentTime;       // Most recently observed time         
  Time             LastTime;          // previous time				   
  Time             LastSightTime;     // last time I saw			      
  Time             LastSenseTime;     // last time I sensed			   
  Time             LastSoundTime;     // last time I heard		      	   

  int              SightsThisCycle;   // number of times I've seen something this cycle
  int              SightsPrevCycle;   // number of times I saw something last cycle
  int              SightsPrev2Cycle;  // number of times I saw something 2 cycles ago
  Bool             Tuned;             // am I tuned to get senses at the right time?

  //this is needed for compatibility with coach/trainer memory
  int             GetTime() { return CurrentTime.t; }

  /* Don't need these after server 5.23 
  void SetRealTimeOfLastSend(struct timeval& tv)
    { real_time_of_last_send = tv;}
  struct timeval GetRealTimeOfLastSend()
    { return real_time_of_last_send;}
  Bool TooSoonForAnotherSend();
  */
  
  int              LastSightInterval; // cycles between last 2 sights 
  inline Time      PreviousSightTime() { return LastSightTime - LastSightInterval; }  // previous time I saw 

  Command       *Action;
  Command       *LastAction;
  Command       ChangeView;
  Command       TurnNeck;
  Command       AttentionTo;
  Command       PointTo;
  Command       Ear;

  inline Bool   ResendNeeded() 
    { return (RequestResend && CurrentTime == LastActionTime() 
	      && ResendTime == CurrentTime && LastActionType() == ResendType) ? TRUE : FALSE; }
  Bool          RequestResend;
  CMDType       ResendType;
  Time          ResendTime;

  inline AngleDeg LastActionAngle() { return LastAction->angle; }
  inline float    LastActionPower() { return LastAction->power; }
  inline Vector   LastActionDest()  { return LastAction->dest; }
  inline CMDType  LastActionType()  { return LastAction->type; }
  inline Time     LastActionTime()  { return LastAction->time; }
  inline Bool     LastActionValid() { return LastAction->valid(); }
  inline Bool     LastActionValid(Time t) { return LastAction->valid(t); }
  Time            LastChangeViewTime;

  /* have I already called a turn-neck? */
  inline Bool TurnNeckThisCycle() { return TurnNeck.time == CurrentTime ? TRUE : FALSE; }

  Time             LastBehaveTime;           // last time into behave		   	  
  Time             LastActionOpTime;         // last time I could have acted	     
  Time             LastInterruptTime;        // last time into the alarm handler
  int              InterruptsThisCycle;	     // number of times in so far					  
  Time             LastStartClockTime;       // time when server clock started again
  Time             SecondLastStartClockTime; // time when server clock started again

  Bool             NewSight;						      
  Bool             NewAction;
  Bool             FirstActionOpSinceLastSight;
  Bool             SightPredictedEarlyThisCycle();
  Bool             GotSightFromCurrentPosition();
  inline Bool      TimeToTurnForScan() 
    { return (SightPredictedEarlyThisCycle() || GotSightFromCurrentPosition()) ? TRUE : FALSE; }

  SenseType        LastSenseType;					   

  Vqual            ViewQuality;
  Vwidth           ViewWidth;
  Vwidth           LastViewWidth;
  Time             ViewWidthTime;
  
  char             AttentionSide;
  Unum             AttentionNum;
  Vector           PointDest;

  Pmode            PlayMode;
  Time             PlayModeTime;
  Pmode            LastPlayMode;
  Kmode            KickOffMode;    
  int              MyScore;
  int              TheirScore;

  AngleDeg         MyViewAngle(Time time);
  inline AngleDeg  MyViewAngle() { return MyViewAngle(CurrentTime); }
  Bool             InViewAngle(Time time, AngleDeg ang, float buffer=5.0);
  inline Bool      InViewAngle(AngleDeg ang, float buffer=5.0) { return InViewAngle(CurrentTime,ang,buffer); }
  int              MySightInterval();
  int              PredictedNextSightInterval();

  inline void      SetMyPos(Vector p, Time t)   { pos = p; conf = 1; my_pos_time = t;}
  inline void      SetMyBodyAng(AngleDeg a)     { body_ang = a; }
  inline void      SetMyNeckRelAng(AngleDeg a)  { neck_rel_ang = a; }
  inline void      SetMyVel(Vector v, Time t)   { vel = v; vel_conf = 1; my_vel_time = t; }
  
  void  SetMySensedInfo(float st, float e, float sp, float h_rel_n, float na, int k, int d, int tu, int sa, int tn, int c, int m, int cv, int am, int ae, float atd, float ata, int pt, char fs, int fn, int fc, int te, int tc, Time ti);

  float GetMySensedSpeed(Time time);
  float GetMySensedHeadingRelToNeckAngle(Time time);
  float GetMySensedStamina(Time time);
  float GetMySensedEffort(Time time);
  float GetMySensedNeckAngle(Time time);
  int   GetMySensedKicks(Time time);
  int   GetMySensedDashes(Time time);
  int   GetMySensedTurns(Time time);
  int   GetMySensedSays(Time time);
  int   GetMySensedTurnNecks(Time time);

  inline float MyStamina()  { return stamina; }
  inline float MyEffort()   { return effort; }
  inline float MyRecovery() { return recovery; }
  inline Bool  Tired()      
    { return ( MyStamina() < SayTiredThreshold ) ? TRUE : FALSE; }

  float RecoveryDecThreshold;
  float EffortDecThreshold;
  float EffortIncThreshold;
  float SayTiredThreshold;
  float CorrectDashPowerForStamina(float dash_power, float stamina, float effort, float recovery);
  inline float CorrectDashPowerForStamina(float dash_power, float stamina)
    { return CorrectDashPowerForStamina(dash_power, stamina, MyEffort(), MyRecovery()); }
  inline float CorrectDashPowerForStamina(float dash_power)
    { return CorrectDashPowerForStamina(dash_power, MyStamina(), MyEffort(), MyRecovery()); }
  void UpdatePredictedStaminaWithDash(float* pStamina, float* pEffort,
				      float* pRecovery, float dash_power);
  inline AngleDeg EffectiveTurn(AngleDeg actual_turn, float my_speed)
    { return actual_turn/(1.0 + My_inertia_moment() * my_speed); }
  inline AngleDeg EffectiveTurn(AngleDeg actual_turn) 
    { return EffectiveTurn(actual_turn,MySpeed()); }
  inline AngleDeg MaxEffectiveTurn() /* how much we'll actually turn if we try max turn */
    { return EffectiveTurn(SP_max_moment); }
  inline AngleDeg MaxEffectiveTurn(float my_speed) /* how much we'll actually turn if we try max turn */
    { return EffectiveTurn(SP_max_moment, my_speed); }

  Bool CanFaceAngleFromNeckWithNeck(AngleDeg ang);
  Bool CanFaceAngleFromBodyWithNeck(AngleDeg ang);
  inline Bool CanFaceGlobalAngleWithNeck  (AngleDeg ang)
    { return CanFaceAngleFromBodyWithNeck(GetNormalizeAngleDeg(ang - MyBodyAng())); }
  inline Bool CanFacePointWithNeck(Vector pt) 
    { return CanFaceAngleFromBodyWithNeck(AngleToFromBody(pt)); }

  Bool CanSeeAngleFromNeckWithNeck(AngleDeg ang);
  Bool CanSeeAngleFromBodyWithNeck(AngleDeg ang);
  inline Bool CanSeeGlobalAngleWithNeck  (AngleDeg ang)
    { return CanSeeAngleFromBodyWithNeck(GetNormalizeAngleDeg(ang - MyBodyAng())); }
  inline Bool CanSeePointWithNeck(Vector pt) 
    { return CanSeeAngleFromBodyWithNeck(AngleToFromBody(pt)); }

  inline AngleDeg LimitTurnNeckAngle(AngleDeg turn_ang)
    { return MinMax(SP_min_neck_angle - MyNeckRelAng(), GetNormalizeAngleDeg(turn_ang),
		    SP_max_neck_angle - MyNeckRelAng()); }
  
  inline Bool SensedInfoKnown(Time time) { return (Bool) (sense_time == time || prev_sense_time == time); }

  inline Vector    MyPos()   { if (!MyConf())    my_error("Don't know my pos"); return pos; }
  inline float     MyX()     { return MyPos().x; }
  inline float     MyY()     { return MyPos().y; }
  inline AngleDeg  MyBodyAng() { return body_ang; }
  inline AngleDeg  MyNeckGlobalAng() { return GetNormalizeAngleDeg(MyBodyAng() + MyNeckRelAng()); }
  inline AngleDeg  MyNeckRelAng() { return neck_rel_ang; }
  inline Vector    MyVel()   { if (!MyVelConf()) my_error("Don't know my vel"); return vel; }
  inline float     MySpeed() { return MyVel().mod(); }
  inline float     MyDir()   { return MyVel().dir(); }
  inline float     MyConf()  { return (conf > CP_min_valid_conf) ? conf : 0; }
  inline float     MyVelConf() { return (vel_conf > CP_min_valid_conf) ? vel_conf : 0; }
  inline Time      MyPosTime() { return my_pos_time; }
  inline Time      MyVelTime() { return my_vel_time; }
  inline Time      MyUpdateTime() { return Min(my_pos_time,my_vel_time); }

  inline float     DistanceTo(Vector vec)  
    { if (!MyConf()) my_error("DistanceTo: no conf"); return (vec - pos).mod(); }
  inline float     Distance2To(Vector vec)  
    { if (!MyConf()) my_error("DistanceTo: no conf"); Vector v = vec-pos; 
    return v.x*v.x+v.y*v.y; }
  inline AngleDeg  AngleToFromBody   (Vector vec)
    { if (!MyConf()) my_error("AngleTo: no conf"); 
      AngleDeg angto = (vec - pos).dir() - MyBodyAng(); NormalizeAngleDeg(&angto); return angto; }
  inline AngleDeg  AngleToFromNeck   (Vector vec)
    { if (!MyConf()) my_error("AngleTo: no conf"); 
      AngleDeg angto = (vec - pos).dir() - MyNeckGlobalAng(); NormalizeAngleDeg(&angto); return angto; }
  inline AngleDeg  AngleToGlobal   (Vector vec)
    { if (!MyConf()) my_error("AngleTo: no conf"); 
      AngleDeg angto = (vec - pos).dir(); NormalizeAngleDeg(&angto); return angto; }
  inline Vector    BodyPolar2Gpos(float dist, AngleDeg ang) 
    { if (!MyConf()) my_error("Polar2Gpos: no conf"); 
      Vector rpos = Polar2Vector(dist,ang); return MyPos() + rpos.rotate(MyBodyAng()); }
  inline Vector    NeckPolar2Gpos(float dist, AngleDeg ang) 
    { if (!MyConf()) my_error("Polar2Gpos: no conf"); 
      Vector rpos = Polar2Vector(dist,ang); return MyPos() + rpos.rotate(MyNeckGlobalAng()); }

  inline Bool AtPos(Vector p, float buffer) 
    { return (DistanceTo(p) <= buffer) ? TRUE : FALSE; }
  inline Bool AtPos(Vector p)
    { return AtPos(p, CP_at_point_buffer); }
  
  inline Vector Global2RelativeToMyBody(Vector v)  { return v.Global2Relative(MyPos(), MyBodyAng()); } 
  inline Vector Global2RelativeToMyNeck(Vector v)  { return v.Global2Relative(MyPos(), MyNeckGlobalAng()); } 
  inline Vector RelativeToMyBody2Global(Vector v)  { return v.Relative2Global(MyPos(), MyBodyAng()); } 
  inline Vector RelativeToMyNeck2Global(Vector v)  { return v.Relative2Global(MyPos(), MyNeckGlobalAng()); } 

  /* Cone stuff */
  Bool AmIInCone(float wid_dist_ratio, Vector end, Vector vert)
    { return IsPointInCone(MyPos(), wid_dist_ratio, end, vert); }
  
  /* Predicted Position */
  Vector MyPredictedPositionWithTurn(float angle,
				     int steps = 1, float dash_power = 0,
				     bool with_turn = TRUE,
				     int idle_cycles = 0);
  inline Vector MyPredictedPosition(int steps = 1, float dash_power = 0,
				    int idle_cycles = 0)
    { return MyPredictedPositionWithTurn(0.0, steps, dash_power, FALSE, idle_cycles); }
  Vector MyPredictedPositionWithQueuedActions();
  AngleDeg MyPredictedBodyAngleWithQueuedActions();
  AngleDeg PredictedPointRelAngFromBodyWithQueuedActions(Vector point);

  Vector MyPredictedPositionAtMaxSpeed(int steps = 1);

  /* How long to point */
  /* Note that you can pass a negative dash_power to get the player to run backwards */
  int PredictedCyclesToPoint(Vector pt, float dash_power, float* endStamina = NULL);
  inline int PredictedCyclesToPoint(Vector pt, float* endStamina = NULL)
    { return PredictedCyclesToPoint(pt, SP_max_power, endStamina); }

  /* how long it takes to turn to an angle */
  int NumTurnsToAngle(float targ_body_ang, float curr_body_ang, float curr_speed);
  int NumTurnsToAngle(float targ_body_ang, float curr_body_ang)
    { return NumTurnsToAngle(targ_body_ang, curr_body_ang, MySpeed()); }
  int NumTurnsToAngle(float targ_body_ang)
    { return NumTurnsToAngle(targ_body_ang, MyBodyAng(), MySpeed()); }
  
  /* Which side of the field am I on? */
  inline Fside LocationSide(Vector p) { return p.y < 0 ? FS_Left : FS_Right; } 
  inline Fside MyLocationSide() { return LocationSide(MyPos()); }

  int              kicks;
  int              dashes;
  int              turns;
  int              says;
  int              turn_necks;
  int              catches;
  int              moves;
  int              change_views;
  int              arm_busy;
  int              arm_expires;
  float            arm_target_dist;
  float            arm_target_ang;
  int              point_tos;
  int              focusses;
  int              tackle_expires;
  int              tackles;

  inline Bool Tackling() { return ((tackle_expires > 0)? TRUE : FALSE); }

  PlayerType       PT[MAX_PLAYER_TYPES];

  inline int   MyPT()               {return player_type;}
  inline void  SetMyPT(int pt )     {player_type = pt;}
  inline float My_speed_max()       {return PT[MyPT()].player_speed_max;}
  inline float My_stamina_inc_max() {return PT[MyPT()].stamina_inc_max;}
  inline float My_stamina_inc()     {return PT[MyPT()].stamina_inc_max;}
  inline float My_decay()           {return PT[MyPT()].player_decay;}
  inline float My_inertia_moment()  {return PT[MyPT()].inertia_moment;}
  inline float My_dash_power_rate() {return PT[MyPT()].dash_power_rate;}
  inline float My_size()            {return PT[MyPT()].player_size;}
  inline float My_kickable_margin() {return PT[MyPT()].kickable_margin;}
  inline float My_kickable_area()   {return My_kickable_margin() + SP_ball_size +
				       My_size();}
  inline float My_kick_rand()       {return PT[MyPT()].kick_rand;}
  inline float My_extra_stamina()   {return PT[MyPT()].extra_stamina;}
  inline float My_effort_max()      {return PT[MyPT()].effort_max;}
  inline float My_effort_min()      {return PT[MyPT()].effort_min;}

protected:
  Time             my_pos_time;
  Time             my_vel_time;

  AngleDeg         my_last_body_ang;
  AngleDeg         my_last_neck_global_ang; 
  AngleDeg         my_last_neck_rel_ang; 

private:
  Vector           pos;
  AngleDeg         body_ang;
  AngleDeg         neck_rel_ang; /* neck angle relative to the body */
  Vector           vel;
  float            conf;
  float            vel_conf;

  int              player_type;

  float            stamina;
  float            effort;
  float            recovery;

  Time             sense_time;
  Time             prev_sense_time;

  float            last_speed; /* from sense_body */
  float            prev_speed;
  float            last_heading_rel_neck;
  float            prev_heading_rel_neck;

  float            last_stamina;
  float            prev_stamina;

  float            last_effort;
  float            prev_effort;

  float            last_neck_rel_ang;
  float            prev_neck_rel_ang;

  int              last_kicks;
  int              prev_kicks;
  int              last_dashes;
  int              prev_dashes;
  int              last_turns;
  int              prev_turns;
  int              last_says;
  int              prev_says;
  int              last_turn_necks;
  int              prev_turn_necks;

  struct timeval   real_time_of_last_send; //the real time of the last send message

};

#endif



