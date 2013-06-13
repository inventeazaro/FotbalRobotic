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
