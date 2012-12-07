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


/* These are types shared only by the coach and trainer */

#ifndef _TYPES_CT_H_
#define _TYPES_CT_H_

#include "types_all.h"

#define num_TeamSide 5
enum TeamSide {
  TS_Left = 0,
  TS_Right = 1,
  TS_Both = 2,
  TS_Neither = 3,
  TS_NoValue = 4
};

#define TEAMSIDE_STRINGS \
   {"Left", "Right", "Both", "Neither", "NoValue" }

typedef int Time;

typedef int Ptype;  /* Player type           */
#define Ptype_Default 0

#define MAXNAMELEN 30

#define MAX_PLAYERS 30

enum SenseType{
  ST_None,
  ST_See,
  ST_HearPlayer,
  ST_HearReferee,
  ST_HearTrainer,
  ST_HearCoach,
  ST_Error,
  ST_TeamNames,
  ST_ChangePlayerType,
  ST_ServerParam,
  ST_PlayerParam,
  ST_PlayerType,
  ST_Warning,
  ST_Ok
};

typedef enum _PlayMode {
	PM_No_Mode,
	PM_Before_Kick_Off,
	PM_Time_Over,
	PM_Play_On,
	PM_Kick_Off_Left,
	PM_Kick_Off_Right,
	PM_Kick_In_Left,
	PM_Kick_In_Right,
	PM_Free_Kick_Left,
	PM_Free_Kick_Right,
	PM_Corner_Kick_Left,
	PM_Corner_Kick_Right,
	PM_Goal_Kick_Left,
	PM_Goal_Kick_Right,
	PM_After_Goal_Left,
	PM_After_Goal_Right,
	PM_Drop_Ball,
	PM_OffSide_Left,
	PM_OffSide_Right,
	PM_MAX 
} Pmode ;


#define PLAYMODE_STRINGS {"",\
   "before_kick_off",\
   "time_over",\
   "play_on",\
   "kick_off_l",\
   "kick_off_r",\
   "kick_in_l",\
   "kick_in_r",\
   "free_kick_l",\
   "free_kick_r",\
   "corner_kick_l",\
   "corner_kick_r",\
   "goal_kick_l",\
   "goal_kick_r",\
   "goal_l",\
   "goal_r",\
   "drop_ball",\
   "offside_l",\
   "offside_r",\
   ""\
}

typedef enum TURNDIR 
{ TURN_NONE = 0,
  TURN_CW = -1,
  TURN_CCW = 1,
  TURN_CLOSEST = 10,
  TURN_AVOID = 11 /* avoid any opponents */
} TurnDir;

#define MAX_OPTION_LENGTH 50
enum TrainerValueType {
  TVT_OnOff,
  TVT_Int,
  TVT_Float
}; /* Bool and string types are not sent by trainer */


/* these are taken from the server params.h file */
#define MAX_PLAYER	11
#define	NO_INFO		0
#define	SHOW_MODE	1
#define	MSG_MODE	2
#define DRAW_MODE	3
#define	BLANK_MODE	4

#define	MSG_BOARD	1
#define	LOG_BOARD	2

#define SHOWINFO_SCALE		16.0

/* these are taken from the server types.h file */
#define	COLOR_NAME_MAX	64

#define LEFT	1
#define NEUTRAL	0
#define RIGHT 	-1

typedef struct {
	short	enable ;
	short	side ;
	short	unum ;
	short	angle ;
	short	x ;
	short	y ;
} pos_t ;

typedef struct {
	char	name[16] ;
	short	score ;
} team_t ;

typedef struct {
	char	pmode ;
	team_t	team[2] ;
	pos_t	pos[MAX_PLAYER * 2 + 1] ;
	short	time ;
} showinfo_t ;

typedef	struct {
	short	board ;
	char	message[2048] ;
} msginfo_t ;

typedef struct {
	short	x ;
	short	y ;
	char	color[COLOR_NAME_MAX] ;
} pointinfo_t ;

typedef struct {
	short	x ;
	short	y ;
	short	r ;
	char	color[COLOR_NAME_MAX] ;
} circleinfo_t ;
	
typedef struct {
	short	x1 ;
	short	y1 ;
	short	x2 ;
	short	y2 ;
	char	color[COLOR_NAME_MAX] ;
} lineinfo_t ;

typedef struct {
	short mode ;
	union {
		pointinfo_t		pinfo ;
		circleinfo_t	cinfo ;
		lineinfo_t		linfo ;
	} object ;
} drawinfo_t ;

typedef struct {
	short	mode ;
	union {
		showinfo_t	show ;
		msginfo_t	msg ;
		drawinfo_t	draw ;
	} body ;
} dispinfo_t ;


#endif
