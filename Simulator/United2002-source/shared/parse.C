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


#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "MemPosition.h"
#include "soccer_utils.h"


/* parsing routines; defined in parse.C */
SenseType Parse(char* SensoryInput, PositionInfo* MemPos);
void Parse_Sight(char *SightInfo, PositionInfo* MemPos);
void Parse_Referee_Sound(char *RefereeSound, int time, PositionInfo* MemPos);
void Parse_Team_Names(char *, PositionInfo* );
void Parse_Player_Type(char *, PositionInfo* );
void Parse_ServerParam_Msg(char *Info, PositionInfo* Mem);
void Parse_PlayerParam_Msg(char *Info, PositionInfo* Mem);
void Parse_PlayerType_Msg(char *Info, PositionInfo* Mem);

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

SenseType Parse (char *SensoryInfo, PositionInfo* MemPos)
{
  SenseType sense_type;
  int     time = -1;

  if (MemPos->MP_save_log)
    MemPos->LogFile << SensoryInfo << endl << endl;

  /* SMURF: need to be able to handle looks and oks better! */

  switch ( SensoryInfo[1] ){            
  case 's':
    if ( SensoryInfo[3] == 'r' ){         /* server_param */
      sense_type = ST_ServerParam;
      break;
    }
    sense_type = ST_See;
    time = get_int(&SensoryInfo);         /* %d    */
    break; /* see   */
  case 'h':
    /* this advances SensoryInfo */
    /* this is wrong right now becuase coach messages come as online_coach_<side> */
    time = get_int(&SensoryInfo);         /* %d    */
    if ( SensoryInfo[1] == 'r' ){   /* Referee or Coach message */
      SensoryInfo += 9;             /* " referee " */
      if (strncmp(SensoryInfo, "training", 8) == 0)
	sense_type = ST_HearTrainer;
      else
	sense_type = ST_HearReferee;
    } else if (strncmp(SensoryInfo, "online_coach_", 13) == 0) {
      sense_type = ST_HearCoach;
    } else {
      /* This is a player hear message */
      //cout << "SMURF: Ignoring player message" << endl;
      sense_type = ST_HearPlayer;
    }
    
    break;
  case 'e':
    my_error("Error message: %s", SensoryInfo);
    sense_type = ST_Error;
    break;
  case 'w':
    my_error("Warning message: %s", SensoryInfo);
    sense_type = ST_Warning;
    break;
  case 'o':
    if (SensoryInfo[4] == 'l') {
      /* look message */
      SensoryInfo += 9; /* "(ok look " */
      sense_type = ST_See;
    } else if (SensoryInfo[4] == 't') {
      SensoryInfo += 14; /* "(ok team_names" */
      sense_type = ST_TeamNames;
    } 
    else if (SensoryInfo[4] == 'c') {
      if ( SensoryInfo[14] == 'y' ){
	SensoryInfo += 23;  /* "(ok change_player_type " */
	sense_type = ST_ChangePlayerType;
      }
      else{
	/* "(ok change_mode " */
	sense_type = ST_Ok;
      }
    }
    else {
      //      my_error("Got ok message: %s\n", SensoryInfo);    
      sense_type = ST_Ok;
    }
    break;
  case 'p':
    if ( SensoryInfo[8] == 'p' )
      sense_type = ST_PlayerParam;        /* player_param */
    else if ( SensoryInfo[8] == 't' )
      sense_type = ST_PlayerType;         /* player_type  */
    else 
      my_error("What kind of message? %s",SensoryInfo);
    break;
  case 'c':
    SensoryInfo += 20;  /* "(change_player_type " */
    sense_type = ST_ChangePlayerType;
    break;
  default: my_error("Sent an illegal message (got %s)",SensoryInfo); return ST_None;
  }

  switch ( sense_type ){
  case ST_See:   
    pthread_mutex_lock(&MemPos->mutex_pos_info);
    MemPos->BeginNewCycle(time);
    Parse_Sight(SensoryInfo, MemPos); 
    pthread_mutex_unlock(&MemPos->mutex_pos_info);
    break;
  case ST_HearPlayer:  
    /* we may want to handle player's sounds sometime */
    break;
  case ST_HearReferee:
    pthread_mutex_lock(&MemPos->mutex_pos_info);
    Parse_Referee_Sound(SensoryInfo, time, MemPos);  
    pthread_mutex_unlock(&MemPos->mutex_pos_info);
    break;
  case ST_HearCoach:
  case ST_HearTrainer:
    break; /* we said it, shouldn't have to do anything with it */
  case ST_TeamNames:
    //printf("team_names: %s\n", SensoryInfo);    
    pthread_mutex_lock(&MemPos->mutex_pos_info);
    Parse_Team_Names(SensoryInfo, MemPos);
    pthread_mutex_unlock(&MemPos->mutex_pos_info);
    break;
  case ST_ChangePlayerType:
    //my_error("player_type change: %s", SensoryInfo);    
    pthread_mutex_lock(&MemPos->mutex_pos_info);
    Parse_Player_Type(SensoryInfo, MemPos);
    pthread_mutex_unlock(&MemPos->mutex_pos_info);
    break;
  case ST_Error:
  case ST_Warning:
  case ST_Ok:
  case ST_None:
    break;
  case ST_ServerParam:
    Parse_ServerParam_Msg(SensoryInfo, MemPos); 
    break;
  case ST_PlayerParam:
    Parse_PlayerParam_Msg(SensoryInfo, MemPos); 
    break;
  case ST_PlayerType:
    Parse_PlayerType_Msg(SensoryInfo, MemPos); 
    break;
  default:
    my_error("Parse: sense_type is bad value: %d around time %d",
	     sense_type, MemPos->GetTime());
    my_error("       message was %s",SensoryInfo);
    break;
  }

  return sense_type;
}





/****************************************************************************/

void Parse_Sight(char *SightInfo, PositionInfo* MemPos)
{
  float X, Y;
  float velX, velY;
  float bodyAng, neckAng;
  char  teamname[MAX_TEAMNAME_LEN];
  
  TeamSide player_side;
  Unum player_number;

  while (*SightInfo != ')'){

    player_number = Unum_Unknown;
    player_side = TS_NoValue;

    get_word(&SightInfo);            /* " ((" */

    if ( *SightInfo=='b') {
      /* this is the ball*/
      X = get_float(&SightInfo);
      Y = get_float(&SightInfo);
      velX = get_float(&SightInfo);
      velY = get_float(&SightInfo);
      MemPos->SetBallInfo(Vector(X,Y), Vector(velX,velY));

      advance_to(')', &SightInfo); /* end of ObjInfo */
      SightInfo++;
    } else if (*SightInfo=='p') {

      if ( MemPos->SP_version >= 7 )
	SightInfo+=3;                          /* 'p "' for 'player "teamname"' */
      else
	SightInfo+=7;                          /* "player " */

      /* copy team name */
      int i = 0;
      while ( *SightInfo != '"' && *SightInfo != ' ' && *SightInfo != ')' ) {
	teamname[i++] = *SightInfo;
	SightInfo++; 
      }
      teamname[i]=0;
      if ( *SightInfo!= ' ' && *SightInfo != '"' )
	my_error("No player number");
      player_number = get_int(&SightInfo);

      X = get_float(&SightInfo);
      Y = get_float(&SightInfo);
      bodyAng = get_float(&SightInfo);
      neckAng = get_float(&SightInfo);
      velX = get_float(&SightInfo);
      velY = get_float(&SightInfo);

      MemPos->SetPlayerInfo(teamname, player_number, Vector(X, Y),
			    Vector(velX, velY), bodyAng, neckAng);
      
      advance_to(')', &SightInfo); /* end of ObjInfo */
      SightInfo++;
    } else {
      /* we need to skip this */
      advance_to(')', &SightInfo); /* end of ObjName */
      SightInfo++;
      advance_to(')', &SightInfo); /* end of ObjInfo */
      SightInfo++;
    }
  } /* while SightInfo */
}



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

void Parse_Referee_Sound(char *msg, int time, PositionInfo* MemPos)
{
  //cut off the newline
  msg[strlen(msg) - 1] = 0;
  LogAction3(150, "Ref Mess: %s", msg);
  
  switch( msg[0] ){
  case 'p': MemPos->SetPlayMode(PM_Play_On); break;           /* play_on */
  case 'k': 
    if ( msg[5] == 'i' ){                                  /* kick_in */
      if ( msg[8] == 'l' )
	MemPos->SetPlayMode(PM_Kick_In_Left);
      else if ( msg[8] == 'r' )
	MemPos->SetPlayMode(PM_Kick_In_Right);
      else 
	my_error("kick_in_?");
    }
    else if ( msg[5] == 'o' ){                            /* kick_off */
      if ( msg[9] == 'l' )
	MemPos->SetPlayMode(PM_Kick_Off_Left);
      else if ( msg[9] == 'r' )
	MemPos->SetPlayMode(PM_Kick_Off_Right);
      else 
	my_error("kick_off_? %c",msg[9]);
    }
    else
      my_error("referee k..?");
    break;
  case 'g': 
    if ( msg[5] == 'k' ){                                 /* goal_kick */
      if ( msg[10] == 'l' )
	MemPos->SetPlayMode(PM_Goal_Kick_Left);
      else if ( msg[10] == 'r' )
	MemPos->SetPlayMode(PM_Goal_Kick_Right);
      else 
	my_error("goal_kick_?");
    }
    else if ( msg[5] == 'e' ){                          /* goalie_catch_ball */
      if ( msg[18] == 'l' ) {
	MemPos->SetLastGoalieCatchTime(time);      
	MemPos->SetPlayMode(PM_Free_Kick_Left);
      } else if ( msg[18] == 'r' ) {
	MemPos->SetLastGoalieCatchTime(time);      
	MemPos->SetPlayMode(PM_Free_Kick_Right);
      } else {
	my_error("goalie_catch_ball_?");
      }      
    }
    /* SMURF: may want to keep track of score */
    else if ( msg[5] == 'l' ){                    /* goal */
      MemPos->SetPlayMode(PM_After_Goal_Left);
    }
    else if ( msg[5] == 'r' ){
      MemPos->SetPlayMode(PM_After_Goal_Right);
    }
    else 
      my_error("referee g..?");
    break;
  case 'c':                                               /* corner_kick */
    if ( msg[12] == 'l' )
      MemPos->SetPlayMode(PM_Corner_Kick_Left);
    else if ( msg[12] == 'r' )
      MemPos->SetPlayMode(PM_Corner_Kick_Right);
    else 
      my_error("corner_kick_?");
    break;
  case 'd':                                               /* drop_ball */    
    MemPos->SetPlayMode(PM_Drop_Ball); 
    MemPos->DropBallTime = MemPos->GetTime();
    break;        
  case 'o':                                               /* offside */    
    if ( msg[8] == 'l' )
      MemPos->SetPlayMode(PM_OffSide_Left);
    else if ( msg[8] == 'r' )
      MemPos->SetPlayMode(PM_OffSide_Right);
    else 
      my_error("offside_?");
    break;
  case 'f':
    if ( msg[5] == 'k' ){                                 /* free_kick */
      if ( msg[10] == 'l' )
	MemPos->SetPlayMode(PM_Free_Kick_Left);
      else if ( msg[10] == 'r' )
	MemPos->SetPlayMode(PM_Free_Kick_Right);
      else 
	my_error("free_kick_?");
    }
    else if ( msg[5] == 'l' )                     /* foul */
      ;
    else if ( msg[5] == 'r' )
      ;
    else 
      my_error("referee f..?");
    break;
  case 'h':                                               /* half_time */
    MemPos->SetPlayMode(PM_Before_Kick_Off);  /* play_mode to before_kick_off        */
    break;             
  case 'b': MemPos->SetPlayMode(PM_Before_Kick_Off); break;       /* before_kick_off */
  case 't': 
    if ( msg[5] == 'u' ){                             /* time_up */
      MemPos->SetPlayMode(PM_Time_Over); 
    }
    else if ( msg[5] == 'o' ) {                            /* time_over */
    }
    else if ( msg[5] == 'e' ){                            /* time_extended */
      MemPos->SetPlayMode(PM_Before_Kick_Off);
    }
    else 
      my_error("referee t..?");
    break;
  default: my_error("Referee msg ????");
  }
}


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

void copy_team_name(char* msg, char* team_name) 
{
  for (int i=0; *msg != ')'; i++, msg++)
    team_name[i] = *msg;
}


void Parse_Team_Names(char *msg, PositionInfo* MemPos)
{
  my_error("parsing team names %s",msg);
  while (*msg != ')') {
    msg += 7; /* " (team " */
    switch (*msg) {
    case 'l': copy_team_name(msg+2, MemPos->MP_left_team_name); break;
    case 'r': copy_team_name(msg+2, MemPos->MP_right_team_name); break;
    default: my_error("I don't understand team_name message: '%s'", msg);
    }
    advance_to(')', &msg);
    msg++;    
  }
}


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

#include "parse-player-change-incl.C"

void Parse_ServerParam_Msg(char *Info, PositionInfo* Mem)
#include <parse-serverparam-incl.C>

void Parse_PlayerParam_Msg(char *Info, PositionInfo* Mem)
#include <parse-playerparam-incl.C>

void Parse_PlayerType_Msg(char *Info, PositionInfo* Mem)
#include <parse-playertype-incl.C>


