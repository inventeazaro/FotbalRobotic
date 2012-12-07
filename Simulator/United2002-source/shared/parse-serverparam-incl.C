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

/****************************************************************************************/


{
  float tmpf;
  int tmpi;

  if ( (tmpf=get_float(&Info)) != Mem->SP_goal_width ) my_error("different goal_width");
  if ( (tmpf=get_float(&Info)) != Mem->SP_inertia_moment ) my_error("different inertia_moment");
  if ( (tmpf=get_float(&Info)) != Mem->SP_player_size ) my_error("different player_size");
  if ( (tmpf=get_float(&Info)) != Mem->SP_player_decay ) my_error("different player_decay");
  if ( (tmpf=get_float(&Info)) != Mem->SP_player_rand ) my_error("different player_rand");
  if ( (tmpf=get_float(&Info)) != Mem->SP_player_weight ) my_error("different player_weight");
  if ( (tmpf=get_float(&Info)) != Mem->SP_player_speed_max ) my_error("different player_speed_max");
  if ( (tmpf=get_float(&Info)) != Mem->SP_player_accel_max ) my_error("different player_accel_max");
  if ( (tmpf=get_float(&Info)) != Mem->SP_stamina_max ) my_error("different stamina_max");
  if ( (tmpf=get_float(&Info)) != Mem->SP_stamina_inc ) my_error("different stamina_inc");
  if ( (tmpf=get_float(&Info)) != Mem->SP_recover_init ) my_error("different recover_init");
  if ( (tmpf=get_float(&Info)) != Mem->SP_recover_dec_thr ) my_error("different recover_dec_thr");
  if ( (tmpf=get_float(&Info)) != Mem->SP_recover_min ) my_error("different recover_min");
  if ( (tmpf=get_float(&Info)) != Mem->SP_recover_dec ) my_error("different recover_dec");
  if ( (tmpf=get_float(&Info)) != Mem->SP_effort_init ) my_error("different effort_init");
  if ( (tmpf=get_float(&Info)) != Mem->SP_effort_dec_thr ) my_error("different effort_dec_thr");
  if ( (tmpf=get_float(&Info)) != Mem->SP_effort_min ) my_error("different effort_min");
  if ( (tmpf=get_float(&Info)) != Mem->SP_effort_dec ) my_error("different effort_dec");
  if ( (tmpf=get_float(&Info)) != Mem->SP_effort_inc_thr ) my_error("different effort_inc_thr, %f",Mem->SP_effort_inc_thr);
  if ( (tmpf=get_float(&Info)) != Mem->SP_effort_inc ) my_error("different effort_inc");
  if ( (tmpf=get_float(&Info)) != Mem->SP_kick_rand ) my_error("different Mem->SP_kick_rand");
  if ( (tmpf=get_int(&Info)) != Mem->SP_team_actuator_noise ) my_error("different team_actuator_noise"); 
  if ( (tmpf=get_float(&Info)) != Mem->SP_prand_factor_l ) my_error("different prand_factor_l %f %f",tmpf,Mem->SP_prand_factor_l);
  if ( (tmpf=get_float(&Info)) != Mem->SP_prand_factor_r ) my_error("different prand_factor_r");
  if ( (tmpf=get_float(&Info)) != Mem->SP_kick_rand_factor_l ) my_error("different kick_rand_factor_l");
  if ( (tmpf=get_float(&Info)) != Mem->SP_kick_rand_factor_l ) my_error("different kick_rand_factor_l");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ball_size ) my_error("different ball_size");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ball_decay ) my_error("different ball_decay");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ball_rand ) my_error("different ball_rand");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ball_weight ) my_error("different ball_weight");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ball_speed_max ) my_error("different ball_speed_max");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ball_accel_max ) my_error("different ball_accel_max");
  if ( (tmpf=get_float(&Info)) != Mem->SP_dash_power_rate ) my_error("different dash_power_rate");
  if ( fabs((tmpf=get_float(&Info)) - Mem->SP_kick_power_rate) > FLOAT_EPS ) my_error("different kick_power_rate %f %f %f %f",tmpf,Mem->SP_kick_power_rate,tmpf-Mem->SP_kick_power_rate,FLOAT_EPS);
  if ( (tmpf=get_float(&Info)) != Mem->SP_kickable_margin ) my_error("different kickable_margin");
  if ( (tmpf=get_float(&Info)) != Mem->SP_control_radius ) my_error("different control_radius");
  (tmpf=get_float(&Info)); //<ctlradius_width>
  if ( (tmpf=get_float(&Info)) != Mem->SP_max_power ) my_error("different max_power");
  if ( (tmpf=get_float(&Info)) != Mem->SP_min_power ) my_error("different min_power");
  if ( (tmpf=get_float(&Info)) != Mem->SP_max_moment ) my_error("different max_moment");
  if ( (tmpf=get_float(&Info)) != Mem->SP_min_moment ) my_error("different min_moment");
  if ( (tmpf=get_float(&Info)) != Mem->SP_max_neck_moment ) my_error("different max_neck_moment");
  if ( (tmpf=get_float(&Info)) != Mem->SP_min_neck_moment ) my_error("different min_neck_moment");
  if ( (tmpf=get_float(&Info)) != Mem->SP_max_neck_angle ) my_error("different max_neck_angle");
  if ( (tmpf=get_float(&Info)) != Mem->SP_min_neck_angle ) my_error("different min_neck_angle");
  if ( (tmpf=get_float(&Info)) != Mem->SP_visible_angle ) my_error("different visible_angle %f",Mem->SP_visible_angle);
  if ( (tmpf=get_float(&Info)) != Mem->SP_feel_distance ) my_error("different feel_distance %f %f",tmpf,Mem->SP_feel_distance);
  if ( (tmpf=get_float(&Info)) != Mem->SP_wind_dir ) my_error("different wind_dir");
  if ( (tmpf=get_float(&Info)) != Mem->SP_wind_force ) my_error("different wind_force");
  (tmpf=get_float(&Info)); //<winang>
  if ( (tmpf=get_float(&Info)) != Mem->SP_wind_rand ) my_error("different wind_rand");
  if ( (tmpf=get_float(&Info)) != Mem->SP_kickable_area ) my_error("different kickable_area");
  if ( (tmpf=get_float(&Info)) != Mem->SP_catch_area_l ) my_error("different catch_area_l");
  if ( (tmpf=get_float(&Info)) != Mem->SP_catch_area_w ) my_error("different catch_area_w");
  if ( (tmpf=get_float(&Info)) != Mem->SP_catch_prob ) my_error("different catch_prob");
  if ( (tmpi=get_int(&Info)) != Mem->SP_goalie_max_moves ) my_error("different goalie_max_moves");
  if ( (tmpf=get_float(&Info)) != Mem->SP_ckmargin ) my_error("different ckmargin");
  if ( (tmpf=get_float(&Info)) != Mem->SP_offside_area ) my_error("different offside_area");
  if ( (tmpi=get_int(&Info)) != Mem->SP_wind_none ) my_error("different wind_none");
  if ( (tmpi=get_int(&Info)) != Mem->SP_wind_random ) my_error("different wind_random");
  if ( (tmpf=get_float(&Info)) != Mem->SP_say_coach_cnt_max ) my_error("different say_coach_cnt_max");
  if ( (tmpf=get_float(&Info)) != Mem->SP_say_coach_msg_size ) my_error("different say_coach_msg_size");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_win_size ) my_error("different clang_win_size");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_define_win ) my_error("different clang_define_win");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_meta_win ) my_error("different clang_meta_win");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_advice_win ) my_error("different clang_advice_win");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_info_win ) my_error("different clang_info_win");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_mess_delay ) my_error("different clang_mess_delay");
  if ( (tmpf=get_float(&Info)) != Mem->SP_clang_mess_per_cycle ) my_error("different clang_mess_per_cycle");
  if ( (tmpi=get_int(&Info)) != Mem->SP_half_time ) my_error("different half_time %d %d",tmpi,Mem->SP_half_time);
  if ( (tmpi=get_int(&Info)) != Mem->SP_simulator_step ) my_error("different simulator_step");
  if ( (tmpi=get_int(&Info)) != Mem->SP_send_step ) my_error("different send_step");
  if ( (tmpi=get_int(&Info)) != Mem->SP_recv_step ) my_error("different recv_step");
  if ( (tmpf=get_int(&Info)) != Mem->SP_sense_body_step ) my_error("different sense_body_step");
  (tmpf=get_int(&Info)); //<lcm_step>  
  if ( (tmpi=get_int(&Info)) != Mem->SP_say_msg_size ) my_error("different say_msg_size");
  if ( (tmpi=get_int(&Info)) != Mem->SP_hear_max ) my_error("different hear_max %d %d",tmpi,Mem->SP_hear_max);
  if ( (tmpi=get_int(&Info)) != Mem->SP_hear_inc ) my_error("different hear_inc");
  if ( (tmpi=get_int(&Info)) != Mem->SP_hear_decay ) my_error("different hear_decay");
  if ( (tmpi=get_int(&Info)) != Mem->SP_catch_ban_cycle ) my_error("different catch_ban_cycle");
  if ( (tmpi=get_int(&Info)) != Mem->SP_slow_down_factor ) my_error("different slow_down_factor");
  if ( (tmpi=get_int(&Info)) != Mem->SP_use_offside ) my_error("different use_offside");
  if ( (tmpi=get_int(&Info)) != Mem->SP_forbid_kickoff_offside ) my_error("different forbid_kickoff_offside");
  if ( fabs((tmpf=get_float(&Info)) - Mem->SP_offside_kick_margin) > .000001 ) my_error("different offside_kick_margin %f %f",tmpf,Mem->SP_offside_kick_margin);
  if ( (tmpf=get_float(&Info)) != Mem->SP_audio_cut_dist ) my_error("different audio_cut_dist");
  if ( (tmpf=get_float(&Info)) != Mem->SP_dist_qstep ) my_error("different dist_qstep");
  if ( (tmpf=get_float(&Info)) != Mem->SP_land_qstep ) my_error("different land_qstep");
  if ( (tmpf=get_float(&Info)) != Mem->SP_dir_qstep ) my_error("different dir_qstep %f %f",tmpf,Mem->SP_dir_qstep);
  if ( (tmpf=get_float(&Info)) != Mem->SP_dist_qstep_l ) my_error("different dist_qstep_l");
  if ( (tmpf=get_float(&Info)) != Mem->SP_dist_qstep_r ) my_error("different dist_qstep_r");
  if ( (tmpf=get_float(&Info)) != Mem->SP_land_qstep_l ) my_error("different land_qstep_l");
  if ( (tmpf=get_float(&Info)) != Mem->SP_land_qstep_r ) my_error("different land_qstep_r");
  if ( (tmpf=get_float(&Info)) != Mem->SP_dir_qstep_l ) my_error("different dir_qstep_l");
  if ( (tmpf=get_float(&Info)) != Mem->SP_dir_qstep_r ) my_error("different dir_qstep_r");
  if ( (tmpi=get_int(&Info)) != Mem->SP_coach_mode ) my_error("different coach_mode");
  if ( (tmpi=get_int(&Info)) != Mem->SP_coach_w_referee_mode ) my_error("different coach_w_referee_mode %d %d",tmpi,Mem->SP_coach_w_referee_mode);
  if ( (tmpi=get_int(&Info)) != Mem->SP_old_coach_hear ) my_error("different old_coach_hear");
  if ( (tmpi=get_int(&Info)) != Mem->SP_send_vi_step ) my_error("different send_vi_step %d %d",tmpi,Mem->SP_send_vi_step);
  if ( (tmpi=get_int(&Info)) != Mem->SP_start_goal_l ) my_error("different start_goal_l");
  if ( (tmpi=get_int(&Info)) != Mem->SP_start_goal_r ) my_error("different start_goal_r");
  if ( (tmpi=get_int(&Info)) != Mem->SP_fullstate_l ) my_error("different fullstate_l");
  if ( (tmpi=get_int(&Info)) != Mem->SP_fullstate_r ) my_error("different fullstate_r %d %d",tmpi, Mem->SP_fullstate_r);
  if ( (tmpi=get_int(&Info)) != Mem->SP_drop_ball_time ) my_error("different drop_ball_time %d %d",tmpi, Mem->SP_drop_ball_time);


//        (server_param <gwidth> <inertia_moment> <psize> <pdecay> <prand> <pweight>
//         <pspeed_max> <paccel_max> <stamina_max> <stamina_inc> <recover_init>
//         <recover_dthr> <recover_min> <recover_dec> <effort_init> <effort_dthr>
//         <effort_min> <effort_dec> <effort_ithr> <effort_inc> <kick_rand>
//         <team_actuator_noise> <prand_factor_l> <prand_factor_r><kick_rand_factor_l>
//         <kick_rand_factor_r> <bsize> <bdecay> <brand> <bweight> <bspeed_max>
//         <baccel_max> <dprate> <kprate> <kmargin> <ctlradius> <ctlradius_width>
//         <maxp> <minp> <maxm> <minm> <maxnm> <minnm> <maxn> <minn> <visangle>
//         <visdist> <windir> <winforce> <winang> <winrand> <kickable_area>
//         <catch_area_l> <catch_area_w> <catch_prob> <goalie_max_moves> <ckmargin>
//         <offside_area> <win_no> <win_random> <say_cnt_max> <SayCoachMsgSize>
//         <clang_win_size> <clang_define_win> <clang_meta_win>
//         <clang_advice_win> <clang_info_win> <clang_mess_delay> <clang_mess_per_cycle>
//         <half_time> <sim_st> <send_st> <recv_st> <sb_step> <lcm_st> <SayMsgSize>
//         <hear_max> <hear_inc> <hear_decay> <cban_cycle> <slow_down_factor>
//         <useoffside> <kickoffoffside> <offside_kick_margin> <audio_dist> <dist_qstep>
//         <land_qstep> <dir_qstep> <dist_qstep_l> <dist_qstep_r> <land_qstep_l>
//         <land_qstep_r> <dir_qstep_l> <dir_qstep_r> <CoachMode> <CwRMode> <old_hear>
//         <sv_st> <start_score_l> <start_score_r> <fullstate_l> <fullstate_r> 
//         <drop_time>)
}

