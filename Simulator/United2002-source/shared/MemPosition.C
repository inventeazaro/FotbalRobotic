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


/* the overall memory structure for the trainer */

#include <string.h>
#include "Memory.h"

/***************************** PlayerInfo *******************************/
PlayerInfo::PlayerInfo()
{
  Reset();
}

void PlayerInfo::Reset()
{
  side     = TS_NoValue;
  num      = Unum_Unknown;
  pos      = Vector(0,0);
  vel      = Vector(0,0);
  body_ang = 0.0;
  neck_ang = 0.0;
  type     = 0;
}


/***************************** BallInfo *******************************/
BallInfo::BallInfo()
{
  Reset();
}

void BallInfo::Reset()
{
  pos = Vector(0,0);
  vel = Vector(0,0);
}

/***************************** CycleInfo *******************************/
int CycleInfo::players_per_side = 0;

CycleInfo::CycleInfo()
{
  pPrev = pNext = NULL;
  players = new PlayerInfo[2*players_per_side];
  scores[0] = scores[1] = -1;
  
  ResetData();
}

CycleInfo::~CycleInfo()
{
  delete [] players;
}

void CycleInfo::ResetData()
{
  time = -1;
  for (int i=0; i<2*players_per_side; i++)
    players[i].Reset();
  ball.Reset();
}

PlayerInfo* CycleInfo::GetPlayer(TeamSide side, Unum num)
{
  if (side != TS_Left && side != TS_Right) {
    my_error("Tried to get player with bad side: %d", side);
    return NULL;
  }
  if (num < 1 || num > players_per_side) {
    my_error("Tried to get player with bad nubmer: %d", num);
    return NULL;
  }
  return  &players[(side*players_per_side) + (num-1)]; 
}



/***************************** PositionInfo *******************************/
PositionInfo::PositionInfo()
{
  pLatestCycle = NULL;
  ShutDown = FALSE;

  playMode = PM_Before_Kick_Off;
  playModeTime = 0;  
  numCycles = 0;

  CycleInfo::players_per_side = SP_team_size;
  cache_BackInfo_cyc = -1;
  cache_pBackInfo = NULL;

  goalie_catch_time = -1;  

  FieldRectangle = Rectangle(-SP_pitch_length/2,
			      SP_pitch_length/2,
			     -SP_pitch_width/2,
			      SP_pitch_width/2);

}

Bool PositionInfo::Initialize()
{
  pCycleInfo pHead, pTail, pCI;

  pHead = pTail = NULL;

  for (int i=0; i < MP_cycles_to_store; i++) {
    pCI = new CycleInfo;
    if (pCI == NULL) {
      my_error("Initialize failed?");
      return FALSE;
    }
    pCI->pNext = NULL;
    if (pHead == NULL) {
      pHead = pTail = pCI;
      pCI->pPrev = NULL;
    } else {
      pTail->pNext = pCI;
      pCI->pPrev = pTail;
      pTail = pCI;
    }
  }

  /* now connect the head an tail so that we have a circle */
  pHead->pPrev = pTail;
  pTail->pNext = pHead;

  pLatestCycle = pHead;

  if (MP_save_log) {
    LogFile.open("save.log");
    if (!LogFile)
      my_error("could not open log file");
  }
  
  DropBallTime = 0;

  int errcode;
  if ((errcode=pthread_mutex_init(&mutex_pos_info, NULL))) {
    my_error("mutex_init: %s\n", strerror(errcode)); 
    return FALSE;
  }

  return TRUE;
}

PositionInfo::~PositionInfo()
{
  /* clean up - be careful because it's a cyclic array */
  if (pLatestCycle) {
    pCycleInfo pTemp;
    for (int i=0; i < MP_cycles_to_store; i++) {
      pTemp = pLatestCycle->pNext;
      delete pLatestCycle;
      pLatestCycle = pTemp;
    }
  }
  
  if (LogFile)
    LogFile.close();
}


void PositionInfo::BeginNewCycle(Time time)
{
  pLatestCycle = pLatestCycle->pNext;
  pLatestCycle->ResetData();
  pLatestCycle->time = time;
  pLatestCycle->playMode = playMode;
  numCycles++;

  /* remove the cached back cycles */
  cache_BackInfo_cyc = -1;
  cache_pBackInfo = NULL;
}

void PositionInfo::SetBallInfo(Vector pos, Vector vel)
{
  pLatestCycle->ball.pos = pos;
  pLatestCycle->ball.vel = vel;
}

void PositionInfo::SetPlayerInfo(char* team_name, Unum num,
				 Vector pos, Vector vel,
				 float body_ang, float face_ang)
{
  if (strcasecmp(team_name, MP_left_team_name) == 0)
    SetPlayerInfo(TS_Left, num, pos, vel, body_ang, face_ang);
  else if (strcasecmp(team_name, MP_right_team_name) == 0)
    SetPlayerInfo(TS_Right, num, pos, vel, body_ang, face_ang);
  else if (MP_left_team_name[0]==0) {
    strcpy(MP_left_team_name, team_name);
    SetPlayerInfo(TS_Left, num, pos, vel, body_ang, face_ang);
  } else if (MP_right_team_name[0]==0) {
    strcpy(MP_right_team_name, team_name);
    SetPlayerInfo(TS_Right, num, pos, vel, body_ang, face_ang);
  } else
    my_error("Told to set player for team that does not exist! (%s) %s %s",team_name,
	       MP_left_team_name,MP_right_team_name);
}

void PositionInfo::SetPlayerInfo(TeamSide side, Unum num,
				 Vector pos, Vector vel,
		   float body_ang, float neck_ang)
{
  PlayerInfo* pPI = pLatestCycle->GetPlayer(side, num);

  pPI->side = side;
  pPI->num = num;
  pPI->pos = pos;
  pPI->vel = vel;
  pPI->body_ang = body_ang;
  pPI->neck_ang = neck_ang;
}

void PositionInfo::SetPlayerType(char *team_name, Unum num, Ptype type)
{
  if (strcasecmp(team_name, MP_left_team_name) == 0)
    SetPlayerType(TS_Left, num, type);
  else if (strcasecmp(team_name, MP_right_team_name) == 0)
    SetPlayerType(TS_Right, num, type);
  else
    my_error("Told to set player for team that does not exist! (%s) %s %s",team_name,
	       MP_left_team_name,MP_right_team_name);
}

void PositionInfo::SetPlayerType(TeamSide side, Unum num, Ptype type)
{
  PlayerInfo* pPI = pLatestCycle->GetPlayer(side, num);

  pPI->type = type;
}


void PositionInfo::SetScores(int l_score, int r_score)
{
  //cout << "Time: " << pLatestCycle->time << "\t"
  //<< l_score << " " << r_score << endl;
  pLatestCycle->scores[0] = l_score;
  pLatestCycle->scores[1] = r_score;
}


Time PositionInfo::GetTime()
{
  if (pLatestCycle == NULL)
    return (Time)0; /* we haven't gotten any info yet */
  else
    return pLatestCycle->time;
}

Pmode PositionInfo::GetPlayMode()
{
  if (pLatestCycle == NULL)
    /* not right if we connect mid game,
       but it	shoudl be right most of the time */
    return PM_Before_Kick_Off; 
  else
  return pLatestCycle->playMode;
}
BallInfo* PositionInfo::GetBall()
{
  if (pLatestCycle == NULL)
    return NULL;
  else
    return &pLatestCycle->ball;
}


PlayerInfo* PositionInfo::GetPlayer(int num)
{
  if (num < 0 || num >= SP_team_size*2) {
    my_error("Trying to get a player out of range: %d", num);
    return NULL;
  }

  if (pLatestCycle == NULL)
    return NULL;
  
  pPlayerInfo pPI = &(pLatestCycle->players[num]);
  if (pPI->side == TS_NoValue)
    return NULL; /* not a valid player */
  else
    return pPI;
}

PlayerInfo* PositionInfo::GetPlayer(TeamSide side, Unum num)
{
  if (pLatestCycle == NULL)
    return NULL;
  
  pPlayerInfo pPI = pLatestCycle->GetPlayer(side, num);
  if (pPI->side == TS_NoValue)
    return NULL; /* not a valid player */
  else
    return pPI;
}

int PositionInfo::GetNumPlayers(TeamSide side)
{
  int num_players = 0;
  for (int num = 1; num <= SP_team_size; num++) {
    PlayerInfo* pPI = GetPlayer(side, num);
    if (pPI != NULL) num_players++;
    else break;
  }
  return num_players;
}


int PositionInfo::GetScore(TeamSide side) 
{
  return GetPrevScore(0, side);
}

pCycleInfo PositionInfo::GetBackInfo(int cycles_back)
{
  if (cycles_back == 0)
    return pLatestCycle;
  
  if (cycles_back >= numCycles)
    return NULL; /* don't have that many cycles */
  
  if (cycles_back >= MP_cycles_to_store) {
    my_error("Tried to get cycles to far in the past: %d >= %d",
	     cycles_back, MP_cycles_to_store);
    return NULL;
  }

  if (pLatestCycle == NULL)
    return NULL;

  if (cycles_back == cache_BackInfo_cyc)
    return cache_pBackInfo;
  
  pCycleInfo pCI = pLatestCycle;
  for (int i=0; i < cycles_back; i++)
    pCI = pCI->pPrev;

  /* store the cache info */
  cache_BackInfo_cyc = cycles_back;
  cache_pBackInfo = pCI;
  
  return pCI;
}


Time  PositionInfo::GetPrevTime(int cycles_back)
{
  pCycleInfo pCI = GetBackInfo(cycles_back);
  if (pCI == NULL)
    return -1;
  else
    return pCI->time;
}

Pmode PositionInfo::GetPrevPlayMode(int cycles_back)
{
  pCycleInfo pCI = GetBackInfo(cycles_back);
  if (pCI == NULL)
    return PM_No_Mode;
  else
    return pCI->playMode;

}


BallInfo*   PositionInfo::GetPrevBall(int cycles_back)
{
  pCycleInfo pCI = GetBackInfo(cycles_back);
  if (pCI == NULL)
    return NULL;
  else
    return &pCI->ball;
}

PlayerInfo* PositionInfo::GetPrevPlayer(int cycles_back, TeamSide side, Unum num)
{
  pCycleInfo pCI = GetBackInfo(cycles_back);
  if (pCI == NULL)
    return NULL;
  else {
    pPlayerInfo pPI = pCI->GetPlayer(side, num);
    if (pPI->side == TS_NoValue)
      return NULL; /* not a valid player */
    else
      return pPI;
  }
}

PlayerInfo* PositionInfo::GetPrevPlayer(int cycles_back, int num)
{
  if (num < 0 || num >= SP_team_size*2) {
    my_error("Trying to get a player out of range: %d", num);
    return NULL;
  }

  pCycleInfo pCI = GetBackInfo(cycles_back);
  if (pCI == NULL)
    return NULL;
  else {
    pPlayerInfo pPI = &pCI->players[num];
    if (pPI->side == TS_NoValue)
      return NULL; /* not a valid player */
    else
      return pPI;
  }
}

int PositionInfo::GetPrevScore(int cycles_back, TeamSide side) 
{
  pCycleInfo pCI = GetBackInfo(cycles_back);
  if (pCI == NULL)
    return -1;

  switch (side) {
  case TS_Left: return pCI->scores[0];
  case TS_Right: return pCI->scores[1];
  case TS_Both: return (pCI->scores[0] << 8) + pCI->scores[1];
  case TS_Neither: return 0;
  default:
    return -1;
  }
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/


int   PositionInfo::GetPlayerPT(TeamSide s, Unum n) 
{
  if ( s==TS_Left ) {
    return LeftPlayerType[n];
  }
  else if (s==TS_Right )
    return RightPlayerType[n];
  else 
    return 0;  /* side unknown */
}

/*********************************************************************************/

void  PositionInfo::SetPlayerPT(TeamSide s, Unum n, int pt)
{
  if ( s==TS_Left ){
    LeftPlayerType[n] = pt;
  }
  else if ( s==TS_Right )
    RightPlayerType[n] = pt;
  else
    my_error("Need to know side to set a player type");
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/



/*********************************************************************************/
/* SMURF: I'm pretty sure that this function doesn't handle sorting both sides
   simultaneously correctly */
int PositionInfo::SortPlayersByDistanceToPoint(int cycles_back, TeamSide side,
					       Vector point, Unum* players)
{
  int result = 0; /*Number of players sorted */

  /* Make aux array big enough */
  int num = (( side == TS_Both) ? SP_team_size*2 : SP_team_size); 
			
  float *vals;
  vals = new float[num];
   
  TeamSide team;
  if (side == TS_Left || side == TS_Both) {
    team = TS_Left;
    for (int i=1; i<=SP_team_size; i++){
      if ( GetPrevPlayer(cycles_back, team, i) ){
	players[result]=i;
	vals[result]   =GetPrevPlayer(cycles_back, team, i)->pos.dist(point);  
	result++;
      }
    }
  }
  if (side == TS_Right || side == TS_Both) {
    team = TS_Right;
    for (int i=1; i<=SP_team_size; i++){
      if ( GetPrevPlayer(cycles_back, team, i) ){
	players[result]=i;
	vals[result]   =GetPrevPlayer(cycles_back, team, i)->pos.dist(point);  
	result++;
      }
    }
  }
    
/* Now should have all values in question in vals, with uniform number in 
   corresponding position of players ( +TEAM_SIZE for their team if 
   side == 'b'):  Just sort em */

  BubbleSort(result,players,vals);
  delete vals;
  return result;

}


Unum PositionInfo::ClosestPlayerToPoint(TeamSide side,Vector point,float* dist)
{
  *dist = HUGE;
  Unum player = Unum_Unknown;
  float tmp_dist;

  TeamSide team;
  if (side == TS_Left || side == TS_Both) {
    team = TS_Left;
    for (int i=1; i<=SP_team_size; i++){
      if ( GetPlayer(team, i) ){
	tmp_dist=GetPlayer(team, i)->pos.dist(point);  
	if ( tmp_dist < *dist ){
	  *dist = tmp_dist;
	  player = i;
	}
      }
    }
  }
  if (side == TS_Right || side == TS_Both) {
    team = TS_Right;
    for (int i=1; i<=SP_team_size; i++){
      if ( GetPlayer(team, i) ){
	tmp_dist=GetPlayer(team, i)->pos.dist(point);  
	if ( tmp_dist < *dist ){
	  *dist = tmp_dist;
	  player = i;
	  if (side == TS_Both)
	    player *= -1;  // If getting both, then right players are negative
	}
      }
    }
  }
    
  return player;

}

