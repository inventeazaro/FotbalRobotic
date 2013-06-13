/* these are some utility functions which are specfic to soccer */

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


#include <string.h>
#include "utils.h"
#include "soccer_utils.h"
#include "client.h"

float VelAtPt2VelAtFoot_sh(Vector pt, float targ_vel_at_pt, 
			   Vector mypos, float ball_decay)
{
  if (targ_vel_at_pt < FLOAT_EPS) {
    return SolveForFirstTermInfGeomSeries(ball_decay, (pt - mypos).mod() );
  } else {
    float ball_steps =
      SolveForLengthGeomSeries(targ_vel_at_pt, 1/ball_decay,
			    (pt - mypos).mod() );
    return targ_vel_at_pt * pow(1/ball_decay, ball_steps);
  }  
}

int CyclesForBallToPoint(Vector pt, float targ_vel_at_pt, float max_kick_speed, 
			 Vector mypos, float ball_decay)
{
  if (targ_vel_at_pt < FLOAT_EPS) 
    my_error("Shouldn't call CyclesForBallToPoint with ~0 targ_vel: %f", targ_vel_at_pt);
  float kick_spd = Min(max_kick_speed,
		       VelAtPt2VelAtFoot_sh(pt, targ_vel_at_pt, mypos, ball_decay));
  float steps = SolveForLengthGeomSeries(kick_spd, ball_decay, mypos.dist(pt));
  return (steps < 0 ? -1 : closest_int(steps));  
}

//SMURF: there may be a better way to write this....
int UpperBForPlayerToPoint(Vector start, Vector end)
{
  int num_cycles = 0;
  float dist = start.dist(end);
  float speed = 0;
  
  num_cycles += 2; //some turns at the beginning
  for (int i= 1; TRUE; i++) {
    if (dist < 0)
      break; // we got there

    if (i % Mem->CP_min_time_turn_when_going != 0) 
      //do a dash
      speed += Mem->SP_max_power * Mem->SP_dash_power_rate;
    speed = Min(speed, Mem->SP_player_speed_max);
    dist -= speed;
    speed *= Mem->SP_player_decay;
    num_cycles++;
  }
  
  return num_cycles;  
}

int kick_time_helper(Vector start, Vector end, float targ_vel_at_pt,
		     float max_kick_speed, int cycles_to_kick)
{
  float init_vel = VelAtPt2VelAtFoot_sh(end, targ_vel_at_pt, start, 
					Mem->SP_ball_decay);
  init_vel = Min(init_vel, max_kick_speed);
  int num_cycles = closest_int(ceil(SolveForLengthGeomSeries(init_vel, 
							     Mem->SP_ball_decay, 
							     start.dist(end))));
  num_cycles += cycles_to_kick;
  
  return num_cycles;
}


int UpperBForKickToPoint(Vector start, Vector end, float targ_vel_at_pt)
{ 
  return kick_time_helper(start,end,targ_vel_at_pt,Mem->CP_min_kick_speed,
			  Mem->CP_max_cycles_to_kick);
}

int AvgTimeForKickToPoint(Vector start, Vector end, float targ_vel_at_pt)
{ 
  return kick_time_helper(start,end,targ_vel_at_pt,Mem->CP_avg_kick_speed,
			  Mem->CP_avg_cycles_to_kick);
}

int MinTimeForKickToPoint(Vector start, Vector end, float targ_vel_at_pt)
{
  return kick_time_helper(start,end,targ_vel_at_pt,Mem->CP_max_kick_speed,
			  Mem->CP_min_cycles_to_kick);
}

/**********************************************************************************/



char* SetPlayMode_strings[num_SetPlayMode] = SETPLAYMODE_STRINGS;

SetPlayMode GetSetPlayMode(char* str)
{
  for (int i=0; i<num_SetPlayMode; i++) {
    if (strncmp(str, SetPlayMode_strings[i], strlen(SetPlayMode_strings[i])) == 0)
      return (SetPlayMode)i;
  }
  return SPM_No_Mode;
}

const char* GetSetPlayModeString(SetPlayMode spm)
{
  return SetPlayMode_strings[spm];
}

bool IsSetPlayDefensive(SetPlayMode spm)
{
  if (spm == SPM_Their_Kick_Off ||
      spm == SPM_Their_Kick_In ||
      spm == SPM_Their_Corner_Kick ||
      spm == SPM_Their_Goal_Kick ||
      spm == SPM_Their_Free_Kick ||
      spm == SPM_Their_Goalie_Catch)
    return true;
  return false;
}

bool IsSetPlayOffensive(SetPlayMode spm)
{
  if (spm == SPM_My_Kick_Off ||
      spm == SPM_My_Kick_In ||
      spm == SPM_My_Corner_Kick ||
      spm == SPM_My_Goal_Kick ||
      spm == SPM_My_Free_Kick ||
      spm == SPM_My_Goalie_Catch)
    return true;
  return false;
}

char* SpecSetPlayMode_strings[num_SpecSetPlayMode] = SPECSETPLAYMODE_STRINGS;

SpecSetPlayMode GetSpecSetPlayMode(char* str)
{
  for (int i=0; i<num_SpecSetPlayMode; i++) {
    if (strncmp(str, SpecSetPlayMode_strings[i], strlen(SpecSetPlayMode_strings[i])) == 0)
      return (SpecSetPlayMode)i;
  }
  return SSPM_No_Mode;
}

const char* GetSpecSetPlayModeString(SpecSetPlayMode spm)
{
  return SpecSetPlayMode_strings[spm];
}
SetPlayMode GetSPMForSpecSPM(SpecSetPlayMode spm)
{
  switch (spm) {
  case SSPM_No_Mode:
    return SPM_No_Mode;
  case SSPM_My_Kick_Off:
    return SPM_My_Kick_Off;
  case SSPM_Their_Kick_Off:
    return SPM_Their_Kick_Off;
  case SSPM_My_Kick_In_Def: 
  case SSPM_My_Kick_In_Mid:
  case SSPM_My_Kick_In_Off:
    return SPM_My_Kick_In;
  case SSPM_Their_Kick_In_Def:
  case SSPM_Their_Kick_In_Mid:
  case SSPM_Their_Kick_In_Off:
    return SPM_Their_Kick_In;
  case SSPM_My_Corner_Kick:
    return SPM_My_Corner_Kick;
  case SSPM_Their_Corner_Kick:
    return SPM_Their_Corner_Kick;
  case SSPM_My_Goal_Kick:
    return SPM_My_Goal_Kick;
  case SSPM_Their_Goal_Kick:
    return SPM_Their_Goal_Kick;
  case SSPM_My_Free_Kick_Upper:
  case SSPM_My_Free_Kick_Mid:
  case SSPM_My_Free_Kick_Lower:
    return SPM_My_Free_Kick;
  case SSPM_Their_Free_Kick_Upper:
  case SSPM_Their_Free_Kick_Mid:
  case SSPM_Their_Free_Kick_Lower:
    return SPM_Their_Free_Kick;
  case SSPM_My_Goalie_Catch:
    return SPM_My_Goalie_Catch;
  case SSPM_Their_Goalie_Catch:
    return SPM_Their_Goalie_Catch;
  default:
    my_error("What is SSPM: %d", spm);
    break;
  }
  return SPM_No_Mode;
}




/**********************************************************************************/

//cycles left in game
int CyclesRemaining(int curr_cycle)
{
  return 2*Mem->SP_half_time - curr_cycle;  
}

//cycles left in current half
int CyclesRemainingHalf(int curr_cycle)
{
  if (curr_cycle <= Mem->SP_half_time)
    return Mem->SP_half_time - curr_cycle;
  else
    return 2*Mem->SP_half_time - curr_cycle;
}


/******************************* LogAction *************************************/

#ifndef NO_ACTION_LOG

#define MAX_LOG_LINE 300



void LogAction2(int level, const char* str)
{
  Mem->LogAction_worker(level, str);
}

void LogAction3(int level, const char* str, const char* param)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, param);
  Mem->LogAction_worker(level, outstring);
}

void LogAction3(int level, const char* str, char c1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, c1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction3(int level, const char* str, float f1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction3(int level, const char* str, int d1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, const char* param1, const char* param2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, param1, param2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, float f1, int d1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, d1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, float f1, float f2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, int d1, int d2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, int d1, float f1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, f1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, int d1, const char* s1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, s1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction4(int level, const char* str, void* p1, void* p2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, p1, p2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, void* p1, void* p2, void* p3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, p1, p2, p3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, int d1, const char* s1, int d2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, s1, d2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, int d1, const char* s1, const char* s2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, s1, s2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, int d1, int d2, int d3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, int d1, int d2, float f1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, f1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, int d1, float f1, float f2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, f1, f2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, float f1, float f2, float f3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, f3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, float f1, float f2, int d1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, d1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction5(int level, const char* str, float f1, int d1, int d2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, d1, d2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, float f1, float f2, float f3, float f4)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, f3, f4);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, int d1, float f1, float f2, float f3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, f1, f2, f3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, int d1, int d2, float f1, float f2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, f1, f2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, int d1, int d2, int d3, float f1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, f1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, int d1, int d2, int d3, int d4)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, d4);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, float f1, int d1, int d2, int d3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, d1, d2, d3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, float f1, float f2, int d1, int d2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, d1, d2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, float f1, float f2, float f3, int d1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, f3, d1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction6(int level, const char* str, int d1, const char* s1, int d2, int d3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, s1, d2, d3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction7(int level, const char* str, float f1, float f2, float f3, float f4, float f5)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, f3, f4, f5);
  Mem->LogAction_worker(level, outstring);
}

void LogAction7(int level, const char* str, int d1, float f1, float f2, float f3, float f4)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, f1, f2, f3, f4);
  Mem->LogAction_worker(level, outstring);
}

void LogAction7(int level, const char* str, int d1, int d2, float f1, float f2, float f3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, f1, f2, f3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction7(int level, const char* str, int d1, int d2, int d3, float f1, float f2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, f1, f2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction7(int level, const char* str, int d1, int d2, int d3, int d4, float f1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, d4, f1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction7(int level, const char* str, int d1, int d2, int d3, int d4, int d5)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, d4, d5);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, float f1, float f2, float f3, float f4, float f5, float f6)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, f1, f2, f3, f4, f5, f6);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, int d1, float f1, float f2, float f3, float f4, float f5)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, f1, f2, f3, f4, f5);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, int d1, int d2, float f1, float f2, float f3, float f4)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, f1, f2, f3, f4);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, int d1, int d2, int d3, float f1, float f2, float f3)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, f1, f2, f3);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, int d1, int d2, int d3, int d4, float f1, float f2)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, d4, f1, f2);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, int d1, int d2, int d3, int d4, int d5, float f1)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, d4, d5, f1);
  Mem->LogAction_worker(level, outstring);
}

void LogAction8(int level, const char* str, int d1, int d2, int d3, int d4, int d5, int d6)
{
  char outstring[MAX_LOG_LINE];
  sprintf(outstring, str, d1, d2, d3, d4, d5, d6);
  Mem->LogAction_worker(level, outstring);
}

#endif //NO_ACTION_LOG
