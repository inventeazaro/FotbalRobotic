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


/* These are types which are shared between coach, trainer, and player */

#ifndef _TYPES_ALL_H_
#define _TYPES_ALL_H_

typedef struct {
  float player_speed_max;
  float stamina_inc_max;
  float player_decay;
  float inertia_moment;
  float dash_power_rate;
  float player_size;
  float kickable_margin;
  float kick_rand;
  float extra_stamina;
  float effort_max;
  float effort_min;
} PlayerType;

typedef bool Bool;
#define FALSE ((bool)false)
#define TRUE ((bool)true)

enum FuzzyBool {
  FB_False,
  FB_True,
  FB_Unknown,
  FB_Maybe,
  FM_NoValue
};


#define NULLCHAR	'\000'

typedef int Unum;  /* Uniform number           */
#define Unum_Unknown 0

const int num_players  = 11;
const int bits_for_player_num = 4;

const int MAX_MESSAGE_CHARS = 128;

//These must start and 0 and be continuous
const int num_SetPlayMode = 13;
enum SetPlayMode{
 SPM_No_Mode,
 SPM_My_Kick_Off,
 SPM_Their_Kick_Off,
 SPM_My_Kick_In,
 SPM_Their_Kick_In,
 SPM_My_Corner_Kick,
 SPM_Their_Corner_Kick,
 SPM_My_Goal_Kick,
 SPM_Their_Goal_Kick,
 SPM_My_Free_Kick,
 SPM_Their_Free_Kick,
 SPM_My_Goalie_Catch,
 SPM_Their_Goalie_Catch
};

#define SETPLAYMODE_STRINGS {\
"NoMode",\
"MyKickOff",\
"TheirKickOff",\
"MyKickIn",\
"TheirKickIn",\
"MyCornerKick",\
"TheirCornerKick",\
"MyGoalKick",\
"TheirGoalKick",\
"MyFreeKick",\
"TheirFreeKick",\
"MyGoalieCatch",\
"TheirGoalieCatch" }

//These must start and 0 and be continuous
//These are slightly more specialized modes from the ones above
//Right now, the coach uses these to get good seeds for hillclimbing
const int num_SpecSetPlayMode = 21;
enum SpecSetPlayMode{
 SSPM_No_Mode,
 SSPM_My_Kick_Off,
 SSPM_Their_Kick_Off,
 SSPM_My_Kick_In_Def, //defensive for us
 SSPM_My_Kick_In_Mid,
 SSPM_My_Kick_In_Off,
 SSPM_Their_Kick_In_Def, //defensive for them
 SSPM_Their_Kick_In_Mid,
 SSPM_Their_Kick_In_Off,
 SSPM_My_Corner_Kick,
 SSPM_Their_Corner_Kick,
 SSPM_My_Goal_Kick,
 SSPM_Their_Goal_Kick,
 //we assume free kicks are defensive since the only we really get them are offsides
 SSPM_My_Free_Kick_Upper,
 SSPM_My_Free_Kick_Mid,
 SSPM_My_Free_Kick_Lower,
 SSPM_Their_Free_Kick_Upper,
 SSPM_Their_Free_Kick_Mid,
 SSPM_Their_Free_Kick_Lower,
 SSPM_My_Goalie_Catch,
 SSPM_Their_Goalie_Catch
};

#define SPECSETPLAYMODE_STRINGS {\
 "NoMode",\
 "MyKickOff",\
 "TheirKickOff",\
 "MyKickInDef",\
 "MyKickInMid",\
 "MyKickInOff",\
 "TheirKickInDef",\
 "TheirKickInMid",\
 "TheirKickInOff",\
 "MyCornerKick",\
 "TheirCornerKick",\
 "MyGoalKick",\
 "TheirGoalKick",\
 "MyFreeKickUpper",\
 "MyFreeKickMid",\
 "MyFreeKickLower",\
 "TheirFreeKickUpper",\
 "TheirFreeKickMid",\
 "TheirFreeKickLower",\
 "MyGoalieCatch",\
 "TheirGoalieCatch" }

#endif
