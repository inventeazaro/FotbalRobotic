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

 
#include "MemPosition.h"
#include "client.h"
#include <soccer_utils.h>


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        Object Class                                          */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

float Object::get_dist()
{
  if ( pos_valid() && Mem->MyConf() )
    return Mem->DistanceTo(gpos);                             /* From global   */

  /* dump_core("dump"); */
  my_error("Can't get_dist %d",type);
  return 0;

  if ( pdtime < Mem->CurrentTime-1 )                          /* In sight      */
    my_error("Can't get_dist %d",type);

  printf("Using in-sight estimate of dist\n");
  return dist;                                              /* From relative */ 
}

/********************************************************************************/

AngleDeg Object::get_ang_from_body()
{
  if ( pos_valid() || Mem->MyConf() )     
    return Mem->AngleToFromBody(gpos);                        /* From global   */

  my_error("Can't get_ang %d",type);
  return 0;

  if ( patime < Mem->CurrentTime-1 )
    my_error("Can't get_ang %d",type);

  /* In sight */
  printf("Using in-sight estimate of ang\n");
  return ang_from_neck + Mem->MyNeckRelAng();                 /* From relative */  
}

/********************************************************************************/

AngleDeg Object::get_ang_from_neck()
{
  if ( pos_valid() || Mem->MyConf() )     
    return Mem->AngleToFromNeck(gpos);                        /* From global   */

  my_error("Can't get_ang %d",type);
  return 0;

  if ( patime < Mem->CurrentTime-1 )
    my_error("Can't get_ang %d",type);

  /* In sight */
  printf("Using in-sight estimate of ang\n");
  return ang_from_neck;                                       /* From relative */  
}

/********************************************************************************/
/* we have some problems keeping these consitent I think,
   (especially during the update process, which has the positions in a state)
   that is nto equal to the current time 
   therefore, we'll just recompute them all the time */

Vector Object::get_rel_to_body_pos() /* relative */
{
  return (gpos - Mem->MyPos()).rotate(-Mem->MyBodyAng());
  /* see the somment above
  if ( rbtime != Mem->CurrentTime ){
    rbpos = (gpos - Mem->MyPos()).rotate(-Mem->MyBodyAng());
    rbtime = Mem->CurrentTime;
  }
  return rbpos;
  */
}

/********************************************************************************/

Vector Object::get_rel_to_neck_pos() /* relative */
{
  return (gpos - Mem->MyPos()).rotate(-Mem->MyNeckGlobalAng());
  /* see the somment above
  if ( rntime != Mem->CurrentTime ){
    rnpos = (gpos - Mem->MyPos()).rotate(-Mem->MyNeckGlobalAng());
    rntime = Mem->CurrentTime;
  }
  return rnpos;
  */
}

/********************************************************************************/

void Object::set_polar_from_neck(float d, float a, Time time)
{ 
  dist = d;
  ang_from_neck  = a;
  pdtime = patime = time;
  seen = TRUE;
  seen_time = time;
}

/********************************************************************************/

void Object::set_angle_from_neck(AngleDeg a, Time time)
{
  ang_from_neck = a;
  patime = time;
  seen = TRUE;
  seen_time = time;
}

/********************************************************************************/

void Object::set_chinfo(float dist, float dir, Time time)
{
  distch = dist;
  dirch  = dir;
  chtime = time;
  seen_moving = TRUE;
}

/********************************************************************************/

void Object::update()
{
  seen = seen_moving = FALSE;
}

/********************************************************************************/

void Object::reset ()
{
  chtime = pdtime = patime = 0;
  seen = FALSE;
  seen_time = -3;
}  

/********************************************************************************/

void Object::clear_seen ()
{
  seen = FALSE;
}  

/********************************************************************************/

void Object::sanitize_times()
{
  Mem->sanitize_time(seen_time);
  Mem->sanitize_time(pdtime);
  Mem->sanitize_time(patime);
  Mem->sanitize_time(chtime);
}  

/********************************************************************************/

Bool Object::in_view_range(AngleDeg view_ang, float angle_buffer, float distance_buffer)
{
  if ( !pos_valid() || !Mem->MyConf() ) return FALSE;
  if ( distance_buffer != 0 ) my_error("Shouldn't have valid distance_buffer at this point");

  Vector tmp_pos = get_rel_to_neck_pos();
  /* angle_buffer is the maximum distance that an object should be off to be forgotten
     We slide the object back before taking the angle in order to accomplish this */
  float slide_dist = angle_buffer / Sin(view_ang);
  //tmp_pos -= Vector(angle_buffer,0);  
  tmp_pos -= Vector(slide_dist,0);  

  if ( fabs(tmp_pos.dir()) <= view_ang ) {
    LogAction7(200, "Object::in_view_range, tmp_pos: (%.2f, %.2f) %.2f %.2f %.2f",
		    tmp_pos.x, tmp_pos.y, tmp_pos.dir(), view_ang, slide_dist);
    return TRUE;
  }
  
  /* if ( fabs(get_ang()) <= Mem->MyViewAngle() - angle_buffer ) return TRUE; */

  return FALSE; 
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        StationaryObject Class                                */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void StationaryObject::Initialize(MarkerType m, Vector pos, float max, float min_valid, Bool rotate)
{
  gpos = rotate ? -pos : pos;  /* If on the right side of the field, flip all coords */

  max_conf = max;
  min_valid_conf = min_valid;

  gconf = max_conf;

  type = OBJ_Marker;
  object_id = m;
}

/********************************************************************************/

void StationaryObject::Initialize(SideLine l, Vector pos, float max, float min_valid, Bool rotate)
{
  gpos = rotate ? -pos : pos;  /* If on the right side of the field, flip all coords */

  max_conf = max;
  min_valid_conf = min_valid;

  gconf = max_conf;

  type = OBJ_Line;
  object_id = l;
}

/********************************************************************************/
 
Vector StationaryObject::get_my_pos(AngleDeg my_neck_global_ang)
{
  if ( type != OBJ_Marker ) my_error("Need to get pos with a marker\n");

  AngleDeg abs_ang = ang_from_neck + my_neck_global_ang;
  NormalizeAngleDeg(&abs_ang);
  Vector rpos = Polar2Vector(dist,abs_ang);
  
  return gpos - rpos;
}

/********************************************************************************/

AngleDeg StationaryObject::get_my_neck_global_ang(){
  
  if ( type != OBJ_Line ){ my_error("Need to get angle with a line\n"); }

  AngleDeg line_ang = gpos.dir();
  AngleDeg my_neck_ang;

  my_neck_ang = (ang_from_neck < 0) ? line_ang - (90 + ang_from_neck) : line_ang + (90 - ang_from_neck);

  NormalizeAngleDeg(&my_neck_ang);

  return my_neck_ang;
}


/********************************************************************************/

Vector StationaryObject::get_my_vel(AngleDeg my_neck_global_ang)
{
  my_error("Shouldn't be estimating velocity from markers -- use sense_body");
  if ( type != OBJ_Marker ) my_error("Need to get vel with a marker\n");

  Vector rvel = Vector(distch, dist*Tan(dirch)).rotate(ang_from_neck+my_neck_global_ang);
  return -rvel; /* Assume the object's not moving */
}




/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        MobileObject Class                                    */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void MobileObject::Initialize(ObjType t, float max, float min_valid, float decay, float motion, float speed)
{
  max_conf = max;
  min_valid_conf = min_valid;
  conf_decay = decay;
  max_speed = speed;
  motion_decay = motion;
  type = t;
  reset();
}

/********************************************************************************/

AngleDeg MobileObject::get_rel_to_body_heading()
{
  float h = get_abs_heading() - Mem->MyBodyAng();
  NormalizeAngleDeg(&h);
  return h;
}

/********************************************************************************/

AngleDeg MobileObject::get_rel_to_neck_heading()
{
  float h = get_abs_heading() - Mem->MyNeckGlobalAng();
  NormalizeAngleDeg(&h);
  return h;
}

/********************************************************************************/

Vector MobileObject::get_rel_to_body_vel() /* relative */
{
  if ( rbvtime != Mem->CurrentTime ){
    /* rvel = (gvel - Mem->MyVel()).rotate(-Mem->MyAng()); */   /* TRUE RELATIVE VEL            */
    rbvel = gvel.rotate(-Mem->MyBodyAng());                     /* ABSOLUTE SPEED, RELATIVE DIR */
    rbvtime = Mem->CurrentTime;
  }
  return rbvel;
}

/********************************************************************************/

Vector MobileObject::get_rel_to_neck_vel() /* relative */
{
  if ( rnvtime != Mem->CurrentTime ){
    /* rvel = (gvel - Mem->MyVel()).rotate(-Mem->MyAng()); */   /* TRUE RELATIVE VEL            */
    rnvel = gvel.rotate(-Mem->MyNeckGlobalAng());               /* ABSOLUTE SPEED, RELATIVE DIR */
    rnvtime = Mem->CurrentTime;
  }
  return rnvel;
}

/********************************************************************************/

void MobileObject::set_heard_info(float x, float y, float conf, float dist, Time time)
{
  /* Don't overwrite better infor from the same cycle */
  if ( heard && 
       (conf < hpconf || (conf == hpconf && hpdist > dist)) )
    return;

  hpos = Vector(x,y);
  hpconf = conf;
  hpdist = dist;
  hptime = time;
  heard = TRUE;
}

/********************************************************************************/

void MobileObject::set_heard_info(float x, float y, float pconf, float dx, float dy, float vconf, float dist, Time time)
{
  /* Don't overwrite better infor from the same cycle      */
  /* If I have better motion info, I have better pos. info */
  if ( heard_moving && 
       (vconf < hvconf || (vconf == hvconf && hvdist > dist)) )
    return;

  set_heard_info(x,y,pconf,dist,time);
  hvel = Vector(dx,dy);
  hvconf = vconf;
  hvdist = dist;
  hvtime = time;
  heard_moving = TRUE;

  float speed = hvel.mod();
  if ( speed > max_speed ){
    if ( speed > max_speed + .1 )
      my_error("object CAN'T be moving that fast %.0f %.2f",(float) type,speed);
    hvel *= max_speed/speed;
  }
}

/********************************************************************************/

void  MobileObject::estimate_pos(Time time)
{
  if ( gtime >= time ) my_error("pos already updated");

  if ( pos_valid() && vel_valid() )
    gpos += gvel;
}

/********************************************************************************/

void  MobileObject::estimate_vel(Time time)
{
  if ( gvtime >= time ) {
    my_error("vel already updated");
  }

  if ( vel_valid() )
    gvel *= motion_decay;
  else
    my_error("Shouldn't be updating invalid vel");
}

/********************************************************************************/

void  MobileObject::update(Time time)
{
  if ( Mem->NewSight && (seen || seen_moving) )
    update_seen(Mem->LastSightTime);

  /* change_view happens instantaneously, so last sight could've been from either view angle */
  AngleDeg forget_view_ang = Min(Mem->MyViewAngle(time),Mem->MyViewAngle(time-1));
  float ang_buff = (type == OBJ_Ball ?
		    Mem->CP_ball_forget_angle_buf :
		    Mem->CP_player_forget_angle_buf);
  float dist_buff = (type == OBJ_Ball ?
		    Mem->CP_ball_forget_dist_buf :
		    Mem->CP_player_forget_dist_buf);
  
  /* If sight was from time-1, and should be in view at this point but isn't, reset */
  if (Mem->NewSight && Mem->LastSightTime == time-1 &&
      !seen && in_view_range(forget_view_ang, ang_buff, dist_buff) )
    forget();

  if ( gvtime < time || gtime < time )
    update_estimate(time);

  if ( heard || heard_moving )
    update_heard(time);

  /* If sight was from time, and should be in view at this point but isn't, reset */
  if (Mem->NewSight && Mem->LastSightTime == time &&
      !seen && in_view_range(forget_view_ang, ang_buff, dist_buff) )
    forget();

  Object::update();
  heard = heard_moving = FALSE;
}

/********************************************************************************/

void  MobileObject::update_seen(Time time)
{
  if ( !Mem->MyConf() ) return;

  sanitize_times();

  if ( seen_time != Mem->LastSightTime ){
    //if ( seen_time != Mem->LastSightTime-1 ){
      my_error("Why the sight delay? %d %d (%d %d) type %d %c %d",
	       seen_time.t, Mem->LastSightTime.t, seen, seen_moving, type, 
	       ((PlayerObject *)this)->side,((PlayerObject *)this)->unum);
      //}
    return;
  }

  if ( Mem->MyUpdateTime() != time ) my_error("Must have missed a cycle (mobile)"); 
  if ( Mem->LastSightTime != Mem->CurrentTime && Mem->LastSightTime != Mem->CurrentTime-1 ) 
    my_error("Didn't see in the past cycle"); 

  /****** Position ********/

  if ( seen ){
    /* Update gpos from dist,dir */
    if ( patime != time || pdtime !=time ){
      // my_error("Need to do something here--got just ang %d %d  %d %d",
      //	       pdtime.t,pdtime.s,time.t,time.s); /*skip update if no dist */
    }
    else{
      rnpos = Polar2Vector(dist,ang_from_neck);
      rntime = time;
      gpos = Mem->MyPos() + rnpos.rotate(Mem->MyNeckGlobalAng());
      gtime = time;
      gconf = max_conf;
    }
  }

  /****** Velocity ********/

  if ( seen_moving ){
    /* Update velocity from distch,dirch */
    /* This isn't the same rvel returned by get_rel_vel(), so don't store it */
    /* this way of computing the realtive velocity didn't work all the time
       especially when the ball was approaching you 
    Vector temp_rvel = Vector(distch, dist*Tan(dirch)).rotate(ang_from_neck);
    */
    /* printf("rvel:  (%f %f)\n",rvel.mod(),rvel.dir()); */

    /* Now compute the relative velocity the right way and see if they agree */
    Vector temp2_rvel;
    Vector rpos = Polar2Vector(dist, ang_from_neck);
    rpos = rpos.Normalize();
    temp2_rvel.x = distch * rpos.x - (dirch * M_PI / 180 * dist * rpos.y);
    temp2_rvel.y = distch * rpos.y + (dirch * M_PI / 180 * dist * rpos.x);
#ifdef NOT_NEEDED_NOW	
    /* now check to see if we get different answers */
    if (fabs(temp_rvel.x - temp2_rvel.x) > .01 ||
	fabs(temp_rvel.y - temp2_rvel.y) > .01) {
      //my_error("rpos: (%6.2f, %6.2f)",rpos.x, rpos.y);
      Vector temp_gvel = Mem->MyVel() + temp_rvel.rotate(Mem->MyNeckGlobalAng());
      Vector temp2_gvel = Mem->MyVel() + temp2_rvel.rotate(Mem->MyNeckGlobalAng());
      my_error("Diff vels! old: (%6.2f, %6.2f)   new: (%6.2f, %6.2f)",
	       temp_gvel.x, temp_gvel.y, temp2_gvel.x, temp2_gvel.y);
    }
#endif
    
    //gvel = Mem->MyVel() + temp_rvel.rotate(Mem->MyNeckGlobalAng());
    gvel = Mem->MyVel() + temp2_rvel.rotate(Mem->MyNeckGlobalAng());
    gvtime = time;
    gvconf = max_conf;
  }
}

/********************************************************************************/

void  MobileObject::update_heard(Time time)
{
  if ( heard ){
    if ( time < hptime ) my_error("How did I fall behind?");
    if ( pos_valid() && gtime < time )  my_error("Should estimate before processing sounds (m1)");
    
    float decayed_hpconf = hpconf * Exp(conf_decay,(time-hptime));
    if ( !pos_valid() || 
	 (get_dist() > Mem->SP_feel_distance &&
	  (decayed_hpconf > gconf + Mem->CP_listen_conf_buffer || 
	   (decayed_hpconf >= gconf && 
	    decayed_hpconf <= gconf + Mem->CP_listen_conf_buffer &&
	    get_dist() > hpdist + 5)) )){
      /* Update gpos from hpos     */
#if 0
      if (!pos_valid()) {
	LogAction4(200, "Update ball position (not valid) %f %d", pos_valid(), !pos_valid());
      } else {	
	LogAction4(200, "Update ball position (valid) %f %d", pos_valid(), !pos_valid());
      }
#endif
      if (type == OBJ_Ball)
	LogAction8(200,"Updating the ball's position based on heard info %d %.4f %.4f %.4f %.4f %.4f",
		   hptime.t, hpconf, decayed_hpconf, hpdist, gconf, get_dist() );
      gpos  = hpos;
      gtime = hptime;
      gconf = hpconf;
    }
  }

  if ( heard_moving ){
    if ( time < hvtime ) my_error("How did I fall behind?");
    if ( vel_valid() && gvtime < time ) my_error("Should estimate before processing sounds (m2) %d %d %d %d",gvtime.t,gvtime.s,time.t,time.s);

    float decayed_hvconf = hvconf * Exp(conf_decay,(time-hptime));
    if ( (!pos_valid() || get_dist() > Mem->SP_feel_distance || !vel_valid()) &&  
	 /* Don't listen to vel if object's near */
	 (!pos_valid() ||
	  decayed_hvconf > gvconf + Mem->CP_listen_conf_buffer || 
	  (decayed_hvconf >= gvconf && 
	   decayed_hvconf <= gvconf + Mem->CP_listen_conf_buffer && 
	   get_dist() > hvdist + 5)) ){
      /* Update gvel from hvel */
      gvel  = hvel;
      gvtime = hvtime;
      gvconf = hvconf;
      if (type == OBJ_Ball)
	LogAction8(200,"Updating the ball's velocity based on heard info %d %.4f %.4f %.4f %.4f %.4f",
		   hvtime.t, hvconf, decayed_hvconf, hvdist, gvconf, get_dist() );
    }
  }

  /* keep updating hptime, hvtime until at time */
  while ( vel_valid() && gvtime < gtime ){
    estimate_vel(gvtime+1);
    ++gvtime;
    gvconf *= conf_decay;
  }

  while ( pos_valid() && gtime < time )
    update_estimate( gtime+1 );
}

/********************************************************************************/

void  MobileObject::update_estimate(Time time)
{
  if ( !pos_valid() ) return;

  if ( gtime == time && vel_valid() ){ /* just vel */
    if ( gvtime == time ) my_error("pos and vel already updated");
    if ( Mem->NewAction &&
	 Mem->LastActionValid(gvtime) && 
	 Mem->LastActionType() == CMD_kick &&
	 type == OBJ_Ball )
      update_kick(gvtime);

    while ( vel_valid() && gvtime < gtime ){
      estimate_vel(time);
      ++gvtime;
      gvconf *= conf_decay;   
    }
    return;
  }

  while ( gtime < time ){ /* both pos and vel */

    if ( Mem->NewAction && Mem->LastActionValid(gtime) && 
	 Mem->LastActionType() == CMD_kick && type == OBJ_Ball && vel_valid() )
      update_kick(gtime);

    estimate_pos(time);
    ++gtime;
    gconf  *= conf_decay;   
    
    while ( vel_valid() && gvtime < gtime ){
      estimate_vel(time);
      ++gvtime;
      gvconf *= conf_decay;   
    }
  }
}

/********************************************************************************/

Vector MobileObject::estimate_future_pos(int steps, Vector extra_vel, Vector extra_vel_per)
{
  if ( !pos_valid() ) my_error("Can't estimate future if don't know present");

  Vector position = gpos;  
  Vector velocity;
  if (vel_valid()) velocity = gvel + extra_vel;
  else velocity = 0;
  
  for (int i=0; i<steps; i++){
    velocity += extra_vel_per;
    if ( velocity.mod() > max_speed ){
      //      my_error("(2) object CAN'T be moving that fast %.0f %f %f",(float) type,velocity.mod(), max_speed);
      velocity *= ( max_speed/velocity.mod() );
    }
    position += velocity;
    velocity *= motion_decay;
  }

  return position;
}

/********************************************************************************/

void  MobileObject::reset()
{
  Object::reset();
  gconf = 0;
  hpconf = hvconf = gvconf = 0;
  gvtime = hptime = hvtime = 0;
  hpdist = hvdist = 0;
  heard = seen_moving = heard_moving = FALSE;
} 

/********************************************************************************/

void  MobileObject::clear_seen()
{
  Object::clear_seen();
  seen_moving = FALSE;
} 

/********************************************************************************/

void  MobileObject::forget()                                                           
{                                                                                      
  gconf = gvconf = 0;                                                                  
#ifndef NO_ACTION_LOG                                                                  
  if (type == OBJ_Ball) {
    Vector pos = get_rel_to_neck_pos();
    LogAction6(175,"Forgetting ball: (%.1f, %.1f) (%.1f, %.1f)",
                   pos.x, pos.y, (pos - Vector(3, 0)).x, (pos - Vector(3, 0)).y);
    LogAction4(175,"Forgetting ball (still): old va: %.0f  new va: %.0f",
		   Mem->MyViewAngle(Mem->LastSightTime),
		   Mem->MyViewAngle(Mem->CurrentTime));
  }
#endif                                                                                 
}


/********************************************************************************/

void MobileObject::sanitize_times()
{
  Object::sanitize_times();
  Mem->sanitize_time(hptime);
  Mem->sanitize_time(hvtime);
}  


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        BallObject Class                                      */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void BallObject::Initialize(float max, float min_valid, float decay, float motion, float max_speed)
{
  MobileObject::Initialize(OBJ_Ball,max,min_valid,decay,motion,max_speed);
  reset();
  ektime = 0;
  use_pos_based_vel_estimate = TRUE;
  pos_based_vel_time = 0;
  past_kick_idx = 0;
  for (int i=0; i<num_past_kicks; i++)
    past_kicks[i].time = -1;
}

/********************************************************************************/

Bool BallObject::moving()
{ 
  /* experimentally checked that if just the player's moving, ball speed could register > .15 */
  return vel_valid() && get_speed() > Mem->CP_ball_moving_threshold ? TRUE : FALSE; 
} 

/********************************************************************************/

Bool BallObject::kickable(float buffer)
{
  if ( !pos_valid() || !Mem->MyConf() ) return FALSE;
  return get_dist() <= Mem->My_kickable_area() - buffer ? TRUE : FALSE; 
}

/********************************************************************************/

Bool BallObject::catchable()
{
  if ( !pos_valid() || !Mem->MyConf() ) return FALSE;
  return get_dist() <= Mem->SP_catch_area_l ? TRUE : FALSE; 
}

/********************************************************************************/

float BallObject::get_kick_rate(Time time)
{ 
  if ( ektime != time ){
    if (!kickable()){
      my_error("not kickable %d %d  %d %d",ektime.t,ektime.s, time.t,time.s);
      my_error("not kickable (ball at %.1f %.1f)",gpos.x,gpos.y);
    }

    if (time != Mem->MyUpdateTime())
      my_error("Calculating a kick_rate for a time not equal to update time (%d.%d) (%d.%d)",
	       time.t, time.s, Mem->MyUpdateTime().t, Mem->MyUpdateTime().s);
    
    effective_kick_rate = calc_kick_rate();
    ektime = time;
  }
  return effective_kick_rate;
}

/********************************************************************************/

void  BallObject::update_kick(Time time)
{
  if ( !(Mem->LastActionValid(time)) ) my_error("No action at that time (kick)");
  if ( Mem->LastActionType() != CMD_kick ) my_error("Last action wasn't a kick");
  if ( !pos_valid() || !vel_valid() ) my_error("Can't update ball if not known before");
  if ( !Mem->MyConf() ) my_error("Can't update ball if not localized");

  LogAction7(180, "Updating kick: time %d.%d, kick %.2f at %.2f, rate %.5f",
		  time.t, time.s, Mem->LastActionPower(), Mem->LastActionAngle(),
		  get_kick_rate(time));
  
  Vector kick_effect =
    Polar2Vector(get_kick_rate(time) * Mem->LastActionPower(), 
		 Mem->MyBodyAng() + Mem->LastActionAngle());
  /* store this kick for later estimation */
  /* this is now done explicitly by send_action in client.C 
  past_kicks[past_kick_idx].time = time;
  past_kicks[past_kick_idx].kick_effect = kick_effect;
  past_kick_idx = past_kick_inc(past_kick_idx);
  */
   
  gvel += kick_effect;
  if ( gvel.mod() > max_speed ){
    //     my_error("(3) object CAN'T be moving that fast %.0f %.2f",(float) type,gvel.mod());
     gvel *= ( max_speed/gvel.mod() );
  }
}

/********************************************************************************/


float BallObject::calc_kick_rate(float dist, float ang)
{
  /* Assumes I'm doing the kicking */
  return  Mem->SP_kick_power_rate *
      (1 - .25 * fabs( ang ) / 180.0 -
       .25 * (dist - Mem->SP_ball_size - Mem->My_size()) / Mem->My_kickable_margin());
}

/********************************************************************************/

void BallObject::set_past_kick(float pow, AngleDeg ang, Time t)
{
  Vector kick_effect =
    Polar2Vector(get_kick_rate(t) * pow, Mem->MyBodyAng() + ang);
  past_kicks[past_kick_idx].time = t;
  past_kicks[past_kick_idx].kick_effect = kick_effect;
  past_kick_idx = past_kick_inc(past_kick_idx);
}


/********************************************************************************/

void BallObject::forget_past_kick(Time t)
{
  for(int i=0; i < num_past_kicks; i++) {
    if (past_kicks[i].time == t)
      past_kicks[i].kick_effect = Vector(0,0); /* make the kick effectless */
  }
  
}

/********************************************************************************/

void BallObject::update(Time time)
{

  MobileObject::update(time);

  if ( kickable() ){
    effective_kick_rate = calc_kick_rate();
    ektime = time;
  }
  
  if ( vel_valid() && get_speed() > max_speed ){
    //    my_error("(4) object CAN'T be moving that fast %.0f %.2f",(float) type,get_speed());
    gvel *= max_speed/get_speed();
  }
}

/********************************************************************************/

void BallObject::update_seen(Time time)
{
  Time   prev_seen_time = last_seen_time;
  Vector prev_seen_pos  = last_seen_pos; 

  Vector epos, evel;
  float estimate_valid = pos_valid();
  if ( estimate_valid ){
    /* if LastSightTime == CurrentTime-1, current is an estimate for the appropriate time */
    epos = gpos;
    evel = gvel;
    if ( Mem->LastSightTime == Mem->CurrentTime ){
      if (time != Mem->CurrentTime)
	my_error("BallObject::update seen: times seen strange: %d.%d %d.%d",
		 time.t, time.s, Mem->CurrentTime.t, Mem->CurrentTime.s);
      /* see if there is a kick we need to update for */
      if (Mem->LastActionValid(Mem->CurrentTime-1) &&
	  Mem->LastActionType() == CMD_kick &&
	  Mem->LastActionTime() == Mem->CurrentTime - 1) {
	/*LogAction5(180, "ball velocity inval: updating for kick %.2f at %.2f, rate %.5f",
			Mem->LastActionPower(), Mem->LastActionAngle(), get_kick_rate(Mem->CurrentTime-1)); */
	evel += Polar2Vector(get_kick_rate(Mem->CurrentTime-1) * Mem->LastActionPower(), 
			     Mem->MyBodyAng() + Mem->LastActionAngle());
      }
      epos += evel;     /* estimate is the old position + velocity */
      evel *= Mem->SP_ball_decay;
      estimate_valid = vel_valid();
    }
  }

  MobileObject::update_seen(time);

  /*****************************************************/
  /* THIS IS DISTANCE BASED BALL VELOCITY INVALIDATION */
  /*****************************************************/
  /* Only if I see the object, but not its velocity and I have a valid estimate */
  if ( seen && !seen_moving && estimate_valid
       && Mem->sight_position_correction_time == time){ 
    /* first we need to see if we estiamted a collision, in which case we should
       NOT invalidate the velocity, but multiply it by -1.0 instead */
    if ((epos + Mem->sight_position_correction).dist(Mem->MyPos()) <
	Mem->My_size() + Mem->SP_ball_size) {
      LogAction3(175, "ball: update seen predicts a collision %.2f",
		      (epos + Mem->sight_position_correction).dist(Mem->MyPos()));
      gvel = evel * -.1;
      gvconf = Mem->CP_min_valid_conf / Mem->CP_ball_conf_decay; //only valid for 2 cycles
      gvtime = time;
      /* now, don't use postion based velocity estimation */
      use_pos_based_vel_estimate = FALSE;
      pos_based_vel_time = time;
    } else {
      /* first we need to figure out the maximum error that the server would give
       total_dist_err is supposed to the the total dist across the error margin of
       the reporting of the ball's position. The angle is +/- .5 degrees and the
       distance is quantize as follows
       Quantize(exp(Quantize(log(vi.distance + EPS),vi.qstep)),0.1) ;

       After that, we have to add in the possible noise from the server.
       We're just going to add in the noise once for every cycle between our
       sight times
    */
      float d = get_dist();
      float just_dist_err = d * Mem->quantize_err_const / 2;
      float inner_perp_err = Mem->Tan_of_half_deg * (d - just_dist_err);
      float outer_perp_err = Mem->Tan_of_half_deg * (d + just_dist_err);
      float total_dist_err =
	sqrt(Sqr(inner_perp_err) + Sqr(just_dist_err)) +
	sqrt(Sqr(outer_perp_err) + Sqr(just_dist_err));

      total_dist_err += Mem->LastSightInterval *
	sqrt(2.0 * Sqr(Mem->SP_ball_rand * gvel.mod()));
    
      Vector diff = gpos - (epos + Mem->sight_position_correction);
      if (diff.mod() > total_dist_err * Mem->CP_ball_vel_invalidation_factor) {
	LogAction6(175, "Invalidating ball velocity: %.2f > %.2f, thought vel was (%.2f, %.2f)",
			diff.mod(), total_dist_err * Mem->CP_ball_vel_invalidation_factor,
			evel.x, evel.y);
	LogAction6(175,"Invalidating ball vel(still): gpos (%.1f %.1f), sight_position_correction (%.1f %.1f)",
			gpos.x, gpos.y,
			Mem->sight_position_correction.x, Mem->sight_position_correction.y);
	gvconf = 0;
	if (!(pos_based_vel_time == time && !use_pos_based_vel_estimate)) {
	  // if it's already been set, don't touch it
	  use_pos_based_vel_estimate = (time-prev_seen_time == 1) ? TRUE : FALSE;
	  pos_based_vel_time = time;
	} 
      }
    }
  }


  /**********************************************/
  /* THIS IS POSITION BASED VELOCITY ESTIMATION */
  /**********************************************/
  if ( Mem->MyConf() && pos_valid() &&
       gvconf <= Exp(Mem->CP_ball_conf_decay, 2) &&
       get_dist() <= Mem->SP_feel_distance &&
       prev_seen_time >= Mem->PreviousSightTime() &&
       !(use_pos_based_vel_estimate == FALSE &&
	 pos_based_vel_time == time)  ){
    /* Don't estimate velocity if the ball's far -- too much noise */

    /* Can estimate based on the last seen position of the ball */
    //cout << "Time: " << time.t << "  Using position based vel estimate" << endl;
    Vector total_kick_eff(0,0);
    /* we're goign to look through our past kicks to find ones that occured in
       the time cycle that we're looking at */
    int pkidx;
    for(pkidx = past_kick_dec(past_kick_idx);
	past_kicks[pkidx].time >= prev_seen_time;
	pkidx = past_kick_dec(pkidx)) {
      /* this kick falls in our range */
      if (past_kicks[pkidx].time > time)
	my_error("Postion Based Vel Estimate: Already have a future kick???");
      total_kick_eff += past_kicks[pkidx].kick_effect *
	SumGeomSeries(1, motion_decay, time - past_kicks[pkidx].time);     
    }
    /* POSITION BASED VELOCITY ESTIMATION
       A problem that arises using position based velocity estimes is that
       we store the global position of the ball as calculated relative to the
       player (whose global position we identify by flags). Becuase of the
       error in seeing the flags, our global position ossilates around the true
       value, adding additional error to the position based velocity
       calculation. The solution is to take the difference between where we
       expected to be and where we observe we are at each new sight and store
       that. We then correct the global ball position for that. This
       essentially removes the error in calculating our global position from
       this calculation, leaving only the ball observation error, giving us a
       better value */
    if (Mem->CP_use_new_position_based_vel && Mem->MyConf() &&
	Mem->sight_position_correction_time == time) {
      //cout << "time: " << time.t << "\tcorr: " << Mem->sight_position_correction.mod() << endl;
      gvel = (gpos - (prev_seen_pos+Mem->sight_position_correction) - total_kick_eff)
	/ SumGeomSeries(1,motion_decay,time-prev_seen_time);      
      LogAction8(175,"Position based velocity estimating: gpos (%.1f %.1f), prev_seen_pos (%.1f %.1f), sight_position_correction (%.1f %.1f)",
		      gpos.x,gpos.y,prev_seen_pos.x,prev_seen_pos.y,
		      Mem->sight_position_correction.x,Mem->sight_position_correction.y);
    } else{ 
      LogAction6(175,"Old position based velocity estimating: gpos (%.1f %.1f), prev_seen_pos (%.1f %.1f)",
		      gpos.x,gpos.y,prev_seen_pos.x,prev_seen_pos.y);
      gvel = (gpos - prev_seen_pos - total_kick_eff)
	/ SumGeomSeries(1,motion_decay,time-prev_seen_time);
    /* gvel is now the velocty at prev_seen_time */
    }

    Time t;
    pkidx = past_kick_inc(pkidx);
    for(t = prev_seen_time; t < time; ++t) {
      if (pkidx != past_kick_idx && past_kicks[pkidx].time == t) {	
	gvel += past_kicks[pkidx].kick_effect;
	pkidx = past_kick_inc(pkidx);
      }      
      gvel *= motion_decay;
    }
    
    gvconf = max_conf; /* so it overrides heard info */
    gvtime = time;
  }
  
  if ( seen_time > last_seen_time ){
    last_seen_time = time; /* == last_seen_time except when clock was stopped */
    last_seen_pos  = gpos;
  }
}

/********************************************************************************/

void BallObject::estimate_pos(Time time)
{
  MobileObject::estimate_pos(time);
  if ( !Mem->MyConf() ) return; 
  if ( !pos_valid() )   return; /* Can't check for collisions */  

  /* Only worry about collisions for the ball */
  if ( get_dist() < Mem->My_size() + Mem->SP_ball_size) {
    if ( vel_valid() ){
      float r = Mem->SP_ball_size + Mem->My_size();
      float d = get_dist();
      float th = fabs(GetNormalizeAngleDeg(get_rel_to_body_heading() - 180));
      float l1 = d * Cos(th);
      float h = d * Sin(th);
      float cosp = h / r;
      float sinp = sqrt(1.0 - Sqr(cosp));
      float l2 = r * sinp;
      gpos += gvel*motion_decay*(-(l1+l2)/Max(get_speed()*motion_decay, 1.0e-10));
      gvel *= -0.1;
      LogAction2(160, "Ball collision");
      /* turn off position based velocity estimation for this cycle */
      use_pos_based_vel_estimate = FALSE;
      pos_based_vel_time = time; 
    }
    /* my_stamp; printf("COLLISION!  --   check computation\n"); */
  }
}

/********************************************************************************/

Bool BallObject::in_view_range(AngleDeg view_ang, float angle_buffer, float distance_buffer)
{
  if ( !pos_valid() || !Mem->MyConf() ) return FALSE;

  if (MobileObject::in_view_range(view_ang, angle_buffer, 0) || 
      get_dist() < Mem->SP_feel_distance - distance_buffer ) 
    return TRUE;
  return FALSE;
}



/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        PlayerObject Class                                    */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void PlayerObject::Initialize(float max, float min_valid, float decay, float motion, float max_speed)
{
  MobileObject::Initialize(OBJ_Player,max,min_valid,decay,motion,max_speed);
  side = 'f'; /* free */
  unum = Unum_Unknown;
  reset();
}

/********************************************************************************/

AngleDeg PlayerObject::get_rel_to_body_body_ang()
{
  float f = get_abs_body_ang() - Mem->MyBodyAng();
  NormalizeAngleDeg(&f);
  return f;
}

/********************************************************************************/

AngleDeg PlayerObject::get_rel_to_body_neck_ang()
{
  float f = get_abs_neck_ang() - Mem->MyBodyAng();
  NormalizeAngleDeg(&f);
  return f;
}

/********************************************************************************/

AngleDeg PlayerObject::get_rel_to_neck_body_ang()
{
  float f = get_abs_body_ang() - Mem->MyNeckGlobalAng();
  NormalizeAngleDeg(&f);
  return f;
}

/********************************************************************************/

AngleDeg PlayerObject::get_rel_to_neck_neck_ang()
{
  float f = get_abs_neck_ang() - Mem->MyNeckGlobalAng();
  NormalizeAngleDeg(&f);
  return f;
}

/********************************************************************************/

AngleDeg PlayerObject::get_neck_rel_ang()
{
  float f = get_rel_to_neck_neck_ang() - get_rel_to_neck_body_ang();
  NormalizeAngleDeg(&f);
  return f;
}

/********************************************************************************/

void PlayerObject::set_body_ang_from_neck      (AngleDeg body_ang, Time time)
{
  rnbang  = body_ang;
  rnbatime = time;
  seen_body_ang = TRUE;
}

/********************************************************************************/

void PlayerObject::set_neck_ang_from_neck      (AngleDeg neck_ang, Time time)
{
  rnnang  = neck_ang;
  rnnatime = time;
  seen_neck_ang = TRUE;
}

/********************************************************************************/

void PlayerObject::set_heard_info_w_angs(float x, float y, float pconf, float dx, float dy, float vconf, 
				  AngleDeg bang, float bconf, AngleDeg nang, float nconf, 
			          float dist, Time time)
{
  /* Don't overwrite better infor from the same cycle      */
  /* If I have better face info, I have better pos. info   */
  /* If I have better face info, I have better neck info   */
  /* IMPORTANT -- above assumes body and neck angles are only heard together 
     and always with position info */
  if ( heard_body_ang && 
       (bconf < hbaconf || (bconf == hbaconf && dist > hbadist)) )
    return;

  MobileObject::set_heard_info(x,y,pconf,dx,dy,vconf,dist,time);
  hbang  = bang;
  hbaconf = bconf;
  hbadist = dist;
  hbatime = time;
  heard_body_ang = TRUE;

  hnang  = nang;
  hnaconf = nconf;
  hnadist = dist;
  hnatime = time;
  heard_neck_ang = TRUE;
}

/********************************************************************************/

void  PlayerObject::update(Time time)
{
  /* IMPORTANT -- assuming that body and neck angle are only seen together
     Otherwise, might have to reason about whether the neck angle has changed */

  if ( Mem->NewSight && seen_body_ang ){
    if ( !seen_neck_ang ) my_error("Should see body and neck angle together");
    update_seen_body_and_neck_angs(Mem->LastSightTime);
  }

  if ( gbatime < time ){
    if ( gnatime >= time ) my_error("Should see body and neck angle together");
    update_estimate(time);
  }

  if ( heard_body_ang ){
    if ( !heard_neck_ang ) my_error("Should hear body and neck angle together");
    update_heard(time);
  }

  MobileObject::update(time);
  seen_body_ang = heard_body_ang = seen_neck_ang = heard_neck_ang = FALSE;
}

/********************************************************************************/

void  PlayerObject::update_seen_body_and_neck_angs(Time time)
{
  if ( !Mem->MyConf() ) return;
  
  sanitize_times();

  if ( seen_time != Mem->LastSightTime ){
    /* if ( seen_time != Mem->LastSightTime-1 ) */
      my_error("Why the sight delay(2)? %d (%d %d) %d %d %c %d",
	       seen_time.t, seen_body_ang, seen_neck_ang,
	       rnbatime.t, rnnatime.t, side, unum);
    return;
  }

  if ( Mem->MyUpdateTime() != time ) my_error("Must have missed a cycle (player)"); 

  gbang   = GetNormalizeAngleDeg(rnbang + Mem->MyNeckGlobalAng());
  gbaconf = max_conf;
  gbatime = time;
  gnang   = GetNormalizeAngleDeg(rnnang + Mem->MyNeckGlobalAng()); 
          /**** OR gbang + rnnang ****/
  gnaconf = max_conf;
  gnatime = time;
}

/********************************************************************************/

void  PlayerObject::update_estimate(Time time)
{
  if ( !body_ang_valid() || !neck_ang_valid() ) return;
  
  while ( gbatime < time ){
    gbaconf *= conf_decay;
    ++gbatime;
  }
  while ( gnatime < time ){
    gnaconf *= conf_decay;
    ++gnatime;
  }
}

/********************************************************************************/

void  PlayerObject::update_heard(Time time)
{
  if ( !heard_body_ang || !heard_neck_ang ) /* Should only be together */
    my_error("Should only be here if I heard face info");
  if ( time < hbatime || time < hnatime ) my_error("How did I fall behind?");
  if ( body_ang_valid() && gbatime < time )  
    my_error("Should estimate before processing sounds (p1)");
  if ( neck_ang_valid() && gnatime < time )  
    my_error("Should estimate before processing sounds (p2)");
    
  float decayed_hbaconf = hbaconf * Exp(conf_decay,(time-hbatime));
  if ( decayed_hbaconf > gbaconf || !pos_valid() ||
       (decayed_hbaconf == gbaconf && get_dist() > hbadist) ){
    /* Update gpos from hpos     */
    gbang   = hbang;
    gbatime = hbatime;
    gbaconf = hbaconf;
  }

  while ( body_ang_valid() && gbatime < time )
    update_estimate(gbatime+1);

  float decayed_hnaconf = hnaconf * Exp(conf_decay,(time-hnatime));
  if ( decayed_hnaconf > gnaconf || !pos_valid() ||
       (decayed_hnaconf == gnaconf && get_dist() > hnadist) ){
    /* Update gpos from hpos     */
    gnang   = hnang;
    gnatime = hnatime;
    gnaconf = hnaconf;
  }

  while ( neck_ang_valid() && gnatime < time )
    update_estimate(gnatime+1);
}

/********************************************************************************/

void  PlayerObject::reset()
{
  MobileObject::reset();
  hbaconf = gbaconf = hnaconf = gnaconf = 0;
  rnbatime = gbatime = hbatime = rnnatime = gnatime = hnatime = 0;
  hbadist = hnadist = 0;
  seen_body_ang = heard_body_ang = seen_neck_ang = heard_neck_ang = FALSE;
}

/********************************************************************************/

void  PlayerObject::clear_seen()
{
  MobileObject::clear_seen();
  seen_body_ang = seen_neck_ang = FALSE;
}

/********************************************************************************/

void  PlayerObject::forget()
{
  MobileObject::forget();
  gbaconf = gnaconf = 0;
#ifndef NO_ACTION_LOG
  float slide_dist = 5 / Sin(Mem->MyViewAngle(Mem->CurrentTime));
  Vector pos = get_rel_to_neck_pos();
  LogAction7(175,"Forgetting player %d: (%.1f, %.1f) (%.1f, %.1f)",
		  (side == Mem->MySide) ? unum : -unum, 
		  pos.x, pos.y,
		  (pos - Vector(slide_dist, 0)).x, (pos - Vector(slide_dist, 0)).y);
  LogAction5(175,"Forgetting player %d (still): old va: %.0f  new va: %.0f",
		  (side == Mem->MySide) ? unum : -unum, 
		  Mem->MyViewAngle(Mem->LastSightTime),
		  Mem->MyViewAngle(Mem->CurrentTime));
#endif                                                                                 
} 

/********************************************************************************/

void PlayerObject::sanitize_times()
{
  MobileObject::sanitize_times();
  Mem->sanitize_time(rnbatime);
  Mem->sanitize_time(hbatime);
  Mem->sanitize_time(rnnatime);
  Mem->sanitize_time(hnatime);
}  

/********************************************************************************/

Bool PlayerObject::in_view_range(AngleDeg view_ang, float angle_buffer, float distance_buffer)
{
  if ( !pos_valid() || !Mem->MyConf() ) return FALSE;

  if ( MobileObject::in_view_range(view_ang, angle_buffer, 0) &&
       get_dist() < Mem->SP_unum_far_length - distance_buffer ) 
    return TRUE;
  return FALSE;
}


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        PositionInfo Class                                    */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void PositionInfo::Initialize()
{
  /* printf("Calling Position Initialize\n"); */

  /* if true, multiply all coords by -1 inside initialize */
  Bool rotate = (Mem->MySide == 'l') ? FALSE : TRUE;  
  
  int i=0;

  Marker = new StationaryObject[SP_num_markers];
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0, 0.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Goal_L */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0, 0.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Goal_R */

  Marker[i].Initialize((MarkerType) i, Vector( 0.0, 0.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_C */
  Marker[i].Initialize((MarkerType) i, Vector( 0.0, -SP_pitch_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_CT */
  Marker[i].Initialize((MarkerType) i, Vector( 0.0, SP_pitch_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_CB */

  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0, -SP_pitch_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LT */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0, SP_pitch_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LB */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0, -SP_pitch_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RT */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0, SP_pitch_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RB */

  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0+SP_penalty_area_length,
				-SP_penalty_area_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_PLT */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0+SP_penalty_area_length, 0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_PLC */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0+SP_penalty_area_length,
				SP_penalty_area_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_PLB */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0-SP_penalty_area_length,
				-SP_penalty_area_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_PRT */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0-SP_penalty_area_length, 0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_PRC */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0-SP_penalty_area_length,
				SP_penalty_area_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_PRB */

  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0, -SP_goal_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_GLT */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0, SP_goal_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_GLB */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0, -SP_goal_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_GRT */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0, SP_goal_width/2.0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_GRB */

  Marker[i].Initialize((MarkerType) i, Vector( -50.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TL50 */
  Marker[i].Initialize((MarkerType) i, Vector( -40.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TL40 */
  Marker[i].Initialize((MarkerType) i, Vector( -30.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TL30 */
  Marker[i].Initialize((MarkerType) i, Vector( -20.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TL20 */
  Marker[i].Initialize((MarkerType) i, Vector( -10.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TL10 */
  Marker[i].Initialize((MarkerType) i, Vector( 0.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_T0 */
  Marker[i].Initialize((MarkerType) i, Vector( 10.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TR10 */
  Marker[i].Initialize((MarkerType) i, Vector( 20.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TR20 */
  Marker[i].Initialize((MarkerType) i, Vector( 30.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TR30 */
  Marker[i].Initialize((MarkerType) i, Vector( 40.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TR40 */
  Marker[i].Initialize((MarkerType) i, Vector( 50.0, -SP_pitch_width/2.0-SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_TR50 */

  Marker[i].Initialize((MarkerType) i, Vector( -50.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BL50 */
  Marker[i].Initialize((MarkerType) i, Vector( -40.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BL40 */
  Marker[i].Initialize((MarkerType) i, Vector( -30.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BL30 */
  Marker[i].Initialize((MarkerType) i, Vector( -20.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BL20 */
  Marker[i].Initialize((MarkerType) i, Vector( -10.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BL10 */
  Marker[i].Initialize((MarkerType) i, Vector( 0.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_B0 */
  Marker[i].Initialize((MarkerType) i, Vector( 10.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BR10 */
  Marker[i].Initialize((MarkerType) i, Vector( 20.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BR20 */
  Marker[i].Initialize((MarkerType) i, Vector( 30.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BR30 */
  Marker[i].Initialize((MarkerType) i, Vector( 40.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BR40 */
  Marker[i].Initialize((MarkerType) i, Vector( 50.0, SP_pitch_width/2.0+SP_pitch_margin ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_BR50 */

  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, -30 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LT30 */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, -20 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LT20 */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, -10 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LT10 */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, 0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_L0 */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, 10 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LB10 */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, 20 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LB20 */
  Marker[i].Initialize((MarkerType) i, Vector( -SP_pitch_length/2.0-SP_pitch_margin, 30 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_LB30 */

  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, -30 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RT30 */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, -20 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RT20 */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, -10 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RT10 */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, 0 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_R0 */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, 10 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RB10 */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, 20 ),
			   CP_max_conf, CP_min_valid_conf, rotate); i++; /* Flag_RB20 */
  Marker[i].Initialize((MarkerType) i, Vector( SP_pitch_length/2.0+SP_pitch_margin, 30 ),
			   CP_max_conf, CP_min_valid_conf, rotate); /* Flag_RB30 */

  if ( MySide == 'l' ){
    RM_My_Goal = Goal_L;
    RM_Their_Goal = Goal_R;
    RM_LB_Flag = Flag_LT;
    RM_LC_Flag = Flag_CT;
    RM_LF_Flag = Flag_RT;
    RM_RB_Flag = Flag_LB;
    RM_RC_Flag = Flag_CB;
    RM_RF_Flag = Flag_RB;
    RM_My_PC_Flag = Flag_PLC;    /* Center of my penalty area */
    RM_Their_PC_Flag = Flag_PRC; /* Center of theirs          */
  }
  else{ /* MySide == 'r' */
    RM_My_Goal = Goal_R;
    RM_Their_Goal = Goal_L;
    RM_LB_Flag = Flag_RB;
    RM_LC_Flag = Flag_CB;
    RM_LF_Flag = Flag_LB;
    RM_RB_Flag = Flag_RT;
    RM_RC_Flag = Flag_CT;
    RM_RF_Flag = Flag_LT;
    RM_My_PC_Flag = Flag_PRC;    /* Center of my penalty area */
    RM_Their_PC_Flag = Flag_PLC; /* Center of theirs          */
  }    

  i=0;

  Fieldline = new StationaryObject[SP_num_lines];
  Fieldline[i].Initialize((SideLine) i, Vector( -SP_pitch_length/2.0, 0.0 ), 
			 CP_max_conf, CP_min_valid_conf, rotate); i++; /* SL_Left */
  Fieldline[i].Initialize((SideLine) i, Vector( SP_pitch_length/2.0, 0.0 ), 
			 CP_max_conf, CP_min_valid_conf, rotate); i++; /* SL_Right */
  Fieldline[i].Initialize((SideLine) i, Vector( 0.0, -SP_pitch_width/2.0 ), 
			 CP_max_conf, CP_min_valid_conf, rotate); i++; /* SL_Top */
  Fieldline[i].Initialize((SideLine) i, Vector( 0.0, SP_pitch_width/2.0 ), 
			 CP_max_conf, CP_min_valid_conf, rotate); /* SL_Bottom */
  
  Ball.Initialize(CP_max_conf, CP_min_valid_conf, CP_ball_conf_decay, SP_ball_decay, SP_ball_speed_max);

  num_players          = SP_team_size*2-1;

  Player = new PlayerObject[num_players];     /* allow for all players but me          */
  for (i=0; i<num_players; i++)
    Player[i].Initialize(CP_max_conf, CP_min_valid_conf, CP_player_conf_decay, 
			 SP_player_decay, SP_player_global_speed_max );

  for (i=0; i<num_players; i++)
    FreePlayer[i] = &(Player[(num_players-1) - i]);   /* Player array backwards: take from end */

  UnknownPlayer = new TempPlayerObject[num_players];

  num_seen_markers     = 0;
  num_my_players       = 0;
  num_their_players    = 0;
  num_teamless_players = 0;
  num_free_players     = num_players;
  num_unknown_players  = 0;

  ClosestMarker = ClosestMotionMarker = No_Marker;
  SeenLine = SL_No_Line;

  for (i=1; i<=SP_team_size; i++){
    TiredTimes[i] = -CP_say_tired_interval;
    TeammateStamina[i] = SP_stamina_max;
  }

  OwnPenaltyArea = Rectangle(-SP_pitch_length/2,                          //left
			     -SP_pitch_length/2 + SP_penalty_area_length, //right
			     -SP_penalty_area_width/2,                    //top
			     SP_penalty_area_width/2);                    //bottom
  OwnGoalieArea = Rectangle(-SP_pitch_length/2,                       //left
			    -SP_pitch_length/2 + SP_goal_area_length, //right
			    -SP_goal_area_width/2,                    //top	
			    SP_goal_area_width/2);                    //bottom

  TheirPenaltyArea = Rectangle(SP_pitch_length/2 - SP_penalty_area_length,//left
			      SP_pitch_length/2,                          //right
			     -SP_penalty_area_width/2,                    //top
			     SP_penalty_area_width/2);                    //bottom
  TheirGoalieArea = Rectangle(SP_pitch_length/2 - SP_goal_area_length,//left
			      SP_pitch_length/2,                          //right
			    -SP_goal_area_width/2,                    //top	
			    SP_goal_area_width/2);                    //bottom

  FieldRectangle = Rectangle(-SP_pitch_length/2,
			      SP_pitch_length/2,
			     -SP_pitch_width/2,
			      SP_pitch_width/2);

  MyLeftGoalKickSpot = OwnGoalieArea.TopRightCorner();
  MyRightGoalKickSpot = OwnGoalieArea.BottomRightCorner();
  TheirLeftGoalKickSpot = TheirGoalieArea.TopLeftCorner();
  TheirRightGoalKickSpot = TheirGoalieArea.BottomLeftCorner();

  my_offside_line = 0;

  quantize_err_const = exp(SP_dist_qstep / 2) - exp(-SP_dist_qstep / 2);
  Tan_of_half_deg = Tan(1.0/2.0);

  opp_goalie_num = Unum_Unknown;

  for (int i=0; i<=SP_team_size; i++){ //Try to keep TeammatePlayerType[0] = me
    TeammatePlayerType[i] = OpponentPlayerType[i] = 0;
  }
}

PositionInfo::~PositionInfo()
{
  delete [] Fieldline;
  delete [] Marker;
  delete [] Player;
  delete [] UnknownPlayer;
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void PositionInfo::SeeLine(SideLine l, float dist, float ang, Time tm)
{
  if ( SeenLine == SL_No_Line || dist > SeenLineDist ){ // use further line
    Fieldline[l].set_polar_from_neck(dist,ang,tm);
    SeenLine = l;
    SeenLineDist = dist;
  }
}

/********************************************************************************/

void PositionInfo::SeeLine(SideLine l, float ang, Time tm)
{
  Fieldline[l].set_angle_from_neck(ang,tm);
  SeenLine = l;
}




/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

void PositionInfo::SeeMarker(MarkerType marker, float dist, float ang, Time tm)
{
  Marker[marker].set_polar_from_neck(dist,ang,tm);
  SeenMarker[num_seen_markers++] = marker;
}

/********************************************************************************/

void PositionInfo::SeeMarker(MarkerType marker, float ang, Time tm)
{
  Marker[marker].set_angle_from_neck(ang,tm);
  SeenMarker[num_seen_markers++] = marker;
  my_error("Shouldn't process markers when using low quality -- no info");
}

/********************************************************************************/

void PositionInfo::SeeMarker(MarkerType marker, float dist, float ang, 
			     float distChng, float dirChng, Time tm){
  Marker[marker].set_chinfo(distChng, dirChng, tm);
  Marker[marker].set_polar_from_neck(dist,ang,tm);
  SeenMarker[num_seen_markers++] = marker;
}



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

void PositionInfo::SeeBall(float ang, Time tm)
{
  Ball.set_angle_from_neck(ang,tm);
}

/*********************************************************************************/

void PositionInfo::SeeBall(float dist, float ang, Time tm)
{
  Ball.set_polar_from_neck(dist,ang,tm);
}

/*********************************************************************************/

void PositionInfo::SeeBall(float dist, float ang, float distChng, float dirChng, Time tm)
{
  Ball.set_chinfo(distChng, dirChng, tm);
  SeeBall(dist,ang,tm);
}




/*********************************************************************************/
/********************************************************************************/
/*********************************************************************************/

void PositionInfo::SeePlayer(char side, Unum num, Bool goalie, float ang, Time time)
{
  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL )
    player = GetNewPlayer(side, num);

  if ( player != NULL ) 
    player->set_angle_from_neck(ang,time);
  else 
    my_error("Can't get a player to see (1) %c %d (%d teamless)",side,num,num_teamless_players);

  if ( goalie ){
    if ( SP_version < 6.00 )
      my_error("How do I know the goalie num??");
    if ( side == TheirSide )
      SetOpponentGoalieNum(num);
    else if ( num != Mem->FP_goalie_number )
      my_error("Shouldn't %d be my goalie?  Looks like %d is",Mem->FP_goalie_number,num);
  }
}

/********************************************************************************/

void PositionInfo::SeePlayer(char side, Unum num, Bool goalie, 
			     float dist, float ang, Time time)
{
  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL )
    player = GetNewPlayer(side, num);

  if ( player != NULL ) 
    player->set_polar_from_neck(dist,ang,time);
  else 
    my_error("Can't get a player to see (2) %c %d (%d teamless)",side,num,num_teamless_players);

  if ( goalie ){
    if ( SP_version < 6.00 )
      my_error("How do I know the goalie num??");
    if ( side == TheirSide )
      SetOpponentGoalieNum(num);
    else if ( num != Mem->FP_goalie_number )
      my_error("Shouldn't %d be my goalie?  Looks like %d is",Mem->FP_goalie_number,num);
  }
}

/********************************************************************************/

void PositionInfo::SeePlayer(char side, Unum num, Bool goalie, float dist, float ang, 
			     float distChng, float dirChng, float bodydir, 
			     float neckdir, Bool tackling, Time time)
{
  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL )
    player = GetNewPlayer(side, num);

  if ( player != NULL ){
    player->set_chinfo(distChng,dirChng,time);
    player->set_body_and_neck_ang_from_neck(bodydir,neckdir,time);
    player->set_polar_from_neck(dist,ang,time);
  }
  else 
    my_error("Can't get a player to see (3) %c %d (%d teamless)",side,num,num_teamless_players);

  if ( goalie ){
    if ( SP_version < 6.00 )
      my_error("How do I know the goalie num??");
    if ( side == TheirSide )
      SetOpponentGoalieNum(num);
    else if ( num != Mem->FP_goalie_number )
      my_error("Shouldn't %d be my goalie?  Looks like %d is",Mem->FP_goalie_number,num);
  }
}

/********************************************************************************/

void PositionInfo::SeePlayer(char side, Unum num, Bool goalie, float dist, float ang, 
			     float distChng, float dirChng, float bodydir, 
			     float neckdir, float pointdir, Bool tackling, Time time)
{
  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL )
    player = GetNewPlayer(side, num);

  if ( player != NULL ){
    player->set_chinfo(distChng,dirChng,time);
    player->set_body_and_neck_ang_from_neck(bodydir,neckdir,time);
    player->set_polar_from_neck(dist,ang,time);
  }
  else 
    my_error("Can't get a player to see (3) %c %d (%d teamless)",side,num,num_teamless_players);

  if ( goalie ){
    if ( SP_version < 6.00 )
      my_error("How do I know the goalie num??");
    if ( side == TheirSide )
      SetOpponentGoalieNum(num);
    else if ( num != Mem->FP_goalie_number )
      my_error("Shouldn't %d be my goalie?  Looks like %d is",Mem->FP_goalie_number,num);
  }
}

/********************************************************************************/

void PositionInfo::SeePlayer(char side, float dist, float ang, Bool tackling, Time time)
{
  if ( num_unknown_players < num_players ){
    UnknownPlayer[num_unknown_players].set(side,dist,ang,time);
    num_unknown_players++;
  } 
  else
    my_error("Too many unknown players (1) %d %d %d %d",Mem->LastSightTime.t,Mem->LastSightTime.s,
	     time.t,time.s);
}

/********************************************************************************/

void PositionInfo::SeePlayer(char side, float dist, float ang, float pointdir, 
			     Bool tackling, Time time)
{
  if ( num_unknown_players < num_players ){
    UnknownPlayer[num_unknown_players].set(side,dist,ang,time);
    num_unknown_players++;
  } 
  else
    my_error("Too many unknown players (1) %d %d %d %d",Mem->LastSightTime.t,Mem->LastSightTime.s,
	     time.t,time.s);
}

/********************************************************************************/

void PositionInfo::SeePlayer(char side, float ang, Time time)
{
  //my_error("What can I do that's useful with just the player angle?");
  return;

  if ( num_unknown_players < num_players ){
    UnknownPlayer[num_unknown_players].set(side,0,ang,time);
    num_unknown_players++;
  } 
  else
    my_error("Too many unknown players (2)");
}

/********************************************************************************/

void PositionInfo::SeePlayer(float dist, float ang, Time time)
{
  if ( num_unknown_players < num_players ){
    UnknownPlayer[num_unknown_players].set('?',dist,ang,time);
    num_unknown_players++;
  } 
  else
    my_error("Too many unknown players (3) %d %d %d %d",Mem->LastSightTime.t,Mem->LastSightTime.s,
	     time.t,time.s);
}

/********************************************************************************/

void PositionInfo::SeePlayer(float ang, Time time)
{
  //my_error("What can I do that's useful with just the player angle?");
  return;

  if ( num_unknown_players < num_players ){
    UnknownPlayer[num_unknown_players].set('?',0,ang,time);  
    num_unknown_players++;
  } 
  else
    my_error("Too many unknown players (4)");
}

/********************************************************************************/

void PositionInfo::ForgetPlayer(char side, Unum num)
{
  PlayerObject *player = GetPlayer(side, num);
  if ( player != NULL )
    player->reset();
}

/********************************************************************************/

void PositionInfo::SetOpponentGoalieNum(Unum goalie){
  
  if ( SP_version < 6.00 ) 
    my_error("How do I know the goalie num??");

  /* Peter -- need to take this out to actually detect their goalie */
  //return;

  if ( opp_goalie_num == Unum_Unknown )
    opp_goalie_num = goalie;
  else if ( opp_goalie_num != goalie )
    my_error ("Goalie change?? %d to %d",opp_goalie_num,goalie);
  return;
}

/********************************************************************************/

Unum PositionInfo::GetOpponentGoalieNum(){

  if ( opp_goalie_num != Unum_Unknown ){
    if ( SP_version < 6.00 ) 
      my_error("How do I know the goalie num??");
    if ( opp_goalie_num && OpponentPositionValid(opp_goalie_num) )
      return opp_goalie_num;
    else 
      return Unum_Unknown;
  }

  return ClosestOpponentTo(MarkerPosition(RM_Their_Goal)); 
}

/********************************************************************************/

void PositionInfo::HearBall(float x, float y, float conf, float dist, Time time)
{
  Ball.set_heard_info(x,y,conf,dist,time);
}

/********************************************************************************/

void PositionInfo::HearBall(float x, float y, float pconf, float dx, float dy, float vconf, 
			    float dist, Time time)
{
  Ball.set_heard_info(x,y,pconf,dx,dy,vconf,dist,time);
}

/********************************************************************************/


void PositionInfo::HearPlayer(char side, Unum num, float x, float y, float conf, float dist, Time time)
{
  /* When hearing a player location, remember not to call just getplayer, but also
     closestPlayerto(side,vector) -- might be one of the unknowns 
     can just update the side---update_players takes care of the rest */

  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL ){
    Vector pos = Vector(x,y);
    if ( (player = ClosestPlayerObjectTo(side, pos)) != NULL ){
      if ( player->side == '?' )
	player->side = side; 
      player->unum = num;
      player->set_max_speed(Player_speed_max(side,num));
      player->set_motion_decay(Player_decay(side,num));
    }
    else 
      player = GetNewPlayer(side, num);
  }

  if ( player != NULL ) 
    player->set_heard_info(x,y,conf,dist,time);
  else {
    /* Be more lenient in ClosestPlayerObjectTo??? */
    Unum teammate = ClosestTeammateTo(Vector(x,y));
    Unum opponent = ClosestOpponentTo(Vector(x,y));
    my_error("Can't get a player to hear (1) %c %d (%.1f %.1f)",side,num,
	     teammate == Unum_Unknown ? -1.0 : TeammateDistanceTo(teammate,Vector(x,y)),
	     opponent == Unum_Unknown ? -1.0 : OpponentDistanceTo(opponent,Vector(x,y)));
  }
}

/********************************************************************************/

void PositionInfo::HearPlayer(char side, Unum num, float x, float y, float pconf, 
			      float dx, float dy, float vconf, float dist, Time time)
{
  /* When hearing a player location, remember not to call just getplayer, but also
     closestPlayerto(side,vector) -- might be one of the unknowns 
     can just update the side---update_players takes care of the rest */

  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL ){
    Vector pos = Vector(x,y);
    if ( (player = ClosestPlayerObjectTo(side, pos)) != NULL ){
      if ( player->side == '?' )
	player->side = side; 
      player->unum  = num;
      player->set_max_speed(Player_speed_max(side,num));
      player->set_motion_decay(Player_decay(side,num));
    }
    else 
      player = GetNewPlayer(side, num);
  }

  if ( player != NULL ) 
    player->set_heard_info(x,y,pconf,dx,dy,vconf,dist,time);
  else {
    /* Be more lenient in ClosestPlayerObjectTo??? */
    Unum teammate = ClosestTeammateTo(Vector(x,y));
    Unum opponent = ClosestOpponentTo(Vector(x,y));
    my_error("Can't get a player to hear (2) %c %d (%.1f %.1f)",side,num,
	     teammate == Unum_Unknown ? -1.0 : TeammateDistanceTo(teammate,Vector(x,y)),
	     opponent == Unum_Unknown ? -1.0 : OpponentDistanceTo(opponent,Vector(x,y)));
  }
}

/********************************************************************************/

void PositionInfo::HearPlayer(char side, Unum num, float x, float y, float pconf, float dx, float dy, 
			      float vconf, AngleDeg bang, float bconf, AngleDeg nang, float nconf, 
			      float dist, Time time)
{
  /* When hearing a player location, remember not to call just getplayer, but also
     closestPlayerto(side,vector) -- might be one of the unknowns 
     can just update the side---update_players takes care of the rest */

  PlayerObject *player;
  if ( (player = GetPlayer(side, num)) == NULL ){
    Vector pos = Vector(x,y);
    if ( (player = ClosestPlayerObjectTo(side, pos)) != NULL ){
      if ( player->side == '?' )
	player->side = side; 
      player->unum  = num;
      player->set_max_speed(Player_speed_max(side,num));
      player->set_motion_decay(Player_decay(side,num));
    }
    else 
      player = GetNewPlayer(side, num);
  }

  if ( player != NULL ) 
    player->set_heard_info_w_angs(x,y,pconf,dx,dy,vconf,bang,bconf,nang,nconf,dist,time);
  else {
    /* Be more lenient in ClosestPlayerObjectTo??? */
    Unum teammate = ClosestTeammateTo(Vector(x,y));
    Unum opponent = ClosestOpponentTo(Vector(x,y));
    my_error("Can't get a player to hear (3) %c %d (%.1f %.1f)",side,num,
	     teammate == Unum_Unknown ? -1.0 : TeammateDistanceTo(teammate,Vector(x,y)),
	     opponent == Unum_Unknown ? -1.0 : OpponentDistanceTo(opponent,Vector(x,y)));
  }
}


/********************************************************************************/
/*********************************************************************************/
/********************************************************************************/

float PositionInfo::PlayerPositionValid(char side, Unum n)
{ 
  if ( side == MySide && n==MyNumber ) 
    return MyConf();

  PlayerObject *player = GetPlayer(side,n);
  if ( player == NULL ) 
    return 0;
  else 
    return player->pos_valid();
}

/********************************************************************************/

float PositionInfo::PlayerVelocityValid(char side, Unum n)
{ 
  if ( side == MySide && n==MyNumber ) 
    return MyVelConf();

  PlayerObject *player = GetPlayer(side,n);
  if ( player == NULL ) 
    return 0;
  else 
    return player->vel_valid();
}  

/********************************************************************************/

float PositionInfo::PlayerBodyAngleValid(char side, Unum n)
{ 
  if ( side == MySide && n==MyNumber ) 
    return MyConf();

  PlayerObject *player = GetPlayer(side, n);
  if ( player == NULL ) 
    return 0;
  else 
    return player->body_ang_valid();
}  

/********************************************************************************/

float PositionInfo::PlayerNeckAngleValid(char side, Unum n)
{ 
  if ( side == MySide && n==MyNumber ) 
    return MyConf();

  PlayerObject *player = GetPlayer(side, n);
  if ( player == NULL ) 
    return 0;
  else 
    return player->neck_ang_valid();
}  


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

Vector PositionInfo::BallPredictedPosition(int steps, float kick_power, AngleDeg kick_angle)
{
  /* kick_angle relative to body */
  if ( !BallVelocityValid() ) my_error("Need to know ball velocity for prediction");

  Vector new_vel = Polar2Vector( BallKickRate()*kick_power, MyBodyAng()+kick_angle );
  return GetBall()->estimate_future_pos(steps, new_vel);
}

/********************************************************************************/

Vector PositionInfo::BallPredictedPositionWithQueuedActions(int steps)
{
  if ( Action->valid() && Action->type == CMD_kick )
    return BallPredictedPosition(steps,Action->power,Action->angle);
  else
    return BallPredictedPosition(steps);
}

/********************************************************************************/

Bool PositionInfo::BallWillBeKickable(int steps, float dash_power, float buffer)
{
  if ( !MyConf() || !BallPositionValid() ) my_error("Can't predict kickable");

  Vector ball_predicted_position = BallPredictedPosition(steps);
  Vector my_predicted_position   = MyPredictedPosition  (steps,dash_power);

  return my_predicted_position.dist(ball_predicted_position) <= My_kickable_area()-buffer ? TRUE : FALSE; 
}

/*****************************************************************************************/

Unum PositionInfo::PlayerWithBall(float buffer)
{
  if ( !BallPositionValid() ) return Unum_Unknown;
  Vector ball = BallAbsolutePosition();

  Unum teammate = ClosestTeammateToBall();
  Unum opponent = ClosestOpponentToBall();

  float teammate_distance = 400, opponent_distance = 400;
  if ( teammate != Unum_Unknown )
    teammate_distance = TeammateDistanceTo(teammate,ball);
  if ( opponent != Unum_Unknown )
    opponent_distance = OpponentDistanceTo(opponent,ball);

  if ( teammate_distance < opponent_distance && BallKickableForTeammate(teammate,buffer) )
    return teammate;
  else if ( opponent_distance < teammate_distance && BallKickableForOpponent(opponent,buffer) )
    return -opponent;
  else
    return Unum_Unknown;
}

/*****************************************************************************************/

Unum PositionInfo::TeammateWithBall(float buffer)
{
  Unum player = PlayerWithBall(buffer);
  if ( player > 0 ) return player;
  else return Unum_Unknown;
}

/*****************************************************************************************/

Unum PositionInfo::OpponentWithBall(float buffer)
{
  Unum player = PlayerWithBall(buffer);
  if ( player < 0 ) return -player;
  else return Unum_Unknown;
}

/*****************************************************************************************/

char PositionInfo::TeamWithBall(float buffer)
{
  Unum player = PlayerWithBall(buffer);
  if ( player == Unum_Unknown ) return '?';
  if ( player > 0 ) return MySide;
  if (player < 0) return TheirSide;
  my_error("player with ball needs to say something");
  return '*';
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int PositionInfo::PlayerPredictedCyclesToPoint(char side, Unum num, Vector pt,
					       float dash_power, float buffer)
{
  if (side == MySide && num == MyNumber)
    return PredictedCyclesToPoint(pt, dash_power);
  
  if (!PlayerPositionValid(side, num)) {
    my_error("Can't predict cycles to point if position invalid %c %d", side, num);
    return 10000;
  }

  LogAction7(210, "PlayerPredCyclesToPoint: %d, (%.1f, %.1f) %.1f, %.1f",
	     (side == MySide ? num : -num), pt.x, pt.y, dash_power, buffer);
  
  float bodyang;
  if (PlayerBodyAngleValid(side,num)) bodyang = PlayerAbsoluteBodyAngle(side,num);
  else bodyang = (pt - PlayerAbsolutePosition(side, num)).dir();
  Vector position = PlayerAbsolutePosition(side,num);
  Vector velocity;
  if ( !PlayerVelocityValid(side,num)) velocity = 0;
  else velocity = PlayerAbsoluteVelocity(side,num);
  /* in Stockholm, it was velocity = MyVel(); */

  for (int i=0; TRUE; i++) {
    if (position.dist(pt) <= buffer) {
      LogAction6(210, "PlayerPredicatedPosition: %d at (%.1f, %.1f) spd: %.2f. Player's there",
		i, position.x, position.y, velocity.mod());
      return i;    
    }
    
    /* decide if they should turn */
    float targ_ang = (pt-position).dir() - bodyang;
    NormalizeAngleDeg(&targ_ang);
    if (fabs(targ_ang) > CP_max_go_to_point_angle_err) {
      /* turning */
      float this_turn = MinMax(-EffectiveTurn(SP_max_moment, velocity.mod()),
			       targ_ang,
			       EffectiveTurn(SP_max_moment, velocity.mod()));
      bodyang += this_turn;
      LogAction7(210, "PlayerPredicatedPosition: %d at (%.1f, %.1f) spd: %.2f. turning %.2f",
		i, position.x, position.y, velocity.mod(), this_turn);
    } else {
      /* dashing */
      velocity += Polar2Vector( dash_power*Player_dash_power_rate(side,num), bodyang );
      LogAction6(210, "PlayerPredicatedPosition: %d at (%.1f, %.1f) spd: %.2f. dashing",
		i, position.x, position.y, velocity.mod());
    }
    
    float max_speed = Player_speed_max(side,num);
    if ( velocity.mod() > max_speed ){
      //      my_error("(5) object CAN'T be moving that fast %c %d %d",side,(int)num,(int)velocity.mod()*100);
      velocity *= ( max_speed/velocity.mod() );
    }
    
    position += velocity;
    velocity *= Player_decay(side,num);
  }
}


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

PlayerObject *PositionInfo::GetTeammate(Unum num)
{
  if ( num==Unum_Unknown ) my_error("Shouldn't get a teammate with unknown num");
  if ( num==MyNumber ) my_error("Shouldn't get self from MyPlayer");

  for (int i=0; i<num_my_players; i++)
    if ( MyPlayer[i]->unum == num ) return MyPlayer[i];

  return NULL;
}

/*********************************************************************************/

PlayerObject *PositionInfo::GetOpponent(Unum num)
{
  if ( num==Unum_Unknown ) my_error("Shouldn't get an opponent with unknown num");

  for (int i=0; i<num_their_players; i++)
    if ( TheirPlayer[i]->unum == num ) return TheirPlayer[i];

  return NULL;
}

/*********************************************************************************/

PlayerObject *PositionInfo::GetPlayer(char side, Unum num)
{
  if ( side == MySide )
    return GetTeammate(num);
  else if ( side == TheirSide ) 
    return GetOpponent(num);
  else 
    my_error("Can't get a player from an unknown side");

  return NULL;
}


/*********************************************************************************/

PlayerObject *PositionInfo::GetNewPlayer(char side, Unum num)
{
  while ( num_free_players <= 0 ||
	  (side == MySide && num_my_players >= SP_team_size-1) ||
	  (side == TheirSide && num_their_players >= SP_team_size) ){
    if ( !ForgetAPlayer(side) )  /* All the players on that side are valid */
      return NULL;
  }

  PlayerObject *player = FreePlayer[--num_free_players]; 
  
  if ( side == MySide )
    MyPlayer[num_my_players++] = player;
  else if ( side == TheirSide )
    TheirPlayer[num_their_players++] = player;
  else /* side == '?' */
    TeamlessPlayer[num_teamless_players++] = player;

  player->side = side;
  player->unum = num;
  player->set_max_speed(Player_speed_max(side,num));
  player->set_motion_decay(Player_decay(side,num));
  
  return player;
}

/*********************************************************************************/

Bool PositionInfo::ForgetAPlayer(char side)
{
  /* char original_side = side; */

  /* If there aren't enough on the team, forget a teamless player */
  if ( side == MySide && num_my_players < SP_team_size-1 ) 
    side = '?';
  if ( side == TheirSide && num_their_players < SP_team_size ) 
    side = '?';

  /* get rid of the one with unknown numbers that have lowest conf */
  int least_conf_index = -1;
  float least_conf = CP_max_conf+1;  /* Must forget a player, even if all at max conf */
  for ( int i=0; i<num_players; i++ ){

    if ( side == 'f' ) continue;
    if ( side != '?' && side != Player[i].side ) continue;

    if ( (Player[i].unum == Unum_Unknown || Player[i].side == '?') && Player[i].pos_valid() < least_conf ){
      least_conf = Player[i].pos_valid();
      least_conf_index = i;
    }
  }

  if ( least_conf_index >= 0 ){
    side = Player[least_conf_index].side;   
    Player[least_conf_index].reset();
  }
  else if ( side == MySide && num_my_players > SP_team_size-1 ){
    if ( ResetMyDuplicatePlayers() == FALSE ){
      my_error("There should be a duplicate MyPlayer");
      return FALSE; /* else do the clean below */
    }
  }
  else if ( side == TheirSide && num_their_players > SP_team_size ){
    if ( ResetTheirDuplicatePlayers() == FALSE ){
      my_error("There should be a duplicate TheirPlayer");
      return FALSE; /* else do the clean below */
    }
  }
  else
    return FALSE;

  CleanAllPlayers();
  /* if      ( side == MySide    ) CleanMyPlayers();
  else if ( side == TheirSide ) CleanTheirPlayers();
  else if ( side == '?'       ) CleanTeamlessPlayers();
  else my_error("Which side?"); */

  return TRUE;
}

/*********************************************************************************/

void PositionInfo::CleanMyPlayers()
{
  int new_num_my_players = 0;

  for (int i=0; i<num_my_players; i++){
    if ( MyPlayer[i]->pos_valid() ) 
      MyPlayer[new_num_my_players++] = MyPlayer[i];
    else{
      MyPlayer[i]->side = 'f';
      MyPlayer[i]->unum = Unum_Unknown;
      MyPlayer[i]->set_max_speed(SP_player_global_speed_max);
      MyPlayer[i]->set_motion_decay(SP_player_decay);
      MyPlayer[i]->reset();
      FreePlayer[num_free_players++] = MyPlayer[i];
    }
  }

  num_my_players = new_num_my_players;

  while ( num_my_players > SP_team_size-1 ){
    /* my_stamp; printf("%d of my players\n",num_my_players);    */
    if ( ForgetAPlayer(MySide) == FALSE ){  /* recurses through CleanMyPlayers */
      my_error("Should be able to forget a teammate");
      printf("Number of players (%d %d %d %d)\n",
	     num_my_players,num_their_players,num_teamless_players,num_free_players);
      for (int i=0; i<num_players; i++)
	printf("%c %d   %f\n",Player[i].side,Player[i].unum,Player[i].pos_valid());
    }
  }

  if (num_my_players > SP_team_size-1){
    my_error("Too many of my players %d",num_my_players);
    /* my_stamp;
    for (int i=0; i<num_my_players; i++)
      printf("%d %.1f  ",MyPlayer[i]->unum,MyPlayer[i]->pos_valid() );
    printf("\n");
    dump_core("dump");*/
  }
}

/*********************************************************************************/

void PositionInfo::CleanTheirPlayers()
{
  int new_num_their_players = 0;

  for (int i=0; i<num_their_players; i++){
    if ( TheirPlayer[i]->pos_valid() ) 
      TheirPlayer[new_num_their_players++] = TheirPlayer[i];
    else{
      TheirPlayer[i]->side = 'f';
      TheirPlayer[i]->unum = Unum_Unknown;
      TheirPlayer[i]->set_max_speed(SP_player_global_speed_max);
      TheirPlayer[i]->set_motion_decay(SP_player_decay);
      TheirPlayer[i]->reset();
      FreePlayer[num_free_players++] = TheirPlayer[i];
    }
  }
  num_their_players = new_num_their_players;

  while ( num_their_players > SP_team_size ){
    /* my_stamp; printf("%d of their players\n",num_their_players);    */
    if ( ForgetAPlayer(TheirSide) == FALSE ){  /* recurses through CleanTheirPlayers */
      my_error("Should be able to forget an opponent");
      printf("Number of players (%d %d %d %d)\n",
	     num_my_players,num_their_players,num_teamless_players,num_free_players);
      for (int i=0; i<num_players; i++)
	printf("%c %d   %f\n",Player[i].side,Player[i].unum,Player[i].pos_valid());
    }
  }

  if (num_their_players > SP_team_size){
    my_error("Too many of their players %d",num_their_players);
    /*my_stamp;
    for (int i=0; i<num_their_players; i++)
      printf("%d %.1f  ",TheirPlayer[i]->unum,TheirPlayer[i]->pos_valid() );
    printf("\n");
    dump_core("dump");*/
  }
}

/*********************************************************************************/

void PositionInfo::CleanTeamlessPlayers()
{
  int new_num_teamless_players = 0;

  for (int i=0; i<num_teamless_players; i++){
    if ( TeamlessPlayer[i]->pos_valid() ){
      /* player may have been identified as being on one team or another */
      if ( TeamlessPlayer[i]->side == MySide ){
	MyPlayer[num_my_players++] = TeamlessPlayer[i];
      }
      else if ( TeamlessPlayer[i]->side == TheirSide ){
	TheirPlayer[num_their_players++] = TeamlessPlayer[i];
      }
      else if ( TeamlessPlayer[i]->side == '?' )
	TeamlessPlayer[new_num_teamless_players++] = TeamlessPlayer[i];
      else
	my_error("Teamless players should have side '?'");
    }      
    else{
      TeamlessPlayer[i]->side = 'f';
      TeamlessPlayer[i]->unum = Unum_Unknown;
      TeamlessPlayer[i]->set_max_speed(SP_player_global_speed_max);
      TeamlessPlayer[i]->set_motion_decay(SP_player_decay);
      TeamlessPlayer[i]->reset();
      FreePlayer[num_free_players++] = TeamlessPlayer[i];
    }
  }

  num_teamless_players = new_num_teamless_players;

  while ( num_teamless_players > num_players )
    if ( ForgetAPlayer('?') == FALSE ) /* recurses through CleanTeamlessPlayers */
      my_error("Should be able to forget a teamless player");

  if (num_teamless_players > num_players ){
    my_error("Too many of teamless players %d",num_teamless_players);
    my_stamp;
    for (int i=0; i<num_teamless_players; i++)
      printf("%d %.1f  ",TeamlessPlayer[i]->unum,TeamlessPlayer[i]->pos_valid() );
    printf("\n");
  }
}

/*********************************************************************************/

void PositionInfo::CleanAllPlayers()
{
  CleanTeamlessPlayers();  /* has to be first to move over reconciled players */
  CleanMyPlayers();
  CleanTheirPlayers();
}

/*********************************************************************************/

Bool PositionInfo::ResetMyDuplicatePlayers()
{
  if ( num_my_players <= SP_team_size-1 ) my_error("Shouldn't be duplicates");
  
  int MyPlayerIndex[MAX_PLAYERS]; /* Map uniform number to index in MyPlayer */
  
  for (int num=1; num<=SP_team_size; num++)
    MyPlayerIndex[num] = -1;

  Bool DuplicateFound = FALSE;
  Unum unum;
  for (int i=0; i<num_my_players; i++){
    unum = MyPlayer[i]->unum;
    if ( unum == Unum_Unknown ) my_error("Catch unknowns before duplicates (my)");

    if ( unum == MyNumber ){
      MyPlayer[i]->reset();
      DuplicateFound = TRUE;
      my_error("How did my number get in the mix?",unum);
    }
    else if ( MyPlayerIndex[unum] == -1 )
      MyPlayerIndex[unum] = i;
    else if ( MyPlayer[MyPlayerIndex[unum]]->pos_valid() >= MyPlayer[i]->pos_valid() ){
      MyPlayer[i]->reset();
      DuplicateFound = TRUE;
      my_error("There were 2 %d's on my side",unum);
    }
    else{
      MyPlayer[MyPlayerIndex[unum]]->reset();
      MyPlayerIndex[unum] = i;
      DuplicateFound = TRUE;
      my_error("There were 2 %d's on my side",unum);
    }
  }
      
  return DuplicateFound;
}


/*********************************************************************************/

Bool PositionInfo::ResetTheirDuplicatePlayers()
{
  if ( num_their_players <= SP_team_size ) my_error("Shouldn't be duplicates");
  
  int TheirPlayerIndex[MAX_PLAYERS]; /* Map uniform number to index in TheirPlayer */
  
  for (int num=1; num<=SP_team_size; num++)
    TheirPlayerIndex[num] = -1;

  Bool DuplicateFound = FALSE;
  Unum unum;
  for (int i=0; i<num_their_players; i++){
    unum = TheirPlayer[i]->unum;
    if ( unum == Unum_Unknown ) my_error("Catch unknowns before duplicates (their)");

    if ( TheirPlayerIndex[unum] == -1 )
      TheirPlayerIndex[unum] = i;
    else if ( TheirPlayer[TheirPlayerIndex[unum]]->pos_valid() >= TheirPlayer[i]->pos_valid() ){
      TheirPlayer[i]->reset();
      DuplicateFound = TRUE;
      my_error("There were 2 %d's on their side",unum);
    }
    else{
      TheirPlayer[TheirPlayerIndex[unum]]->reset();
      TheirPlayerIndex[unum] = i;
      DuplicateFound = TRUE;
      my_error("There were 2 %d's on their side",unum);
    }
  }
      
  return DuplicateFound;
}

/*********************************************************************************/

void PositionInfo::ClearSeenInfo()
{
  /* In case we need to forget a sight because a new one came in before we could update */
  Ball.clear_seen();
  for (int i=0; i<num_players; i++)
    Player[i].clear_seen();
  num_unknown_players = 0;
  
  for (int i=0; i<num_seen_markers; i++)
    Marker[SeenMarker[i]].clear_seen();  /* Not necessarily needed... */
  num_seen_markers = 0;
  ClosestMarker = ClosestMotionMarker = No_Marker;
  SeenLine = SL_No_Line;
}

/*********************************************************************************/

MarkerType PositionInfo::ClosestGoal() 
{
  if ( MyConf() && MyX() < 0 )
    return RM_My_Goal;
  else
    return RM_Their_Goal;
}

/*********************************************************************************/

MarkerType PositionInfo::ClosestFlagTo() 
{
  return No_Marker; 
}
 
/*********************************************************************************/

PlayerObject *PositionInfo::ClosestPlayerObjectTo(Vector gpos) 
{
  /* Need to check teammates, opponents, teamless */
  /* return NULL if no-one's within player-max-speed (sqr) of the right place */

  int i;
  PlayerObject *player = NULL;
  float min_dist_sqr = 40000;   /* More than pitch_length*pitch_length + pitch_width*pitch_width */
  /* If player is farther than 4 times the maximum single-cycle movement, it can't be the same one */
  float max_dist_sqr = (SP_player_global_speed_max * CP_max_player_move_factor) * 
                       (SP_player_global_speed_max * CP_max_player_move_factor);
  float dist_sqr;

  for (i=0; i<num_my_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(MyPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(MyPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = MyPlayer[i];
    }
  }
  for (i=0; i<num_their_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(TheirPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(TheirPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = TheirPlayer[i];
    }
  }
  for (i=0; i<num_teamless_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(TeamlessPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(TeamlessPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = TeamlessPlayer[i];
    }
  }

  return player; 
}

/*********************************************************************************/

PlayerObject *PositionInfo::ClosestTeammateObjectTo(Vector gpos) 
{
  /* Need to check teammates, teamless */
  /* return NULL if no-one's within player-max-speed (sqr) of the right place */

  int i;
  PlayerObject *player = NULL;
  float min_dist_sqr = 40000;   /* More than pitch_length*pitch_length + pitch_width*pitch_width */
  /* If player is farther than 4 times the maximum single-cycle movement, it can't be the same one */
  float max_dist_sqr = (SP_player_global_speed_max * CP_max_player_move_factor) * 
                       (SP_player_global_speed_max * CP_max_player_move_factor);
  float dist_sqr;

  for (i=0; i<num_my_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(MyPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(MyPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = MyPlayer[i];
    }
  }
  for (i=0; i<num_teamless_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(TeamlessPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(TeamlessPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = TeamlessPlayer[i];
    }
  }

  return player; 
}


/*********************************************************************************/

PlayerObject *PositionInfo::ClosestOpponentObjectTo(Vector gpos) 
{
  /* Need to check opponents, teamless */
  /* return NULL if no-one's within player-max-speed (sqr) of the right place */

  int i;
  PlayerObject *player = NULL;
  float min_dist_sqr = 40000;   /* More than pitch_length*pitch_length + pitch_width*pitch_width */
  /* If player is farther than 4 times the maximum single-cycle movement, it can't be the same one */
  float max_dist_sqr = (SP_player_global_speed_max * CP_max_player_move_factor) * 
                       (SP_player_global_speed_max * CP_max_player_move_factor);
  float dist_sqr;

  for (i=0; i<num_their_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(TheirPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(TheirPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = TheirPlayer[i];
    }
  }
  for (i=0; i<num_teamless_players; i++){
    /* not in memory yet, so not valid -- must be different player */
    if ( !(TeamlessPlayer[i]->pos_valid()) ) continue; 

    dist_sqr = gpos.dist2(TeamlessPlayer[i]->get_abs_pos());
    if ( dist_sqr < max_dist_sqr && dist_sqr < min_dist_sqr ){
      min_dist_sqr = dist_sqr;
      player = TeamlessPlayer[i];
    }
  }

  return player; 
}

/*********************************************************************************/

PlayerObject *PositionInfo::ClosestPlayerObjectTo(char side, Vector gpos) 
{
  if ( side == MySide ) 
    return ClosestTeammateObjectTo(gpos);
  else if ( side == TheirSide ) 
    return ClosestOpponentObjectTo(gpos);
  else /* side == '?' */
    return ClosestPlayerObjectTo(gpos);
}

/*********************************************************************************/

/* SMURF: as a hack, if n is Unum_Teamless, it does it for closest
   teamless player */
Bool PositionInfo::BallKickableForPlayer(char s, Unum n, float buffer=0)
{
  Vector pos;
  if (n == Unum_Teamless) {    
    if (NumTeamlessPlayers() < 1)
      my_error("Can't tell kickable for teamless if there aren;t any");
    pos = ClosestTeamlessPlayerPosition();
  } else {
    if (!PlayerPositionValid(s, n)){
      my_error("Can't tell you if they can kick it if we don;t know where they are");
      return FALSE;
    }
    pos = PlayerAbsolutePosition(s,n); 
  }
  return ((pos - BallAbsolutePosition()).mod() <
    Player_kickable_area(s,n) - buffer) ? TRUE : FALSE;
} 



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

void PositionInfo::update()
{
  if ( NewSight && LastSightTime < CurrentTime - 1 ){
    /* Special case--ignore sights from just before change to play_on_mode
       if they seem to be 2 cycles ago.  Better would be to adjust the sight
       time, but then sight times of all other objects also need to be adjusted */
    NewSight = FALSE;  
    if ( LastSightTime < CurrentTime - 2 )
      my_error("last sight shouldn't be so out of date");
  }

  /* before updating from any sights: */
  my_last_body_ang = MyBodyAng(); 
  my_last_neck_global_ang = MyNeckGlobalAng();
  my_last_neck_rel_ang = MyNeckRelAng();

  /* see the lengthy comment above titled POSITION BASED VELOCITY ESTIMATION */
  /* this next part used to be just for position based velocity estimation, but now
     we use it for ball velocity invalidation as well
     The comment referred to previously also applies to ball velocity invalidation */
  Vector estimated_pos;
  Bool estimated_pos_valid = FALSE;
  //if (CP_use_new_position_based_vel && NewSight && MyConf()) {
  if (NewSight && MyConf()) {
    update_self_estimate(LastSightTime);
    estimated_pos = MyPos();
    estimated_pos_valid = TRUE;
    //cout << "Time: " << CurrentTime.t << "\tEstPos: " << estimated_pos << endl;
  }
  
  /* So my velocity and position match for computing ball, player positions */
  /* Updates my position, angle and velocity at LastSightTime               */
  if ( NewSight && LastSightTime < CurrentTime )
    update_self_seen(LastSightTime);
  else 
    update_self(CurrentTime);

  /* see the lengthy comment above titled POSITION BASED VELOCITY ESTIMATION */
  /* this next part used to be just for position based velocity estimation, but now
     we use it for ball velocity invalidation as well
     The comment referred to previously also applies to ball velocity invalidation */
  //if (CP_use_new_position_based_vel && NewSight && MyConf() && estimated_pos_valid) {    
  if (NewSight && MyConf() && estimated_pos_valid) {
    sight_position_correction = MyPos() - estimated_pos;    
    sight_position_correction_time = LastSightTime;
  }
  
  update_ball(CurrentTime);
  update_players(CurrentTime);

  /* Brings me from time prev time to CurrentTime if necessary */
  if ( MyUpdateTime() < CurrentTime )
    update_self_estimate(CurrentTime);

  update_stamina(CurrentTime);
  update_offside_lines();

#ifndef NO_ACTION_LOG
  if (CP_save_action_log_level >= 175) {
    char outstring[200], *pc;
    
    if (!Mem->MyConf())
      sprintf(outstring, "My Pos: (?, ?)\tangle: ??\trel_neck: %.2f",
	      MyNeckRelAng());
    else
      sprintf(outstring, "My Pos: (%.2f, %.2f)\tangle: %.2f\trel_neck: %.2f\tvel: (%.2f %.2f)\tconf: %.2f\tstamina: %.2f",
	      MyX(), MyY(), MyBodyAng(), MyNeckRelAng(), MySpeed(), MyDir(), MyConf(), MyStamina());
    LogAction2(175, outstring);

    if (!Mem->BallPositionValid())
      sprintf(outstring, "Ball Pos: (??, ?)");
    else if (!Mem->BallVelocityValid())
      sprintf(outstring, "Ball Pos: (%.2f, %.2f)conf: %.2f\tVel: (??, ?)",
	      BallX(), BallY(), BallPositionValid());
    else 
      sprintf(outstring, "Ball Pos: (%.2f, %.2f)conf: %.2f\tVel: (%.2f,%.2f)dx/dy = (%.2f,%.2f)sp/head conf: %.2f",
	      BallX(), BallY(), BallPositionValid(),
	      BallAbsoluteVelocity().x, BallAbsoluteVelocity().y,
	      BallSpeed(), BallAbsoluteHeading(),
	      BallVelocityValid());
    LogAction2(175, outstring);

    strcpy(outstring, "Player pos known: team: ");
    pc = outstring + strlen(outstring);
    for (int num = 1; num <= SP_team_size; num++){
      if ( TeammatePositionValid(num) ){
	*(pc++) = char_for_num(num);
	if ( isnan(TeammateX(num)) )
	  *(pc++) = '*';
      }
      else
	*(pc++) = '_';
    }
    *pc = 0; /* null terminate */
    strcat(outstring, "\topp: ");
    pc = outstring + strlen(outstring);
    for (int num = 1; num <= SP_team_size; num++){
      if ( OpponentPositionValid(num) ){
	*(pc++) = char_for_num(num);
	if ( isnan(OpponentX(num)) )
	  *(pc++) = '*';
      }
      else
	*(pc++) = '_';
    }
    *pc = 0; /* null terminate */
    LogAction2(200, outstring);

    strcpy(outstring, "Player pos conf:  team: "); 
    pc = outstring + strlen(outstring); 
    for (int num = 1; num <= SP_team_size; num++){ 
      int conf = 0; 
      if ( TeammatePositionValid(num) ){ 
        conf = (int) (TeammatePositionValid(num)*10); 
        if ( conf==10 ) conf=1; 
      } 
      *(pc++) = (TeammatePositionValid(num) ? char_for_num(conf) : '_'); 
    } 
    *pc = 0; /* null terminate */ 
    strcat(outstring, "\topp: "); 
    pc = outstring + strlen(outstring); 
    for (int num = 1; num <= SP_team_size; num++){ 
      int conf = 0; 
      if ( OpponentPositionValid(num) ){ 
        conf = (int) (OpponentPositionValid(num)*10); 
        if ( conf==10 ) conf=1; 
      } 
      *(pc++) = (OpponentPositionValid(num) ? char_for_num(conf) : '_'); 
    } 
    *pc = 0; /* null terminate */ 
    LogAction2(200, outstring); 
 
    strcpy(outstring, "Player tired:     team: "); 
    pc = outstring + strlen(outstring); 
    for (int num = 1; num <= SP_team_size; num++) 
      *(pc++) = (TeammateTired(num) ? 'T' : 'F'); 
    *pc = 0; /* null terminate */ 
    LogAction2(200, outstring); 

    //for (int num = 1; num <= SP_team_size; num++) 
    //LogAction4(200,"teammate %d stamina %f",num,GetTeammateStamina(num))

    if (NewSight) {
      /* record what we saw for this new sight */
      sprintf(outstring, "Sight at %d.%d: %c%c team: ", LastSightTime.t,
	      LastSightTime.s,
	      (Ball.GetSeenTime() == LastSightTime ? 'B' : '_'),
	      (Ball.GetSeenMovingTime() == LastSightTime ? 'v' : '_'));
      pc = outstring + strlen(outstring);
      for (int num = 1; num <= SP_team_size; num++) {
	if (num != MyNumber && GetTeammate(num))
	  *(pc++) = (GetTeammate(num)->GetSeenTime() == LastSightTime ?
		     char_for_num(num) : '_');
	else
	  *(pc++) = '_';
      }      
      *pc = 0; /* null terminate */
      strcat(outstring, "\topp: ");
      pc = outstring + strlen(outstring);
      for (int num = 1; num <= SP_team_size; num++) {
	if (GetOpponent(num))
	  *(pc++) = (GetOpponent(num)->GetSeenTime() == LastSightTime ?
		     char_for_num(num) : '_');
	else
	  *(pc++) = '_';
      }
      *pc = 0; /* null terminate */
      LogAction2(175, outstring);
    }
  }
#endif

  for (int num = 1; num <= SP_team_size; num++){
    if ( TeammatePositionValid(num) ){
      if ( isnan(TeammateX(num)) || isnan(TeammateY(num)) ){
	LogAction3(200,"Forgetting teammate %d!!",num);
	ForgetTeammate(num);
      }
    }
    if ( OpponentPositionValid(num) ){
      if ( isnan(OpponentX(num)) || isnan(OpponentY(num)) ){
	LogAction3(200,"Forgetting opponent %d!!",num);
	ForgetOpponent(num);
      }
    }
  }



  NewAction = FALSE;
  NewSight = FALSE;
}


/*********************************************************************************/

void PositionInfo::update_self_seen(Time time)
{
  /* Brings me to the best known position and speed as of time x through vision and 
     past values */
  if ( !NewSight || time < CurrentTime - 1 )
    my_error("No new sight with which to update %d %d (%d %d)",
	     time.t,time.s,LastStartClockTime.t,LastStartClockTime.s);

  update_self_neck_rel_ang(time);

  if ( SeenLine != SL_No_Line && ClosestMarker != No_Marker ){
    Fieldline[SeenLine].sanitize_times();
    Marker[ClosestMarker].sanitize_times();

    SetMyBodyAng( GetNormalizeAngleDeg(Fieldline[SeenLine].get_my_neck_global_ang() - MyNeckRelAng()) );
    SetMyPos( Marker[ClosestMarker].get_my_pos(MyNeckGlobalAng()), time );

    if ( ( MyVelConf() < CP_max_conf || my_vel_time != time ) && SensedInfoKnown(time) ){
      if ( SP_version >= 6.00 )
	SetMyVel(Polar2Vector(GetMySensedSpeed(time),
		            GetNormalizeAngleDeg(GetMySensedHeadingRelToNeckAngle(time)+ 
						 MyNeckGlobalAng())), time ); 
      else
	SetMyVel( Polar2Vector(GetMySensedSpeed(time),MyBodyAng()), time ); 
    }
  }
  // else my_error("NO MARKER");  /* not really an error---can estimate */

  for (int i=0; i<num_seen_markers; i++)
    Marker[SeenMarker[i]].reset();  /* Not necessarily needed... */
  num_seen_markers = 0;
  ClosestMarker = ClosestMotionMarker = No_Marker;
  SeenLine = SL_No_Line;
}

/*********************************************************************************/

void PositionInfo::update_self(Time time)
{
  if ( NewSight ){
    if ( LastSightTime != CurrentTime ) my_error("shouldn't be here");
    update_self_seen(time);
  }

  /* Brings unknown values up to date */
  if ( MyUpdateTime() < time )
    update_self_estimate(time);

  if ( arm_expires ){
    PointDest = RelativeToMyNeck2Global(Polar2Vector(arm_target_dist,arm_target_ang));
    //my_error("PointDest is %.2f %.2f  (dist/ang %.2f %.2f)",PointDest.x,PointDest.y,
    //     arm_target_dist,arm_target_ang);
  }
}

/*********************************************************************************/

void PositionInfo::update_ball(Time time)
{
  Ball.update(time);
}

/*********************************************************************************/

void PositionInfo::update_players(Time time)
{
  int i;

  /* Assume my position's already updated here */
  if ( NewSight && MyConf() ) reconcile_unknown_players();

  for (i=0; i<num_my_players; i++)
    MyPlayer[i]->update(time);
  for (i=0; i<num_their_players; i++)
    TheirPlayer[i]->update(time);
  for (i=0; i<num_teamless_players; i++)
    TeamlessPlayer[i]->update(time);

  CleanAllPlayers();
  num_unknown_players=0;

  if ( num_free_players + num_my_players + num_their_players + num_teamless_players != num_players )
    my_error("Number of players doesn't add up (%d %d %d %d)",
	     num_my_players,num_their_players,num_teamless_players,num_free_players);
}

/*********************************************************************************/

void PositionInfo::reconcile_unknown_players()
{
  if ( !MyConf() || MyPosTime() != LastSightTime ) 
    my_error("Can't reconcile unknown players if not localized");

  PlayerObject *player;
  TempPlayerObject *unknown_player;

  char s;
  float d; 
  AngleDeg a;
  Time t;

  for ( int i=0; i<num_unknown_players; i++ ){

    unknown_player = &(UnknownPlayer[i]);
    s = unknown_player->side;
    d = unknown_player->dist;
    a = unknown_player->ang_from_neck;
    t = unknown_player->time;

    if ( t != LastSightTime ) continue; /* For some reason, some old players get to here */

    Vector rel_pos = Polar2Vector(d,a);
    Vector global_pos = MyPos() + rel_pos.rotate(MyNeckGlobalAng());

    if ( (player = ClosestPlayerObjectTo(s,global_pos)) == NULL )
      player = GetNewPlayer(s,Unum_Unknown);
    else if ( s != '?' && player->side == '?' )
      player->side = s; /* know the teamless player's side now  */
                        /* need to call CleanTeamlessPlayers() before doing much else
                           (like CleanMyPlayers(), ForgetAPlayer(), etc.*/
    if ( player != NULL && ( player->unum != MyNumber || player->side != MySide ) )
      player->set_polar_from_neck(d,a,t);
  }
}

/*********************************************************************************/

void PositionInfo::update_offside_lines()
{
  if ( !SP_use_offside ) return;

  float first = 0, second = 0, tmp;

  for ( int i=0; i < num_my_players; i++) {
    if ( MyPlayer[i]->pos_valid() && MyPlayer[i]->get_abs_pos().Behind(second) ){
      second = MyPlayer[i]->get_abs_pos().x;
      if (second < first) {
	tmp = first ;
	first = second ;
	second = tmp ;
      }
    }
  }
  if ( BallPositionValid() && BallAbsolutePosition().Behind(second) )
    their_offside_line = BallAbsolutePosition().x;
  else
    their_offside_line = second;

  first = second = 0;

  for ( int i=0; i < num_their_players; i++) {
    if ( TheirPlayer[i]->pos_valid() && TheirPlayer[i]->get_abs_pos().InFrontOf(second) ){
      second = TheirPlayer[i]->get_abs_pos().x;
      if (second > first) {
	tmp = first ;
	first = second ;
	second = tmp ;
      }
    }
  }

  // Assume there's a goalie at most 20 from the end line
  if (first < 25 && Mem->NumOpponents() < SP_team_size) 
    second = first;

  if ( BallPositionValid() && BallAbsolutePosition().InFrontOf(second) )
    my_offside_line = BallAbsolutePosition().x;
  else
    my_offside_line = second;
}

/*********************************************************************************/

Bool PositionInfo::OffsidePosition(float x, char side)
{
  if ( !SP_use_offside ) return FALSE;

  if      ( side == MySide )    return ( x > my_offside_line ) ? TRUE : FALSE;
  else if ( side == TheirSide ) return ( x < their_offside_line) ? TRUE : FALSE;
  else my_error("Can't tell offside if don't know the player's team");
  return FALSE;
}

/*********************************************************************************/

Bool PositionInfo::TeammateInOffsidePosition(Unum num)
{
  if ( !SP_use_offside ) return FALSE;
  if ( !TeammatePositionValid(num) ) my_error("Can't tell if teammate offside--not valid");

  return OffsidePosition(TeammateAbsolutePosition(num),MySide);
}

/*********************************************************************************/

Bool PositionInfo::OpponentInOffsidePosition(Unum num)
{
  if ( !SP_use_offside ) return FALSE;
  if ( !OpponentPositionValid(num) ) my_error("Can't tell if opponent offside--not valid");

  return OffsidePosition(OpponentAbsolutePosition(num),TheirSide);
}

/*********************************************************************************/

Bool PositionInfo::PlayerInOffsidePosition(char side, Unum num)
{
  if ( !SP_use_offside ) return FALSE;

  if      ( side == MySide )    return TeammateInOffsidePosition(num);
  else if ( side == TheirSide ) return OpponentInOffsidePosition(num);
  else my_error("Can't tell offside if don't know the player's team");
  return FALSE;
}

/*********************************************************************************/

Unum PositionInfo::TeammateOffsideIfIKick()
{
  if ( !SP_use_offside ) return Unum_Unknown;

  for (int teammate=1; teammate<=SP_team_size; teammate++){
    if ( teammate != MyNumber && TeammatePositionValid(teammate) && 
	 TeammateDistance(teammate) < SP_offside_area && TeammateInOffsidePosition(teammate) )
      return teammate;
  }

  return Unum_Unknown;
}

/*****************************************************************************************/

float PositionInfo::XToAdjustForOffsideX(float x, float buffer)
{
  if ( !SP_use_offside ) return x;

  float back_x = -SP_pitch_length/2;
  if ( PullOffside ) back_x = PullOffsidePosition+buffer;

  float front_x = my_offside_line;

  if ( BallPositionValid() )
    back_x = Min(back_x, BallX());  
  
  if ( x < back_x && x < -20 )  /* bring players up -20's a HACK -- want midfielders up*/
    x = back_x;
  else if ( x <= my_offside_line )
    /* move midfielders back in proportion to difference between pull_x and my_offside_line
       relative to the length of the field */
    x = ((x + SP_pitch_length/2)/SP_pitch_length) * (front_x - back_x) + back_x;

  if ( OffsidePosition(x,MySide) )
    x = XToOnsideX(x,buffer);
    
  return x;
}

/*****************************************************************************************/

Rectangle PositionInfo::RectangleToAdjustForOffsideRectangle(Rectangle *rect, float buffer)
{
  if ( !SP_use_offside ) return *rect;

  float right   = XToAdjustForOffsideX(rect->RightX(),buffer);
  float left    = Min(right-5,XToAdjustForOffsideX(rect->LeftX() ,buffer));

  float top     = rect->TopY();
  float bottom  = rect->BottomY();
  
  Rectangle return_rect = Rectangle(left,right,top,bottom);
  return ShiftRectangleInBounds(&return_rect);
}

/*****************************************************************************************/

Vector PositionInfo::PositionToPullOffsidePosition(Vector pos, float buffer)
{
  if ( !SP_use_offside ) return pos;

  float pull_x = PullOffsidePosition+buffer;
  if ( BallPositionValid() )
    pull_x = Min(pull_x, BallX());  
  
  if ( pos.x < pull_x )  /* bring players up */
    pos.x = pull_x;
  else if ( !OffsidePosition(pos,MySide) ) 
    /* move midfielders back in proportion to difference between pull_x and my_offside_line
       relative to the length of the field */
    pos.x = ((pos.x + SP_pitch_length/2)/(my_offside_line + SP_pitch_length/2)) * 
             (my_offside_line - pull_x) + pull_x;

  return pos;
}




/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

void PositionInfo::VerifyDash(float *dash_power)
{
  switch ( PlayMode ){
  case PM_Their_Kick_Off:
  case PM_Their_Kick_In:
  case PM_Their_Free_Kick:
  case PM_Their_Offside_Kick:
  case PM_Their_Corner_Kick:      /* Don't waste stamina trying to get closer to ball */
    if ( MyConf() && BallPositionValid() &&  
	 (MyPos()+NewVelFromDash( MyVel(), *dash_power )).dist(BallAbsolutePosition()) < SP_free_kick_buffer )
      *dash_power = 0;
    break;
  case PM_Their_Goal_Kick:       /* Don't waste stamina trying to get closer to ball */
    if ( MyConf() && BallPositionValid() &&  
	 TheirPenaltyArea.IsWithin(MyPos()+NewVelFromDash( MyVel(), *dash_power )) )
      *dash_power = 0;
    break;
  default: ;
  }

  PlayerInfo::VerifyDash(dash_power);
}




/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

int PositionInfo::SortPlayersBy(char side, char KeyFunc, float KeyNum, Unum* players)
{
  int result = 0; /*Number of players sorted */

  float (PositionInfo::*KeyFunction)(char,Unum);
  /* taking player angles from body */
  KeyFunction =
    (( KeyFunc == 'd' ) ? &(PositionInfo::PlayerDistance) : &(PositionInfo::PlayerAngleFromBody));

  int num = (( side == 'b') ? SP_team_size*2 : SP_team_size); /* Make aux array big 
							         enough */
  float *vals;
  vals = new float[num];
   
  char team = (( side == 't' ) ? TheirSide : MySide);
  for (int i=1; i<=SP_team_size; i++){
    if ( PlayerPositionValid(team,i) ){
      players[result]=i;
      vals[result]   =fabs(KeyNum - (this->*KeyFunction)(team,i));  /* use diff from key*/
      result++;
    }
  }
  
  if ( side == 'b' ){   /* Need to put in Their  team too */
    team = TheirSide;
    for (int i=1; i<=SP_team_size; i++){
      if ( PlayerPositionValid(team,i) ){
	players[result]=i+SP_team_size;    /* to distinguish from my team */
	vals[result]   =fabs(KeyNum - (this->*KeyFunction)(team,i));  /* use diff from key*/
	result++;
      }
    } 
  }
		
/* Now should have all values in question in vals, with uniform number in 
   corresponding position of players ( +TEAM_SIZE for their team if 
   side == 'b'):  Just sort em */

  BubbleSort(result,players,vals);
  delete [] vals;
  return result;
}

/*********************************************************************************/

int PositionInfo::SortPlayersByDistanceToPoint(char side, Vector point, Unum* players)
{
  int result = 0; /*Number of players sorted */

  int num = (( side == 'b') ? SP_team_size*2 : SP_team_size); /* Make aux array big 
							         enough */
  float *vals;
  vals = new float[num];
   
  char team = (( side == 't' ) ? TheirSide : MySide);
  for (int i=1; i<=SP_team_size; i++){
    if ( PlayerPositionValid(team,i) ){
      players[result]=i;
      vals[result]   =PlayerDistance2To(team,i,point);  
      result++;
    }
  }
  
  if ( side == 'b' ){   /* Need to put in Their  team too */
    team = TheirSide;
    for (int i=1; i<=SP_team_size; i++){
      if ( PlayerPositionValid(team,i) ){
	players[result]=i+SP_team_size;    /* to distinguish from my team */
	vals[result]   =PlayerDistance2To(team,i,point);  
	result++;
      }
    } 
  }
		
/* Now should have all values in question in vals, with uniform number in 
   corresponding position of players ( +TEAM_SIZE for their team if 
   side == 'b'):  Just sort em */

  BubbleSort(result,players,vals);
  delete [] vals;
  return result;
}

/*********************************************************************************/

int PositionInfo::SortPlayersByDistanceToLine(char side, Line line, Unum* players, Bool TestEndPoints, Vector ep1, Vector ep2)
{
  int result = 0; /*Number of players sorted */

  int num = (( side == 'b') ? SP_team_size*2 : SP_team_size); /* Make aux array big 
							         enough */
  float *vals;
  vals = new float[num];

  Line perp1, perp2;
  float ep_dist2,d1,d2;
  if ( TestEndPoints==TRUE ){
    perp1 = line.perpendicular(ep1);
    perp2 = line.perpendicular(ep2);
    ep_dist2 = ep1.dist2(ep2);
  }
   
  char team = (( side == 't' ) ? TheirSide : MySide);
  for (int i=1; i<=SP_team_size; i++){
    if ( PlayerPositionValid(team,i) ){
      d1 = PlayerDistance2ToLine(team,i,perp1);
      d2 = PlayerDistance2ToLine(team,i,perp2);
      if ( TestEndPoints==TRUE ){
	if ( d1 > ep_dist2 || d2 > ep_dist2 ) /* not between points */
	  continue;
      }
      players[result]=i;
      vals[result]   =PlayerDistance2ToLine(team,i,line);  
      result++;
    }
  }
  
  if ( side == 'b' ){   /* Need to put in Their  team too */
    team = TheirSide;
    for (int i=1; i<=SP_team_size; i++){
      if ( PlayerPositionValid(team,i) ){
	if ( TestEndPoints ){
	  if ( PlayerDistance2ToLine(team,i,perp1) > ep_dist2 || 
	       PlayerDistance2ToLine(team,i,perp2) > ep_dist2 ) /* not between points */
	    continue;
	}
	players[result]=i+SP_team_size;    /* to distinguish from my team */
	vals[result]   =PlayerDistance2ToLine(team,i,line);  
	result++;
      }
    } 
  }
		
/* Now should have all values in question in vals, with uniform number in 
   corresponding position of players ( +TEAM_SIZE for their team if 
   side == 'b'):  Just sort em */

  BubbleSort(result,players,vals);
  delete [] vals;
  return result;
}

/*********************************************************************************/


int PositionInfo::NumTeammatesWithin(float Dist, Vector of_pos)
{
  int result = 0;
  float Dist2 = Dist*Dist;
  for (int i=1; i<=SP_team_size; i++){
    if ( TeammatePositionValid(i) && TeammateAbsolutePosition(i).dist2(of_pos) <= Dist2 + FLOAT_EPS)
      result++;
  }
  return result;
}

/*********************************************************************************/

int PositionInfo::NumOpponentsWithin(float Dist, Vector of_pos)
{
  int result = 0;
  float Dist2 = Dist*Dist;
  for (int i=1; i<=SP_team_size; i++){
    if ( OpponentPositionValid(i) && OpponentAbsolutePosition(i).dist2(of_pos) <= Dist2 + FLOAT_EPS)
      result++;
  }
  return result;
}

/*********************************************************************************/

int PositionInfo::NumTeammatesWithin(float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng){
  /* ofAng relative to body */
  int result = 0;
  for (int i=1; i<=SP_team_size; i++){
    if ( i != MyNumber && TeammatePositionValid(i) &&
	 fabs(TeammateDistance(i) - ofDist) <= Dist + FLOAT_EPS &&
	 fabs(GetNormalizeAngleDeg(TeammateAngleFromBody(i) - ofAng)) <= Ang + FLOAT_EPS  )
      result++;
  }
  return result;
}

/*********************************************************************************/

int PositionInfo::NumOpponentsWithin(float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng){
  /* ofAng relative to body */
  int result = 0;
  for (int i=1; i<=SP_team_size; i++){
    if ( OpponentPositionValid(i) &&
	 fabs(OpponentDistance(i) - ofDist) <= Dist  + FLOAT_EPS &&
	 fabs(GetNormalizeAngleDeg(OpponentAngleFromBody(i) - ofAng)) <= Ang  + FLOAT_EPS )
      result++;
  }
  return result;
}

/*********************************************************************************/

PlayerObject *PositionInfo::GetPlayerWithin(float Dist, Vector ofPos){
  float Dist2 = Dist*Dist;
  for (int i=0; i<num_players; i++){
    if ( !(Player[i].unum == MyNumber && Player[i].side == MySide) && 
	 Player[i].pos_valid() &&
	 Player[i].get_abs_pos().dist2(ofPos) <= Dist2 + FLOAT_EPS  )
      return &Player[i]; 
  }
  return NULL;
}

/*********************************************************************************/

PlayerObject *PositionInfo::GetPlayerWithin(float Dist, AngleDeg Ang, float ofDist, AngleDeg ofAng){
  /* ofAng relative to body */
  for (int i=0; i<num_players; i++){
    if ( !(Player[i].unum == MyNumber && Player[i].side == MySide) && 
	 Player[i].pos_valid() &&
	 fabs(Player[i].get_dist() - ofDist) <= Dist  + FLOAT_EPS &&
	 fabs(GetNormalizeAngleDeg(Player[i].get_ang_from_body() - ofAng)) <= Ang  + FLOAT_EPS )
      return &Player[i]; 
  }
  return NULL;
}

/*********************************************************************************/

/* end is the end point of the center line of the cone
   wid_dist ratio defines the wid of the cone = wid at dist 1 */
int PositionInfo::NumOpponentsInCone(float wid_dist_ratio, Vector end, Vector vert)
{
  //LogAction7(200,"wid_dist %.1f  vert(%.2f %.2f) end(%.2f %.2f)",wid_dist_ratio,
  //     vert.x,vert.y,end.x,end.y);
  int count = 0;
  Line l = LineFromTwoPoints(vert, end);
  for (Unum opp = 1; opp <= SP_team_size; opp++) {
    if (!OpponentPositionValid(opp))
      continue;
    Vector pt = l.ProjectPoint(OpponentAbsolutePosition(opp));
    if (pt.dist2(OpponentAbsolutePosition(opp)) < pt.dist2(vert)*wid_dist_ratio*wid_dist_ratio
	&& l.InBetween(pt, vert, end)) {
      count++;
    }    
  }
  return count;
}

/*********************************************************************************/

/* end is the end point of the center line of the cone
   wid_dist ratio defines the wid of the cone = wid at dist 1 */
int PositionInfo::NumTeammatesInCone(float wid_dist_ratio, Vector end,
				     Vector vert, Bool IncludeMe)
{
  int count = 0;
  Line l = LineFromTwoPoints(vert, end);
  for (Unum num = 1; num <= SP_team_size; num++) {
    if (IncludeMe && num == MyNumber)
      continue;
    if (!TeammatePositionValid(num))
      continue;
    Vector pt = l.ProjectPoint(TeammateAbsolutePosition(num));
    if (pt.dist(TeammateAbsolutePosition(num)) < pt.dist(vert)*wid_dist_ratio*wid_dist_ratio
	&& l.InBetween(pt, vert, end)) {
      count++;
    }    
  }
  return count;
}

/*********************************************************************************/

int PositionInfo::NumPlayersInConeToPlayer(char which,
				      float wid_dist_ratio, char side,
				      Unum num, float extra_len, Vector vert)
{
  if (!PlayerPositionValid(side, num))
    my_error("Can't do cone calc to player if we don;t know where he is");

  Vector center = PlayerAbsolutePosition(side, num) - vert;
  float cent_mod = center.mod();
  center *= (cent_mod + extra_len) / cent_mod;
  switch (which) {
  case 'm':
    return NumTeammatesInCone(wid_dist_ratio,
			      PlayerAbsolutePosition(side, num)+center, vert);
  case 't':
    return NumOpponentsInCone(wid_dist_ratio,
			      PlayerAbsolutePosition(side, num)+center, vert);
  case 'b':
    return NumPlayersInCone(wid_dist_ratio,
			    PlayerAbsolutePosition(side, num)+center, vert);
  default:
    my_error("Bad which to NumPlayersInConeToPlayer");
    return 0;
  }
  
}

/*********************************************************************************/

Unum PositionInfo::ClosestTeammateTo(Vector p, Bool include_me)
{
  Unum ClosestPlayer = Unum_Unknown;
  float dist2, ClosestDist2 = SP_pitch_length*2 * SP_pitch_length*2;
  for (int i=1; i<=SP_team_size; i++){
    if ( !include_me && i == MyNumber ) continue;
    if ( TeammatePositionValid(i) && (dist2=TeammateAbsolutePosition(i).dist2(p)) < ClosestDist2 ){
      ClosestDist2 = dist2;
      ClosestPlayer = i;
    }
  }
  return ClosestPlayer;
}

/*********************************************************************************/

Unum PositionInfo::ClosestOpponentTo(Vector p)
{
  Unum ClosestPlayer = Unum_Unknown;
  float dist2, ClosestDist2 = SP_pitch_length*2 * SP_pitch_length*2;
  for (int i=1; i<=SP_team_size; i++){
    if ( OpponentPositionValid(i) && (dist2=OpponentAbsolutePosition(i).dist2(p)) < ClosestDist2 ){
      ClosestDist2 = dist2;
      ClosestPlayer = i;
    }
  }
  return ClosestPlayer;
}

/*********************************************************************************/

Vector PositionInfo::ClosestTeamlessPlayerPosition()
{
  if ( num_teamless_players < 1 ) my_error("no teamless players");
  float tmp,closest_dist = HUGE;
  Vector position;
  for (int i=0; i<num_teamless_players; i++){
    if ( !TeamlessPlayer[i]->pos_valid() ) my_error("teammless player not valid");
    if ( (tmp=TeamlessPlayer[i]->get_dist()) < closest_dist ){
      position = TeamlessPlayer[i]->get_abs_pos();
      closest_dist = tmp;
    }
  }
  return position;
}

/*********************************************************************************/

Unum PositionInfo::ClosestTeammateToBall(Bool include_me)
{
  if ( !BallPositionValid() ) my_error("can't do closest if ball unknown");
  return ClosestTeammateTo(BallAbsolutePosition(),include_me);
}

/*********************************************************************************/

Unum PositionInfo::ClosestOpponentToBall()
{
  if ( !BallPositionValid() ) my_error("can't do closest if ball unknown");
  return ClosestOpponentTo(BallAbsolutePosition());
}

/*********************************************************************************/

float PositionInfo::ClosestTeammateToBallDistance(Bool include_me)
{
  Unum teammate = ClosestTeammateToBall(include_me);
  return BallAbsolutePosition().dist(TeammateAbsolutePosition(teammate));
} 

/*********************************************************************************/

float PositionInfo::PlayerDistanceTo(char s, Unum n, Vector p)
{
  if ( !PlayerPositionValid(s,n) ) my_error("can't get distance from invalid player");
  return PlayerAbsolutePosition(s,n).dist(p);
}

/*********************************************************************************/

float PositionInfo::PlayerDistance2To(char s, Unum n, Vector p)
{
  if ( !PlayerPositionValid(s,n) ) my_error("can't get distance from invalid player");
  return PlayerAbsolutePosition(s,n).dist2(p);
}

/*********************************************************************************/

float PositionInfo::PlayerDistanceToLine(char s, Unum n, Line l)
{
  if ( !PlayerPositionValid(s,n) ) my_error("can't get line distance from invalid player");
  return l.dist(PlayerAbsolutePosition(s,n));
}

/*********************************************************************************/

float PositionInfo::PlayerDistance2ToLine(char s, Unum n, Line l)
{
  if ( !PlayerPositionValid(s,n) ) my_error("can't get line distance from invalid player");
  return l.dist2(PlayerAbsolutePosition(s,n));
}

/*********************************************************************************/

Unum PositionInfo::FurthestBackTeammate(Bool IncludeUs)
{
  Unum first = Unum_Unknown;
  float first_x = HUGE;
  for (Unum num=1; num <= SP_team_size; num++) {
    if (!TeammatePositionValid(num))
      continue;
    if (!IncludeUs && num == MyNumber)
      continue;
    if (TeammateX(num) < first_x) {
      first = num;
      first_x = TeammateX(num);
    }
  }
  return first;
}

/*********************************************************************************/

Vector PositionInfo::PositionOfFurthestBackTeammate(Bool IncludeUs)
{
  Unum team = FurthestBackTeammate(IncludeUs);
  if (team == Unum_Unknown)
    return Vector(0,0); // no players found
  return TeammateAbsolutePosition(team);
}

/*********************************************************************************/

Unum PositionInfo::FurthestBackOpponent()
{
  Unum first = Unum_Unknown;
  float first_x = HUGE;
  for (Unum num=1; num <= SP_team_size; num++) {
    if (!OpponentPositionValid(num))
      continue;
    if (OpponentX(num) < first_x) {
      first = num;
      first_x = OpponentX(num);
    }
  }
  return first;  
}

/*********************************************************************************/

Vector PositionInfo::PositionOfFurthestBackPlayer(Bool IncludeUs)
{
  Unum team = FurthestBackTeammate(IncludeUs);
  Unum opp  = FurthestBackOpponent();
  if (team == Unum_Unknown && opp == Unum_Unknown)
    return Vector(0,0); // no players found
  if (team == Unum_Unknown)
    return OpponentAbsolutePosition(opp);
  if (opp == Unum_Unknown)
    return TeammateAbsolutePosition(team);
  if (TeammateX(team) < OpponentX(opp))
    return TeammateAbsolutePosition(team);
  else
    return OpponentAbsolutePosition(opp);
}
/*********************************************************************************/

Unum PositionInfo::FurthestForwardTeammate(Bool IncludeUs)
{
  Unum first = Unum_Unknown;
  float first_x = -SP_pitch_length;
  for (Unum num=1; num <= SP_team_size; num++) {
    if (!TeammatePositionValid(num))
      continue;
    if (!IncludeUs && num == MyNumber)
      continue;
    if (TeammateX(num) > first_x) {
      first = num;
      first_x = TeammateX(num);
    }
  }
  return first;
}

/*********************************************************************************/

Unum PositionInfo::FurthestForwardOpponent(Bool IncludeGoalie)
{
  Unum first = Unum_Unknown;
  Unum second = Unum_Unknown;
  float first_x = -SP_pitch_length;
  float second_x = -SP_pitch_length;
  for (Unum num=1; num <= SP_team_size; num++) {
    if (!OpponentPositionValid(num))
      continue;
    if (OpponentX(num) > first_x) {
      second = first;
      second_x = first_x;
      first = num;
      first_x = OpponentX(num);
    }
    else if ( OpponentX(num) > second_x) {
      second = num;
      second_x = OpponentX(num);
    }
  }

  if ( !IncludeGoalie && first != Unum_Unknown && 
       TheirPenaltyArea.IsWithin(OpponentAbsolutePosition(first)) )
    return second;

  return first;  
}

/*********************************************************************************/

Vector PositionInfo::PositionOfFurthestForwardPlayer(Bool IncludeUs)
{
  Unum team = FurthestForwardTeammate(IncludeUs);
  Unum opp  = FurthestForwardOpponent();
  if (team == Unum_Unknown && opp == Unum_Unknown)
    return Vector(0,0); // no players found
  if (team == Unum_Unknown)
    return OpponentAbsolutePosition(opp);
  if (opp == Unum_Unknown)
    return TeammateAbsolutePosition(team);
  if (TeammateX(team) > OpponentX(opp))
    return TeammateAbsolutePosition(team);
  else
    return OpponentAbsolutePosition(opp);
}

/*********************************************************************************/

float PositionInfo::AngleBetweenClosestTwoOpponents(Vector p)
{
  Unum play1 = ClosestOpponentTo(p);
  Unum play2;
  float dist2, ClosestDist2 = Sqr(SP_pitch_length*2);
  for (int i=1; i<=SP_team_size; i++){
    if (i == play1) continue;
    if ( OpponentPositionValid(i) &&
	 (dist2=OpponentAbsolutePosition(i).dist2(p)) < ClosestDist2 ){
      ClosestDist2 = dist2;
      play2 = i;
    }
  }
  return fabs(GetNormalizeAngleDeg(OpponentAngleFromBody(play1) - OpponentAngleFromBody(play2)));  
}

/*********************************************************************************/

Bool PositionInfo::InOwnPenaltyArea()
{
  if ( MyConf() && InOwnPenaltyArea(MyPos()) )
    return TRUE;
  else 
    return FALSE;
}

/*********************************************************************************/

Bool PositionInfo::BallInOwnPenaltyArea()
{
  if ( BallPositionValid() && InOwnPenaltyArea(BallAbsolutePosition()) )
    return TRUE;
  else 
    return FALSE;
}

/*********************************************************************************/

Bool PositionInfo::InOwnPenaltyArea(Vector p)
{
  if ( p.x < MarkerX(RM_My_PC_Flag) && p.x > MarkerX(RM_My_Goal) && 
       fabs(p.y) < SP_penalty_area_width/2.0 )
    return TRUE;
  else 
    return FALSE;
}

/*********************************************************************************/

Bool PositionInfo::FacingBackInOwnPA()
{
  /* neck facing back => can't see what's coming */
  if ( InOwnPenaltyArea() && fabs(MyNeckGlobalAng()) > 90 )
    return TRUE;
  else 
    return FALSE;
}

/*********************************************************************************/

Bool PositionInfo::FacingBackNearOwnGoal()
{
  /* neck facing back => can't see what's coming */
  if ( MyConf() && MarkerDistance(RM_My_Goal) < 25 && fabs(MyNeckGlobalAng()) > 90 )
    return TRUE;
  else 
    return FALSE;
}

/*********************************************************************************/

Bool PositionInfo::IsPointInBounds(float x, float y, float buffer)
{
  return (x > -SP_pitch_length/2 + buffer && x < SP_pitch_length/2 - buffer &&
	  y > -SP_pitch_width /2 + buffer && y < SP_pitch_width /2 - buffer ) ? TRUE : FALSE;
}

/*********************************************************************************/

Rectangle PositionInfo::ShiftRectangleInBounds(Rectangle *rect)
{
  float w = rect->Width();
  float h = rect->Height();

  Vector new_center = rect->Center();
  
  if ( w > SP_pitch_length ) my_error("rectangle too wide for field");
  if ( h > SP_pitch_width ) my_error("rectangle too high for field");

  if ( rect->RightX() > SP_pitch_length/2 )
    new_center.x = SP_pitch_length/2 - w/2;
  if ( rect->LeftX() < -SP_pitch_length/2 )
    new_center.x = -SP_pitch_length/2 + w/2;
  if ( rect->BottomY() > SP_pitch_width/2 )
    new_center.y = SP_pitch_width/2 - h/2;
  if ( rect->TopY() < -SP_pitch_width/2 )
    new_center.y = -SP_pitch_width/2 + h/2;

  return Rectangle(new_center,Vector(w,h));
}

/*********************************************************************************/

Vector PositionInfo::PositionToKickoffPosition( const Vector pos ){

  Vector ko_pos = pos;

  if (ko_pos.x > 0 && fabs(ko_pos.y)<9){ 
    ko_pos.x = -1;                    /* Don't put the center forward behind the circle */
    ko_pos.y = 10;
  }

  if ( fabs(ko_pos.x) <= 10 && ko_pos.y == 0){      
    if ( KickOffMode == KO_Theirs ){
      ko_pos.x = -10;
      ko_pos.y = 0;
    }
    else{
      /* Put the center midfielder right at the ball */
      ko_pos.x = -CP_hardest_kick_ball_dist * Sin(45);
      //ko_pos.y = int_random(2) ? -(My_kickable_area() - 1) : My_kickable_area()-1;
      ko_pos.y = -CP_hardest_kick_ball_dist * Cos(45);
    }
  }
  else if (ko_pos.x > -1) {
    if ( ko_pos.y > 0 )                              /* Stagger the players     */
      ko_pos.y += ko_pos.x/5;
    else 
      ko_pos.y -= ko_pos.x/5;
    ko_pos.x = -3;                                   /* Always start on my side */
  }

  if ( KickOffMode == KO_Theirs ){ /* Stay out of the center circle */ 
    if ( sqrt(ko_pos.x * ko_pos.x + ko_pos.y * ko_pos.y) < SP_free_kick_buffer ) 
      ko_pos.x = -10; 
  }

  if (ko_pos.x > SP_pitch_length/2 ) ko_pos.x = SP_pitch_length/2;
  if (ko_pos.x < -SP_pitch_length/2 ) ko_pos.x = -SP_pitch_length/2;
  if (ko_pos.y > SP_pitch_width/2 ) ko_pos.y = SP_pitch_width/2;
  if (ko_pos.y < -SP_pitch_width/2 ) ko_pos.y = -SP_pitch_width/2;

  return ko_pos;
}

/*********************************************************************************/

/* consider_me : should I take myself into consideration when computing congestion */
float PositionInfo::Congestion(Vector pos, Bool consider_me) 
{
  float congestion = 0;
  if ( consider_me == TRUE && pos != MyPos() ) congestion = 1/MyPos().dist2(pos);
  Vector player_pos;
  for (int i=0; i<num_players; i++)
    /* Don't want to count a player in its own congestion measure */
    if ( Player[i].pos_valid() && (player_pos = Player[i].get_abs_pos()) != pos )
      congestion += 1/player_pos.dist2(pos);
  
  return congestion;
}

/*********************************************************************************/

float PositionInfo::TeammateCongestion(Unum teammate, Bool consider_me)
{
  if ( !TeammatePositionValid(teammate) ) my_error("unknown teammate congestion");
  return Congestion(TeammateAbsolutePosition(teammate),consider_me);
}

/*********************************************************************************/

Unum PositionInfo::LeastCongestedTeammate()
{
  float least_congestion = Congestion(MyPos());
  Unum  least_congested  = MyNumber;
  Unum  num;
  float temp;

  for (int i=0; i<num_my_players; i++){
    num = MyPlayer[i]->unum;
    if ( (temp=Congestion(MyPlayer[i]->get_abs_pos(),TRUE)) < least_congestion ){
      least_congested  = num;
      least_congestion = temp;
    }
  }
  return least_congested;
}

/*********************************************************************************/

Vector PositionInfo::LeastCongestedValidPointInRectangle(Rectangle *rect, Bool attract, Vector attract_point)
{
  int x_granularity = 5;
  int y_granularity = 5;
  
  float x_mesh = rect->Width()/(x_granularity+1);
  float y_mesh = rect->Height()/(y_granularity+1);
  
  float start_x = rect->LeftX() + x_mesh/2;
  float start_y = rect->TopY()  + y_mesh/2;

  float x = start_x, y = start_y;

  float best_congestion = 1000;
  Vector best_point, point;
  float tmp;

  for ( int i=0; i<x_granularity; i++ ){
    for ( int j=0; j<y_granularity; j++ ){
      tmp = Congestion(point=Vector(x,y));
      if ( attract ) tmp-=1/point.dist(attract_point);
      if ( tmp < best_congestion &&
	   !OffsidePosition(point,MySide) && 
	   IsPointInBounds(point,5) ){
	best_congestion = tmp;
	best_point = point;
      }
      y+=y_mesh;
    }
    x+=x_mesh;
    y =start_y;
  }

  if ( best_congestion == 1000 ){
    //my_error("No valid point in rectangle -- taking center %.1f %.1f",rect->LeftX(),rect->RightX());
    /* take the point out of the rectangle -- meaning no point was valid */ 
    best_point = rect->Center() + Vector(rect->Width(),0); 
  }

  return best_point;
}

/*********************************************************************************/

Vector PositionInfo::LeastCongestedValidPointForPassFromInRectangle(Rectangle *rect, Vector from, Bool attract, Vector attract_point)
{
  int x_granularity = 5;
  int y_granularity = 5;
  
  float x_mesh = rect->Width()/(x_granularity+1);
  float y_mesh = rect->Height()/(y_granularity+1);
  
  float start_x = rect->LeftX() + x_mesh/2;
  float start_y = rect->TopY()  + y_mesh/2;

  float x = start_x, y = start_y;

  float best_congestion = 1000;
  Vector best_point, point;
  float tmp;

  for ( int i=0; i<x_granularity; i++ ){
    for ( int j=0; j<y_granularity; j++ ){
      tmp = Congestion(point=Vector(x,y));
      if ( attract ) tmp-=1/point.dist(attract_point);
      
      if ( tmp < best_congestion &&
	   !OffsidePosition(point,MySide) &&
	   !NumOpponentsInCone(.6,point,from) &&  // was '&' in Paris
	    IsPointInBounds(point,5) ){
	best_congestion = tmp;
	best_point = point;
      }
      y+=y_mesh;
    }
    x+=x_mesh;
    y =start_y;
  }

  if ( best_congestion == 1000 ){
    //my_error("No valid point in rectangle -- taking center %.1f %.1f",rect->TopY(),rect->BottomY());
    /* take the point out of the rectangle -- meaning no point was valid */ 
    best_point = rect->Center() + Vector(rect->Width(),0); 
  }

  return best_point;
}

/*********************************************************************************/

void PositionInfo::SetTeammateStamina(Unum num, float stamina, Time time) 
{
  if (TiredTimes[num] >= time) return; 
  TeammateStamina[num] = stamina;
  TiredTimes[num] = time; 
}

/*********************************************************************************/

Bool PositionInfo::TeammateTired(Unum num) 
{ 
  if ( CurrentTime.t == 0 )
    return FALSE;
  
  if ( num == MyNumber ) 
    return Tired();
  
  if ( GetTeammateStamina(num) > SayTiredThreshold )
    return FALSE;

  return (CurrentTime - CP_say_tired_interval > SecondLastStartClockTime.t &&
	  CurrentTime - CP_say_tired_interval < TiredTimes[num]) ? TRUE : FALSE; 
}

/*********************************************************************************/

float PositionInfo::GetTeammateStamina(Unum num)
{ 
  if ( num == MyNumber ) 
    return MyStamina();
  else 
    return TeammateStamina[num];
}

/*********************************************************************************/

Time PositionInfo::TeammateStaminaTime(Unum num)
{ 
  if ( num == MyNumber ) 
    return CurrentTime;
  else 
    return TiredTimes[num];
}

/*********************************************************************************/

Bool PositionInfo::NeedToAnnounceTired()
{ 
  if (!Tired()) my_error("I'm not tired"); 
  return (CurrentTime.t - CP_say_tired_interval > SecondLastStartClockTime.t &&
	  CurrentTime - CP_say_tired_interval < TiredTimes[MyNumber]) ? FALSE : TRUE; 
}

/*********************************************************************************/

int   PositionInfo::GetPlayerPT(char s, int n) 
{
  if ( s==MySide ) {
    if ( n==MyNumber )
      return MyPT();
    else
      return TeammatePlayerType[n];
  }
  else if (s==TheirSide )
    return OpponentPlayerType[n];
  else 
    return 0;  /* side unknown */
}

/*********************************************************************************/

void  PositionInfo::SetPlayerPT(char s, int n, int pt)
{
  if ( s==MySide ){
    if ( n==MyNumber )
      SetMyPT(pt);
    else
      TeammatePlayerType[n] = pt;
  }
  else if ( s==TheirSide )
    OpponentPlayerType[n] = pt;
  else
    my_error("Need to know side to set a player type");
}
