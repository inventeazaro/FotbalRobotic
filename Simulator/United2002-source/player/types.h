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


#ifndef _TYPES_H_
#define _TYPES_H_

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>

//types shared between coach, trainer, and player
#include <types_all.h>

#include "utils.h"

typedef int Pnum;  /* Position number          */
typedef int SPnum; /* Setplay position number  */

enum SenseType{
  See_Msg,
  Hear_Msg,
  Sense_Msg,
  Score_Msg,
  ChangePlayer_Msg,
  ServerParam_Msg,
  PlayerParam_Msg,
  PlayerType_Msg,
  OK_Msg,
  Error_Msg
};

enum CMDType {
  CMD_none,
  CMD_dash,
  CMD_turn,
  CMD_kick,
  CMD_tackle,
  CMD_catch,
  CMD_move,
  CMD_bye,
  CMD_change_view,
  CMD_turn_neck,
  CMD_say,
  CMD_sense_body,
  CMD_point_to,
  CMD_attention_to,
  CMD_ear
};

enum EarType {
  ET_complete,
  ET_partial,
  ET_both
};

enum EarMode {
  EM_on,
  EM_off
};

enum ObjType{
  OBJ_Line,
  OBJ_Ball,
  OBJ_Marker,
  OBJ_Marker_Behind,  /* Not seen */
  OBJ_Player
};

enum Vqual{
  VQ_Low,
  VQ_High
};

enum Vwidth{
  VW_Narrow,
  VW_Normal,
  VW_Wide
};

enum Kmode{
  KO_Mine,
  KO_Theirs
};

enum Pmode{
 PM_No_Mode,
 PM_Before_Kick_Off,
 PM_My_Kick_Off,
 PM_Their_Kick_Off,
 PM_My_Kick_In,
 PM_Their_Kick_In,
 PM_My_Corner_Kick,
 PM_Their_Corner_Kick,
 PM_My_Goal_Kick,
 PM_Their_Goal_Kick,
 PM_My_Free_Kick,
 PM_Their_Free_Kick,
 PM_My_Goalie_Free_Kick,     /* not a real play mode */
 PM_Their_Goalie_Free_Kick,  /* not a real play mode */
 PM_Drop_Ball,
 PM_My_Offside_Kick,
 PM_Their_Offside_Kick,
 PM_Play_On,
 PM_Half_Time,
 PM_Time_Up,
 PM_Extended_Time
};


enum SideLine{
  SL_Left,
  SL_Right,
  SL_Top,
  SL_Bottom,

  SL_No_Line
};


enum MarkerType{
  Goal_L,
  Goal_R,

  Flag_C,
  Flag_CT,
  Flag_CB,
  Flag_LT,
  Flag_LB,
  Flag_RT,
  Flag_RB,

  Flag_PLT,
  Flag_PLC,
  Flag_PLB,
  Flag_PRT,
  Flag_PRC,
  Flag_PRB,

  Flag_GLT,
  Flag_GLB,
  Flag_GRT,
  Flag_GRB,

  Flag_TL50,
  Flag_TL40,
  Flag_TL30,
  Flag_TL20,
  Flag_TL10,
  Flag_T0,
  Flag_TR10,
  Flag_TR20,
  Flag_TR30,
  Flag_TR40,
  Flag_TR50,

  Flag_BL50,
  Flag_BL40,
  Flag_BL30,
  Flag_BL20,
  Flag_BL10,
  Flag_B0,
  Flag_BR10,
  Flag_BR20,
  Flag_BR30,
  Flag_BR40,
  Flag_BR50,

  Flag_LT30,
  Flag_LT20,
  Flag_LT10,
  Flag_L0,
  Flag_LB10,
  Flag_LB20,
  Flag_LB30,

  Flag_RT30,
  Flag_RT20,
  Flag_RT10,
  Flag_R0,
  Flag_RB10,
  Flag_RB20,
  Flag_RB30,

  No_Marker
};

enum Ptype{
  PT_None,
  PT_Goaltender,
  PT_Sweeper,
  PT_Defender,
  PT_Midfielder,
  PT_Forward
};

enum Pside{
  PS_None,
  PS_Left,
  PS_Center,
  PS_Right
};

enum Fside{ /* Side of the field */
  FS_Right,
  FS_Left
};

enum Utype{
  UT_Defense,
  UT_Midfield,
  UT_Forward,
  UT_Left,
  UT_Center,
  UT_Right,
  UT_None
};

enum Ftype{
  FT_None,
  FT_433,
  FT_442,
  FT_352,
  FT_72,
  FT_334,
  FT_244,
  FT_532,
  FT_right,
  FT_left
};

enum MCtype{  /* Mark change type */
  MC_Obey,
  MC_Closest,
  MC_Open
};

enum HCtype{  /* Home change type */
  HC_Obey,
  HC_Get_Open,
  HC_Shift,
  HC_Mark
};

enum SPAtype{ /* Set play action type */
  SPA_None,
  SPA_Starter,
  SPA_Passer,       /* goes and passes   */
  SPA_Shooter,      /* goes and shoots   */
  SPA_Knocker,      /* aims at a point   */
  SPA_Blaster,        /* blasts at a point */
  SPA_Getter      /* goes and ends setplay */
};

/* Communication targets and messages */

enum TargetType{
  TT_Player,
  TT_Position,
  TT_Unit,
  TT_All
};

enum MessageType{
  CMsg_new_coach,         /* coach  */

  PMsg_none,              /* player */
  PMsg_ping,              
  PMsg_ping_ball,
  PMsg_ping_teammate,
  PMsg_ping_teammate_passing_decision,              
  PMsg_ping_opponent,
  PMsg_ready_to_pass,
  PMsg_ready_to_receive,
  PMsg_passing_decision,
  PMsg_my_ball,
  PMsg_leaving_position,
  PMsg_already_there,
  PMsg_tired,
  PMsg_Setplay_Ready,
  PMsg_Setplay_OK_Ready,
  PMsg_Setplay_Starter,
  PMsg_Setplay_Ping_Starter,
  PMsg_marking,
  PMsg_SetplayPlan_Abort,

  UMsg_assign_mark,       /* unit   */
  UMsg_assign_position
};

/* Action Modes */

enum ActionMode{
  AM_Unknown,
  
  AM_goaltend,

  AM_Localize,
  AM_Face_Ball,
  AM_Watch_Pass,
  AM_Receive_Pass,
  AM_Recover,
  AM_Before_Kick_Off,
  AM_Setplay,
  AM_SetplayPlan,
  AM_GetOnSide,

  AM_With_Ball,
  AM_Offense_Active,
  AM_Offense_Auxiliary,
  AM_Offense_Passive,

  AM_Defense_Active,
  AM_Defense_Auxiliary,
  AM_Defense_Passive
};

enum PassChoiceType{
  PC_None,
  PC_Fixed,
  PC_Random,
  PC_DT_Max,
  PC_DT_Max_No_Thresh,
  PC_DT_Thresh,
  PC_Congestion
};

enum PassFilterType{
  PF_None,
  PF_GoalDist_Congestion_And_Shot,
  PF_GoalDist_And_Congestion,
  PF_GoalDist_Or_Congestion,
  PF_Congestion,
  PF_XPos_Congestion_And_Shot,
  PF_XPos_And_Congestion,
  PF_XPos_Or_Congestion,
  PF_Breakaway,
  PF_BetterShot,
  PF_SlightlyBetterShot,
  PF_No_Opponent_Near
};

enum DodgeType{
  DT_none,
  DT_all,
  DT_unless_with_ball,
  DT_only_with_ball
};

/* these are for things in kick.* */
typedef enum TURNDIR 
{ TURN_NONE = 0,
  TURN_CW = -1,
  TURN_CCW = 1,
  TURN_CLOSEST = 10,
  TURN_AVOID = 11 /* avoid any opponents */
} TurnDir;

typedef enum KICKTORES
{ KT_None,
  KT_Success,
  KT_DidKick,
  KT_DidNothing,
  KT_TurnedToBall,
  KT_LostBall
} KickToRes; 

typedef enum KICKMODE
{ KM_None,
  KM_HardestKick,
  KM_Hard,
  KM_ModerateWTurn,
  KM_Moderate,
  KM_Quickly,
  KM_QuickestRelease
} KickMode;


/* these are for things in intercept.* */
typedef enum INTERCEPTRES
{ BI_None,        /* no value yet */
  BI_Invalid,     /* could not get an answer */
  BI_Failure,     /* won;t be able to intercept ball */
  BI_CanChase,    /* we're getting there - returned a GoToPoint command*/
  BI_ReadyToKick  /* ball is in kickable area, we haven;t done anything yet */
} InterceptRes; 

typedef enum ACTIONQUEUERES
{
  AQ_ActionQueued,
  AQ_ActionNotQueued
} ActionQueueRes;


#endif
