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

{
  float tmpf;
  int tmpi;
  
  if ( (tmpi=get_int(&Info)) != Mem->PP_player_types ) 
    my_error("different %d %d",tmpi,Mem->PP_player_types);  
  if ( (tmpi=get_int(&Info)) != Mem->PP_subs_max ) 
    my_error("different %d %d",tmpi,Mem->PP_subs_max);  
  if ( (tmpi=get_int(&Info)) != Mem->PP_pt_max ) 
    my_error("different %d %d",tmpi,Mem->PP_pt_max);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_player_speed_max_delta_min ) 
    my_error("different %d %d",tmpf,Mem->PP_player_speed_max_delta_min);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_player_speed_max_delta_max ) 
    my_error("different %d %d",tmpf,Mem->PP_player_speed_max_delta_max);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_stamina_inc_max_delta_factor ) 
    my_error("different %d %d",tmpf,Mem->PP_stamina_inc_max_delta_factor);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_player_decay_delta_min ) 
    my_error("different %d %d",tmpf,Mem->PP_player_decay_delta_min);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_player_decay_delta_max ) 
    my_error("different %d %d",tmpf,Mem->PP_player_decay_delta_max);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_inertia_moment_delta_factor ) 
    my_error("different %d %d",tmpf,Mem->PP_inertia_moment_delta_factor);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_dash_power_rate_delta_min ) 
    my_error("different %d %d",tmpf,Mem->PP_dash_power_rate_delta_min);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_dash_power_rate_delta_max ) 
    my_error("different %d %d",tmpf,Mem->PP_dash_power_rate_delta_max);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_player_size_delta_factor ) 
    my_error("different %d %d",tmpf,Mem->PP_player_size_delta_factor);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_kickable_margin_delta_min ) 
    my_error("different %d %d",tmpf,Mem->PP_kickable_margin_delta_min);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_kickable_margin_delta_max ) 
    my_error("different %d %d",tmpf,Mem->PP_kickable_margin_delta_max);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_kick_rand_delta_factor ) 
    my_error("different %d %d",tmpf,Mem->PP_kick_rand_delta_factor);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_extra_stamina_delta_min ) 
    my_error("different %d %d",tmpf,Mem->PP_extra_stamina_delta_min);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_extra_stamina_delta_max ) 
    my_error("different %d %d",tmpf,Mem->PP_extra_stamina_delta_max);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_effort_max_delta_factor ) 
    my_error("different %d %d",tmpf,Mem->PP_effort_max_delta_factor);  
  if ( (tmpf=get_float(&Info)) != Mem->PP_effort_min_delta_factor ) 
    my_error("different %d %d",tmpf,Mem->PP_effort_min_delta_factor);  

//        (player_param <player_types> <subs_max> <pt_max>
//         <player_speed_max_delta_min> <player_speed_max_delta_max>
//         <stamina_inc_max_delta_factor> <player_decay_delta_min>
//         <player_decay_delta_max> <inertia_moment_delta_factor>
//         <dash_power_rate_delta_min> <dash_power_rate_delta_max> 
//         <player_size_delta_factor> <kickable_margin_delta_min>
//         <kickable_margin_delta_max> <kick_rand_delta_factor>
//         <extra_stamina_delta_min> <extra_stamina_delta_max>
//         <effort_max_delta_factor> <effort_min_delta_factor>)
}
