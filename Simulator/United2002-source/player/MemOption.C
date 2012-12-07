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


#include "netif.h"
#include "utils.h"
#include "MemOption.h"

/* setting defaults to match version 4.06 server.conf */
OptionInfo::OptionInfo(){

  VP_test_l   = FALSE;
  VP_test_r   = FALSE;
  VP_test     = FALSE;
  VP_train_DT = FALSE;
  VP_use_DT   = FALSE;
  VP_keepaway = FALSE;

  CP_keepaway_mode = 0; //used to test breakaway types
  TP_keepaway_rect_length = 30;
  TP_keepaway_rect_width = 30;

  IP_my_score             = 0;
  IP_their_score          = 0;
  IP_reconnect            = 0;
  IP_only_connect         = FALSE;

  sprintf(MyTeamName,     "myteam");

#include<opt-def.C>
  
  CP_goalie               = FALSE;
  CP_save_log             = FALSE;
  CP_save_freq            = 10;
  CP_save_sound_log       = FALSE;
  CP_save_sound_freq      = 10;
  CP_send_ban_recv_step_factor = 3.0;
  CP_interrupts_per_cycle = 2;
  CP_interrupts_left_to_act = 2;
  CP_wait_for_sight       = FALSE;
  CP_max_conf             = 1.0;
  CP_min_valid_conf       = 0.5;
  CP_conf_decay           = 0.98;
  CP_player_conf_decay    = 0.99;
  CP_ball_conf_decay      = 0.9;
  CP_listen_conf_buffer   = .008;
  CP_max_player_move_factor = 4;
  CP_max_say_interval     = 100;
  CP_ball_moving_threshold = .2; /* experimentally checked -- ball still, player fast => .15 ball speed */
  CP_dodge_angle_buffer    = 25;
  CP_dodge_distance_buffer = 3.5;
  CP_dodge_power           = 30;
  CP_dodge_angle           = 90;
  sprintf(CP_tree_stem,      "pass");
  CP_DT_evaluate_interval  = 10;
  CP_say_tired_interval    = 20;
  CP_tired_buffer          = 10;
  CP_say_tired_buffer      = 10;
  CP_use_new_set_plays     = FALSE;  
  CP_set_plays             = FALSE;
  CP_Setplay_Delay         = 5;
  CP_Setplay_Say_Delay     = SP_hear_decay*5;
  CP_Setplay_Max_Delay     = 100;
  CP_Setplay_Time_Limit    = 150;

  CP_kickable_buffer       = .1;
  CP_mark_persist_time     = 100;
  CP_mark_max_dist_from_offside_line = 20;
  CP_max_midfield_markers  = 11;
  CP_max_midfield_markers_when_tied  = 11;
  CP_max_midfield_markers_when_winning  = 11;
  CP_max_midfield_markers_when_losing  = 11;
  CP_max_midfield_markers_when_losing_by_lots  = 11;
  CP_midfielder_leave_mark_min_x = -25;

  CP_track_min_distance    = 0;
  CP_track_max_distance    = 15;
  CP_pull_offsides         = FALSE;
  CP_pull_offsides_when_winning = TRUE;
  CP_spar                  = TRUE;
  CP_mark                  = TRUE;
  CP_communicate           = TRUE;
  CP_change_view_for_ball_cycles = 2;
  CP_defer_kick_to_teammate_buffer = .05;
  CP_scan_overlap_angle    = 2;

  CP_pull_offside_threshold = 5;
  CP_pull_offside_buffer = 3;

  CP_ball_forget_angle_buf = 3;
  CP_player_forget_angle_buf = 5;
  CP_ball_forget_dist_buf = 1;
  CP_player_forget_dist_buf = 1;

  CP_beat_offsides_buffer = 20;
  CP_beat_offsides_threshold = 30;
  CP_beat_offsides_max_x = 25;
  CP_congestion_epsilon = .01;
  CP_back_pass_opponent_buffer = 10;
  CP_back_pass_offside_buffer = 10;
  CP_min_less_congested_pass_dist = 7;

  CP_no_dangerous_pass_or_dribble_x_thresh = -10;
  CP_min_pass_dist = 3;
  CP_min_dangerous_pass_dist = 5;
  
  /* pat added these */
  CP_use_new_position_based_vel = TRUE;
  CP_stop_on_error = TRUE;
  
  CP_opt_ctrl_dist        = 1.6;//(SP_player_size + .8 *SP_kickable_margin);
  CP_KickTo_err           = 3;
  CP_closest_margin       = .97;//(SP_player_size + 2.0*SP_ball_size);
  CP_dokick_factor        = .22;
  CP_max_turn_kick_pow    = 20;

  CP_max_ignore_vel       = .005;
  CP_kick_time_space      = 1;
  CP_max_est_err          = .3;
  CP_max_go_to_point_angle_err = 5;
  CP_holdball_kickable_buffer = .1;
  CP_stop_ball_power          = 30;
  CP_possessor_intercept_space = 4;
  CP_can_keep_ball_cycle_buffer = 0;

  /* no longer used
  CP_hard_kick_margin     = .97;//(SP_player_size + 2.0*SP_ball_size);
  CP_hard_kick_factor     = .25;
  CP_hard_kick_end_turn_dist = 1.1;//(SP_player_size + .3 *SP_kickable_margin);
  */
  CP_hard_kick_dist_buffer = .1; 
  CP_max_hard_kick_angle_err  = 5;

  CP_hardest_kick_player_ang = 90; //angle relative to direction of ball
  CP_hardest_kick_ball_dist = .831; //kickable_area * .6 
  CP_hardest_kick_ball_ang = 15; // this is realtive to the direction of travel 
  CP_max_dash_help_kick_angle = 60;

  CP_max_int_lookahead    = 20;
  CP_intercept_step       = 2;
  CP_my_intercept_step    = 2;
  CP_intercept_aim_ahead  = 1;
  CP_no_turn_max_cyc_diff = -1;
  CP_no_turn_max_dist_diff = 1.0;
  CP_turnball_opp_worry_dist = 5; 
  CP_collision_buffer = .1;
  CP_behind_angle  = 80;
  CP_time_for_full_rotation = 24; /* average guestimate */
  CP_ball_vel_invalidation_factor = 2.0;

  CP_dribble_dash_pow = 75;
  CP_dribble_ball_dist    = 1.2;
  CP_dribble_behind_angle = 90;
  CP_dribble_angle_norm = 60;
  CP_dribble_exp_angle_buffer = 10;

  CP_dribble_ignore_opp_dist = 15;
  CP_dribble_worry_opp_dist = 4;
  CP_dribble_dodge_max_dist = 6;
  CP_dribble_dodge_angle_err = 15;
  CP_dribble_angle_ignore_buffer = 5;
  CP_dribble_dodge_close_dist = 2;
  CP_dribble_scan_field = TRUE; 
  CP_can_dribble_cone_ratio = .75;

  CP_dribble_towards_length = 10;
  CP_dribble_sideline_buffer = 1.5;
  CP_dribble_circle_inner_rad = 3;
  CP_dribble_circle_outer_rad = 4;
  CP_dribble_circle_ang = 90;

  CP_dribble_fast_max_dist = 5;

  CP_move_imp_1v1_initial =  0.0;
  CP_move_imp_1v1_inc =       .2;
  CP_move_imp_1v1_threshold = 1.0 ;
  CP_at_point_buffer = 1;
  CP_overrun_dist = 3;
  CP_def_block_dist = 2;
  CP_def_block_dist_ratio = .5;
  CP_overrun_buffer = 2.5;

  CP_breakaway_buffer = 3;
  CP_our_breakaway_kickable_buffer = 1.5;
  CP_their_breakaway_front_kickable_buffer = 5.0;
  CP_their_breakaway_back_kickable_buffer = 2.0;

  CP_breakaway_approach_x = 35;
  CP_breakaway_approach_y = 8;
  CP_breakaway_targ_valid_time = 3;
  CP_breakaway_min_goalie_steal_time = 6;
  CP_breakaway_kick_run_min_cycles = 7;
  CP_breakaway_kick_run_max_cycles = 14;
  CP_our_breakaway_min_cone_dist_wid = 18;
  CP_their_breakaway_min_cone_dist_wid = 12;
  CP_breakaway_middle_buffer = 3;
  CP_breakaway_kick_run_worry_dist = 10;
  CP_breakaway_mode = 0;
  
  CP_static_kick_dist_err = .3;//old: .14
  CP_static_kick_ang_err = 15;//old: 5
  //no longer used
  //CP_static_kick_dist = .985;
  //CP_static_kick_ang = 47;  /* caculated value! */
    //CP_static_kick_ang = 42;  /* caculated value!- extar buffer */
    //  CP_static_kick_overrun_dist = 4;

  CP_goalie_baseline_buffer = 1;
  CP_goalie_scan_angle_err = 10;
  CP_goalie_at_point_buffer = .1;
  CP_goalie_vis_angle_err = 5;
  CP_goalie_max_shot_distance = 40;
  CP_goalie_min_pos_dist = 15;
  CP_goalie_max_pos_dist = SP_pitch_length * .75;
  CP_goalie_max_forward_percent = .75;
  CP_goalie_ball_ang_for_corner = 90;
  CP_goalie_max_come_out_dist = 10;
  CP_goalie_ball_dist_for_corner = SP_penalty_area_length;
  CP_goalie_ball_dist_for_center = SP_pitch_length / 2;
  CP_goalie_free_kick_dist = 3;
  CP_goalie_go_to_ball_cone_ratio = .25;
  CP_goalie_warn_space = 10;
  CP_goalie_comes_out = TRUE;
  CP_goalie_catch_wait_time = 2;
  CP_goalie_opponent_dist_to_block = 15;
  CP_goalie_always_block_dist = 11;
  CP_goalie_position_weight_dist = 10;
  CP_goalie_narrow_sideline_cyc = 3;
  CP_goalie_no_buffer_dist = 10;
  CP_goalie_forward_stamina_buffer = 500;

  CP_clear_ball_ang_step = 5.0;
  CP_clear_ball_cone_ratio = .5;
  CP_clear_ball_max_dist = 30;
  CP_clear_offensive_min_horiz_dist = 20;
  CP_clear_offensive_min_angle = 60;

  CP_should_cross_corner_dist = 14; //pitch_width /2 - penalty_area_w / 2
  CP_should_cross_baseline_buffer = 3;
  CP_should_consider_cross_baseline_buffer = 3;
  CP_should_move_to_cross_corner_dist = 20; 
  CP_cross_pt_x = 36;  //pitch_length / 2 - penalty_area_l
  CP_cross_pt_y = 9; //goalie_area_w / 2
  CP_cross_target_vel = .5;

  CP_dont_dribble_to_middle_min_x = 20;

  CP_kickop_backang_step = 10;
  CP_kickop_frontang_step = 4;
  CP_min_kickop_vel = 1.3;
  CP_max_kickop_vel = 2.0;
  CP_kickop_vel_step = .2;

  /* not used anymore
     CP_hardest_kick_shot_distance = 13;
     CP_moderate_kick_shot_distance = 9;
  */
  CP_good_shot_distance = 20;
  CP_shot_distance = 30;
  CP_cycles_to_kick_buffer = 1;
  CP_better_shot_cyc_diff = 5;
  //CP_breakaway_shot_distance = 16;
  CP_shot_speed = 2.0; //our average shot speed
  CP_shot_goalie_react_buffer = 5;
  CP_good_shot_goalie_react_buffer = 2;

  sprintf(FP_initial_formation, "433");
  sprintf(FP_formation_when_tied, "initial");
  sprintf(FP_formation_when_losing, "initial");
  sprintf(FP_formation_when_losing_lots, "initial");
  sprintf(FP_formation_when_winning, "initial");
  sprintf(FP_initial_hc_method, "Obey");
  sprintf(FP_initial_mc_method, "Obey");
  FP_initial_player_1_pos     = 1;
  FP_initial_player_2_pos     = 2;
  FP_initial_player_3_pos     = 3;
  FP_initial_player_4_pos     = 4;
  FP_initial_player_5_pos     = 5;
  FP_initial_player_6_pos     = 6;
  FP_initial_player_7_pos     = 7;
  FP_initial_player_8_pos     = 8;
  FP_initial_player_9_pos     = 9;
  FP_initial_player_10_pos    = 10;
  FP_initial_player_11_pos    = 11;
  FP_goalie_number            = 0;

  CP_SPP_execution_type = 0;

  CP_tune_timing = FALSE;
}

// InputOptions and then do a few calculations
void OptionInfo::GetOptions(int argc, char **argv)
{
  InputOptions(argc,argv);
}

void OptionInfo::InputOptions(int argc, char **argv)
{
  option_t opt[] = {
    {"test_l",        		(void *)&VP_test_l,             V_ONOFF},
    {"test_r",        		(void *)&VP_test_r,             V_ONOFF},
    {"test",        		(void *)&VP_test,               V_ONOFF},
    {"train_DT",                    (void *)&VP_train_DT,           V_ONOFF},
    {"use_DT",                      (void *)&VP_use_DT,             V_ONOFF},
    {"keepaway",                      (void *)&VP_keepaway,             V_ONOFF},

    {"keepaway_mode",              (void *)&CP_keepaway_mode, V_INT},
    {"keepaway_rect_length",        (void *)&TP_keepaway_rect_length , V_FLOAT},
    {"keepaway_rect_width",         (void *)&TP_keepaway_rect_width , V_FLOAT},

    {"my_score",        	    (void *)&IP_my_score,           V_INT},
    {"their_score",        	    (void *)&IP_their_score,        V_INT},
    {"reconnect",                   (void *)&IP_reconnect,          V_INT},
    {"only_connect",                (void *)&IP_only_connect,       V_ONOFF},

    {"team_name",        		(void *)&MyTeamName,            V_STRING},
    {"goalie",        		(void *)&CP_goalie,             V_ONOFF},
    {"save_log",        		(void *)&CP_save_log,           V_ONOFF},
    {"save_freq",                   (void *)&CP_save_freq,          V_INT},
    {"save_sound_log",  		(void *)&CP_save_sound_log,     V_ONOFF},
    {"save_sound_freq",             (void *)&CP_save_sound_freq,    V_INT},
    {"send_ban_recv_step_factor",   (void *)&CP_send_ban_recv_step_factor, V_FLOAT},
    {"interrupts_per_cycle",        (void *)&CP_interrupts_per_cycle,   V_INT},
    {"interrupts_left_to_act",      (void *)&CP_interrupts_left_to_act,   V_INT},
    {"wait_for_sight",              (void *)&CP_wait_for_sight,     V_ONOFF},
    {"max_conf",                    (void *)&CP_max_conf,           V_FLOAT},
    {"min_conf",                    (void *)&CP_min_valid_conf,     V_FLOAT},
    {"conf_decay",                  (void *)&CP_conf_decay,         V_FLOAT},
    {"player_conf_decay",           (void *)&CP_player_conf_decay,  V_FLOAT},
    {"ball_conf_decay",             (void *)&CP_ball_conf_decay,    V_FLOAT},
    {"listen_conf_buffer",          (void *)&CP_listen_conf_buffer, V_FLOAT},

    {"max_player_move_factor",      (void *)&CP_max_player_move_factor, V_FLOAT},
    {"max_say_interval",            (void *)&CP_max_say_interval,   V_INT},
    {"ball_moving_threshold",       (void *)&CP_ball_moving_threshold, V_FLOAT},
    {"dodge_distance_buffer",       (void *)&CP_dodge_distance_buffer, V_FLOAT},
    {"dodge_angle_buffer",          (void *)&CP_dodge_angle_buffer, V_FLOAT},
    {"dodge_power",                 (void *)&CP_dodge_power,        V_FLOAT},
    {"dodge_angle",                 (void *)&CP_dodge_angle,        V_FLOAT},
    {"tree_stem",                   (void *)&CP_tree_stem,          V_STRING},
    {"DT_evaluate_interval",        (void *)&CP_DT_evaluate_interval, V_INT},
    {"say_tired_interval",          (void *)&CP_say_tired_interval, V_INT},
    {"tired_buffer",                (void *)&CP_tired_buffer,       V_FLOAT},
    {"say_tired_buffer",            (void *)&CP_say_tired_buffer,   V_FLOAT},
    {"set_plays",                   (void *)&CP_set_plays,          V_ONOFF},

    {"use_new_set_plays",           (void *)&CP_use_new_set_plays,  V_ONOFF},
    {"set_play_delay",              (void *)&CP_Setplay_Delay,     V_INT},
    {"set_play_max_delay",          (void *)&CP_Setplay_Max_Delay,     V_INT},
    {"set_play_say_delay",          (void *)&CP_Setplay_Say_Delay,  V_INT},
    {"set_play_time_limit",         (void *)&CP_Setplay_Time_Limit, V_INT},

    {"kickable_buffer",             (void *)&CP_kickable_buffer,    V_FLOAT},
    {"mark_persist_time",           (void *)&CP_mark_persist_time,  V_INT},
    {"mark_max_dist_from_offside_line", (void *)&CP_mark_max_dist_from_offside_line,  V_FLOAT},
    {"max_midfield_markers",        (void *)&CP_max_midfield_markers, V_INT},
    {"max_midfield_markers_when_tied",        (void *)&CP_max_midfield_markers_when_tied, V_INT},
    {"max_midfield_markers_when_winning",        (void *)&CP_max_midfield_markers_when_winning, V_INT},
    {"max_midfield_markers_when_losing",        (void *)&CP_max_midfield_markers_when_losing, V_INT},
    {"max_midfield_markers_when_losing_by_lots",        (void *)&CP_max_midfield_markers_when_losing_by_lots, V_INT},
    {"midfielder_leave_mark_min_x", (void *)&CP_midfielder_leave_mark_min_x, V_FLOAT},

    {"track_min_distance",          (void *)&CP_track_min_distance, V_FLOAT},
    {"pull_offsides",               (void *)&CP_pull_offsides,      V_ONOFF},
    {"pull_offsides_when_winning",  (void *)&CP_pull_offsides_when_winning, V_ONOFF},
    {"spar",                        (void *)&CP_spar,            V_ONOFF},
    {"mark",                        (void *)&CP_mark,            V_ONOFF},
    {"communicate",                 (void *)&CP_communicate,        V_ONOFF},
    {"change_view_for_ball_cycles", (void *)&CP_change_view_for_ball_cycles, V_INT},
    {"defer_kick_to_teammate_buffer",(void *)&CP_defer_kick_to_teammate_buffer, V_FLOAT},
    {"scan_overlap_angle",          (void *)&CP_scan_overlap_angle, V_FLOAT},

    {"pull_offside_threshold",      (void *)&CP_pull_offside_threshold, V_FLOAT},
    {"pull_offside_buffer",         (void *)&CP_pull_offside_buffer,    V_FLOAT},

    {"ball_forget_angle_buf",       (void *)&CP_ball_forget_angle_buf,   V_FLOAT},
    {"player_forget_angle_buf",     (void *)&CP_player_forget_angle_buf, V_FLOAT},
    {"ball_forget_dist_buf",        (void *)&CP_ball_forget_dist_buf,    V_FLOAT},
    {"player_forget_dist_buf",      (void *)&CP_player_forget_dist_buf,  V_FLOAT},

    {"beat_offsides_buffer",        (void *)&CP_beat_offsides_buffer,    V_FLOAT},
    {"beat_offsides_threshold",     (void *)&CP_beat_offsides_threshold,    V_FLOAT},
    {"beat_offsides_max_x",         (void *)&CP_beat_offsides_max_x,    V_FLOAT},
    {"congestion_epsilon",          (void *)&CP_congestion_epsilon,    V_FLOAT},
    {"back_pass_opponent_buffer",   (void *)&CP_back_pass_opponent_buffer,    V_FLOAT},
    {"back_pass_offside_buffer",   (void *)&CP_back_pass_offside_buffer,    V_FLOAT},
    {"min_less_congested_pass_dist",(void *)&CP_min_less_congested_pass_dist,    V_FLOAT},
    {"no_dangerous_pass_or_dribble_x_thresh",(void *)&CP_no_dangerous_pass_or_dribble_x_thresh,    V_FLOAT},
    {"min_pass_dist",               (void *)&CP_min_pass_dist,    V_FLOAT},
    {"min_dangerous_pass_dist",     (void *)&CP_min_dangerous_pass_dist,    V_FLOAT},

    {"use_new_position_based_vel",  (void *)&CP_use_new_position_based_vel, V_ONOFF},
    {"stop_on_error",               (void *)&CP_stop_on_error,      V_ONOFF},

    {"opt_ctrl_dist",               (void *)&CP_opt_ctrl_dist,      V_FLOAT},
    {"KickTo_err",                  (void *)&CP_KickTo_err,         V_FLOAT},
    {"closest_margin",              (void *)&CP_closest_margin,     V_FLOAT},
    {"dokick_factor",               (void *)&CP_dokick_factor,      V_FLOAT},
    {"max_turn_kick_pow",           (void *)&CP_max_turn_kick_pow,  V_FLOAT},
    {"kick_time_space",             (void *)&CP_kick_time_space,    V_INT},
    {"max_ignore_vel",              (void *)&CP_max_ignore_vel,     V_FLOAT},
    {"max_est_err",                 (void *)&CP_max_est_err,        V_FLOAT},
    {"holdball_kickable_buffer",    (void *)&CP_holdball_kickable_buffer, V_FLOAT},
    {"stop_ball_power",             (void *)&CP_stop_ball_power, V_INT},
    {"possessor_intercept_space",   (void *)&CP_possessor_intercept_space, V_INT},
    {"can_keep_ball_cycle_buffer",  (void *)&CP_can_keep_ball_cycle_buffer, V_INT},

    /* no longer used 
    {"hard_kick_margin",            (void *)&CP_hard_kick_margin,   V_FLOAT},
    {"hard_kick_end_turn_dist",     (void *)&CP_hard_kick_end_turn_dist,  V_FLOAT},
    {"hard_kick_factor",            (void *)&CP_hard_kick_factor,   V_FLOAT},
    */
    {"max_hard_kick_angle_err",     (void *)&CP_max_hard_kick_angle_err, V_INT},
    {"hard_kick_dist_buffer",       (void *)&CP_hard_kick_dist_buffer, V_FLOAT},
    {"hardest_kick_ball_ang",       (void *)&CP_hardest_kick_ball_ang,  V_INT},
    {"hardest_kick_ball_dist",      (void *)&CP_hardest_kick_ball_dist,  V_FLOAT},
    {"hardest_kick_player_ang",     (void *)&CP_hardest_kick_player_ang, V_INT},
    {"max_dash_help_kick_angle",    (void *)&CP_max_dash_help_kick_angle, V_FLOAT},

    {"max_go_to_point_angle_err",   (void *)&CP_max_go_to_point_angle_err, V_INT},
    {"max_int_lookahead",           (void *)&CP_max_int_lookahead,  V_INT},
    {"intercept_close_dist",        (void *)&CP_intercept_close_dist,  V_FLOAT},
    {"intercept_step",              (void *)&CP_intercept_step,     V_INT},	
    {"my_intercept_step",           (void *)&CP_my_intercept_step,  V_INT},	
    {"intercept_aim_ahead",         (void *)&CP_intercept_aim_ahead, V_INT},
    {"no_turn_max_cyc_diff",        (void *)&CP_no_turn_max_cyc_diff, V_INT},
    {"no_turn_max_dist_diff",       (void *)&CP_no_turn_max_dist_diff, V_FLOAT},

    {"turnball_opp_worry_dist",     (void *)&CP_turnball_opp_worry_dist,V_FLOAT},
    {"collision_buffer",            (void *)&CP_collision_buffer,  V_FLOAT},
    {"behind_angle",                (void *)&CP_behind_angle, V_FLOAT},
    {"ball_vel_invalidation_factor",(void *)&CP_ball_vel_invalidation_factor, V_FLOAT},
    {"time_for_full_rotation",      (void *)&CP_time_for_full_rotation, V_INT},

    {"dribble_dash_pow",            (void *)&CP_dribble_dash_pow, V_INT},
    {"dribble_ball_dist",           (void *)&CP_dribble_ball_dist,  V_FLOAT},
    {"dribble_behind_angle",        (void *)&CP_dribble_behind_angle,  V_FLOAT},
    {"dribble_ignore_opp_dist",     (void *)&CP_dribble_ignore_opp_dist, V_FLOAT},
    {"dribble_worry_opp_dist",      (void *)&CP_dribble_worry_opp_dist, V_FLOAT},
    {"dribble_angle_norm",          (void *)&CP_dribble_angle_norm, V_FLOAT},
    {"dribble_dodge_max_dist",      (void *)&CP_dribble_dodge_max_dist, V_FLOAT},
    {"dribble_dodge_angle_err",     (void *)&CP_dribble_dodge_angle_err, V_FLOAT},
    {"dribble_exp_angle_buffer",    (void *)&CP_dribble_exp_angle_buffer, V_FLOAT},
    {"dribble_angle_ignore_buffer", (void *)&CP_dribble_angle_ignore_buffer, V_FLOAT},
    {"dribble_dodge_close_dist",    (void *)&CP_dribble_dodge_close_dist, V_FLOAT},
    {"dribble_scan_field",          (void *)&CP_dribble_scan_field, V_ONOFF},

    {"dribble_fast_max_dist",       (void *)&CP_dribble_fast_max_dist, V_FLOAT},

    {"can_dribble_cone_ratio",      (void *)&CP_can_dribble_cone_ratio, V_FLOAT},
    {"dribble_towards_length",      (void *)&CP_dribble_towards_length, V_FLOAT},
    {"dribble_sideline_buffer",     (void *)&CP_dribble_sideline_buffer, V_FLOAT},
    {"dribble_circle_inner_rad",    (void *)&CP_dribble_circle_inner_rad, V_FLOAT},
    {"dribble_circle_outer_rad",    (void *)&CP_dribble_circle_outer_rad, V_FLOAT},
    {"dribble_circle_ang",          (void *)&CP_dribble_circle_ang, V_FLOAT},
 
    {"move_imp_1v1_initial",        (void *)&CP_move_imp_1v1_initial, V_FLOAT},
    {"move_imp_1v1_inc",            (void *)&CP_move_imp_1v1_inc, V_FLOAT},
    {"move_imp_1v1_threshold",      (void *)&CP_move_imp_1v1_threshold, V_FLOAT},
    {"at_point_buffer",             (void *)&CP_at_point_buffer, V_FLOAT},
    {"overrun_dist",                (void *)&CP_overrun_dist, V_FLOAT},
    {"def_block_dist",              (void *)&CP_def_block_dist, V_FLOAT},
    {"def_block_dist_ratio",        (void *)&CP_def_block_dist_ratio, V_FLOAT},
    {"overrun_buffer",              (void *)&CP_overrun_buffer, V_FLOAT},

    {"breakaway_buffer",            (void *)&CP_breakaway_buffer, V_FLOAT},
    {"our_breakaway_kickable_buffer",   (void *)&CP_our_breakaway_kickable_buffer, V_FLOAT},
    {"their_breakaway_front_kickable_buffer",   (void *)&CP_their_breakaway_front_kickable_buffer, V_FLOAT},
    {"their_breakaway_back_kickable_buffer",   (void *)&CP_their_breakaway_back_kickable_buffer, V_FLOAT},
    {"goalie_breakaway_kickable_buffer",   (void *)&CP_goalie_breakaway_kickable_buffer, V_FLOAT},

    {"breakaway_approach_x",        (void *)&CP_breakaway_approach_x,  V_FLOAT},
    {"breakaway_approach_y",        (void *)&CP_breakaway_approach_y,  V_FLOAT},
    {"breakaway_targ_valid_time",   (void *)&CP_breakaway_targ_valid_time,       V_INT},
    {"breakaway_min_goalie_steal_time", (void *)&CP_breakaway_min_goalie_steal_time, V_INT},
    {"breakaway_kick_run_min_cycles",(void *)&CP_breakaway_kick_run_min_cycles, V_INT},
    {"breakaway_kick_run_max_cycles",(void *)&CP_breakaway_kick_run_max_cycles, V_INT},
    {"their_breakaway_min_cone_dist_wid",  (void *)&CP_their_breakaway_min_cone_dist_wid,     V_FLOAT},
    {"our_breakaway_min_cone_dist_wid",  (void *)&CP_our_breakaway_min_cone_dist_wid,     V_FLOAT},
    {"breakaway_middle_buffer",      (void *)&CP_breakaway_middle_buffer, V_FLOAT},
    {"breakaway_kick_run_worry_dist",(void *)&CP_breakaway_kick_run_worry_dist, V_FLOAT},
    {"breakaway_mode",              (void *)&CP_breakaway_mode, V_INT},

    {"static_kick_dist_err",        (void *)&CP_static_kick_dist_err, V_FLOAT},
    {"static_kick_ang_err",         (void *)&CP_static_kick_ang_err, V_FLOAT},
    /* no longer used
    {"static_kick_dist",            (void *)&CP_static_kick_dist, V_FLOAT},
    {"static_kick_ang",             (void *)&CP_static_kick_ang, V_FLOAT},
    {"static_kick_overrun_dist",    (void *)&CP_static_kick_overrun_dist, V_FLOAT},
    */
    
    {"goalie_baseline_buffer",      (void *)&CP_goalie_baseline_buffer, V_FLOAT},
    {"goalie_scan_angle_err",       (void *)&CP_goalie_scan_angle_err, V_FLOAT},
    {"goalie_at_point_buffer",      (void *)&CP_goalie_at_point_buffer, V_FLOAT},
    {"goalie_vis_angle_err",        (void *)&CP_goalie_vis_angle_err, V_FLOAT},
    {"goalie_max_shot_distance",    (void *)&CP_goalie_max_shot_distance, V_FLOAT},
    {"goalie_min_pos_dist",         (void *)&CP_goalie_min_pos_dist, V_FLOAT},
    {"goalie_max_pos_dist",         (void *)&CP_goalie_max_pos_dist, V_FLOAT},
    {"goalie_max_forward_percent",  (void *)&CP_goalie_max_forward_percent, V_FLOAT},
    {"goalie_ball_ang_for_corner",  (void *)&CP_goalie_ball_ang_for_corner, V_FLOAT},
    {"goalie_max_come_out_dist",    (void *)&CP_goalie_max_come_out_dist, V_FLOAT},
    {"goalie_ball_dist_for_corner", (void *)&CP_goalie_ball_dist_for_corner, V_FLOAT},
    {"goalie_ball_dist_for_center", (void *)&CP_goalie_ball_dist_for_center, V_FLOAT},
    {"goalie_free_kick_dist",       (void *)&CP_goalie_free_kick_dist, V_FLOAT},
    {"goalie_go_to_ball_cone_ratio",(void *)&CP_goalie_go_to_ball_cone_ratio, V_FLOAT},
    {"goalie_warn_space",           (void *)&CP_goalie_warn_space, V_INT},
    {"goalie_comes_out",            (void *)&CP_goalie_comes_out, V_ONOFF},
    {"goalie_catch_wait_time",      (void *)&CP_goalie_catch_wait_time, V_INT},
    {"goalie_opponent_dist_to_block", (void *)&CP_goalie_opponent_dist_to_block, V_FLOAT},
    {"goalie_always_block_dist",    (void *)&CP_goalie_always_block_dist, V_FLOAT},
    {"goalie_position_weight_dist", (void *)&CP_goalie_position_weight_dist, V_FLOAT},
    {"goalie_narrow_sideline_cyc",  (void *)&CP_goalie_narrow_sideline_cyc,  V_INT},
    {"goalie_no_buffer_dist",       (void *)&CP_goalie_no_buffer_dist, V_FLOAT},
    {"goalie_forward_stamina_buffer",(void *)&CP_goalie_forward_stamina_buffer, V_INT},
    
    {"clear_ball_ang_step",         (void *)&CP_clear_ball_ang_step, V_FLOAT},
    {"clear_ball_cone_ratio",       (void *)&CP_clear_ball_cone_ratio, V_FLOAT},
    {"clear_ball_max_dist",         (void *)&CP_clear_ball_max_dist, V_FLOAT},
    {"clear_offensive_min_horiz_dist", (void *)&CP_clear_offensive_min_horiz_dist, V_FLOAT},
    {"clear_offensive_min_angle",   (void *)&CP_clear_offensive_min_angle, V_FLOAT},


    {"should_cross_corner_dist",    (void *)&CP_should_cross_corner_dist, V_FLOAT},
    {"should_cross_baseline_buffer",(void *)&CP_should_cross_baseline_buffer, V_FLOAT},
    {"should_consider_cross_baseline_buffer",(void *)&CP_should_consider_cross_baseline_buffer, V_FLOAT},
    {"should_move_to_cross_corner_dist", (void *)&CP_should_move_to_cross_corner_dist, V_FLOAT},
    {"cross_pt_x",                  (void *)&CP_cross_pt_x, V_FLOAT},
    {"cross_pt_y",                  (void *)&CP_cross_pt_y, V_FLOAT},
    {"cross_target_vel",            (void *)&CP_cross_target_vel, V_FLOAT},

    {"dont_dribble_to_middle_min_x",(void *)&CP_dont_dribble_to_middle_min_x, V_FLOAT},

    {"kickop_backang_step",          (void *)&CP_kickop_backang_step, V_FLOAT},
    {"kickop_frontang_step",         (void *)&CP_kickop_frontang_step, V_FLOAT},
    {"min_kickop_vel",               (void *)&CP_min_kickop_vel, V_FLOAT},
    {"max_kickop_vel",               (void *)&CP_max_kickop_vel, V_FLOAT},
    {"kickop_vel_step",              (void *)&CP_kickop_vel_step, V_FLOAT}, 
   
  /* not used anymore
    {"hardest_kick_shot_distance",  (void *)&CP_hardest_kick_shot_distance, V_FLOAT},
    {"moderate_kick_shot_distance", (void *)&CP_moderate_kick_shot_distance, V_FLOAT},
    */
    {"good_shot_distance",          (void *)&CP_good_shot_distance, V_FLOAT},
    {"shot_distance",               (void *)&CP_shot_distance, V_FLOAT},
    {"cycles_to_kick_buffer",       (void *)&CP_cycles_to_kick_buffer, V_INT},
    {"shot_speed",                  (void *)&CP_shot_speed,    V_FLOAT},
    {"shot_goalie_react_buffer",      (void *)&CP_shot_goalie_react_buffer,      V_INT},
    {"good_shot_goalie_react_buffer", (void *)&CP_good_shot_goalie_react_buffer, V_INT},
    {"better_shot_cyc_diff",        (void *)&CP_better_shot_cyc_diff,            V_INT},
    //{"breakaway_shot_distance",     (void *)&CP_breakaway_shot_distance, V_FLOAT},
    
    {"formation",                   (void *)&FP_initial_formation,  V_STRING},
    {"formation_when_losing",       (void *)&FP_formation_when_losing,  V_STRING},
    {"formation_when_losing_lots",  (void *)&FP_formation_when_losing_lots,  V_STRING},
    {"formation_when_winning",      (void *)&FP_formation_when_winning,  V_STRING},
    {"formation_when_tied",         (void *)&FP_formation_when_tied,  V_STRING},
    
    {"home_change",                 (void *)&FP_initial_hc_method,  V_STRING},
    {"mark_change",                 (void *)&FP_initial_mc_method,  V_STRING},
    {"player_1_pos",                (void *)&FP_initial_player_1_pos,  V_INT},
    {"player_2_pos",                (void *)&FP_initial_player_2_pos,  V_INT},
    {"player_3_pos",                (void *)&FP_initial_player_3_pos,  V_INT},
    {"player_4_pos",                (void *)&FP_initial_player_4_pos,  V_INT},
    {"player_5_pos",                (void *)&FP_initial_player_5_pos,  V_INT},
    {"player_6_pos",                (void *)&FP_initial_player_6_pos,  V_INT},
    {"player_7_pos",                (void *)&FP_initial_player_7_pos,  V_INT},
    {"player_8_pos",                (void *)&FP_initial_player_8_pos,  V_INT},
    {"player_9_pos",                (void *)&FP_initial_player_9_pos,  V_INT},
    {"player_10_pos",               (void *)&FP_initial_player_10_pos, V_INT},
    {"player_11_pos",               (void *)&FP_initial_player_11_pos, V_INT},
    {"goalie_number",               (void *)&FP_goalie_number        , V_INT},

    {"SPP_execution_type",          (void *)&CP_SPP_execution_type,    V_INT},

    {"tune_timing",                 (void *)&CP_tune_timing,    V_ONOFF},
    
#include <opt-array.C>
    
    {"\0",				NULL, 			       	0}
  } ;


#include <opt-code.C>
}


/* explode the line into argc and argv */
void OptionInfo::GetOptions(char* line) 
{
  const int MAXOPT = 100;
  char* argv[MAXOPT];
  int argc = 1; /* executable name */
  char* pc;

  advance_past_space(&line);
  while (*line != 0) {
    pc = line;
    get_token(&line);
    argv[argc] = new char[line-pc+1];
    strncpy(argv[argc], pc, line-pc);
    argv[argc][line-pc] = 0; /* null terminate */
    argc++;
    advance_past_space(&line);
  }

  argv[argc] = NULL;

  InputOptions(argc, argv);

  for (int i = 1; i<argc; i++)
    delete [] argv[i];
}

