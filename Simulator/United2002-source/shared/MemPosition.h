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


/* the overall memory structure for the coach/trainer */

#ifndef _MEMPOSITION_H_
#define _MEMPOSITION_H_

#include <fstream>
#include <pthread.h>
#include "MemClient.h"
#include "geometry.h"

#define MAX_PLAYER_TYPES 25

class PlayerInfo 
{
public:
  PlayerInfo();
  void Reset();
  
  TeamSide side;
  Unum     num;
  Vector   pos;
  Vector   vel;
  float    body_ang;
  float    neck_ang; /* relative to body angle */
  Ptype    type;
};
typedef PlayerInfo* pPlayerInfo;


struct BallInfo
{
public:
  BallInfo();
  void Reset();
  
  Vector pos;
  Vector vel;
};
typedef BallInfo* pBallInfo;


struct CycleInfo 
{
public:
  CycleInfo();
  ~CycleInfo();
  void ResetData();

  PlayerInfo* GetPlayer(TeamSide side, Unum num);

  static int players_per_side;
  
  CycleInfo* pPrev;
  CycleInfo* pNext;

  Time        time;
  Pmode       playMode;
  PlayerInfo* players;
  BallInfo    ball;
  int         scores[2]; // left team first
};
typedef CycleInfo* pCycleInfo;


/* the Memory will have a circular linked list of CycleInfo to automatically
   store some previous cycles */

class PositionInfo : public ClientInfo {
private:
  pCycleInfo pLatestCycle;
  Pmode      playMode;
  Time       playModeTime;  

  int numCycles;
  
  pCycleInfo GetBackInfo(int cycles_back);
  int cache_BackInfo_cyc;
  pCycleInfo cache_pBackInfo;

  Time goalie_catch_time;  
  
public:
  PositionInfo();
  ~PositionInfo();
  Bool Initialize();

  Bool ShutDown;
  ofstream   LogFile;

  //to make sure that memory is updated while another thread is reading
  pthread_mutex_t mutex_pos_info;

  void SetLastGoalieCatchTime(Time t) { goalie_catch_time = t; }
  Time GetLastGoalieCatchTime() { return goalie_catch_time; }
      
  void SetPlayMode(Pmode pm)
    { playMode = pm; playModeTime = GetTime(); }
  Time GetPlayModeTime() {return playModeTime; }

  void BeginNewCycle(Time time);
  void SetBallInfo(Vector pos, Vector vel);
  void SetPlayerInfo(char* team_name, Unum num,
		     Vector pos, Vector vel,
		     float body_ang, float face_ang);
  void SetPlayerInfo(TeamSide side, Unum num,
		     Vector pos, Vector vel,
		     float body_ang, float neck_ang);
  void SetPlayerType(char* team_name, Unum num, Ptype type);
  void SetPlayerType(TeamSide side, Unum num, Ptype type);
  void SetScores(int left_team_score, int right_team_score);
  
  Time        GetTime();
  Time        DropBallTime;
  /* so this is a little strange. The playMode in PositionInfo is used to
     keep track of the referee messages, but then when we get a sight, it's
     recorded onto the CycleInfo. All access functions go to the Cycle
     Info */  
  Pmode       GetPlayMode();
  BallInfo*   GetBall();
  PlayerInfo* GetPlayer(int num);
  PlayerInfo* GetPlayer(TeamSide side, Unum num);
  int         GetNumPlayers(TeamSide side);
  int         GetScore(TeamSide side);

  Time        GetPrevTime(int cycles_back);
  Pmode       GetPrevPlayMode(int cycles_back);
  BallInfo*   GetPrevBall(int cycles_back);
  PlayerInfo* GetPrevPlayer(int cycles_back, TeamSide side, Unum num);
  PlayerInfo* GetPrevPlayer(int cycles_back, int num);
  int         GetPrevScore(int cycles_back, TeamSide side);

  /* heterogeneous player stuff */
  PlayerType       PT[MAX_PLAYER_TYPES];
  int              LeftPlayerType[MAX_PLAYERS];
  int              RightPlayerType[MAX_PLAYERS];

  int   GetPlayerPT(TeamSide s, Unum n); 
  void  SetPlayerPT(TeamSide s, Unum n, int pt);
  inline float Player_speed_max(TeamSide s, Unum n)       
    {return PT[GetPlayerPT(s,n)].player_speed_max;}
  inline float Player_stamina_inc_max(TeamSide s, Unum n) 
    {return PT[GetPlayerPT(s,n)].stamina_inc_max;}
  inline float Player_stamina_inc(TeamSide s, Unum n) 
    {return PT[GetPlayerPT(s,n)].stamina_inc_max;}
  inline float Player_decay(TeamSide s, Unum n)          
    {return PT[GetPlayerPT(s,n)].player_decay;}
  inline float Player_inertia_moment(TeamSide s, Unum n) 
    {return PT[GetPlayerPT(s,n)].inertia_moment;}
  inline float Player_dash_power_rate(TeamSide s, Unum n)
    {return PT[GetPlayerPT(s,n)].dash_power_rate;}
  inline float Player_size(TeamSide s, Unum n)           
    {return PT[GetPlayerPT(s,n)].player_size;}
  inline float Player_kickable_margin(TeamSide s, Unum n)
    {return PT[GetPlayerPT(s,n)].kickable_margin;}
  inline float Player_kickable_area(TeamSide s, Unum n)
    {return Player_kickable_margin(s,n) + SP_ball_size + Player_size(s,n);}
  inline float Player_kick_rand(TeamSide s, Unum n)      
    {return PT[GetPlayerPT(s,n)].kick_rand;}
  inline float Player_extra_stamina(TeamSide s, Unum n)  
    {return PT[GetPlayerPT(s,n)].extra_stamina;}
  inline float Player_effort_max(TeamSide s, Unum n)     
    {return PT[GetPlayerPT(s,n)].effort_max;}
  inline float Player_effort_min(TeamSide s, Unum n)     
    {return PT[GetPlayerPT(s,n)].effort_min;}


  /* these are somewhat higher level functions */
  Unum ClosestPlayerToPoint(TeamSide side,Vector point,float* dist);
  int SortPlayersByDistanceToPoint(int cycles_back, TeamSide side,
				   Vector point, Unum* players);
  int SortPlayersByDistanceToPoint(TeamSide side, Vector point, Unum* players)
    { return SortPlayersByDistanceToPoint(0, side, point, players); }


  Rectangle FieldRectangle;


};


#endif
