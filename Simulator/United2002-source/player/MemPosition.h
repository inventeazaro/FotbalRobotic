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


#ifndef _POSITION_H_
#define _POSITION_H_

#include "MemPlayer.h"



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class Object {
public:

  /* Methods to get the position  */
  float    get_dist();  /* relative */
  AngleDeg get_ang_from_body();
  AngleDeg get_ang_from_neck();

  inline Vector get_abs_pos()
    { if (!pos_valid()) my_error ("can't get_abs_pos %d",type);  return gpos; }
  Vector get_rel_to_body_pos();
  Vector get_rel_to_neck_pos();
  inline float get_x() { return get_abs_pos().x; }
  inline float get_y() { return get_abs_pos().y; }
  inline float pos_valid() { return ( (gconf >= min_valid_conf) ? gconf : 0 ); }
  
  /* Methods to change the position                      */
  /* Changes do NOT take effect until update() is called */
  virtual void set_polar_from_neck(float d, float a, Time time);
  virtual void set_angle_from_neck(AngleDeg a, Time time);
  virtual void set_chinfo(float dist, float dir, Time time);

  /* Method which processes all changes */
  virtual void update();  
  virtual void reset ();  /* 0 confidences */
  virtual void clear_seen();
  virtual void sanitize_times();
  
  ObjType type;
 
  inline Time GetSeenTime() { return seen_time; }
  inline Time GetSeenMovingTime() { return chtime; }
 
protected:

  float max_conf;
  float min_valid_conf;

  Bool    seen;
  Bool    seen_moving;
  Time    seen_time;

  /* Based on the object's current position, should it be seen? */
  virtual Bool in_view_range(AngleDeg view_ang, float angle_buffer, float distance_buffer);
  
  float    dist;  /* polar */
  AngleDeg ang_from_neck;
  Time     pdtime;
  Time     patime;

  inline Time ptime() { return Min(pdtime,patime); }

  float  distch;  /* polar */
  float  dirch;
  Time   chtime;

  Vector rbpos;   /* relative to body */
  Time   rbtime;
  Vector rnpos;   /* relative to neck */
  Time   rntime;
  
  Vector gpos;    /* global   */
  Time   gtime;
  float  gconf;
};



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class StationaryObject : public Object {
public:

  void Initialize(MarkerType m, Vector pos, float max, float min_valid, Bool rotate);
  void Initialize(SideLine  l, Vector pos, float max, float min_valid, Bool rotate);

  Vector get_my_pos(AngleDeg my_neck_global_ang);
  AngleDeg get_my_neck_global_ang();
  Vector get_my_vel(AngleDeg my_neck_global_ang);

  int object_id;
};



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class MobileObject : public Object {
public:

  virtual void Initialize(ObjType t, float max, float min_valid, float decay, float motion, float speed);

  AngleDeg get_rel_to_body_heading();
  AngleDeg get_rel_to_neck_heading();
  Vector get_rel_to_body_vel(); /* relative direction, absolute speed */
  Vector get_rel_to_neck_vel(); /* relative direction, absolute speed */

  /* Methods to get the velocity */
  inline Vector get_abs_vel()  /* global   */
    { if (!vel_valid()) my_error ("can't get_abs_vel %d",type); return gvel; }

  inline float    get_speed()       { return get_abs_vel().mod(); }
  inline AngleDeg get_abs_heading() { return get_abs_vel().dir(); }     
  inline float    vel_valid()       { return ( (gvconf >= min_valid_conf) ? gvconf : 0 ); }
  virtual inline Bool moving()      { return vel_valid() && get_speed() != 0 ? TRUE : FALSE; } 
      
    /* Methods to change the position                      */
  /* Changes do NOT take effect until update() is called */
  virtual void set_heard_info (float x, float y, float conf, float dist, Time time);
  virtual void set_heard_info (float x, float y, float pconf, float dx, float dy, float vconf, 
			       float dist, Time time);

  /* Method which processes all changes */
  virtual void  update_kick(Time time) = 0; /* pure virtual function */
  virtual void  estimate_pos(Time time);
  void          estimate_vel(Time time);
  virtual void  update(Time time);
  virtual void  update_seen(Time time);
  void          update_estimate(Time time);
  void          update_heard(Time time);
  virtual void  reset();
  virtual void  clear_seen();
  virtual void  forget();
  virtual void  sanitize_times();

  Vector        estimate_future_pos(int steps, Vector extra_vel=0.0, Vector extra_vel_per=0.0);

  inline void   set_max_speed(float sp) {max_speed = sp;}
  inline void   set_motion_decay(float md) {motion_decay = md;}

protected:

  float conf_decay;
  float motion_decay;

  float max_speed;

  Vector gvel;    /* global   */

  Time   gvtime;
  float  gvconf;

private:

  Bool heard;
  Bool heard_moving;

  Vector rbvel;    /* body -- relative direction, absolute speed */
  Time   rbvtime;
  Vector rnvel;    /* neck -- relative direction, absolute speed */
  Time   rnvtime;

  Vector hpos;    /* heard */
  Vector hvel;    
  Time   hptime;
  Time   hvtime;
  float  hpdist;
  float  hvdist;
  float  hpconf;
  float  hvconf;
};



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class BallObject : public MobileObject {
public:
  void Initialize(float max, float min_valid, float decay, float motion, float max_speed);

  Bool moving();
  Bool kickable(float buffer = 0.0);
  Bool catchable();
  float get_kick_rate(Time time);

  void update_kick(Time time);
  void estimate_pos(Time time);
  void update(Time time);
  void update_seen(Time time);
        
  float calc_kick_rate(float dist, float ang);
  float calc_kick_rate() {return calc_kick_rate(get_dist(), get_ang_from_body());}

  void set_past_kick(float pow, AngleDeg ang, Time t);
  void forget_past_kick(Time t);

 protected:
  Bool  in_view_range(AngleDeg view_ang, float angle_buffer, float distance_buffer);
  
private:
  float effective_kick_rate;
  Time  ektime;

  Bool use_pos_based_vel_estimate;
  Time pos_based_vel_time;
  
  Time   last_seen_time;
  Vector last_seen_pos;

  /* we keep a record of the most recent kicks we try to do so that
     we can estimate the velocity of the ball from subsequent positions */
#define num_past_kicks (7)
  int past_kick_idx;
  struct PastKick {
    Time time;
    Vector kick_effect;
  } past_kicks[num_past_kicks];

  int past_kick_inc(int i)
    { return (i+1) % num_past_kicks; }
  int past_kick_dec(int i)
    { return ((i-1)+num_past_kicks) % num_past_kicks; }
};


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class PlayerObject : public MobileObject {
public:
  void Initialize(float max, float min_valid, float decay, float motion, float max_speed);

  char side;
  Unum unum;

  AngleDeg get_rel_to_body_body_ang();
  AngleDeg get_rel_to_body_neck_ang();
  AngleDeg get_rel_to_neck_body_ang();
  AngleDeg get_rel_to_neck_neck_ang();
  
  AngleDeg get_neck_rel_ang(); /* The player's relative neck angle */

  /* Methods to get the velocity */
  inline AngleDeg get_abs_body_ang() /* global   */
    { if (!body_ang_valid()) my_error ("can't get_abs_body_ang %d",type); return gbang; } 
  inline AngleDeg get_abs_neck_ang() /* global   */
    { if (!neck_ang_valid()) my_error ("can't get_abs_neck_ang %d",type); return gnang; } 
  inline float  body_ang_valid() { return ( (gbaconf >= min_valid_conf) ? gbaconf : 0 ); }
  inline float  neck_ang_valid() { return ( (gnaconf >= min_valid_conf) ? gnaconf : 0 ); }

  /* Methods to change the position                      */
  /* Changes do NOT take effect until update() is called */
  void set_body_ang_from_neck                 (AngleDeg body_ang, Time time);
  void set_neck_ang_from_neck                 (AngleDeg neck_ang, Time time);
  inline void set_body_and_neck_ang_from_neck (AngleDeg body_ang, AngleDeg neck_ang, Time time)
    { set_body_ang_from_neck(body_ang,time); set_neck_ang_from_neck(neck_ang,time); }

  void set_heard_info_w_angs (float x, float y, float pconf, float dx, float dy, float vconf, 
				  AngleDeg bang, float bconf, AngleDeg nang, float nconf, 
			          float dist, Time time);

  /* Method which processes all changes */
  void  update(Time time);
  void  update_seen_body_and_neck_angs(Time time);
  void  update_estimate(Time time);
  void  update_heard(Time time);
  void  reset();
  void  clear_seen();
  void  forget();
  void  sanitize_times();

  /* This is a pure virtual MobileObject fn.  Shouldn't be called for playerobject */
  inline void update_kick(Time time) { my_error("Can't kick players %d",time.t); }

protected:
  Bool  in_view_range(AngleDeg view_ang, float angle_buffer, float distance_buffer);

private:

  Bool     seen_body_ang;
  Bool     heard_body_ang;
  Bool     seen_neck_ang;
  Bool     heard_neck_ang;

  AngleDeg rnbang;  /* relative to neck body angle */
  Time     rnbatime;
  AngleDeg rnnang;  /* relative to neck neck angle */
  Time     rnnatime;

  AngleDeg gbang;  /* global body angle */
  float    gbaconf;
  Time     gbatime;
  AngleDeg gnang;  /* global neck angle */
  float    gnaconf;
  Time     gnatime;

  AngleDeg hbang;  /* heard body angle (global) */
  float    hbaconf;
  float    hbadist;
  Time     hbatime;
  AngleDeg hnang;  /* heard neck angle (global) */
  float    hnaconf;
  float    hnadist;
  Time     hnatime;
};


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

class TempPlayerObject {
public:

  float    dist;  /* relative */
  AngleDeg ang_from_neck;

  Time     time;
  char     side;

  inline void set(char s, float d, float a, Time t) { time = t; side = s; dist = d; ang_from_neck = a; }
};




/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

#define MAX_MARKERS 100
#define MAX_PLAYERS 25

#define Unum_Unknown  0
#define Unum_Teamless 100

class PositionInfo : public PlayerInfo{
public:

  ~PositionInfo();
  void Initialize();

  void SeeLine  (SideLine l, float dist, float ang, Time tm);
  void SeeLine  (SideLine l, float ang, Time tm);

  void SeeMarker(MarkerType marker, float dist, float ang, Time tm);
  void SeeMarker(MarkerType marker, float ang, Time tm);
  void SeeMarker(MarkerType marker, float dist, float ang, float distChng, float dirChng, Time tm);

  void SeeBall(float ang, Time tm);
  void SeeBall(float dist, float ang, Time tm);
  void SeeBall(float dist, float ang, float distChng, float dirChng, Time tm);

  void SeePlayer(float ang, Time time);
  void SeePlayer(float dist, float ang, Time time);
  void SeePlayer(char side, float ang, Time time);
  void SeePlayer(char side, float dist, float ang, Bool tackling, Time time);
  void SeePlayer(char side, float dist, float ang, float pointdir, Bool tackling, Time time);
  void SeePlayer(char side, Unum num, Bool goalie, float dist, float ang, 
		 float distChng, float dirChng, float bodydir, float neckdir, 
		 Bool tackling, Time time);
  void SeePlayer(char side, Unum num, Bool goalie, float dist, float ang, 
		 float distChng, float dirChng, float bodydir, float neckdir, 
		 float pointdir, Bool tackling, Time time);
  void SeePlayer(char side, Unum num, Bool goalie, float dist, float ang, Time time);
  void SeePlayer(char side, Unum num, Bool goalie, float ang, Time time);

  void ForgetPlayer(char side, Unum num);
  inline void ForgetTeammate(Unum num) { ForgetPlayer(MySide,num); }
  inline void ForgetOpponent(Unum num) { ForgetPlayer(TheirSide,num); }

  Unum GetOpponentGoalieNum();
  void SetOpponentGoalieNum(Unum goalie);

  void HearBall(float x, float y, float conf, float dist, Time time);
  void HearBall(float x, float y, float pconf, float dx, float dy, float vconf, float dist, Time time);

  void HearPlayer(char side, Unum num, float x, float y, float conf, float dist, Time time);
  void HearPlayer(char side, Unum num,
		  float x, float y, float pconf, float dx, float dy, float vconf, float dist, Time time);
  void HearPlayer(char side, Unum num, float x, float y, float pconf, float dx, float dy, float vconf, 
		  AngleDeg bang, float bconf, AngleDeg nang, float nconf, float dist, Time time);

  /* Access shortcuts -- Markers */
  inline float    MarkerDistance     (MarkerType m) { return GetMarker(m)->get_dist();  }
  inline float    MarkerDistance2     (MarkerType m) { return Sqr(GetMarker(m)->get_dist());  }
  inline AngleDeg MarkerAngleFromBody(MarkerType m) { return GetMarker(m)->get_ang_from_body();   }
  inline AngleDeg MarkerAngleFromNeck(MarkerType m) { return GetMarker(m)->get_ang_from_neck();   }
  inline Vector   MarkerPosition     (MarkerType m) { return GetMarker(m)->get_abs_pos();   }
  inline float    MarkerX            (MarkerType m) { return GetMarker(m)->get_x();     }
  inline float    MarkerY            (MarkerType m) { return GetMarker(m)->get_y();     }
  inline float    MarkerPositionValid(MarkerType m) { return GetMarker(m)->pos_valid(); }

  /* Access shortcuts -- Ball */
  inline float    BallDistance     () { return GetBall()->get_dist();  }
  inline AngleDeg BallAngleFromBody() { return GetBall()->get_ang_from_body();}
  inline AngleDeg BallAngleFromNeck() { return GetBall()->get_ang_from_neck();}
  inline Vector   BallAbsolutePosition() { return GetBall()->get_abs_pos();   }
  inline Vector   BallRelativeToBodyPosition() { return GetBall()->get_rel_to_body_pos();   }
  inline Vector   BallRelativeToNeckPosition() { return GetBall()->get_rel_to_neck_pos();   }
  inline float    BallX            () { return GetBall()->get_x();     }
  inline float    BallY            () { return GetBall()->get_y();     }
  inline float    BallPositionValid() { return GetBall()->pos_valid(); }

  inline float    BallSpeed          () { return GetBall()->get_speed();       }
  inline AngleDeg BallRelativeToBodyHeading() { return GetBall()->get_rel_to_body_heading(); }  
  inline AngleDeg BallRelativeToNeckHeading() { return GetBall()->get_rel_to_body_heading(); }  
  inline AngleDeg BallAbsoluteHeading() { return GetBall()->get_abs_heading(); }  
  inline Vector   BallRelativeToBodyVelocity() { return GetBall()->get_rel_to_body_vel();    }  
  inline Vector   BallRelativeToNeckVelocity() { return GetBall()->get_rel_to_neck_vel();    }  
  inline Vector   BallAbsoluteVelocity() { return GetBall()->get_abs_vel();    }  
  inline float    BallVelocityValid  () { return GetBall()->vel_valid();       }  
  inline Bool     BallMoving         () { return GetBall()->moving();          }
  inline Bool     BallKickable       (float buffer=0.0)
    { return GetBall()->kickable(buffer); }  
  inline Bool     BallCatchable      () 
    { return (GetBall()->catchable() && InOwnPenaltyArea() && 
	      CP_goalie && PlayMode == PM_Play_On)             ? TRUE : FALSE; }  
  inline float    BallKickRate       () { return GetBall()->get_kick_rate(CurrentTime);   }  

  /* Access shortcuts -- Players */
  inline float    PlayerDistance        (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0                         : GetPlayer(s,n)->get_dist();    }
  inline AngleDeg PlayerAngleFromBody   (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0                         : GetPlayer(s,n)->get_ang_from_body();  }
  inline AngleDeg PlayerAngleFromNeck   (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0                         : GetPlayer(s,n)->get_ang_from_neck();  }
  inline Vector   PlayerAbsolutePosition(char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyPos()                   : GetPlayer(s,n)->get_abs_pos(); }
  inline Vector   PlayerRelativeToBodyPosition(char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? Vector(0,0)               : GetPlayer(s,n)->get_rel_to_body_pos(); }
  inline Vector   PlayerRelativeToNeckPosition(char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? Vector(0,0)               : GetPlayer(s,n)->get_rel_to_neck_pos(); }
  inline float    PlayerX               (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyX()                     : GetPlayer(s,n)->get_x();       }
  inline float    PlayerY               (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyY()                     : GetPlayer(s,n)->get_y();       }
  float    PlayerPositionValid          (char s, Unum n);

  inline float    PlayerSpeed           (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MySpeed()                 : GetPlayer(s,n)->get_speed();       }
  inline AngleDeg PlayerRelativeToBodyHeading (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0                 : GetPlayer(s,n)->get_rel_to_body_heading(); }  
  inline AngleDeg PlayerRelativeToNeckHeading (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0                 : GetPlayer(s,n)->get_rel_to_neck_heading(); }  
  inline AngleDeg PlayerAbsoluteHeading (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyDir()                   : GetPlayer(s,n)->get_abs_heading(); }  
  inline Vector   PlayerAbsoluteVelocity(char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyVel()                   : GetPlayer(s,n)->get_abs_vel();     }  
  inline Vector   PlayerRelativeToBodyVelocity(char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? Polar2Vector(MySpeed(),0) : GetPlayer(s,n)->get_rel_to_body_vel();}
  inline Vector   PlayerRelativeToNeckVelocity(char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? Polar2Vector(MySpeed(),0) : GetPlayer(s,n)->get_rel_to_neck_vel();}
  float    PlayerVelocityValid          (char s, Unum n); 
  inline AngleDeg PlayerRelativeToBodyBodyAngle  (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0              : GetPlayer(s,n)->get_rel_to_body_body_ang();    }  
  inline AngleDeg PlayerRelativeToNeckBodyAngle  (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0              : GetPlayer(s,n)->get_rel_to_neck_body_ang();    }  
  inline AngleDeg PlayerRelativeToBodyNeckAngle  (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0              : GetPlayer(s,n)->get_rel_to_body_neck_ang();    }  
  inline AngleDeg PlayerRelativeToNeckNeckAngle  (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0              : GetPlayer(s,n)->get_rel_to_neck_neck_ang();    }  
  inline AngleDeg PlayerRelativeNeckAngle (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? 0              : GetPlayer(s,n)->get_neck_rel_ang();    }  
  inline AngleDeg PlayerAbsoluteBodyAngle    (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyBodyAng()               : GetPlayer(s,n)->get_abs_body_ang(); }  
  float    PlayerBodyAngleValid              (char s, Unum n);
  inline AngleDeg PlayerAbsoluteNeckAngle    (char s, Unum n) 
    { return (s==MySide && n==MyNumber) ? MyNeckGlobalAng()         : GetPlayer(s,n)->get_abs_neck_ang(); }  
  float    PlayerNeckAngleValid              (char s, Unum n);

  /* Access shortcuts -- Teammates */
  inline float    TeammateDistance        (Unum n) { return PlayerDistance        (MySide,n); }
  inline AngleDeg TeammateAngleFromBody   (Unum n) { return PlayerAngleFromBody   (MySide,n); }
  inline AngleDeg TeammateAngleFromNeck   (Unum n) { return PlayerAngleFromNeck   (MySide,n); }
  inline Vector   TeammateAbsolutePosition(Unum n) { return PlayerAbsolutePosition(MySide,n); }
  inline Vector   TeammateRelativeToBodyPosition(Unum n) { return PlayerRelativeToBodyPosition(MySide,n); }
  inline Vector   TeammateRelativeToNeckPosition(Unum n) { return PlayerRelativeToNeckPosition(MySide,n); }
  inline float    TeammateX               (Unum n) { return PlayerX               (MySide,n); }
  inline float    TeammateY               (Unum n) { return PlayerY               (MySide,n); }
  inline float    TeammatePositionValid   (Unum n) { return PlayerPositionValid   (MySide,n); }

  inline float    TeammateSpeed           (Unum n) { return PlayerSpeed           (MySide,n); }
  inline AngleDeg TeammateRelativeToBodyHeading (Unum n) { return PlayerRelativeToBodyHeading (MySide,n); }
  inline AngleDeg TeammateRelativeToNeckHeading (Unum n) { return PlayerRelativeToNeckHeading (MySide,n); }
  inline AngleDeg TeammateAbsoluteHeading (Unum n) { return PlayerAbsoluteHeading (MySide,n); }
  inline Vector   TeammateAbsoluteVelocity(Unum n) { return PlayerAbsoluteVelocity(MySide,n); }
  inline Vector   TeammateRelativeToBodyVelocity(Unum n) { return PlayerRelativeToBodyVelocity(MySide,n); }
  inline Vector   TeammateRelativeToNeckVelocity(Unum n) { return PlayerRelativeToNeckVelocity(MySide,n); }
  inline float    TeammateVelocityValid   (Unum n) { return PlayerVelocityValid   (MySide,n); }
  inline AngleDeg TeammateRelativeToBodyBodyAngle(Unum n) { return PlayerRelativeToBodyBodyAngle (MySide,n); }
  inline AngleDeg TeammateRelativeToNeckBodyAngle(Unum n) { return PlayerRelativeToNeckBodyAngle (MySide,n); }
  inline AngleDeg TeammateRelativeToBodyNeckAngle(Unum n) { return PlayerRelativeToBodyNeckAngle (MySide,n); }
  inline AngleDeg TeammateRelativeToNeckNeckAngle(Unum n) { return PlayerRelativeToNeckNeckAngle (MySide,n); }
  inline AngleDeg TeammateRelativeNeckAngle    (Unum n) { return PlayerRelativeNeckAngle (MySide,n); }
  inline AngleDeg TeammateAbsoluteBodyAngle    (Unum n) { return PlayerAbsoluteBodyAngle    (MySide,n); }
  inline float    TeammateBodyAngleValid       (Unum n) { return PlayerBodyAngleValid       (MySide,n); }
  inline AngleDeg TeammateAbsoluteNeckAngle    (Unum n) { return PlayerAbsoluteNeckAngle    (MySide,n); }
  inline float    TeammateNeckAngleValid       (Unum n) { return PlayerNeckAngleValid       (MySide,n); }

  /* Access shortcuts -- Opponents */
  inline float    OpponentDistance        (Unum n) { return PlayerDistance        (TheirSide,n); }
  inline AngleDeg OpponentAngleFromBody   (Unum n) { return PlayerAngleFromBody   (TheirSide,n); }
  inline AngleDeg OpponentAngleFromNeck   (Unum n) { return PlayerAngleFromNeck   (TheirSide,n); }
  inline Vector   OpponentAbsolutePosition(Unum n) { return PlayerAbsolutePosition(TheirSide,n); }
  inline Vector   OpponentRelativeToBodyPosition(Unum n) { return PlayerRelativeToBodyPosition(TheirSide,n); }
  inline Vector   OpponentRelativeToNeckPosition(Unum n) { return PlayerRelativeToNeckPosition(TheirSide,n); }
  inline float    OpponentX               (Unum n) { return PlayerX               (TheirSide,n); }
  inline float    OpponentY               (Unum n) { return PlayerY               (TheirSide,n); }
  inline float    OpponentPositionValid   (Unum n) { return PlayerPositionValid   (TheirSide,n); }

  inline float    OpponentSpeed           (Unum n) { return PlayerSpeed           (TheirSide,n); }
  inline AngleDeg OpponentRelativeToBodyHeading (Unum n) { return PlayerRelativeToBodyHeading (TheirSide,n); }
  inline AngleDeg OpponentRelativeToNeckHeading (Unum n) { return PlayerRelativeToNeckHeading (TheirSide,n); }
  inline AngleDeg OpponentAbsoluteHeading (Unum n) { return PlayerAbsoluteHeading (TheirSide,n); }
  inline Vector   OpponentAbsoluteVelocity(Unum n) { return PlayerAbsoluteVelocity(TheirSide,n); }
  inline Vector   OpponentRelativeToBodyVelocity(Unum n) { return PlayerRelativeToBodyVelocity(TheirSide,n); }
  inline Vector   OpponentRelativeToNeckVelocity(Unum n) { return PlayerRelativeToNeckVelocity(TheirSide,n); }
  inline float    OpponentVelocityValid   (Unum n) { return PlayerVelocityValid   (TheirSide,n); }
  inline AngleDeg OpponentRelativeToBodyBodyAngle(Unum n){ return PlayerRelativeToBodyBodyAngle(TheirSide,n);}
  inline AngleDeg OpponentRelativeToNeckBodyAngle(Unum n){ return PlayerRelativeToNeckBodyAngle(TheirSide,n);}
  inline AngleDeg OpponentRelativeToBodyNeckAngle(Unum n){ return PlayerRelativeToBodyNeckAngle(TheirSide,n);}
  inline AngleDeg OpponentRelativeToNeckNeckAngle(Unum n){ return PlayerRelativeToNeckNeckAngle(TheirSide,n);}
  inline AngleDeg OpponentRelativeNeckAngle    (Unum n) { return PlayerRelativeNeckAngle (TheirSide,n); }
  inline AngleDeg OpponentAbsoluteBodyAngle    (Unum n) { return PlayerAbsoluteBodyAngle    (TheirSide,n); }
  inline float    OpponentBodyAngleValid       (Unum n) { return PlayerBodyAngleValid       (TheirSide,n); }
  inline AngleDeg OpponentAbsoluteNeckAngle    (Unum n) { return PlayerAbsoluteNeckAngle    (TheirSide,n); }
  inline float    OpponentNeckAngleValid       (Unum n) { return PlayerNeckAngleValid       (TheirSide,n); }

  /* more complex shortcuts */
  /* kickable for other players */
  /* A positive buffer makes it more difficult for it to be considered kickable */
  Bool BallKickableForPlayer(char s, Unum n, float buffer=0);
  inline Bool BallKickableForTeammate(Unum n, float buffer=0) 
    { return BallKickableForPlayer(MySide, n, buffer); }
  inline Bool BallKickableForOpponent(Unum n, float buffer=0) 
    { return BallKickableForPlayer(TheirSide, n, buffer); }

  inline Bool CanFaceBallWithNeck()
    { return CanFaceAngleFromBodyWithNeck(BallAngleFromBody()); }
  inline Bool CanFacePlayerWithNeck(char s, Unum n)
    { return CanFaceAngleFromBodyWithNeck(PlayerAngleFromBody(s,n)); }
  inline Bool CanFaceTeammateWithNeck(Unum n)
    { return CanFaceAngleFromBodyWithNeck(TeammateAngleFromBody(n)); }
  inline Bool CanFaceOpponentWithNeck(Unum n)
    { return CanFaceAngleFromBodyWithNeck(OpponentAngleFromBody(n)); }

  inline Bool CanSeeBallWithNeck()
    { return CanSeeAngleFromBodyWithNeck(BallAngleFromBody()); }
  inline Bool CanSeePlayerWithNeck(char s, Unum n)
    { return CanSeeAngleFromBodyWithNeck(PlayerAngleFromBody(s,n)); }
  inline Bool CanSeeTeammateWithNeck(Unum n)
    { return CanSeeAngleFromBodyWithNeck(TeammateAngleFromBody(n)); }
  inline Bool CanSeeOpponentWithNeck(Unum n)
    { return CanSeeAngleFromBodyWithNeck(OpponentAngleFromBody(n)); }

  /* is player behind us */
  /* three forms for third arg:
     angle: absolute angle indicating the front
     Vector: position whose direction indicates the front
     none: use MyAng */
  /* is point behind us */
  /* three forms for third arg:
     angle: absolute angle indicating the front
     Vector: position whose direction indicates the front
     none: use MyAng */
  inline Bool IsPointBehind(Vector pt, AngleDeg ang)
    {
      return (fabs(GetNormalizeAngleDeg(AngleToFromBody(pt) + MyBodyAng() - ang)) > CP_behind_angle)
	? TRUE : FALSE;      
    }
  inline Bool IsPointBehind(Vector pt, Vector targPos)
    {
      return IsPointBehind(pt, (targPos - MyPos()).dir());
    }
  inline Bool IsPointBehindBody(Vector pt)
    {
      return (fabs(AngleToFromBody(pt)) > CP_behind_angle)
	? TRUE : FALSE;
    }
  inline Bool IsPointBehindNeck(Vector pt)
    {
      return (fabs(AngleToFromNeck(pt)) > CP_behind_angle)
	? TRUE : FALSE;
    }

  inline Bool IsPlayerBehind(char s, Unum n, AngleDeg ang)
    {      
      if (!PlayerPositionValid(s, n))
	my_error("Can't tell if player is behind if I don't know where he is");
      return (fabs(PlayerAngleFromBody(s, n) + MyBodyAng() - ang) > CP_behind_angle)
	? TRUE : FALSE;
    }
  inline Bool IsPlayerBehind(char s, Unum n, Vector targPos)
    {
      return IsPlayerBehind(s, n, (targPos - MyPos()).dir());
    }
  inline Bool IsPlayerBehindFromBody(char s, Unum n)
    {
      if (!PlayerPositionValid(s, n))
	my_error("Can't tell if player is behind if I don't know where he is");
      return (fabs(PlayerAngleFromBody(s, n)) > CP_behind_angle)
	? TRUE : FALSE;
    }

  inline Bool IsPlayerBehindFromNeck(char s, Unum n)
    {
      if (!PlayerPositionValid(s, n))
	my_error("Can't tell if player is behind if I don't know where he is");
      return (fabs(PlayerAngleFromNeck(s, n)) > CP_behind_angle)
	? TRUE : FALSE;
    }

  inline Bool IsTeammateBehind(Unum n, AngleDeg ang)
    { return IsPlayerBehind(MySide, n, ang); }
  inline Bool IsTeammateBehind(Unum n, Vector targPos)
    { return IsPlayerBehind(MySide, n, targPos); }
  inline Bool IsTeammateBehindFromBody(Unum n)
    { return IsPlayerBehindFromBody(MySide, n); }
  inline Bool IsTeammateBehindFromNeck(Unum n)
    { return IsPlayerBehindFromNeck(MySide, n); }
  
  inline Bool IsOpponentBehind(Unum n, AngleDeg ang)
    { return IsPlayerBehind(TheirSide, n, ang); }
  inline Bool IsOpponentBehind(Unum n, Vector targPos)
    { return IsPlayerBehind(TheirSide, n, targPos); }
  inline Bool IsOpponentBehindFromBody(Unum n)
    { return IsPlayerBehindFromBody(TheirSide, n); }
  inline Bool IsOpponentBehindFromNeck(Unum n)
    { return IsPlayerBehindFromNeck(TheirSide, n); }
  
  
  /* Relative markers */
  MarkerType RM_My_Goal;
  MarkerType RM_Their_Goal;
  MarkerType RM_LB_Flag;
  MarkerType RM_LC_Flag;
  MarkerType RM_LF_Flag;
  MarkerType RM_RB_Flag;
  MarkerType RM_RC_Flag;
  MarkerType RM_RF_Flag;
  MarkerType RM_My_PC_Flag;    /* Center of my penalty area */
  MarkerType RM_Their_PC_Flag; /* Center of theirs          */

  /* predicted positions */
  Vector BallPredictedPosition(int steps, float kick_power, AngleDeg kick_dir);
  Vector BallPredictedPositionWithQueuedActions(int steps = 1);
  inline Vector BallPredictedPosition(int steps = 1) 
    { return GetBall()->estimate_future_pos(steps); }

  inline Vector PlayerPredictedPosition(char side, Unum num, int steps = 1, Vector dash_per=Vector(0,0))
    { return GetPlayer(side,num)->estimate_future_pos(steps, 0, dash_per); }
  inline Vector TeammatePredictedPosition(Unum num, int steps = 1, Vector dash_per=Vector(0,0)) 
    { return PlayerPredictedPosition(MySide,num,steps, dash_per); }
  inline Vector OpponentPredictedPosition(Unum num, int steps = 1, Vector dash_per=Vector(0,0)) 
    { return PlayerPredictedPosition(TheirSide,num,steps, dash_per); }

  Bool BallWillBeKickable(int steps = 1, float dash_power = 0, float buffer = 0);

  Bool WillKickBeCollision(float kick_power, AngleDeg kick_dir, float buffer = 0) 
    { return ((BallPredictedPosition(1, kick_power, kick_dir) - MyPredictedPosition()).mod() <= 
	      My_size() + SP_ball_size + buffer)  ? TRUE : FALSE;  } 
  Bool WillDashBeCollision(float dash_power, float buffer = 0) 
    { return ((BallPredictedPosition() - MyPredictedPosition(1, dash_power)).mod() <= 
          My_size() + SP_ball_size + buffer) ? TRUE : FALSE;  } 

  /* PredictedCyclesToPoint */
  /* a buffer of 0 is a bad idea! */
  int PlayerPredictedCyclesToPoint(char side, Unum num, Vector pt,
				   float dash_power, float buffer);
  int PlayerPredictedCyclesToPoint(char side, Unum num, Vector pt, float dash_power)
    { return PlayerPredictedCyclesToPoint(side, num, pt, dash_power, CP_at_point_buffer); }
  int PlayerPredictedCyclesToPoint(char side, Unum num, Vector pt)
    { return PlayerPredictedCyclesToPoint(side, num, pt, SP_max_power); }
  int TeammatePredictedCyclesToPoint(Unum num, Vector pt, float dash_power, float buffer)
    { return PlayerPredictedCyclesToPoint(MySide, num, pt, dash_power, buffer); }
  int TeammatePredictedCyclesToPoint(Unum num, Vector pt, float dash_power)
    { return PlayerPredictedCyclesToPoint(MySide, num, pt, dash_power); }
  int TeammatePredictedCyclesToPoint(Unum num, Vector pt)
    { return PlayerPredictedCyclesToPoint(MySide, num, pt); }
  int OpponentPredictedCyclesToPoint(Unum num, Vector pt, float dash_power, float buffer)
    { return PlayerPredictedCyclesToPoint(TheirSide, num, pt, dash_power, buffer); }
  int OpponentPredictedCyclesToPoint(Unum num, Vector pt, float dash_power)
    { return PlayerPredictedCyclesToPoint(TheirSide, num, pt, dash_power); }
  int OpponentPredictedCyclesToPoint(Unum num, Vector pt)
    { return PlayerPredictedCyclesToPoint(TheirSide, num, pt); }
  
  Unum PlayerWithBall(float buffer=0);    /* with ball means actually kickable      */
  Unum TeammateWithBall(float buffer=0);  
  Unum OpponentWithBall(float buffer=0);  
  char TeamWithBall(float buffer=0);   

  /* Direct access functions */
  inline StationaryObject *GetMarker(MarkerType m) { return &Marker[m]; }

  inline BallObject *GetBall() { return &Ball; }

  inline PlayerObject **Teammates() { return MyPlayer; }
  inline PlayerObject **Opponents() { return TheirPlayer; }
  inline PlayerObject **TeamlessPlayers() { return TeamlessPlayer; }

  inline int NumTeammates() { return num_my_players; }
  inline int NumOpponents() { return num_their_players; }
  inline int NumTeamlessPlayers()  { return num_teamless_players; }
  inline int NumPlayers()   { return num_my_players + num_their_players + num_teamless_players; }

  PlayerObject *GetTeammate(Unum num);
  PlayerObject *GetOpponent(Unum num);
  PlayerObject *GetPlayer(char side, Unum num);

  /* Player data-structure management */
  PlayerObject *GetNewPlayer(char side, Unum num);
  Bool         ForgetAPlayer(char side);
  void         CleanMyPlayers();
  void         CleanTheirPlayers();
  void         CleanTeamlessPlayers();
  void         CleanAllPlayers();
  Bool         ResetMyDuplicatePlayers();
  Bool         ResetTheirDuplicatePlayers();
  void         ClearSeenInfo();

  MarkerType ClosestMarker;
  MarkerType ClosestMotionMarker;

  MarkerType ClosestGoal();
  MarkerType ClosestFlagTo();
  
  int SortPlayersBy(char side, char keyFunc, float keyNum, Unum* players);
                                    /* side = m,t,b (both) keyFunc = d,a
				       (dist/ang), keyNum = central val.
				       players = array.  Return num found */
  int SortPlayersByDistanceToPoint(char side, Vector point, Unum* players);
  int SortPlayersByDistanceToLine(char side, Line line, Unum* players, 
				  Bool TestEndPoints=FALSE, Vector ep1=0, Vector ep2=0);

  int NumTeammatesWithin(float Dist, Vector of_pos);
  inline int NumTeammatesWithin(float Dist) 
    { if (!MyConf()) my_error("nope"); return NumTeammatesWithin(Dist, MyPos()); }
  int NumOpponentsWithin(float Dist, Vector of_pos);
  inline int NumOpponentsWithin(float Dist) 
    { if (!MyConf()) my_error("nope"); return NumOpponentsWithin(Dist, MyPos()); }
  inline int NumPlayersWithin(float Dist, Vector of_pos)
    { return NumTeammatesWithin(Dist,of_pos) +  NumOpponentsWithin(Dist,of_pos); }
  inline int NumPlayersWithin(float Dist)   
    { if (!MyConf()) my_error("nope"); return NumPlayersWithin(Dist,MyPos()); }

  /* These are needed for the Decision Tree, Dodge player -- use dist and ang */
  int NumTeammatesWithin(float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng);
  inline int NumTeammatesWithin(float Dist, AngleDeg Ang) {return NumTeammatesWithin(Dist, Ang, 0, 0);}
  int NumOpponentsWithin(float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng);
  inline int NumOpponentsWithin(float Dist, AngleDeg Ang) {return NumOpponentsWithin(Dist, Ang, 0, 0);}
  inline int NumPlayersWithin  (float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng)
    { return NumTeammatesWithin(Dist,Ang,ofDist,ofAng) +  NumOpponentsWithin(Dist,Ang,ofDist,ofAng); }
  inline int NumPlayersWithin(float Dist, AngleDeg Ang)   {return NumPlayersWithin(Dist, Ang, 0, 0);}

  PlayerObject *GetPlayerWithin(float Dist, Vector ofPos);
  PlayerObject *GetPlayerWithin(float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng);

  int NumOpponentsInCone(float wid_dist_ratio, Vector end, Vector vert);
  int NumTeammatesInCone(float wid_dist_ratio, Vector end, Vector vert, Bool IncludeMe = FALSE);
  inline int NumPlayersInCone(float wid_dist_ratio, Vector end, Vector vert)
    { return NumOpponentsInCone(wid_dist_ratio, end, vert) +
	NumTeammatesInCone(wid_dist_ratio, end, vert); 
    }

  int NumOpponentsInCone(float wid_dist_ratio, Vector end)
    { return NumOpponentsInCone(wid_dist_ratio, end, MyPos()); }
  int NumTeammatesInCone(float wid_dist_ratio, Vector end)
    { return NumTeammatesInCone(wid_dist_ratio, end, MyPos()); }
  int NumPlayersInCone(float wid_dist_ratio, Vector end)
    { return NumPlayersInCone(wid_dist_ratio, end, MyPos()); }

  /* which is 'm' (my team), 't' (thier team), 'b' (both) */
  /* this should probably be private */
  int NumPlayersInConeToPlayer(char which,
			       float wid_dist_ratio, char side, Unum num,
			       float extra_len, Vector vert);

  int NumPlayersInConeToPlayer(float wid_dist_ratio, char side, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('b', wid_dist_ratio, side, num, extra_len, vert); }
  int NumTeammatesInConeToPlayer(float wid_dist_ratio, char side, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('m', wid_dist_ratio, side, num, extra_len, vert); }
  int NumOpponentsInConeToPlayer(float wid_dist_ratio, char side, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('t', wid_dist_ratio, side, num, extra_len, vert); }
  int NumPlayersInConeToTeammate(float wid_dist_ratio, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('b', wid_dist_ratio, MySide, num, extra_len, vert); }
  int NumTeammatesInConeToTeammate(float wid_dist_ratio, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('m', wid_dist_ratio, MySide, num, extra_len, vert); }
  int NumOpponentsInConeToTeammate(float wid_dist_ratio, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('t', wid_dist_ratio, MySide, num, extra_len, vert); }
  int NumPlayersInConeToOpponent(float wid_dist_ratio, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('b', wid_dist_ratio, TheirSide, num, extra_len, vert); }
  int NumTeammatesInConeToOpponent(float wid_dist_ratio, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('m', wid_dist_ratio, TheirSide, num, extra_len, vert); }
  int NumOpponentsInConeToOpponent(float wid_dist_ratio, Unum num, float extra_len, Vector vert)
    { return NumPlayersInConeToPlayer('t', wid_dist_ratio, TheirSide, num, extra_len, vert); }

  int NumPlayersInConeToPlayer(float wid_dist_ratio, char side, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('b', wid_dist_ratio, side, num, extra_len, MyPos()); }
  int NumTeammatesInConeToPlayer(float wid_dist_ratio, char side, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('m', wid_dist_ratio, side, num, extra_len, MyPos()); }
  int NumOpponentsInConeToPlayer(float wid_dist_ratio, char side, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('t', wid_dist_ratio, side, num, extra_len, MyPos()); }
  int NumPlayersInConeToTeammate(float wid_dist_ratio, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('b', wid_dist_ratio, MySide, num, extra_len, MyPos()); }
  int NumTeammatesInConeToTeammate(float wid_dist_ratio, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('m', wid_dist_ratio, MySide, num, extra_len, MyPos()); }
  int NumOpponentsInConeToTeammate(float wid_dist_ratio, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('t', wid_dist_ratio, MySide, num, extra_len, MyPos()); }
  int NumPlayersInConeToOpponent(float wid_dist_ratio, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('b', wid_dist_ratio, TheirSide, num, extra_len, MyPos()); }
  int NumTeammatesInConeToOpponent(float wid_dist_ratio, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('m', wid_dist_ratio, TheirSide, num, extra_len, MyPos()); }
  int NumOpponentsInConeToOpponent(float wid_dist_ratio, Unum num, float extra_len = 0.0)
    { return NumPlayersInConeToPlayer('t', wid_dist_ratio, TheirSide, num, extra_len, MyPos()); }

  Unum ClosestTeammateTo(Vector p, Bool include_me = TRUE);
  Unum ClosestOpponentTo(Vector p);
  Vector ClosestTeamlessPlayerPosition();
  Unum ClosestTeammateToBall(Bool include_me = TRUE);
  Unum ClosestOpponentToBall();
  float ClosestTeammateToBallDistance(Bool include_me = TRUE); 

  inline int NumTeammatesCloserTo(Vector p) { return NumTeammatesWithin(DistanceTo(p),p) - 1; }
  inline int NumOpponentsCloserTo(Vector p) { return NumOpponentsWithin(DistanceTo(p),p); }
  inline int NumPlayersCloserTo(Vector p)   { return NumTeammatesCloserTo(p) + NumOpponentsCloserTo(p); }
  inline int NumTeammatesCloserToBall()     { return NumTeammatesCloserTo(BallAbsolutePosition()); }
  inline int NumOpponentsCloserToBall()     { return NumOpponentsCloserTo(BallAbsolutePosition()); }
  inline int NumPlayersCloserToBall()       { return NumPlayersCloserTo(BallAbsolutePosition()); }

  float PlayerDistanceTo(char s, Unum n, Vector p);
  inline float TeammateDistanceTo(Unum n, Vector p) { return PlayerDistanceTo(MySide,n,p); }
  inline float OpponentDistanceTo(Unum n, Vector p) { return PlayerDistanceTo(TheirSide,n,p); }

  float PlayerDistance2To(char s, Unum n, Vector p);
  inline float TeammateDistance2To(Unum n, Vector p) { return PlayerDistance2To(MySide,n,p); }
  inline float OpponentDistance2To(Unum n, Vector p) { return PlayerDistance2To(TheirSide,n,p); }

  float PlayerDistanceToLine(char s, Unum n, Line l);
  inline float TeammateDistanceToLine(Unum n, Line l) { return PlayerDistanceToLine(MySide,n,l); }
  inline float OpponentDistanceToLine(Unum n, Line l) { return PlayerDistanceToLine(TheirSide,n,l); }

  float PlayerDistance2ToLine(char s, Unum n, Line l);
  inline float TeammateDistance2ToLine(Unum n, Line l) { return PlayerDistance2ToLine(MySide,n,l); }
  inline float OpponentDistance2ToLine(Unum n, Line l) { return PlayerDistance2ToLine(TheirSide,n,l); }

  inline float PlayerDistanceToBall(char side, Unum num)  
    { if (!BallPositionValid()) my_error("Ball unknown"); 
      return PlayerDistanceTo(side,num,BallAbsolutePosition()); } 
  inline float TeammateDistanceToBall(Unum num) { return PlayerDistanceToBall(MySide,num); } 
  inline float OpponentDistanceToBall(Unum num) { return PlayerDistanceToBall(TheirSide,num); } 

  /* Which side of the field is the object on? */
  inline Fside BallLocationSide() { return LocationSide(BallAbsolutePosition()); }
  inline Fside TeammateLocationSide(Unum t) { return LocationSide(TeammateAbsolutePosition(t)); }
  inline Fside OpponentLocationSide(Unum o) { return LocationSide(OpponentAbsolutePosition(o)); }
  inline Fside PlayerLocationSide(char s, Unum p) { return LocationSide(PlayerAbsolutePosition(s,p)); }

  inline Unum ClosestTeammate() 
    { if (!MyConf()) my_error("I'm lost"); return ClosestTeammateTo(MyPos(),FALSE); }
  inline Unum ClosestOpponent() 
    { if (!MyConf()) my_error("I'm lost"); return ClosestOpponentTo(MyPos()); }


  Unum FurthestBackTeammate(Bool IncludeUs = TRUE);
  /* if we can't find any players, this will return (0,0) */
  Vector PositionOfFurthestBackTeammate(Bool IncludeUs = TRUE);
  Unum FurthestBackOpponent();
  /* if we can't find any players, this will return (0,0) */
  Vector PositionOfFurthestBackPlayer(Bool IncludeUs = TRUE);

  Unum FurthestForwardTeammate(Bool IncludeUs = TRUE);
  Unum FurthestForwardOpponent(Bool IncludeGoalie = TRUE);
  /* if we can't find any players, this will return (0,0) */
  Vector PositionOfFurthestForwardPlayer(Bool IncludeUs = TRUE);

  float AngleBetweenClosestTwoOpponents(Vector p);

  Bool InOwnPenaltyArea(Vector p);
  Bool InOwnPenaltyArea();
  Bool BallInOwnPenaltyArea();
  Bool FacingBackInOwnPA();
  Bool FacingBackNearOwnGoal();

  Bool IsPointInBounds(float x, float y, float buffer=0);
  Bool IsPointInBounds(Vector pt, float buffer=0) { return IsPointInBounds(pt.x, pt.y, buffer); }
  Rectangle ShiftRectangleInBounds(Rectangle *rect);

  void update();
  void update_self_seen(Time time);
  void update_self(Time time);
  void update_ball(Time time);
  void update_players(Time time);
  void reconcile_unknown_players();

  void VerifyDash(float *dash_power);

  Vector PositionToKickoffPosition(const Vector pos);

  /* offside stuff */

  float my_offside_line;    /* In front of which I'm offside      */
  float their_offside_line; /* Behind which other team is offside */
  void  update_offside_lines();

  Bool OffsidePosition(float x, char side);
  inline Bool OffsidePosition(Vector pos, char side) { return OffsidePosition(pos.x,side); }
  Bool TeammateInOffsidePosition(Unum num);
  Bool OpponentInOffsidePosition(Unum num);
  Bool PlayerInOffsidePosition(char side, Unum num);
  inline Bool InOffsidePosition() { return TeammateInOffsidePosition(MyNumber); }
  Unum TeammateOffsideIfIKick();

  float XToAdjustForOffsideX(float x, float buffer);
  inline float XToAdjustForOffsideX(float x) { return XToAdjustForOffsideX(x,CP_at_point_buffer); }
  inline Vector PositionToAdjustForOffsidePosition(Vector pos, float buffer)
    { pos.x = XToAdjustForOffsideX(pos.x,buffer); return pos; }
  inline Vector PositionToAdjustForOffsidePosition(Vector pos)
    { pos.x = XToAdjustForOffsideX(pos.x,CP_at_point_buffer); return pos; }
  Rectangle RectangleToAdjustForOffsideRectangle(Rectangle *rect, float buffer);
  inline Rectangle RectangleToAdjustForOffsideRectangle(Rectangle *rect)
    { return RectangleToAdjustForOffsideRectangle(rect,CP_at_point_buffer); }
  
  inline float XToOnsideX(float x, float buffer) { return Min(x, my_offside_line-buffer); }
  inline float XToOnsideX(float x) { return XToOnsideX(x,CP_at_point_buffer); }

  inline Vector PositionToOnsidePosition(Vector pos, float buffer) 
    { pos.x = XToOnsideX( pos.x, buffer ); return pos; }
  inline Vector PositionToOnsidePosition(Vector pos) 
    { return PositionToOnsidePosition(pos,CP_at_point_buffer); }

  Vector PositionToPullOffsidePosition(Vector pos, float buffer);
  inline Vector PositionToPullOffsidePosition(Vector pos) 
    { return PositionToPullOffsidePosition(pos,CP_at_point_buffer); }

  Bool  PullOffside;
  float PullOffsidePosition;

  /* congestion stuff */
  
  float Congestion(Vector pos, Bool consider_me = FALSE);
  float TeammateCongestion(Unum teammate, Bool consider_me = TRUE);
  inline float MyCongestion() { return TeammateCongestion(MyNumber,FALSE); }
  Unum  LeastCongestedTeammate();
  Vector LeastCongestedValidPointInRectangle(Rectangle *rect, Bool attract = FALSE, Vector attract_point = 0);
  Vector LeastCongestedValidPointForPassFromInRectangle(Rectangle *rect, Vector from, 
					     Bool attract = FALSE, Vector attract_point = 0);

  inline void SetTeammateTired(Unum num, Time time) { TiredTimes[num] = time; }
  void SetTeammateStamina(Unum num, float stamina, Time time);
  Bool TeammateTired(Unum num);
  float GetTeammateStamina(Unum num);
  Time TeammateStaminaTime(Unum num);
  inline void SetAnnouncedTired(Time time) { SetTeammateTired(MyNumber,time); }
  Bool NeedToAnnounceTired();
  
  Rectangle OwnPenaltyArea;
  Rectangle OwnGoalieArea;
  Rectangle TheirPenaltyArea;
  Rectangle TheirGoalieArea;
  Rectangle FieldRectangle;
  Vector    MyLeftGoalKickSpot;
  Vector    MyRightGoalKickSpot;
  Vector    TheirLeftGoalKickSpot;
  Vector    TheirRightGoalKickSpot;

  /* for the new position based velocity correction */
  Time   sight_position_correction_time;
  Vector sight_position_correction;

  /* for ball vcelocity invalidation */
  float quantize_err_const;
  float Tan_of_half_deg; /* the tangent of 1/2 degree */

  /* heterogeneous player stuff */
  int   GetPlayerPT(char s, int n); 
  void  SetPlayerPT(char s, int n, int pt);
  inline float Player_speed_max(char s, int n)       
    {return PT[GetPlayerPT(s,n)].player_speed_max;}
  inline float Player_stamina_inc_max(char s, int n) 
    {return PT[GetPlayerPT(s,n)].stamina_inc_max;}
  inline float Player_stamina_inc(char s, int n) 
    {return PT[GetPlayerPT(s,n)].stamina_inc_max;}
  inline float Player_decay(char s, int n)          
    {return PT[GetPlayerPT(s,n)].player_decay;}
  inline float Player_inertia_moment(char s, int n) 
    {return PT[GetPlayerPT(s,n)].inertia_moment;}
  inline float Player_dash_power_rate(char s, int n)
    {return PT[GetPlayerPT(s,n)].dash_power_rate;}
  inline float Player_size(char s, int n)           
    {return PT[GetPlayerPT(s,n)].player_size;}
  inline float Player_kickable_margin(char s, int n)
    {return PT[GetPlayerPT(s,n)].kickable_margin;}
  inline float Player_kickable_area(char s, int n)
    {return Player_kickable_margin(s,n) + SP_ball_size + Player_size(s,n);}
  inline float Player_kick_rand(char s, int n)      
    {return PT[GetPlayerPT(s,n)].kick_rand;}
  inline float Player_extra_stamina(char s, int n)  
    {return PT[GetPlayerPT(s,n)].extra_stamina;}
  inline float Player_effort_max(char s, int n)     
    {return PT[GetPlayerPT(s,n)].effort_max;}
  inline float Player_effort_min(char s, int n)     
    {return PT[GetPlayerPT(s,n)].effort_min;}

  inline int   GetTeammatePT(int tm) {return GetPlayerPT(MySide,tm);}
  inline void  SetTeammatePT(int tm, int pt){SetPlayerPT(MySide,tm,pt);}
  inline float Teammate_speed_max(int tm)      {return Player_speed_max(MySide,tm);}    
  inline float Teammate_stamina_inc_max(int tm){return Player_stamina_inc_max(MySide,tm);} 
  inline float Teammate_stamina_inc(int tm)  {return Player_stamina_inc_max(MySide,tm);} 
  inline float Teammate_decay(int tm)          {return Player_decay(MySide,tm);}
  inline float Teammate_inertia_moment(int tm) {return Player_inertia_moment(MySide,tm);}
  inline float Teammate_dash_power_rate(int tm){return Player_dash_power_rate(MySide,tm);}
  inline float Teammate_size(int tm)           {return Player_size(MySide,tm)          ;}
  inline float Teammate_kickable_margin(int tm){return Player_kickable_margin(MySide,tm);}
  inline float Teammate_kickable_area(int tm)  {return Player_kickable_area(MySide,tm);}
  inline float Teammate_kick_rand(int tm)      {return Player_kick_rand(MySide,tm)     ;}
  inline float Teammate_extra_stamina(int tm)  {return Player_extra_stamina(MySide,tm) ;}
  inline float Teammate_effort_max(int tm)     {return Player_effort_max(MySide,tm)    ;}
  inline float Teammate_effort_min(int tm)     {return Player_effort_min(MySide,tm)    ;}

  inline int   GetOpponentPT(int op) {return GetPlayerPT(TheirSide,op);}
  inline void  SetOpponentPT(int op, int pt){SetPlayerPT(TheirSide,op,pt);}
  inline float Opponent_speed_max(int op)      {return Player_speed_max(TheirSide,op);}
  inline float Opponent_stamina_inc_max(int op){return Player_stamina_inc_max(TheirSide,op);} 
  inline float Opponent_stamina_inc(int op){return Player_stamina_inc_max(TheirSide,op);} 
  inline float Opponent_decay(int op)          {return Player_decay(TheirSide,op);}
  inline float Opponent_inertia_moment(int op) {return Player_inertia_moment(TheirSide,op);}
  inline float Opponent_dash_power_rate(int op){return Player_dash_power_rate(TheirSide,op);}
  inline float Opponent_size(int op)           {return Player_size(TheirSide,op);}
  inline float Opponent_kickable_margin(int op){return Player_kickable_margin(TheirSide,op);}
  inline float Opponent_kickable_area(int op){return Player_kickable_area(TheirSide,op);}
  inline float Opponent_kick_rand(int op)      {return Player_kick_rand(TheirSide,op) ;}
  inline float Opponent_extra_stamina(int op){return Player_extra_stamina(TheirSide,op);}
  inline float Opponent_effort_max(int op)     {return Player_effort_max(TheirSide,op) ;}
  inline float Opponent_effort_min(int op)     {return Player_effort_min(TheirSide,op) ;}
  
private:

  Time TiredTimes[MAX_PLAYERS];
  float TeammateStamina[MAX_PLAYERS];

  PlayerObject *ClosestPlayerObjectTo(Vector gpos); 
  PlayerObject *ClosestTeammateObjectTo(Vector gpos);
  PlayerObject *ClosestOpponentObjectTo(Vector gpos);
  PlayerObject *ClosestPlayerObjectTo(char side, Vector gpos); 

  StationaryObject *Fieldline;
  StationaryObject *Marker;            
  BallObject       Ball;
  PlayerObject     *Player;        

  SideLine      SeenLine;
  float         SeenLineDist;
  MarkerType    SeenMarker[MAX_MARKERS];      /* for clearing the seen flag            */
  PlayerObject  *MyPlayer[MAX_PLAYERS];       /* pointers to the players on my team    */
  PlayerObject  *TheirPlayer[MAX_PLAYERS];    /* pointers to the players on their team */
  PlayerObject  *TeamlessPlayer[MAX_PLAYERS]; /* pointers to the players with no team  */
  PlayerObject  *FreePlayer[MAX_PLAYERS];     /* elements in Player not assigned       */

  TempPlayerObject  *UnknownPlayer;           /* Players seen with missing team or num */

  int TeammatePlayerType[MAX_PLAYERS];
  int OpponentPlayerType[MAX_PLAYERS];

  int num_seen_markers;
  int num_my_players;
  int num_their_players;
  int num_teamless_players;
  int num_free_players;
  int num_unknown_players;
  int num_players;         /* The maximum -- doesn't include me */

  Unum opp_goalie_num;
};

#endif
