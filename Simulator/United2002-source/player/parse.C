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


#include "client.h"
#include "utils.h"

void Parse_Sight(Time time, char *SightInfo);
void Parse_Sense(Time time, char *SenseInfo);
void Parse_Sound(Time time, char *SoundInfo);
void Parse_Score_Msg(char *Info);
void Parse_ChangePlayer_Msg(char *Info);
void Parse_ServerParam_Msg(char *Info);
void Parse_PlayerParam_Msg(char *Info);
void Parse_PlayerType_Msg(char *Info);
void Parse_Referee_Sound(char *RefereeSound);
void Parse_Trainer_Sound(char *msg);
void Parse_My_Coach_Sound(Time time, char *msg);
void Parse_Their_Coach_Sound(Time time, char *msg);
void Parse_OK_Msg(char *msg);
void Parse_Error(char *msg);

/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

void Parse(char *SensoryInfo)
{
  SenseType sense_type;
  int     time;

  switch ( SensoryInfo[3] ){            
  case 'e': sense_type = See_Msg;   break; /* see   */
  case 'n': sense_type = Sense_Msg; break; /* sense */
  case 'r': 
    if ( SensoryInfo[1] == 'e' )           /* error */
      sense_type = Error_Msg;    
    else if ( SensoryInfo[1] == 's' )      /* server_param */
      sense_type = ServerParam_Msg; 
    else
      my_error("What kind of server message?  %s",SensoryInfo);
    break;
  case 'o': sense_type = Score_Msg; break; /* score */
  case 'a': 
    if ( SensoryInfo[1] == 'c' )
      sense_type = ChangePlayer_Msg;       /* change_player_type */
    else if ( SensoryInfo[1] == 'h' )
      sense_type = Hear_Msg;               /* hear  */
    else if ( SensoryInfo[8] == 'p' )
      sense_type = PlayerParam_Msg;        /* player_param */
    else if ( SensoryInfo[8] == 't' )
      sense_type = PlayerType_Msg;         /* player_type  */
    else
      my_error("What kind of server message?  %s",SensoryInfo);
    break;
  case 'l': 
    my_error("Sent an illegal message: %s",SensoryInfo); break;
  default:  
    if ( SensoryInfo[1] == 'o' ){           /* ok */
      sense_type = OK_Msg;
    }
    else{
      my_error("What kind of server message?  %s",SensoryInfo);
      return;
    }
  }

  Time tm;

  switch ( sense_type ){
  case See_Msg:   
    time = get_int(&SensoryInfo);         /* %d    */
    tm = Mem->CurrentTime;
    if ( time != tm.t ) my_error ("Missed a sense_body?");
    if ( !Mem->LastActionOpTime ) break;   /* Don't parse until I've started counting time   */
    Mem->SightsThisCycle++;
    if ( tm == Mem->LastSightTime ) break; /* Don't parse a second sight from the same cycle */
    if ( Mem->NewSight == TRUE ){
      Mem->ClearSeenInfo();
      LogAction2(190,"Sight from last cycle lying around -- clearing it");
    }
    Parse_Sight(tm, SensoryInfo); 
    Mem->LastSightInterval = tm - Mem->LastSightTime;
    if ( Mem->CP_tune_timing && Mem->LastSightTime !=tm && Mem->LastActionOpTime == tm ){
      my_error("getting sight after action %d %d (%d %d)",Mem->LastActionOpTime.t,
	       Mem->LastSightTime.t,Mem->LastActionOpTime.s,Mem->LastSightTime.s);
      Mem->Tuned = FALSE;
    }
    Mem->LastSightTime = tm;
    Mem->NewSight = TRUE;
    break;
  case Sense_Msg: 
    time = get_int(&SensoryInfo);         /* %d    */
    tm = Mem->update_time(time);
    Parse_Sense(tm, SensoryInfo);      
    Mem->LastSenseTime = tm;
    break;
  case Hear_Msg:  
    time = get_int(&SensoryInfo);         /* %d    */
    tm = Mem->CurrentTime;
    if ( time != tm.t ) my_error ("Missed a sense_body?");
    if ( !Mem->LastActionOpTime ) break; /* Don't parse until I've started counting time */
    Parse_Sound(tm, SensoryInfo); 
    Mem->LastSoundTime = tm;
    break;
  case Score_Msg:
    time = get_int(&SensoryInfo);         /* %d    */
    Parse_Score_Msg(SensoryInfo);
    break;
  case ChangePlayer_Msg:
    Parse_ChangePlayer_Msg(SensoryInfo);
    break;
  case ServerParam_Msg:
    Parse_ServerParam_Msg(SensoryInfo);
    break;
  case PlayerParam_Msg:
    Parse_PlayerParam_Msg(SensoryInfo);
    break;
  case PlayerType_Msg:
    Parse_PlayerType_Msg(SensoryInfo);
    break;
  case OK_Msg:
    Parse_OK_Msg(SensoryInfo);
    break;
  case Error_Msg:
    Parse_Error(SensoryInfo);
    break;
  }

  Mem->LastSenseType = sense_type;
}




/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

void Parse_Sense(Time time, char *SenseInfo)
{
  get_word(&SenseInfo);  
  SenseInfo += 10;       /* "view_mode " */

  switch ( SenseInfo[0] ){
  case 'h': Mem->ViewQuality = VQ_High; break;  /* high */
  case 'l': Mem->ViewQuality = VQ_Low;  break;  /* low  */
  default:  my_error("Unknown view quality");
  }

  Mem->LastViewWidth = Mem->ViewWidth;
  Mem->ViewWidthTime = time;
  get_next_word(&SenseInfo);  
  switch ( SenseInfo[1] ){
  case 'o': Mem->ViewWidth = VW_Normal; break;  /* normal */
  case 'a': Mem->ViewWidth = VW_Narrow; break;  /* narrow */
  case 'i': Mem->ViewWidth = VW_Wide;   break;  /* wide   */
  default:  my_error("Unknown view quality");
  }
   
  float stamina = get_float(&SenseInfo);
  float effort  = get_float(&SenseInfo);
  float speed   = get_float(&SenseInfo);
  float heading_rel_neck = 0; 
  if (Mem->SP_version >= 6.00) 
    heading_rel_neck = get_float(&SenseInfo); 
  float rel_neck_angle = get_float(&SenseInfo);

  int kicks  =   get_int(&SenseInfo);
  int dashes =   get_int(&SenseInfo);
  int turns  =   get_int(&SenseInfo);
  int says   =   get_int(&SenseInfo);
  int turn_necks   =   get_int(&SenseInfo);

  int catches = get_int(&SenseInfo);
  int moves   = get_int(&SenseInfo);
  int change_views = get_int(&SenseInfo);

  int arm_busy      = get_int(&SenseInfo);
  int arm_expires      = get_int(&SenseInfo);
  float arm_target_dist= get_float(&SenseInfo);
  float arm_target_ang = get_float(&SenseInfo);
  int point_tos        = get_int(&SenseInfo);

  SenseInfo += 18;
  char focus_side = '?';
  int focus_num = 0;
  switch (*SenseInfo){
  case 'n': break; //none
  case 'l': focus_side = 'l'; focus_num = get_int(&SenseInfo); break;
  case 'r': focus_side = 'r'; focus_num = get_int(&SenseInfo); break;
  }
  int focus_count = get_int(&SenseInfo);

  int tackle_expires = get_int(&SenseInfo);
  int tackle_count = get_int(&SenseInfo);

  Mem->SetMySensedInfo(stamina,effort,speed,heading_rel_neck,rel_neck_angle,kicks,dashes,turns,says,turn_necks,catches,moves,change_views,arm_busy,arm_expires,arm_target_dist,arm_target_ang,point_tos,focus_side,focus_num,focus_count,tackle_expires,tackle_count,time);
}


/****************************************************************************************/

#define NOCHNGINFO -500
#define NOFACEINFO -500
#define NOPOINTINFO -500

void Parse_Sight(Time time, char *SightInfo)
{
  float dist, ang;
  float dirChng; 
  float distChng;
  ObjType object_type;  
  char player_side;
  Unum player_number;
  Bool goalie;  
  float facedir;
  float neckdir;
  float pointdir;
  Bool tackling = FALSE;
  MarkerType marker;
  SideLine  line;
  Vqual view_qual;
  MarkerType closestMarker = No_Marker;
  Bool processThisMarker;
  float closestMarkerDist;
  /* float motionInfoDist = 1000; */

  while (*SightInfo != ')'){

    dirChng = NOCHNGINFO;
    facedir = NOFACEINFO;
    neckdir = NOFACEINFO;
    pointdir= NOPOINTINFO;
    player_number = player_side = 0;
    goalie = FALSE;    

    get_word(&SightInfo);            /* " ((" */

    if ( *SightInfo=='g' ){
      object_type = OBJ_Marker;
      if (Mem->SP_version < 6.00) {
	SightInfo+=5;          /* "goal " */
      } else{
	SightInfo += 2; /* "g " */ /* pfr v6 protocol */
      }
      if ( *SightInfo=='r' )         marker = Goal_R; else
      if ( *SightInfo=='l' )         marker = Goal_L; else
      my_error("goal ?");
    } else
    if ( *SightInfo=='G' ){
      object_type = OBJ_Marker_Behind;  marker = Mem->ClosestGoal();  
    } else
    if ( *SightInfo=='f' ){
      object_type = OBJ_Marker;
      if (Mem->SP_version < 6.00) {
	SightInfo+=5;          /* "flag " */
      } else{
	SightInfo += 2; /* "f " */ /* pfr v6 protocol */
      }
      if ( *SightInfo=='r' ){
	SightInfo+=2;
	if ( *SightInfo=='0' )       marker = Flag_R0;   else
	if ( *SightInfo=='b' ){
	  SightInfo+=1;
	  if ( *SightInfo==')' )     marker = Flag_RB; 
	  else{
	    SightInfo+=1;
	    if ( *SightInfo=='1' )   marker = Flag_RB10; else
	    if ( *SightInfo=='2' )   marker = Flag_RB20; else
	    if ( *SightInfo=='3' )   marker = Flag_RB30; else
	    my_error("flag r b ?");
	  }  
	} else
	if ( *SightInfo=='t' ){
	  SightInfo+=1;
	  if ( *SightInfo==')' )     marker = Flag_RT; 
	  else{
	    SightInfo+=1;
	    if ( *SightInfo=='1' )   marker = Flag_RT10; else
	    if ( *SightInfo=='2' )   marker = Flag_RT20; else
	    if ( *SightInfo=='3' )   marker = Flag_RT30; else
	    my_error("flag r t ?");
	  }  
	} else
	my_error("flag r ?");
      } else
      if ( *SightInfo=='l' ){
	SightInfo+=2;
	if ( *SightInfo=='0' )       marker = Flag_L0;   else
	if ( *SightInfo=='b' ){
	  SightInfo+=1;
	  if ( *SightInfo==')' )     marker = Flag_LB; 
	  else{
	    SightInfo+=1;
	    if ( *SightInfo=='1' )   marker = Flag_LB10; else
	    if ( *SightInfo=='2' )   marker = Flag_LB20; else
	    if ( *SightInfo=='3' )   marker = Flag_LB30; else
	    my_error("flag l b ?");
	  }  
	} else
	if ( *SightInfo=='t' ){
	  SightInfo+=1;
	  if ( *SightInfo==')' )     marker = Flag_LT; 
	  else{
	    SightInfo+=1;
	    if ( *SightInfo=='1' )   marker = Flag_LT10; else
	    if ( *SightInfo=='2' )   marker = Flag_LT20; else
	    if ( *SightInfo=='3' )   marker = Flag_LT30; else
	    my_error("flag l t ?");
	  }  
	} else
	my_error("flag l ?");
      } else
      if ( *SightInfo=='t' ){
	SightInfo+=2;
	if ( *SightInfo=='0' )       marker = Flag_T0;   else
	if ( *SightInfo=='l' ){
	  SightInfo+=2;
	  if ( *SightInfo=='1' )     marker = Flag_TL10; else
	  if ( *SightInfo=='2' )     marker = Flag_TL20; else
	  if ( *SightInfo=='3' )     marker = Flag_TL30; else
	  if ( *SightInfo=='4' )     marker = Flag_TL40; else
	  if ( *SightInfo=='5' )     marker = Flag_TL50; else
	  my_error("flag t l ?");
	} else
	if ( *SightInfo=='r' ){
	  SightInfo+=2;
	  if ( *SightInfo=='1' )     marker = Flag_TR10; else
	  if ( *SightInfo=='2' )     marker = Flag_TR20; else
	  if ( *SightInfo=='3' )     marker = Flag_TR30; else
	  if ( *SightInfo=='4' )     marker = Flag_TR40; else
	  if ( *SightInfo=='5' )     marker = Flag_TR50; else
	  my_error("flag t r ?");
	} else
	my_error("flag t ?");
      } else
      if ( *SightInfo=='b' ){
	SightInfo+=2;
	if ( *SightInfo=='0' )       marker = Flag_B0;   else
	if ( *SightInfo=='l' ){
	  SightInfo+=2;
	  if ( *SightInfo=='1' )     marker = Flag_BL10; else
	  if ( *SightInfo=='2' )     marker = Flag_BL20; else
	  if ( *SightInfo=='3' )     marker = Flag_BL30; else
	  if ( *SightInfo=='4' )     marker = Flag_BL40; else
	  if ( *SightInfo=='5' )     marker = Flag_BL50; else
	  my_error("flag b l ?");
	} else
	if ( *SightInfo=='r' ){
	  SightInfo+=2;
	  if ( *SightInfo=='1' )     marker = Flag_BR10; else
	  if ( *SightInfo=='2' )     marker = Flag_BR20; else
	  if ( *SightInfo=='3' )     marker = Flag_BR30; else
	  if ( *SightInfo=='4' )     marker = Flag_BR40; else
	  if ( *SightInfo=='5' )     marker = Flag_BR50; else
	  my_error("flag b r ?");
	} else
	my_error("flag b ?");
      } else
      if ( *SightInfo=='c' ){
	SightInfo+=1;
	if ( *SightInfo==')' )       marker = Flag_C;
	else{
	  SightInfo+=1;
	  if ( *SightInfo=='b' )     marker = Flag_CB; else
	  if ( *SightInfo=='t' )     marker = Flag_CT; else
          my_error("flag c ?");
	}
      } else
      if ( *SightInfo=='p' ){
	SightInfo+=2;
	if ( *SightInfo=='r' ){
	  SightInfo+=2;
	  if ( *SightInfo=='t')      marker = Flag_PRT; else
	  if ( *SightInfo=='c')      marker = Flag_PRC; else
	  if ( *SightInfo=='b')      marker = Flag_PRB; else	    
	  my_error("flag p r ?");
	} else
	if ( *SightInfo=='l' ){
	  SightInfo+=2;
	  if ( *SightInfo=='t')      marker = Flag_PLT; else
	  if ( *SightInfo=='c')      marker = Flag_PLC; else
	  if ( *SightInfo=='b')      marker = Flag_PLB; else	    
	  my_error("flag p l ?");
	} else
	my_error("flag p ?");
      } else
      if ( *SightInfo=='g' ){
	SightInfo+=2;
	if ( *SightInfo=='l' ){
	  SightInfo+=2;
	  if ( *SightInfo=='t' )     marker = Flag_GLT; else
	  if ( *SightInfo=='b' )     marker = Flag_GLB; else
	  my_error("flag g l ?");
	} else
	if ( *SightInfo=='r' ){
	  SightInfo+=2;
	  if ( *SightInfo=='t' )     marker = Flag_GRT; else
	  if ( *SightInfo=='b' )     marker = Flag_GRB; else
	  my_error("flag g r ?");
	} else
	my_error("flag g ?");
      } else
      my_error("flag ?");
    } else
    if ( *SightInfo=='F' ){
      object_type = OBJ_Marker_Behind;
                                     marker = Mem->ClosestFlagTo(); /* could be No_Marker */
    } else
    if ( *SightInfo=='l' ){
      object_type = OBJ_Line;
      if (Mem->SP_version < 6.00) {
	SightInfo+=5;          /* "line " */
      } else{
	SightInfo += 2; /* "l " */ /* pfr v6 protocol */
      }
      if ( *SightInfo=='r' )         line   = SL_Right;  else
      if ( *SightInfo=='l' )         line   = SL_Left;   else
      if ( *SightInfo=='t' )         line   = SL_Top;    else
      if ( *SightInfo=='b' )         line   = SL_Bottom; else
	my_error("line ?");
    } else
    if ( *SightInfo=='p' || *SightInfo=='P' ){
      object_type = OBJ_Player;
      if (Mem->SP_version < 6.00) {
	SightInfo+=6;          /* "player" */
      } else{
	SightInfo += 1; /* "p" */ /* pfr v6 protocol */
      }
      if ( *SightInfo == ' ' ){              /* there's a team */ 
	if ( Mem->SP_version < 6.00 )
	  SightInfo++;
	else
	  SightInfo+=2;                      /* ' "' */
	if ( !strncmp(SightInfo,Mem->MyTeamName,Mem->MyTeamNameLen) )
	  player_side = Mem->MySide;
	else{
	  if ( Mem->TheirTeamName[0] == '\n' ){
	    int a=0;
	    if ( Mem->SP_version < 6.00 )
	      while ( isalpha(*SightInfo) ) Mem->TheirTeamName[a++]=*SightInfo++;
	    else 
	      while ( *SightInfo!='"' ) Mem->TheirTeamName[a++]=*SightInfo++;
	  }
	  player_side = Mem->TheirSide;
	}
	/* advance past team name */
	if ( Mem->SP_version < 6.00 )
	  while ( *SightInfo != ' ' && *SightInfo != ')' ) SightInfo++; 
	else {
	  while ( *SightInfo != '"' ) SightInfo++;
	  SightInfo++; /* '"' */
	}

	if ( *SightInfo== ' ' ){             /* there's a number */
	  player_number = get_int(&SightInfo);
	}
	if ( *SightInfo == ' '){ /* this player is the goalie */
	  SightInfo++;
	  if ( *SightInfo == 'g' ){ 
	    LogAction2(210, "Saw a goalie");
	    SightInfo+=6;	  /* "goalie" */
	  }
	  else
	    my_error("What is after player number?");
	    
	  goalie = TRUE;	  
	}
      }
    } else
    if ( *SightInfo=='b' || *SightInfo=='B' )
      object_type = OBJ_Ball;
    else
      my_error("unknown object");

    advance_to(')',&SightInfo);              /* advance to end of object */

    /************************************/

    ang = get_float(&SightInfo);

    if ( *SightInfo != ')' ) {                  /* 'high' quality     */
      view_qual = VQ_High;
      dist = ang;
      ang = get_float(&SightInfo);
    }
    else {
      //printf("%s",SightInfo-30);
      //printf("got low qual\n");
      view_qual = VQ_Low;
    }

    //if ( view_qual != Mem->ViewQuality ) my_error("View quality %d correct?",view_qual);

    if ( *SightInfo != ')' ){
      if ( SightInfo[1]=='t' ){ // tackle
	tackling = TRUE;
	SightInfo+=2;           /* " t" */
      }
      else{
	distChng = get_float(&SightInfo);
	if ( *SightInfo == ')' ){
	  pointdir = distChng;  // discarding distchng
	}
	else if ( SightInfo[1]=='t' ){ // tackle
	  pointdir = distChng;  // discarding distchng
	  tackling = TRUE;
	  SightInfo+=2;           /* " t" */
	}
	else
	  dirChng  = get_float(&SightInfo);
      }
    }

    if ( *SightInfo != ')' ){
      if (object_type != OBJ_Player) my_error("Only players should have facedir");
      facedir = get_float(&SightInfo);
      neckdir = get_float(&SightInfo);
    }

    if ( *SightInfo != ')' ){
      if ( SightInfo[1]=='t' ){ // tackle
	tackling = TRUE;
	SightInfo+=2;           /* " t" */
      }
      else{
	pointdir = get_float(&SightInfo);
	if ( *SightInfo != ')' ){
	  if ( SightInfo[1]=='t' ){ // tackle
	    tackling = TRUE;
	    SightInfo+=2;           /* " t" */
	  }
	  else
	    my_error("What could come after pointdir other than tackle? %s",SightInfo);
	}
      }
    }
    
    if ( *SightInfo != ')' ) my_error("Should be done with object info here %s",
				      SightInfo);
    SightInfo++;                                /* ")" */

    /************************************/

    switch (object_type){
    case OBJ_Marker:
    case OBJ_Marker_Behind:
      /* Want to save 2 closest for triangulation  */
      /* don't want marker_behind unless necessary */

      /* If it was a Marker_Behind and we don't know which one */
      if ( marker == No_Marker ){
	if ( object_type != OBJ_Marker_Behind ) my_error("Should know the marker");
	break;
      }

      // don't want to use a "behind" marker if we might have just moved
      // since then we might incorrectly calculate the "closest" marker
      if ( Mem->PlayMode != PM_Play_On && object_type == OBJ_Marker_Behind )
	break;

      processThisMarker = FALSE;
      if ( view_qual == VQ_Low ){               /* Low quality   */
        /* DON'T BOTHER PROCESSING ANY??? I don't think it helps ... */
	/* COULD process 2---then triangulate */
	/*if ( closestMarkerDist > 0 ){ */
	/*  closestMarkerDist = 0;  */                 /* Only process 1*/
	/*  processThisMarker = TRUE; */
	/*}*/
      }
      else{                                          /* high quality  */
	if ( closestMarker == No_Marker || dist < closestMarkerDist){
	  closestMarker = marker;
	  closestMarkerDist = dist;
	  processThisMarker = TRUE;
	  Mem->ClosestMarker = marker;
	}
	/* Don't bother with marker motion info -- get it from sense_body and my angle 
	if ( dirChng != NOCHNGINFO && dist < motionInfoDist ){
	  motionInfoDist = dist;
	  processThisMarker = TRUE;
	  Mem->ClosestMotionMarker = marker;
	}
	*/
      }
      if ( processThisMarker ){
	if ( view_qual == VQ_Low )              /* low quality   */
	  Mem->SeeMarker(marker, ang, time);
	else /* if (dirChng == NOCHNGINFO) */                  /* high quality  */
	  Mem->SeeMarker(marker, dist, ang, time);   /* No motion info*/
	/* else
	  Mem->SeeMarker(marker, dist, ang, distChng, dirChng, time); */
      }
      break;
    case OBJ_Line:
      if ( *SightInfo != ')' )
	/* There's another line coming.  Assuming lines happen
	   last in the visual string and the closer line comes first */
	; 
      // else - commenting out because lines come in any order now
      if ( view_qual == VQ_Low )           /* low quality   */
	Mem->SeeLine(line, ang, time);
      else                                           /* high quality  */
	Mem->SeeLine(line, dist, ang, time);
      break;
    case OBJ_Ball:
      if ( view_qual == VQ_Low )                /* low quality   */
	Mem->SeeBall(ang, time);
      else if ( dirChng == NOCHNGINFO )                   /* high quality  */
	Mem->SeeBall(dist, ang, time);
      else                                           /* know direction*/
	Mem->SeeBall(dist, ang, distChng, dirChng, time);         
      break;
    case OBJ_Player:
      if ( !player_side ){                      /* Too far for team or num */
	if ( view_qual == VQ_Low )                /* low quality   */
	  Mem->SeePlayer(ang, time);
	else if ( dirChng == NOCHNGINFO ){                   /* high quality  */
	  Mem->SeePlayer(dist, ang, time);
	}
	else                                           /* know direction*/
	  my_error("Shouldn't know dirChng when the player's far");
      }  

      else{
	if ( !player_number ){                  /* Too far for number     */
	  if ( view_qual == VQ_Low )                /* low quality   */
	    Mem->SeePlayer(player_side, ang, time);
	  else if ( dirChng == NOCHNGINFO ){                  /* high quality  */
	    if ( pointdir == NOPOINTINFO )
	      Mem->SeePlayer(player_side, dist, ang, tackling, time);
	    else
	      Mem->SeePlayer(player_side, dist, ang, pointdir, tackling, time);
	  }
	  else                                           /* know direction*/
	    my_error("Shouldn't know dirChng when the team member's far");
	}

	else{                                   /* Know side AND number   */
	  if ( view_qual == VQ_Low )                /* low quality   */
	    Mem->SeePlayer(player_side, player_number, goalie, ang, time);
	  else if ( dirChng == NOCHNGINFO ){                  /* high quality  */
	    printf("%s\n",SightInfo-30);
	    my_error("Should know dirChng when know number");
	    Mem->SeePlayer(player_side, player_number, goalie, dist, ang, time);
	  }
	  else if (pointdir == NOPOINTINFO ) /* know direction*/
	    Mem->SeePlayer(player_side, player_number, goalie, dist, ang, distChng, dirChng, facedir, neckdir, tackling, time);
	  else
	    Mem->SeePlayer(player_side, player_number, goalie, dist, ang, distChng, dirChng, facedir, neckdir, pointdir, tackling, time);
	}
      }
    }
  }
}

/****************************************************************************************/

void Parse_Score_Msg(char *Info)
{
  Mem->MyScore = get_int(&Info);
  Mem->TheirScore = get_int(&Info);
  //my_error("parsed score: %d to %d",Mem->MyScore,Mem->TheirScore);
}

/****************************************************************************************/

void Parse_ChangePlayer_Msg(char *Info)
{
  Unum player_number = get_int(&Info);
  int player_type = -1;
  if ( *Info != ')' ){
    player_type = get_int(&Info);
    if ( player_type < 0 || player_type >= Mem->PP_player_types )
      my_error("What player type is this? %d",player_type);
  }

  if ( *Info != ')' )
    my_error("More info to come for change_player???");

  if ( player_type != -1 ){
    Mem->SetTeammatePT(player_number,player_type);
    my_error("Changing player %d to type %d",player_number,player_type);
  }
  else{
    /* What to do here? */
    my_error("Opponent player %d changing",player_number);
  }
}

/****************************************************************************************/

void Parse_ServerParam_Msg(char *Info)
{
  float tmpf;
  int tmpi;
  char param[100];

  while(1){
    Info++; // past first '('
    advance_to('(',&Info);
    Info++;
    sscanf(Info,"%s",param);
    //printf("getting %s\n",param);

    if ( !strcmp(param,"goal_width") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_goal_width ) my_error("different goal_width");}
    else if ( !strcmp(param,"player_size") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_player_size ) my_error("different player_size");}
    else if ( !strcmp(param,"player_decay") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_player_decay ) my_error("different player_decay");}
    else if ( !strcmp(param,"player_rand") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_player_rand ) my_error("different player_rand");}
    else if ( !strcmp(param,"player_weight") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_player_weight ) my_error("different player_weight");}
    else if ( !strcmp(param,"player_speed_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_player_speed_max ) my_error("different player_speed_max");}
    else if ( !strcmp(param,"player_accel_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_player_accel_max ) my_error("different player_accel_max");}
    else if ( !strcmp(param,"stamina_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_stamina_max ) my_error("different stamina_max");}
    else if ( !strcmp(param,"stamina_inc_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_stamina_inc ) my_error("different stamina_inc");}
    else if ( !strcmp(param,"recover_init") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_recover_init ) my_error("different recover_init");}
    else if ( !strcmp(param,"recover_dec_thr") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_recover_dec_thr ) my_error("different recover_dec_thr");}
    else if ( !strcmp(param,"recover_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_recover_min ) my_error("different recover_min");}
    else if ( !strcmp(param,"recover_dec") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_recover_dec ) my_error("different recover_dec");}
    else if ( !strcmp(param,"effort_init") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_effort_init ) my_error("different effort_init");}
    else if ( !strcmp(param,"effort_dec_thr") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_effort_dec_thr ) my_error("different effort_dec_thr");}
    else if ( !strcmp(param,"effort_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_effort_min ) my_error("different effort_min");}
    else if ( !strcmp(param,"effort_dec") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_effort_dec ) my_error("different effort_dec");}
    else if ( !strcmp(param,"effort_inc_thr") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_effort_inc_thr ) my_error("different effort_inc_thr, %f",Mem->SP_effort_inc_thr);}
    else if ( !strcmp(param,"effort_inc") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_effort_inc ) my_error("different effort_inc");}
    else if ( !strcmp(param,"kick_rand") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_kick_rand ) my_error("different Mem->SP_kick_rand");}
    else if ( !strcmp(param,"team_actuator_noise") ){
      if ( (tmpf=get_int(&Info)) != Mem->SP_team_actuator_noise ) my_error("different team_actuator_noise"); }
    else if ( !strcmp(param,"prand_factor_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_prand_factor_l ) my_error("different prand_factor_l %f %f",tmpf,Mem->SP_prand_factor_l);}
    else if ( !strcmp(param,"prand_factor_r") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_prand_factor_r ) my_error("different prand_factor_r");}
    else if ( !strcmp(param,"kick_rand_factor_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_kick_rand_factor_l ) my_error("different kick_rand_factor_l");}
    else if ( !strcmp(param,"kick_rand_factor_r") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_kick_rand_factor_r ) my_error("different kick_rand_factor_r");}
    else if ( !strcmp(param,"ball_size") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ball_size ) my_error("different ball_size");}
    else if ( !strcmp(param,"ball_decay") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ball_decay ) my_error("different ball_decay");}
    else if ( !strcmp(param,"ball_rand") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ball_rand ) my_error("different ball_rand");}
    else if ( !strcmp(param,"ball_weight") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ball_weight ) my_error("different ball_weight");}
    else if ( !strcmp(param,"ball_speed_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ball_speed_max ) my_error("different ball_speed_max");}
    else if ( !strcmp(param,"ball_accel_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ball_accel_max ) my_error("different ball_accel_max");}
    else if ( !strcmp(param,"dash_power_rate") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dash_power_rate ) my_error("different dash_power_rate");}
    else if ( !strcmp(param,"kick_power_rate") ){
      if ( fabs((tmpf=get_float(&Info)) - Mem->SP_kick_power_rate) > FLOAT_EPS ) my_error("different kick_power_rate %f %f %f %f",tmpf,Mem->SP_kick_power_rate,tmpf-Mem->SP_kick_power_rate,FLOAT_EPS);}
    else if ( !strcmp(param,"kickable_margin") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_kickable_margin ) my_error("different kickable_margin");}
    else if ( !strcmp(param,"control_radius") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_control_radius ) my_error("different control_radius");}
    //  (tmpf=get_float(&Info)); //<ctlradius_width>
    else if ( !strcmp(param,"catch_probability") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_catch_prob ) my_error("different catch_prob");}
    else if ( !strcmp(param,"catchable_area_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_catch_area_l ) my_error("different catch_area_l");}
    else if ( !strcmp(param,"catchable_area_w") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_catch_area_w ) my_error("different catch_area_w");}
    else if ( !strcmp(param,"goalie_max_moves") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_goalie_max_moves ) my_error("different goalie_max_moves");}
    else if ( !strcmp(param,"maxpower") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_max_power ) my_error("different max_power");}
    else if ( !strcmp(param,"minpower") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_min_power ) my_error("different min_power");}
    else if ( !strcmp(param,"maxmoment") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_max_moment ) my_error("different max_moment");}
    else if ( !strcmp(param,"minmoment") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_min_moment ) my_error("different min_moment");}
    else if ( !strcmp(param,"maxneckmoment") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_max_neck_moment ) my_error("different max_neck_moment");}
    else if ( !strcmp(param,"minneckmoment") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_min_neck_moment ) my_error("different min_neck_moment");}
    else if ( !strcmp(param,"maxneckang") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_max_neck_angle ) my_error("different max_neck_angle");}
    else if ( !strcmp(param,"minneckang") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_min_neck_angle ) my_error("different min_neck_angle");}
    else if ( !strcmp(param,"visible_angle") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_visible_angle ) my_error("different visible_angle %f",Mem->SP_visible_angle);}
    else if ( !strcmp(param,"visible_distance") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_feel_distance ) my_error("different feel_distance %f %f",tmpf,Mem->SP_feel_distance);}
    else if ( !strcmp(param,"audio_cut_dist") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_audio_cut_dist ) my_error("different audio_cut_dist");}
    else if ( !strcmp(param,"quantize_step") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dist_qstep ) my_error("different dist_qstep");}
    else if ( !strcmp(param,"quantize_step_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_land_qstep ) my_error("different land_qstep");}
    else if ( !strcmp(param,"dir_qstep") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dir_qstep ) my_error("different dir_qstep %f %f",tmpf,Mem->SP_dir_qstep);}
    else if ( !strcmp(param,"dist_qstep_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dist_qstep_l ) my_error("different dist_qstep_l");}
    else if ( !strcmp(param,"dist_qstep_r") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dist_qstep_r ) my_error("different dist_qstep_r");}
    else if ( !strcmp(param,"land_qstep_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_land_qstep_l ) my_error("different land_qstep_l");}
    else if ( !strcmp(param,"land_qstep_r") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_land_qstep_r ) my_error("different land_qstep_r");}
    else if ( !strcmp(param,"dir_qstep_l") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dir_qstep_l ) my_error("different dir_qstep_l");}
    else if ( !strcmp(param,"dir_qstep_r") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_dir_qstep_r ) my_error("different dir_qstep_r");}
    else if ( !strcmp(param,"ckmargin") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_ckmargin ) my_error("different ckmargin");}
    else if ( !strcmp(param,"wind_dir") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_wind_dir ) my_error("different wind_dir");}
    else if ( !strcmp(param,"wind_force") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_wind_force ) my_error("different wind_force");}
    //  (tmpf=get_float(&Info)); //<winang>
    else if ( !strcmp(param,"wind_rand") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_wind_rand ) my_error("different wind_rand");}
    else if ( !strcmp(param,"kickable_area") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_kickable_area ) my_error("different kickable_area");}
    else if ( !strcmp(param,"inertia_moment") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_inertia_moment ) my_error("different inertia_moment");}
    else if ( !strcmp(param,"wind_none") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_wind_none ) my_error("different wind_none");}
    else if ( !strcmp(param,"wind_random") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_wind_random ) my_error("different wind_random");}
    else if ( !strcmp(param,"half_time") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_half_time ) my_error("different half_time %d %d",tmpi,Mem->SP_half_time);}
    else if ( !strcmp(param,"drop_ball_time") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_drop_ball_time ) my_error("different drop_ball_time %d %d",tmpi, Mem->SP_drop_ball_time);}
    //  get_int(&Info); //port
    //  get_int(&Info); //coachport
    //  get_int(&Info); //olcoachport 
    else if ( !strcmp(param,"say_coach_cnt_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_say_coach_cnt_max ) my_error("different say_coach_cnt_max");}
    else if ( !strcmp(param,"say_coach_msg_size") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_say_coach_msg_size ) my_error("different say_coach_msg_size");}
    else if ( !strcmp(param,"simulator_step") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_simulator_step ) my_error("different simulator_step");}
    else if ( !strcmp(param,"send_step") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_send_step ) my_error("different send_step");}
    else if ( !strcmp(param,"recv_step") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_recv_step ) my_error("different recv_step");}
    else if ( !strcmp(param,"sense_body_step") ){
      if ( (tmpf=get_int(&Info)) != Mem->SP_sense_body_step ) my_error("different sense_body_step");}
    //(tmpf=get_int(&Info)); //<lcm_step>  
    else if ( !strcmp(param,"say_msg_size") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_say_msg_size ) my_error("different say_msg_size");}
    else if ( !strcmp(param,"clang_win_size") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_win_size ) my_error("different clang_win_size");}
    else if ( !strcmp(param,"clang_define_win") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_define_win ) my_error("different clang_define_win");}
    else if ( !strcmp(param,"clang_meta_win") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_meta_win ) my_error("different clang_meta_win");}
    else if ( !strcmp(param,"clang_advice_win") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_advice_win ) my_error("different clang_advice_win");}
    else if ( !strcmp(param,"clang_info_win") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_info_win ) my_error("different clang_info_win");}
    else if ( !strcmp(param,"clang_mess_delay") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_mess_delay ) my_error("different clang_mess_delay");}
    else if ( !strcmp(param,"clang_mess_per_cycle") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_clang_mess_per_cycle ) my_error("different clang_mess_per_cycle");}
    else if ( !strcmp(param,"hear_max") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_hear_max ) my_error("different hear_max %d %d",tmpi,Mem->SP_hear_max);}
    else if ( !strcmp(param,"hear_inc") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_hear_inc ) my_error("different hear_inc");}
    else if ( !strcmp(param,"hear_decay") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_hear_decay ) my_error("different hear_decay");}
    else if ( !strcmp(param,"catch_ban_cycle") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_catch_ban_cycle ) my_error("different catch_ban_cycle");}
    else if ( !strcmp(param,"coach") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_coach_mode ) my_error("different coach_mode");}
    else if ( !strcmp(param,"coach_w_referee") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_coach_w_referee_mode ) my_error("different coach_w_referee_mode %d %d",tmpi,Mem->SP_coach_w_referee_mode);}
    else if ( !strcmp(param,"old_coach_hear") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_old_coach_hear ) my_error("different old_coach_hear");}
    else if ( !strcmp(param,"send_vi_step") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_send_vi_step ) my_error("different send_vi_step %d %d",tmpi,Mem->SP_send_vi_step);}
    else if ( !strcmp(param,"use_offside") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_use_offside ) my_error("different use_offside");}
    else if ( !strcmp(param,"offside_active_area_size") ){
      if ( (tmpf=get_float(&Info)) != Mem->SP_offside_area ) my_error("different offside_area");}
    else if ( !strcmp(param,"forbid_kick_off_offside") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_forbid_kickoff_offside ) my_error("different forbid_kickoff_offside");}
    //  get_int(&Info); //verbose
    //  get_int(&Info); //replay
    else if ( !strcmp(param,"offside_kick_margin") ){
      if ( fabs((tmpf=get_float(&Info)) - Mem->SP_offside_kick_margin) > .000001 ) my_error("different offside_kick_margin %f %f",tmpf,Mem->SP_offside_kick_margin);}
    else if ( !strcmp(param,"slow_down_factor") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_slow_down_factor ) my_error("different slow_down_factor");}
    //  get_int(&Info); // synch_mode
    //  get_int(&Info); // synch_offset
    //  get_int(&Info); // synch_micro_sleep
    else if ( !strcmp(param,"start_goal_l") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_start_goal_l ) my_error("different start_goal_l");}
    else if ( !strcmp(param,"start_goal_r") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_start_goal_r ) my_error("different start_goal_r");}
    else if ( !strcmp(param,"fullstate_l") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_fullstate_l ) my_error("different fullstate_l");}
    else if ( !strcmp(param,"fullstate_r") ){
      if ( (tmpi=get_int(&Info)) != Mem->SP_fullstate_r ) my_error("different fullstate_r %d %d",tmpi, Mem->SP_fullstate_r);}
    else 
      ;//my_error("no match to server param %s",param);
  
    //  get_int(&Info); //(slowness_on_top_for_left_team 1) 

    //  get_int(&Info); //(slowness_on_top_for_right_team 1) 
    //  get_int(&Info); //(send_comms 0) 
    //  get_int(&Info); //(text_logging 1) 
    //  get_int(&Info); //(game_logging 1) 
    //  get_int(&Info); //(game_log_version 3) 
  //; //(text_log_dir ./) 
  //; //(game_log_dir ./) 
  //; //(text_log_fixed_name rcssserver) 
  //; //(game_log_fixed_name rcssserver) 
    //  get_int(&Info); //(text_log_fixed 0) 
    //  get_int(&Info); //(game_log_fixed 0) 
    //  get_int(&Info); //(text_log_dated 1) 
    //  get_int(&Info); //(game_log_dated 1) 
  //; //(log_date_format %Y%m%d%H%M-) 
    //  get_int(&Info); //(log_times 0) 
  //  get_int(&Info); //(record_messages 0) 
    //  get_int(&Info); //(text_log_compression 0) 
    //  get_int(&Info); //(game_log_compression 0) 
    //  get_int(&Info); //(profile 0) 
    //  get_int(&Info); //(point_to_ban 5) 
    //  get_int(&Info); //(point_to_duration 20) 

    advance_to(')',&Info);
    if ( Info[1]==')' ) break;
  }
}

/****************************************************************************************/

void Parse_PlayerParam_Msg(char *Info)
{
  float tmpf;
  int tmpi;
  char param[100];

  while(1){
    Info++; // past first '('
    advance_to('(',&Info);
    Info++;
    sscanf(Info,"%s",param);
    //printf("getting %s\n",param);
    
    if ( !strcmp(param,"player_types") ){
      if ( (tmpi=get_int(&Info)) != Mem->PP_player_types ) 
	my_error("different %d %d",tmpi,Mem->PP_player_types); }
    else if ( !strcmp(param,"subs_max") ){
      if ( (tmpi=get_int(&Info)) != Mem->PP_subs_max ) 
	my_error("different %d %d",tmpi,Mem->PP_subs_max); }
    else if ( !strcmp(param,"pt_max") ){
      if ( (tmpi=get_int(&Info)) != Mem->PP_pt_max ) 
	my_error("different %d %d",tmpi,Mem->PP_pt_max); }
    else if ( !strcmp(param,"player_speed_max_delta_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_player_speed_max_delta_min ) 
	my_error("different %d %d",tmpf,Mem->PP_player_speed_max_delta_min); }
    else if ( !strcmp(param,"player_speed_max_delta_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_player_speed_max_delta_max ) 
	my_error("different %d %d",tmpf,Mem->PP_player_speed_max_delta_max); }
    else if ( !strcmp(param,"stamina_inc_max_delta_factor") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_stamina_inc_max_delta_factor ) 
	my_error("different %d %d",tmpf,Mem->PP_stamina_inc_max_delta_factor); }
    else if ( !strcmp(param,"player_decay_delta_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_player_decay_delta_min ) 
	my_error("different %d %d",tmpf,Mem->PP_player_decay_delta_min); }
    else if ( !strcmp(param,"player_decay_delta_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_player_decay_delta_max ) 
	my_error("different %d %d",tmpf,Mem->PP_player_decay_delta_max); }
    else if ( !strcmp(param,"inertia_moment_delta_factor") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_inertia_moment_delta_factor ) 
	my_error("different %d %d",tmpf,Mem->PP_inertia_moment_delta_factor); }
    else if ( !strcmp(param,"dash_power_rate_delta_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_dash_power_rate_delta_min ) 
	my_error("different %d %d",tmpf,Mem->PP_dash_power_rate_delta_min); }
    else if ( !strcmp(param,"dash_power_rate_delta_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_dash_power_rate_delta_max ) 
	my_error("different %d %d",tmpf,Mem->PP_dash_power_rate_delta_max); }
    else if ( !strcmp(param,"player_size_delta_factor") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_player_size_delta_factor ) 
	my_error("different %d %d",tmpf,Mem->PP_player_size_delta_factor); }
    else if ( !strcmp(param,"kickable_margin_delta_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_kickable_margin_delta_min ) 
	my_error("different %d %d",tmpf,Mem->PP_kickable_margin_delta_min); }
    else if ( !strcmp(param,"kickable_margin_delta_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_kickable_margin_delta_max ) 
	my_error("different %d %d",tmpf,Mem->PP_kickable_margin_delta_max); }
    else if ( !strcmp(param,"kick_rand_delta_factor") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_kick_rand_delta_factor ) 
	my_error("different %d %d",tmpf,Mem->PP_kick_rand_delta_factor); }
    else if ( !strcmp(param,"extra_stamina_delta_min") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_extra_stamina_delta_min ) 
	my_error("different %d %d",tmpf,Mem->PP_extra_stamina_delta_min); }
    else if ( !strcmp(param,"extra_stamina_delta_max") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_extra_stamina_delta_max ) 
	my_error("different %d %d",tmpf,Mem->PP_extra_stamina_delta_max); }
    else if ( !strcmp(param,"effort_max_delta_factor") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_effort_max_delta_factor ) 
	my_error("different %d %d",tmpf,Mem->PP_effort_max_delta_factor); }
    else if ( !strcmp(param,"effort_min_delta_factor") ){
      if ( (tmpf=get_float(&Info)) != Mem->PP_effort_min_delta_factor ) 
	my_error("different %d %d",tmpf,Mem->PP_effort_min_delta_factor); }
    else
      ;//my_error("no match to player param %s",param);

    advance_to(')',&Info);
    if ( Info[1]==')' ) break;
  }
}

//(player_param (player_types 7)(pt_max 3)(random_seed -1)(subs_max 3)(dash_power_rate_delta_max 0.002)(dash_power_rate_delta_min 0)(effort_max_delta_factor -0.002)(effort_min_delta_factor -0.002)(extra_stamina_delta_max 100)(extra_stamina_delta_min 0)(inertia_moment_delta_factor 25)(kick_rand_delta_factor 0.5)(kickable_margin_delta_max 0.2)(kickable_margin_delta_min 0)(new_dash_power_rate_delta_max 0.002)(new_dash_power_rate_delta_min 0)(new_stamina_inc_max_delta_factor -10000)(player_decay_delta_max 0.2)(player_decay_delta_min 0)(player_size_delta_factor -100)(player_speed_max_delta_max 0.2)(player_speed_max_delta_min 0)(stamina_inc_max_delta_factor -100))


/****************************************************************************************/

void Parse_PlayerType_Msg(char *Info)
{
  int id = get_int(&Info);

  Mem->PT[id].player_speed_max = get_float(&Info);
  Mem->PT[id].stamina_inc_max = get_float(&Info); 
  Mem->PT[id].player_decay = get_float(&Info); 
  Mem->PT[id].inertia_moment = get_float(&Info); 
  Mem->PT[id].dash_power_rate = get_float(&Info); 
  Mem->PT[id].player_size = get_float(&Info); 
  Mem->PT[id].kickable_margin = get_float(&Info); 
  Mem->PT[id].kick_rand = get_float(&Info); 
  Mem->PT[id].extra_stamina = get_float(&Info); 
  Mem->PT[id].effort_max = get_float(&Info); 
  Mem->PT[id].effort_min = get_float(&Info); 

//      (player_type <id> <player_speed_max> <stamina_inc_max> <player_decay>
//      <inertia_moment> <dash_power_rate> <player_size> <kickable_margin>
//      <kick_rand> <extra_stamina> <effort_max> <effort_min>)
}


/****************************************************************************************/

void Parse_Sound(Time time, char *SoundInfo)
{
  if ( SoundInfo[1] == 'r' ){   /* Referee or Coach message */
    SoundInfo += 9;             /* " referee " */
    if (strncmp(SoundInfo, "training", 8) == 0)
      Parse_Trainer_Sound(SoundInfo);
    else if ( islower(SoundInfo[0]) )
      Parse_Referee_Sound(SoundInfo);  
    else
      my_error("Referee sounds should start with lower case letters!");
    return;
  }
  else if ( SoundInfo[1] == 'o' && SoundInfo[2] == 'n' ){  /* Online coach message */
    SoundInfo += 14;                /* online_coach_ */
    if ( SoundInfo[0] == Mem->MySide ){
      advance_to('(',&SoundInfo);   /* advance to end of side */
      SoundInfo++;
      Parse_My_Coach_Sound(time,SoundInfo);
    }
    else if ( SoundInfo[0] == Mem->TheirSide ){
      advance_to('"',&SoundInfo);   /* advance to end of side */
      SoundInfo++;
      Parse_Their_Coach_Sound(time,SoundInfo);
    }
    else 
      my_error("online_coach_? %s",&SoundInfo[-14]);
    return;
  }
  //  Mem->ParsePlayerSound(SoundInfo, time);
}




/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

void Parse_Referee_Sound(char *msg)
{
  //printf("Parsing ref sound: '%s\'\n", msg);
  //msg[strlen(msg) - 2] = 0; /* cut off the newline and paren */
  LogAction3(200,"Referee message: %s",msg);

  switch( msg[0] ){
  case 'p': Mem->SetPlayMode(PM_Play_On); break;           /* play_on */
  case 'k': 
    if ( msg[5] == 'i' ){                                  /* kick_in */
      if ( msg[8] == Mem->MySide )
	Mem->SetPlayMode(PM_My_Kick_In);
      else if ( msg[8] == Mem->TheirSide )
	Mem->SetPlayMode(PM_Their_Kick_In);
      else 
	my_error("kick_in_?");
    }
    else if ( msg[5] == 'o' ){                            /* kick_off */
      if ( msg[9] == Mem->MySide )
	Mem->SetPlayMode(PM_My_Kick_Off);
      else if ( msg[9] == Mem->TheirSide )
	Mem->SetPlayMode(PM_Their_Kick_Off);
      else 
	my_error("kick_off_? %s",msg);
    }
    else
      my_error("referee k..?");
    break;
  case 'g': 
    if ( msg[5] == 'k' ){                                 /* goal_kick */
      if ( msg[10] == Mem->MySide )
	Mem->SetPlayMode(PM_My_Goal_Kick);
      else if ( msg[10] == Mem->TheirSide )
	Mem->SetPlayMode(PM_Their_Goal_Kick);
      else 
	my_error("goal_kick_?");
    }
    else if ( msg[5] == 'e' ){                           /* goalie_catch_ball */
      if ( msg[18] == Mem->MySide )
	Mem->SetPlayMode(PM_My_Goalie_Free_Kick);
      else if ( msg[18] == Mem->TheirSide )
	Mem->SetPlayMode(PM_Their_Goalie_Free_Kick);
      else
	my_error("goalie_catch_ball_?");
    }
    else if ( msg[5] == Mem->MySide ){                    /* goal */
      Mem->MyScore++;
      //Mem->MyScore = get_int(&msg[7]);
      Mem->KickOffMode = KO_Theirs;
      Mem->SetPlayMode(PM_Before_Kick_Off);
    }
    else if ( msg[5] == Mem->TheirSide ){
      Mem->TheirScore++;
      //Mem->TheirScore = get_int(&msg[7]);
      Mem->KickOffMode = KO_Mine;
      Mem->SetPlayMode(PM_Before_Kick_Off);
    }
    else 
      my_error("referee g..?");
    break;
  case 'c':                                               /* corner_kick */
    if ( msg[12] == Mem->MySide )
      Mem->SetPlayMode(PM_My_Corner_Kick);
    else if ( msg[12] == Mem->TheirSide )
      Mem->SetPlayMode(PM_Their_Corner_Kick);
    else 
      my_error("corner_kick_?");
    break;
  case 'd': Mem->SetPlayMode(PM_Drop_Ball); break;        /* drop_ball */    
  case 'o':                                               /* offside */    
    if ( msg[8] == Mem->MySide )
      Mem->SetPlayMode(PM_Their_Offside_Kick);
    else if ( msg[8] == Mem->TheirSide )
      Mem->SetPlayMode(PM_My_Offside_Kick);
    else 
      my_error("offside_?");
    break;
  case 'f':
    if ( msg[5] == 'k' ){                                 /* free_kick */
      if ( msg[10] == Mem->MySide )
	Mem->SetPlayMode(PM_My_Free_Kick);
      else if ( msg[10] == Mem->TheirSide )
	Mem->SetPlayMode(PM_Their_Free_Kick);
      else if ( msg[10] == 'f' ){                         /* free_kick_fault */
	if ( msg[16] == Mem->MySide )
	  Mem->SetPlayMode(PM_Their_Free_Kick);
	else if ( msg[16] == Mem->TheirSide )
	  Mem->SetPlayMode(PM_My_Free_Kick);
	else 
	  my_error("free_kick_fault_?");
      }
      else 
	my_error("free_kick_?");
    }
    else if ( msg[5] == Mem->MySide )                     /* foul */
      ;
    else if ( msg[5] == Mem->TheirSide )
      ;
    else 
      my_error("referee f..?");
    break;
  case 'h':                                               /* half_time */
    Mem->SetPlayMode(PM_Half_Time);  /* play_mode to before_kick_off        */
    if ( Mem->MySide == 'l' )
      Mem->KickOffMode = KO_Theirs;
    else 
      Mem->KickOffMode = KO_Mine;
    break;             
  case 'b': 
    if ( msg[1] == 'e' )                                  /* before_kick_off */
      Mem->SetPlayMode(PM_Before_Kick_Off);               
    else if ( msg[1] == 'a' ){                            /* back_pass */
      if ( msg[10] == Mem->MySide )
	Mem->SetPlayMode(PM_Their_Free_Kick);
      else if ( msg[10] == Mem->TheirSide )
	Mem->SetPlayMode(PM_My_Free_Kick);
      else 
	my_error("back_pass_?");
    }
    else 
      my_error("referee b..?");
    break;
  case 't': 
    if ( msg[5] == 'u' ){                                 /* time_up */
      Mem->SetPlayMode(PM_Time_Up); 
    }
    else if ( msg[5] == 'o' )                             /* time_over */
      {
	break;
      }
    else if ( msg[5] == 'e' ){                            /* time_extended */
      Mem->SetPlayMode(PM_Extended_Time);
      if ( Mem->MySide == 'l' )
	Mem->KickOffMode = KO_Mine;
      else 
	Mem->KickOffMode = KO_Theirs;
    } else 
      my_error("referee t..?");
    break;
  default: my_error("Referee msg ????");
  }
}

/****************************************************************************************/

/* the trainer send a string that is essentially command line options */
void Parse_Trainer_Sound(char *msg)
{
  msg += 9; /* 'training ' */

  if (msg[0] == 'K'){ /*Keepaway %d*/
    msg += 9;
    Mem->CP_keepaway_mode = get_int(&msg);
    return;
  }

  //msg[strlen(msg) - 2] = 0; /* cut off the newline and paren */
  Mem->GetOptions(msg);
  Mem->GetBall()->forget(); /* set 0 confidence in ball pos */
  cout << "Incorp trainer message" << endl;
  LogAction2(175, "Incorporated trainer sound");
}

/****************************************************************************************/

void Parse_My_Coach_Sound(Time time, char *msg)
{
  Mem->CoachActive = TRUE;

  //msg[strlen(msg) - 2] = 0; /* cut off the newline and paren */
  LogAction3(200,"Coach message: %s",msg);

  if ( msg[0] == 'f' ){ /* freeform */
    msg += 10; /*freeform (*/
    if (msg[0] == 'S'){ /*SetPlay (%.1f %.1f)*/
      ;
    } else 
      my_error("What's my coach saying??? %s",msg);
  } else
    my_error("not supporting standard lang yet: %s",msg);
} 

/****************************************************************************************/

void Parse_Their_Coach_Sound(Time time, char *msg)
{
  return;
  time = time;
  //msg[strlen(msg) - 2] = 0; /* cut off the newline and paren */
  LogAction3(200,"Their coach message: %s",msg);
}

/****************************************************************************************/

void Parse_OK_Msg(char *msg)
{
  LogAction3(200,"OK message: %s",msg);
  if (msg[4] != 'c')  /* (ok clang (ver 8 8)) */
    my_error("What kind of OK message is this??  %s",msg);
}

/****************************************************************************************/

void Parse_Error(char *msg)
{
  my_error("%s",msg);
}
