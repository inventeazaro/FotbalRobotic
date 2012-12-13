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


#include "types.h"
#include "MemPlayer.h"
#include "client.h"

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

PlayerInfo::PlayerInfo()
{
  Initialized = FALSE;
  ServerAlive = FALSE;
  CoachActive = FALSE;
  ViewQuality = VQ_High;
  ViewWidth   = VW_Normal;
  NewSight = FALSE;
  NewAction = FALSE;
  FirstActionOpSinceLastSight = FALSE;
  ClockStopped = FALSE;
  StoppedClockMSec = 0;
  LastStartClockTime = Time(-1,0); /* If a problem, change back to 0,0 */
  SecondLastStartClockTime = LastStartClockTime;
  CurrentTime = 0;
  LastSightTime = LastSoundTime = LastActionOpTime = Time(0, 0);
  SightsThisCycle = SightsPrevCycle = SightsPrev2Cycle = 0;
  Tuned = FALSE;
  PlayModeTime = 0;
  PlayMode = PM_Before_Kick_Off;

  Action = new Command;
  LastAction = new Command;
  LastChangeViewTime = Time(-1,0);
  RequestResend = FALSE;

  last_dashes = prev_dashes = dashes = 0;
  last_turns  = prev_turns  = turns  = 0;
  last_kicks  = prev_kicks  = kicks  = 0;
  last_says   = prev_says   = says   = 0;
  last_turn_necks   = prev_turn_necks   = turn_necks   = 0;

  TheirTeamName[0] = '\n';

  conf=0;

  body_ang = 0;
  neck_rel_ang = 0;
  
  player_type = 0;
}

/*********************************************************************************/

PlayerInfo::~PlayerInfo()
{
  if (CP_save_log)
    fclose(SaveLogFile);

  if (CP_save_sound_log)
    fclose(SaveSoundLogFile);

  if (CP_save_action_log_level > 0)
    fclose(SaveActionLogFile);
  
  delete Action;
  delete LastAction;
}

/*********************************************************************************/

void PlayerInfo::Initialize()
{
  /* printf("Calling Player Initialize\n"); */

  TheirSide = ( MySide == 'l' ? 'r' : 'l' );
  MyTeamNameLen = strlen(MyTeamName);

  TestVersion = (VP_test || ( MySide == 'l' && VP_test_l ) || ( MySide == 'r' && VP_test_r )) ? TRUE : FALSE;
  if ( TestVersion == TRUE ) printf("%d : test version\n",MyNumber);
  if ( VP_train_DT == TRUE ) printf("%d : training DT\n",MyNumber);

  MyScore    = IP_my_score;
  TheirScore = IP_their_score;

  if (CP_save_log){
    sprintf(SaveLogFileName,"Logfiles/%s%d-%c.log",MyTeamName,(int)MyNumber,MySide);
    SaveLogFile = fopen(SaveLogFileName,"w");
    SaveLogCounter = 0;
  }

  if (CP_save_sound_log){
    sprintf(SaveSoundLogFileName,"Logfiles/%s%d-%c-sounds.log",MyTeamName,(int)MyNumber,MySide);
    SaveSoundLogFile = fopen(SaveSoundLogFileName,"w");
    SaveSoundLogCounter = 0;
  }

  if (CP_save_action_log_level > 0){
    sprintf(SaveActionLogFileName,"Logfiles/%s%d-%c-actions.log",MyTeamName,(int)MyNumber,MySide);
    SaveActionLogFile = fopen(SaveActionLogFileName,"w");
    SaveActionLogCounter = 0;
  }

  TimerInterval = SP_simulator_step/CP_interrupts_per_cycle;

  stamina = SP_stamina_max;
  effort = 1;
  recovery = 1;

  neck_rel_ang = 0;

  RecoveryDecThreshold = SP_stamina_max * SP_recover_dec_thr;
  EffortDecThreshold   = SP_stamina_max * SP_effort_dec_thr;
  EffortIncThreshold   = SP_stamina_max * SP_effort_inc_thr;
  SayTiredThreshold    = EffortDecThreshold + SP_max_power + CP_say_tired_buffer;

  my_vel_time = my_pos_time = 0;

  LastInterruptTime = 0;
  InterruptsThisCycle = 0;						  

  Initialized = TRUE;
}


void PlayerInfo::FlushFiles()
{
  if (CP_save_log)
    fflush(SaveLogFile);

  if (CP_save_sound_log)
    fflush(SaveSoundLogFile);

  if (CP_save_action_log_level > 0)
    fflush(SaveActionLogFile);
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

void PlayerInfo::LogAction_worker(int level, const char* str)
{
  if (level <= 0 ||
      level > CP_save_action_log_level)
    return;

  if (!Initialized)
    return; /* the log files hasn't been opened yet! */

  fprintf(Mem->SaveActionLogFile, "%d.%d %s%s\n", CurrentTime.t, CurrentTime.s,
	  repeat_char('-', level / 10), str);
  if (Mem->SaveActionLogCounter++ % Mem->CP_save_action_freq == 0){
    fclose(Mem->SaveActionLogFile);
    Mem->SaveActionLogFile = fopen(Mem->SaveActionLogFileName,"a");
  }

}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

void PlayerInfo::SetPlayMode(Pmode mode)
{
  if ( (PlayMode!=PM_My_Goalie_Free_Kick && PlayMode!=PM_Their_Goalie_Free_Kick) || 
       PlayModeTime != CurrentTime ){ /* Already set the play mode for this time */
    LastPlayMode = PlayMode;
    PlayMode = mode;
    PlayModeTime = CurrentTime;
  }

  if (mode == PM_Half_Time) {
    //this check is for reconnect
    if (CurrentTime.t < SP_half_time - 1)
      PlayModeTime = Time(SP_half_time, 0);
    reset_stamina();
  }
  if (mode == PM_Extended_Time) {
    //this check is for reconnect
    if (CurrentTime.t < 2*SP_half_time - 1)
      PlayModeTime = Time(2*SP_half_time, 0);
    reset_stamina();
  }
  if (mode == PM_Before_Kick_Off) {
    //this check is for reconnect
    if (LastPlayMode == PM_Half_Time && CurrentTime.t < SP_half_time - 1)
      PlayModeTime = Time(SP_half_time, 0);
  }

/* in others: */
//    if ( Mem->QActionTaken )
//      Mem->CloseRewards();

}

/*********************************************************************************/

void PlayerInfo::sanitize_time(Time &tm)
{
  if ( !LastStartClockTime ) return;

  /* This is to take care of times that were prematurely updated before we knew that
     the clock was about to start again */
  if ( tm.t == LastStartClockTime.t && tm.s > LastStartClockTime.s ){
    tm = Time( LastStartClockTime.t + (tm.s - LastStartClockTime.s), 0 );
  }
}

/*********************************************************************************/

void PlayerInfo::EstimateMyPos()
{
  /* Takes me from previous time to time */
  if ( MyConf() && MyVelConf() )
    pos += vel;
}

/*********************************************************************************/

void PlayerInfo::EstimateMyVel(Time time)
{
  if ( my_vel_time == time && vel_conf == CP_max_conf )
    return;

  /* Takes me from previous time to time */
  if ( SensedInfoKnown(time) ){
    float old_speed = vel.mod();
    if ( old_speed && SP_version < 6.00 )
      vel = vel * GetMySensedSpeed(time)/old_speed; /* don't change the direction */
    else{ 
      if ( SP_version >= 6.00 )
	vel = Polar2Vector( GetMySensedSpeed(time), 
			    GetNormalizeAngleDeg(GetMySensedHeadingRelToNeckAngle(time)+ 
						 MyNeckGlobalAng()) );
      else
	vel = Polar2Vector( GetMySensedSpeed(time), MyBodyAng() ); /* use my direction */
    }
    vel_conf = CP_max_conf;
  }
  else if ( vel_conf < CP_max_conf && SensedInfoKnown(time-1) ) {
    if ( SP_version >= 6.00 )
      vel = Polar2Vector( GetMySensedSpeed(time-1)*My_decay(), 
			  GetNormalizeAngleDeg(GetMySensedHeadingRelToNeckAngle(time-1)+ 
					       MyNeckGlobalAng()) );
    else
      vel = Polar2Vector( GetMySensedSpeed(time-1)*My_decay(), MyBodyAng() );
    vel_conf = CP_conf_decay;
  }
  else if ( !MyVelConf() )
    return;
  else if ( my_vel_time == time-1 ){
    vel *= My_decay();
    vel_conf *= CP_conf_decay;
  }
  else if ( my_vel_time > time-10 ){  /* missed up to 10 cycles */
    while ( my_vel_time < time && MyVelConf() ){
      vel *= My_decay();
      vel_conf *= CP_conf_decay;
      ++my_vel_time;
    }
  }
  else
    my_error("Having trouble in vel update -- must have missed at least 10 cycles %.1f %.1f    %f",
	     (float)my_vel_time.t,(float)my_vel_time.s,MyVelConf());

  my_vel_time = time;
}

/*********************************************************************************/

Vector PlayerInfo::NewVelFromDash(Vector old_vel, float dash_power)
{
   float effective_power = MyEffort() * dash_power;
   effective_power *= My_dash_power_rate();
   Vector new_vel = old_vel +  Polar2Vector( effective_power, MyBodyAng() );

   if ( new_vel.mod() > My_speed_max() )
     new_vel *= ( My_speed_max()/new_vel.mod() );

   return new_vel;
}

/*********************************************************************************/

void PlayerInfo::VerifyDash(float *dash_power)
{
  /* Check if recovery going down, or max_speed exceeded */
  float available_power, needed_power = *dash_power;
  if ( needed_power < 0 ){ needed_power *= -2; }
  if ( needed_power < 0 ) my_error("power should be positive now");

  float new_stamina = MyStamina() -  MyEffort() * needed_power;
  if ( new_stamina <= SP_recover_dec_thr * SP_stamina_max && recovery > SP_recover_min ){
    /* printf("%d:%d.%d ",MyNumber,CurrentTime.t,CurrentTime.s); */
    printf("WARNING: recovery about to go to %.3f\n",recovery - SP_recover_dec);
    ;
  }
  if ( new_stamina <= SP_effort_dec_thr * SP_stamina_max && effort > My_effort_min() ){
    printf("WARNING: effort about to go to %.2f\n",MyEffort() - SP_effort_dec);
  }
  if ( new_stamina < 0 ){
    /* printf("%d:%d.%d ",MyNumber,CurrentTime.t,CurrentTime.s); */
    /* printf("WARNING: not enough stamina for dash\n"); */
    available_power = (MyStamina()+My_extra_stamina())/MyEffort();
    if ( *dash_power >= 0 ) { *dash_power = available_power; }
    else { *dash_power = -available_power/2; }
  }

  if ( NewVelFromDash( MyVel(), *dash_power ).mod() > My_speed_max() ){
    /* printf("%d:%d.%d ",MyNumber,CurrentTime.t,CurrentTime.s); */
    /* printf("WARNING: can't move that fast (assuming vel and dash in same dir)\n"); */
    /* printf("my speed %f   dash_power %f   ",MySpeed(),*dash_power); */
    float new_power = signf(*dash_power)*(My_speed_max() - MySpeed())/(MyEffort()*My_dash_power_rate());
    LogAction8(100, "VerifyDash, changing power; MyVel=(%.2f, %.2f), NewVel(%.2f, %.2f) old: %.2f, new %.2f",
	       MyVel().x, MyVel().y, 
	       NewVelFromDash( MyVel(), *dash_power ).x,
	       NewVelFromDash( MyVel(), *dash_power ).y,
	       *dash_power, new_power);
    *dash_power = new_power;
    /* printf("new dash_power %f\n",*dash_power); */
  }
}

/*********************************************************************************/

void PlayerInfo::UpdateFromMyAction(Time time)
{
  /* Assume vel and pos are correct for time -- going to time+1 */
  if ( !MyConf() ) my_error("Can't update from action if not localized"); 
  /* But I'm pretty good at estimating... up conf_decay?? */
  if ( !NewAction || !(LastActionValid(time)) ) my_error("No action at that time");

  /* AngleDeg delta_ang, expected_delta; */
  switch(LastActionType()){
  case CMD_turn:
    if ( my_pos_time > time ) break;
    /* be careful not to estimate in a turn that's already been seen -- 
       server updates turns instantaneously */
    /* THIS SHOULDN'T HAPPEN ANYMORE */
    /*     delta_ang = GetNormalizeAngleDeg(ang - my_last_ang); */
    /*     expected_delta = LastActionAngle()/(1.0 + My_inertia_moment() * MySpeed()); */

    /* only if the change is closer to 0 than to the expected change */
    /*     if ( fabs(delta_ang) < fabs(delta_ang-expected_delta) ){ */
    /*        body_ang += expected_delta; */
    body_ang += LastActionAngle()/(1.0 + My_inertia_moment() * MySpeed());
    NormalizeAngleDeg(&body_ang);
    /*     } */
    /*     else */
    /*       my_error("Turns should NOT happen instantaneously anymore"); */
    break;
  case CMD_dash:
    if ( my_vel_time > time ) break;
    vel = NewVelFromDash( vel, LastActionPower() );
    break;
  default: ;
  }
}

/*********************************************************************************/

void PlayerInfo::update_self_estimate(Time time)
{
  update_self_neck_rel_ang(time);

  if ( !MyConf() ){
    vel_conf = 0;   /* If don't know my position, can't know my velocity */
    return;
  }

  if (CP_use_new_position_based_vel) {
    if ( my_pos_time == time ){ /* just vel */
      if ( my_vel_time == time )
	return;
      if ( NewAction && LastActionValid(my_vel_time) )
	UpdateFromMyAction(my_vel_time);
      
      EstimateMyVel(time);
    }
  } else {
    if ( my_pos_time == time ){ /* just vel */
      if ( my_vel_time == time ) my_error("my pos and vel already updated\n");
      if ( NewAction && LastActionValid(my_vel_time) )
	UpdateFromMyAction(my_vel_time);
      
      EstimateMyVel(time);
    }
  }
    
  while ( my_pos_time < time ){
    if ( NewAction && LastActionValid(my_pos_time) )
      UpdateFromMyAction(my_pos_time);

    ++my_pos_time;

    EstimateMyPos();
    EstimateMyVel(time);

    conf *= CP_conf_decay;
  }
}

/*********************************************************************************/

void PlayerInfo::update_self_neck_rel_ang(Time time){

  if ( SensedInfoKnown(time) )
    SetMyNeckRelAng(GetMySensedNeckAngle(time));
  else if ( SensedInfoKnown(time-1) ){
    /* Bring it up to date from the action */
    AngleDeg neck_ang = GetMySensedNeckAngle(time-1);
    if ( TurnNeck.valid(time-1) ){
      neck_ang += TurnNeck.angle;
      if ( neck_ang < SP_min_neck_angle )
	neck_ang = SP_min_neck_angle;
      if ( neck_ang > SP_max_neck_angle )
	neck_ang = SP_max_neck_angle;
    }
    SetMyNeckRelAng(neck_ang);
  }
  else 
    /* could write an "estimate_neck_angle" that updates from the last known time */
    /* could also assume neck unchanged */
    ;/*my_error("Don't know neck angle at time %d.%d or %d.%d",
	     time.t,time.s,(time-1).t,(time-1).s);*/
}


/*********************************************************************************/

void PlayerInfo::update_stamina(Time time)
{
  if ( NewAction && LastActionType() == CMD_dash )
    stamina -= (LastActionPower() > 0) ? LastActionPower() : (-2.0 * LastActionPower());

  if ( stamina < 0 ) stamina = 0;

  if ( stamina <= SP_recover_dec_thr * SP_stamina_max && recovery > SP_recover_min ) {
    recovery -= SP_recover_dec;
  }

  if ( SensedInfoKnown(time) ){
    stamina = GetMySensedStamina(time);
    effort  = GetMySensedEffort(time);
  }
  else {
    if ( stamina <= SP_effort_dec_thr * SP_stamina_max && effort > My_effort_min() ){
      effort -= SP_effort_dec;
      if ( effort < My_effort_min() )
	effort = My_effort_min();
    }
    if (stamina >= SP_effort_inc_thr * SP_stamina_max && effort < 1.0){
      effort += SP_effort_inc;
      if ( effort > My_effort_max() )
	effort = My_effort_max();
    }
    stamina += recovery * My_stamina_inc_max();
    if ( stamina > SP_stamina_max )
      stamina = SP_stamina_max;
  }
  
  if ( stamina < EffortDecThreshold )
    my_error("How did stamina go below EffortDecThreshold?");
}

/*********************************************************************************/

void PlayerInfo::reset_stamina()
{
  stamina = SP_stamina_max;
  recovery = 1.0;
  effort = My_effort_max();
}

/*********************************************************************************/

Time PlayerInfo::update_time(int time)
{
  // executed on a sense_body
  LastTime = CurrentTime;

  if ( time == LastTime.t )
    ClockStopped = TRUE;
  else if ( ClockStopped ){
    /* If clock is starting, save the old time */
    ClockStopped = FALSE;
    SecondLastStartClockTime = LastStartClockTime;
    LastStartClockTime = LastActionOpTime;
    StoppedClockMSec = 0; 

    sanitize_time(CurrentTime);
    sanitize_time(LastSightTime);
    sanitize_time(LastSoundTime);
    sanitize_time(sense_time);
  }

  if ( ClockStopped ){
    if ( CurrentTime.t != time ){
      if ( CurrentTime.t == time - 1 ) /* Sometimes happens in offsides mode */
	CurrentTime = Time(time,0);
      else {
	my_error("server time should be the same %d %d %d",CurrentTime.t, CurrentTime.s, time);
	CurrentTime = Time(time,0);
      }
    }
    else 
      CurrentTime.s++;
  }
  else if ( LastStartClockTime.t == time )
    CurrentTime = LastStartClockTime;
  else
    CurrentTime = Time(time,0);

  if ( Mem->LastSightTime != CurrentTime ){
    SightsPrev2Cycle = Mem->SightsPrevCycle;
    SightsPrevCycle = Mem->SightsThisCycle;
    SightsThisCycle = 0;
  }
  
  return CurrentTime;
}



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

Bool PlayerInfo::SightPredictedEarlyThisCycle()
{
  if ( InterruptsThisCycle > CP_interrupts_per_cycle/2 )
    /* already past the beginning of the cycle */
    return FALSE;

  /* Number of full cycles since last sight * simulator_step */
  if ( MySightInterval() - ((CurrentTime-LastSightTime)-1)*SP_simulator_step <= SP_simulator_step/2 )
    return TRUE;

  return FALSE;
}

/*********************************************************************************/
 
Bool PlayerInfo::GotSightFromCurrentPosition()
{
  if (FirstActionOpSinceLastSight && 
      /* sight from this time or didn't change view angle last time step */
      /* could replace valids with MyNeckGlobalAng() == my_last_neck_global_ang)) */
      /* but when the angle's estimated, it might be off by up to 10 degrees */
      (LastSightTime == CurrentTime || 
       (!LastAction->valid(CurrentTime-1) && !TurnNeck.valid(CurrentTime-1))))
    return TRUE;

  return FALSE;
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

AngleDeg PlayerInfo::MyViewAngle(Time time)
{
  AngleDeg view_angle = SP_visible_angle;
  Vwidth width;

  if ( time < ViewWidthTime ) 
    width = LastViewWidth;
  else 
    width = ViewWidth;

  if ( width == VW_Narrow ) view_angle /= 2;
  else if ( width == VW_Wide ) view_angle *= 2;

  return view_angle/2;
}

/*********************************************************************************/

Bool PlayerInfo::InViewAngle(Time time, AngleDeg ang, float buffer)
{
  if ( fabs(ang) < MyViewAngle(time) - buffer ) return TRUE;
  return FALSE;
}

/*********************************************************************************/

int PlayerInfo::MySightInterval()
{
  int interval = SP_send_step;
  
  if ( ViewWidth == VW_Narrow ) interval /= 2;
  else if ( ViewWidth == VW_Wide ) interval *= 2;

  if ( ViewQuality == VQ_Low ) interval /=2;

  return interval;
}

/*********************************************************************************/

int PlayerInfo::PredictedNextSightInterval()
{
  int interval = MySightInterval();
  if ( interval < SP_simulator_step )   /* 37 or 75 */
    return 1;
  if ( interval == 3*SP_simulator_step )     /* 300 */
    return 3;
  if ( interval == 1.5*SP_simulator_step )   /* 150 */
    return (LastSightInterval <= 1 ? 2 : 1);
  
  my_error("Sight interval should be 37, 75, 150, or 300: %d",MySightInterval());
  return 0;
}

/*********************************************************************************/

void PlayerInfo::SetMySensedInfo(float st, float e, float sp, float h_rel_n, float na, int k, int d, int tu, int sa, int tn, int c, int m, int cv, int am, int ae, float atd, float ata, int pt, char fs, int fn, int fc, int te, int tc, Time ti)
{
  if ( sense_time == ti )
    return;

  prev_sense_time = sense_time;
  sense_time      = ti;

  prev_stamina    = last_stamina;
  last_stamina    = st;
  prev_effort     = last_effort;
  last_effort     = e;
  prev_speed      = last_speed;
  last_speed      = sp;
  if ( SP_version >= 6.00 ){
    prev_heading_rel_neck  = last_heading_rel_neck;
    last_heading_rel_neck  = h_rel_n;
  }
  prev_neck_rel_ang = last_neck_rel_ang;
  last_neck_rel_ang = na;

//  neck_rel_ang    = na;  /** Want to do this here??? No! **/

  prev_kicks     = last_kicks;
  last_kicks     = k;
  if ( last_kicks != kicks ){
    if (!ClockStopped)
      my_error("Server missed a kick at time %d (%d %d)",prev_sense_time.t,last_kicks,kicks);
    LastAction->type = CMD_none;
    kicks = last_kicks;
    Mem->GetBall()->forget_past_kick(LastAction->time);
    /* RequestResend = TRUE;
       ResendType    = CMD_kick;
       ResendTime    = LastActionTime(); */
  }

  prev_dashes    = last_dashes;
  last_dashes    = d;
  if ( last_dashes != dashes ){
    if (!ClockStopped)
      my_error("Server missed a dash at time %d (%d %d)",prev_sense_time.t,last_dashes,dashes);
    LastAction->type = CMD_none;
    dashes = last_dashes;
    /* RequestResend = TRUE;
       ResendType   = CMD_dash;
       ResendTime    = LastActionTime(); */
  }

  prev_turns     = last_turns;
  last_turns     = tu;
  if ( last_turns != turns ){
    if (!ClockStopped)
      my_error("Server missed a turn at time %d (%d %d)",prev_sense_time.t,last_turns,turns);
    LastAction->type = CMD_none;
    turns = last_turns;
    /* RequestResend = TRUE;
       ResendType   = CMD_turn;
       ResendTime    = LastActionTime(); */
  }

  prev_turn_necks     = last_turn_necks;
  last_turn_necks     = tn;
  if ( last_turn_necks != turn_necks ){
    if (!ClockStopped)
      my_error("Server missed a turn_neck at time %d (%d %d)",prev_sense_time.t,last_turn_necks,turn_necks);
    TurnNeck.type = CMD_none;
    turn_necks = last_turn_necks;
    /* RequestResend = TRUE;
       ResendType   = CMD_turn;
       ResendTime    = LastActionTime(); */
  }

  prev_says      = last_says;
  last_says      = sa;
  if ( last_says != says ){
    says = last_says;
  }

  catches = c;
  moves = m;
  change_views = cv;

  arm_busy = am;
  arm_expires = ae;
  arm_target_dist = atd;
  arm_target_ang = ata;
  point_tos = pt;

  AttentionSide = fs;
  AttentionNum = fn;
  focusses = fc;

  tackle_expires = te;
  tackles = tc;
}

/*********************************************************************************/

float PlayerInfo::GetMySensedSpeed(Time time){

  if (time == sense_time)
    return last_speed;
  if (time == prev_sense_time)
    return prev_speed;

  my_error("Don't know my speed at time %d",time.t);
  return 0;
}

/*********************************************************************************/

float PlayerInfo::GetMySensedHeadingRelToNeckAngle(Time time){

  if ( SP_version < 6.00 )
    my_error("Shouldn't call GetMySensedRelToNeckAngle before version 6.00");

  if (time == sense_time)
    return last_heading_rel_neck;
  if (time == prev_sense_time)
    return prev_heading_rel_neck;

  my_error("Don't know my rel to neck angle at time %d",time.t);
  return 0;
}

/*********************************************************************************/

float PlayerInfo::GetMySensedStamina(Time time){

  if (time == sense_time)
    return last_stamina;
  if (time == prev_sense_time)
    return prev_stamina;

  my_error("Don't know my stamina at time %d",time.t);
  return 0;
}

/*********************************************************************************/

float PlayerInfo::GetMySensedEffort(Time time){

  if (time == sense_time)
    return last_effort;
  if (time == prev_sense_time)
    return prev_effort;

  my_error("Don't know my effort at time %d",time.t);
  return 0;
}

/*********************************************************************************/

float PlayerInfo::GetMySensedNeckAngle(Time time){

  if (time == sense_time)
    return last_neck_rel_ang;
  if (time == prev_sense_time)
    return prev_neck_rel_ang;

  my_error("Don't know my neck angle at time %d",time.t);
  return 0;
}

/*********************************************************************************/

int PlayerInfo::GetMySensedKicks(Time time){

  if (time == sense_time)
    return last_kicks;
  if (time == prev_sense_time)
    return prev_kicks;

  my_error("Don't know my kicks at time %d",time.t);
  return 0;
}

/*********************************************************************************/

int PlayerInfo::GetMySensedDashes(Time time){

  if (time == sense_time)
    return last_dashes;
  if (time == prev_sense_time)
    return prev_dashes;

  my_error("Don't know my dashes at time %d",time.t);
  return 0;
}

/*********************************************************************************/

int PlayerInfo::GetMySensedTurns(Time time){

  if (time == sense_time)
    return last_turns;
  if (time == prev_sense_time)
    return prev_turns;

  my_error("Don't know my turns at time %d",time.t);
  return 0;
}

/*********************************************************************************/

int PlayerInfo::GetMySensedSays(Time time){

  if (time == sense_time)
    return last_says;
  if (time == prev_sense_time)
    return prev_says;

  my_error("Don't know my says at time %d",time.t);
  return 0;
}

/*********************************************************************************/

int PlayerInfo::GetMySensedTurnNecks(Time time){

  if (time == sense_time)
    return last_turn_necks;
  if (time == prev_sense_time)
    return prev_turn_necks;

  my_error("Don't know my turn_necks at time %d",time.t);
  return 0;
}

/*********************************************************************************/

float PlayerInfo::CorrectDashPowerForStamina(float dash_power, float stamina, float, float)
{
  float new_power;
  if (dash_power >= 0) {
    new_power = Min( dash_power, stamina-(EffortDecThreshold+CP_tired_buffer) );
    if ( new_power < 0 ) new_power = 0;
  } else {
    new_power = Min(-dash_power, (stamina-(EffortDecThreshold+CP_tired_buffer)) / 2.0);
    if ( new_power < 0 ) new_power = 0;

    new_power = -new_power;
  }
    
  return new_power;
}

/*********************************************************************************/

Bool PlayerInfo::CanFaceAngleFromNeckWithNeck(AngleDeg ang)
{
  AngleDeg total_ang = MyNeckRelAng() + ang;
  NormalizeAngleDeg(&total_ang);
  if ( total_ang > SP_min_neck_angle && total_ang < SP_max_neck_angle )
    return TRUE;
  return FALSE;
}

/*********************************************************************************/

Bool PlayerInfo::CanFaceAngleFromBodyWithNeck(AngleDeg ang)
{
  NormalizeAngleDeg(&ang);
  if ( ang > SP_min_neck_angle && ang < SP_max_neck_angle )
    return TRUE;
  return FALSE;
}

/*********************************************************************************/

Bool PlayerInfo::CanSeeAngleFromNeckWithNeck(AngleDeg ang)
{
  AngleDeg total_ang = MyNeckRelAng() + ang;
  NormalizeAngleDeg(&total_ang);
  if ( total_ang > SP_min_neck_angle - MyViewAngle() && 
       total_ang < SP_max_neck_angle + MyViewAngle() )
    return TRUE;
  return FALSE;
}

/*********************************************************************************/

Bool PlayerInfo::CanSeeAngleFromBodyWithNeck(AngleDeg ang)
{
  if (ang > 180 || ang < -180) {
    my_error("Passing unnormalized angle to CanSeeAngleFromBodyWithNeck: %.1f", ang);
    NormalizeAngleDeg(&ang);
  }
  if ( ang > SP_min_neck_angle - MyViewAngle() && 
       ang < SP_max_neck_angle + MyViewAngle() )
    return TRUE;
  return FALSE;
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
void PlayerInfo::UpdatePredictedStaminaWithDash(float* pStamina, float* pEffort,
						float* pRecovery, float dash_power)
{
  if (dash_power > 0)
    *pStamina -= dash_power;
  else
    *pStamina -= 2 * dash_power;
  if (*pStamina < 0) *pStamina = 0;

  if ( *pStamina <= SP_recover_dec_thr * SP_stamina_max && *pRecovery > SP_recover_min ) {
    *pRecovery -= SP_recover_dec;
  }

  if ( *pStamina <= SP_effort_dec_thr * SP_stamina_max && *pEffort > My_effort_min() ){
    *pEffort -= SP_effort_dec;
    if ( *pEffort < My_effort_min() )
      *pEffort = My_effort_min();
  }
  if (*pStamina >= SP_effort_inc_thr * SP_stamina_max && *pEffort < My_effort_max()){
    *pEffort += SP_effort_inc;
    if ( *pEffort > My_effort_max() )
      *pEffort = My_effort_max();
  }
  *pStamina += *pRecovery * My_stamina_inc();
  if ( *pStamina > SP_stamina_max )
    *pStamina = SP_stamina_max;
}

/*********************************************************************************/


Vector PlayerInfo::MyPredictedPositionAtMaxSpeed(int steps)
{
  if ( !MyConf() ) my_error("Can't estimate future if don't know present (max speed)");

  Vector new_position = MyPos();
  Vector max_velocity = Polar2Vector(My_speed_max(),MyBodyAng());
  for (int i=0; i<steps; i++){
    new_position += max_velocity;
  }
  return new_position;
}

/*********************************************************************************/

Vector PlayerInfo::MyPredictedPositionWithTurn(float turn_ang,
					       int steps, float dash_power,
					       bool with_turn,
					       int idle_cycles)
{
  if ( !MyConf() ) my_error("Can't estimate future if don't know present");
  
  float curr_turn_ang = GetNormalizeAngleDeg(turn_ang);
  float corrected_dash_power = dash_power;
  float effective_power;
  float predicted_stamina = MyStamina();
  float predicted_effort = MyEffort();
  float predicted_recovery = MyRecovery();
  float myang = MyBodyAng();
  Vector position = MyPos();
  Vector velocity;
  if ( !MyVelConf() ) velocity = 0;
  else                velocity = MyVel();
  /* debug code 
  cout << "steps: " << steps << "\tpow: " << dash_power << "\tmyang: " << myang
       << "\tposition: " << position << "\tvel: " << velocity
       << "\tturn?: " << turn_first << "\tturn_ang: " << turn_angle
       << "\tstam: " << predicted_stamina << "\teff: " << predicted_effort
       << "\trec: " << predicted_recovery << endl; */
    
  for (int i=0; i<steps; i++){
    corrected_dash_power = CorrectDashPowerForStamina(dash_power,predicted_stamina);
    /* cout << " in func: i=" << i << "\tpos" << position << endl; */
    if (i < idle_cycles) {
      /* do nothing, we're idling! */
      effective_power = 0;
    } else if (with_turn &&
	(i==0 || curr_turn_ang != 0.0)) {
      float this_turn = MinMax(-EffectiveTurn(SP_max_moment, velocity.mod()),
			       curr_turn_ang,
			       EffectiveTurn(SP_max_moment, velocity.mod()));
      myang += this_turn;
      curr_turn_ang -= this_turn;
      effective_power = 0;
    } else if (fabs(corrected_dash_power) > predicted_stamina)
      effective_power = Sign(corrected_dash_power) * predicted_stamina ;
    else
      effective_power = corrected_dash_power;
    
    effective_power *= predicted_effort;
    effective_power *= My_dash_power_rate();
    velocity += Polar2Vector( effective_power, myang );
    /* cout << " in func: i=" << i << "\tvel" << velocity << endl; */
    
    if ( velocity.mod() > My_speed_max() )
      velocity *= ( My_speed_max()/velocity.mod() );
    
    position += velocity;
    velocity *= My_decay();

    UpdatePredictedStaminaWithDash(&predicted_stamina, &predicted_effort,
				   &predicted_recovery, corrected_dash_power);
  }
  /* cout << "returning " << position << endl; */
  return position;
  
}

/*********************************************************************************/

Vector PlayerInfo::MyPredictedPositionWithQueuedActions()
{
  /* Only goes one step in the future so far (other function assumes repeated dashes) */
  if ( Action->valid() && Action->type == CMD_dash )
    return MyPredictedPosition(1,Action->power);
  else
    return MyPredictedPosition();
}

/*********************************************************************************/

AngleDeg PlayerInfo::MyPredictedBodyAngleWithQueuedActions()
{
  /* Only goes one step in the future so far (other function assumes repeated dashes) */
  if ( Action->valid() && Action->type == CMD_turn )
    return GetNormalizeAngleDeg(MyBodyAng() + EffectiveTurn(Action->angle));
  else
    return MyBodyAng();
}

/********************************************************************************/

AngleDeg PlayerInfo::PredictedPointRelAngFromBodyWithQueuedActions(Vector point)
{
    Vector   pred_my_pos             = MyPredictedPositionWithQueuedActions();
    AngleDeg pred_my_body_ang        = MyPredictedBodyAngleWithQueuedActions();
    Vector   pred_relative_point_pos = point - pred_my_pos;
    AngleDeg target_abs_ang          = pred_relative_point_pos.dir();
    AngleDeg target_rel_ang          = target_abs_ang - pred_my_body_ang;
    NormalizeAngleDeg(&target_rel_ang);
    
    return target_rel_ang;
}

/*********************************************************************************/

int PlayerInfo::PredictedCyclesToPoint(Vector pt, float dash_power, float* endStamina)
{
  float corrected_dash_power = dash_power;
  float effective_power;
  float predicted_stamina = MyStamina();
  float predicted_effort = MyEffort();
  float predicted_recovery = MyRecovery();
  float myang = MyBodyAng();
  Vector position = MyPos();
  Vector velocity;
  if ( !MyVelConf() ) velocity = 0;
  else                velocity = MyVel();

  for (int i=0; TRUE; i++) {
    if (position.dist(pt) <= CP_at_point_buffer) {
      if (endStamina)
	*endStamina = predicted_stamina;
      return i;
    }
    

    /* decide if we should turn */
    float targ_ang = (pt-position).dir() - myang;
    if (dash_power < 0)
      targ_ang += 180;
    NormalizeAngleDeg(&targ_ang);
    if (fabs(targ_ang) > CP_max_go_to_point_angle_err) {
      /* turning */
      float this_turn = MinMax(-EffectiveTurn(SP_max_moment, velocity.mod()),
			       targ_ang,
			       EffectiveTurn(SP_max_moment, velocity.mod()));
      myang += this_turn;
      corrected_dash_power = 0; //so that stamina is updated correctly
    } else {
      /* dashing */
      corrected_dash_power = CorrectDashPowerForStamina(dash_power,predicted_stamina);
      effective_power = corrected_dash_power * predicted_effort * My_dash_power_rate();
      velocity += Polar2Vector( effective_power, myang );
    }
    
    if ( velocity.mod() > My_speed_max() )
      velocity.SetLength(My_speed_max());
    
    position += velocity;
    velocity *= My_decay();

    UpdatePredictedStaminaWithDash(&predicted_stamina, &predicted_effort,
				   &predicted_recovery, corrected_dash_power);
    
  }
  
}

/*********************************************************************************/
int PlayerInfo::NumTurnsToAngle(float targ_body_ang, float curr_body_ang, float curr_speed)
{
  int steps;

  NormalizeAngleDeg(&targ_body_ang);
  NormalizeAngleDeg(&curr_body_ang);

  for (steps = 0;
       fabs(targ_body_ang - curr_body_ang) > CP_max_go_to_point_angle_err;
       steps++) {
    AngleDeg this_turn = targ_body_ang - curr_body_ang;
    NormalizeAngleDeg(&this_turn);
    this_turn = signf(this_turn)*Min(fabs(this_turn), MaxEffectiveTurn(curr_speed));
    LogAction5(210, "NumTurnsToAngle: curr: %.1f  targ: %.1f  turn: %.1f",
		    curr_body_ang, targ_body_ang, this_turn);
    curr_body_ang += this_turn;
    NormalizeAngleDeg(&curr_body_ang);
    curr_speed *= My_decay();
  }
  
  return steps;
}



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/* returns whether we're trying to send a command too soon after the previous command.
   Helps to keep us from missing commands */
/* Not needed after server 5.23
Bool PlayerInfo::TooSoonForAnotherSend()
{
  struct timeval tv_new;

  gettimeofday(&tv_new, NULL); // no time zone info;
  int usec_diff = (tv_new.tv_sec - real_time_of_last_send.tv_sec) * 1000000 +
    ((signed)tv_new.tv_usec - (signed)real_time_of_last_send.tv_usec);
  return (usec_diff < Mem->SP_recv_step*1000 / Mem->CP_send_ban_recv_step_factor)
    ? TRUE : FALSE;
}
*/
