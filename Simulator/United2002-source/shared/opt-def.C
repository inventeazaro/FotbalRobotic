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


/* this file is meant to be included in a MemOption.h */

/* Server params */
  /* no option flags for these */
  SP_pitch_length 	  = 105.0;
  SP_pitch_width 	  = 68.0;
  SP_pitch_margin	  = 5.0;
  SP_penalty_area_length  = 16.5;
  SP_penalty_area_width   = 40.32;
  SP_goal_area_length	  = 5.5;
  SP_goal_area_width	  = 18.32;
  SP_penalty_spot_dist    = 11.0;
  SP_corner_arc_r	  = 1.0;
  SP_free_kick_buffer     = 9.15;
  SP_after_goal_wait  	  = 50;
  SP_after_offside_wait   = 30;
  SP_drop_ball_time       = 200;
  SP_feel_distance        = 3.0;
  SP_num_lines            = 4;
  SP_num_markers          = 55;
  SP_unum_far_length      = 20.0;
  SP_unum_too_far_length  = 40.0;
  SP_team_far_length      = 40.0;
  SP_team_too_far_length  = 60.0;

  SP_version              = 4.06;
  SP_team_size            = 11;
  SP_half                 = 1;
  sprintf(SP_host,        "localhost");
  SP_goal_width           = 14.02;       
  SP_player_size          = 0.8;
  SP_player_decay         = 0.4;
  SP_player_rand 	  = 0.1;
  SP_player_weight	  = 60.0;
  SP_player_speed_max     = 32.0;
  SP_stamina_max	  = 2000.0;
  SP_stamina_inc          = 20.0;
  SP_recover_dec_thr      = 0.3;	
  SP_recover_dec          = 0.05; 	
  SP_recover_min          = 0.1; 	
  SP_effort_dec_thr       = 0.4;	
  SP_effort_min           = 0.1;	
  SP_effort_dec           = 0.05;	
  SP_effort_inc_thr       = 0.9;	
  SP_effort_inc           = 0.05;	
  SP_prand_factor_l       = 1.0;
  SP_prand_factor_r       = 1.0;
  SP_kick_rand            = 0.0;
  SP_kick_rand_factor_l   = 1.0;
  SP_kick_rand_factor_r   = 1.0;
  SP_ball_size            = 0.085;		
  SP_ball_decay           = 0.96;	
  SP_ball_rand            = 0.05;		
  SP_ball_weight          = 0.2;;	
  SP_ball_speed_max       = 32.0;	
  SP_dash_power_rate      = 0.01;	
  SP_kick_power_rate      = 0.016;
  SP_kickable_margin      = 1.0;	
  SP_kickable_area        = SP_kickable_margin + SP_ball_size + SP_player_size;
  SP_control_radius       = 2.0;
  SP_catch_prob           = 1.0;	
  SP_catch_area_l         = 2.0;	
  SP_catch_area_w         = 1.0;	
  SP_catch_ban_cycle      = 5;
  SP_max_power            = 100;		
  SP_min_power            = -30;		
  SP_max_moment           = 180;		
  SP_min_moment           = -180;		
  SP_min_neck_angle       = -90.0;
  SP_max_neck_angle       = 90.0;
  SP_min_neck_moment      = -180.0;
  SP_max_neck_moment      = 180.0;
  SP_visible_angle        = 90.0;	
  SP_audio_cut_dist       = 50.0;	
  SP_dist_qstep           = 0.1;	
  SP_land_qstep           = 0.01;	
  SP_dir_qstep            = -1;	
  SP_dist_qstep_l         = -1;	
  SP_dist_qstep_r         = -1;	
  SP_land_qstep_l         = -1;	
  SP_land_qstep_r         = -1;	
  SP_dir_qstep_l         = -1;	
  SP_dir_qstep_r         = -1;	
  SP_ckmargin             = 1.0;	
  SP_wind_dir             = 0.0;	
  SP_wind_force           = 0.0;
  SP_wind_rand            = 0.0;	
  SP_wind_none            = FALSE;
  SP_wind_random          = FALSE;
  SP_half_time            = 300;
  SP_port                 = 6000;
  SP_coach_port           = 6001;
  SP_olcoach_port         = 6002;
  SP_slow_down_factor     = 1;
  SP_simulator_step       = 100;
  SP_send_step            = 150;
  SP_recv_step            = 10;
  SP_say_msg_size         = 10;
  SP_hear_max             = 2;
  SP_hear_inc             = 1;
  SP_hear_decay           = 2;
  SP_coach_mode           = FALSE;
  SP_coach_w_referee_mode = FALSE;
  SP_say_coach_cnt_max    = 128;
  SP_say_coach_msg_size   = 128;
  SP_send_vi_step         = 100;
  SP_look_step            = 100;
  SP_use_offside          = FALSE;
  SP_forbid_kickoff_offside = TRUE;
  SP_verbose              = TRUE;
  SP_offside_area         = 9.15;
  SP_inertia_moment       = 5.0;
  SP_sense_body_step      = 100;
  SP_offside_kick_margin  = 9.15;
  SP_record_messages      = FALSE;
  SP_goalie_max_moves     = 2;
  SP_start_goal_l         = 0;
  SP_start_goal_r         = 0;
  SP_fullstate_l          = 0;
  SP_fullstate_r          = 0;

  SP_synch_mode = 0;
  SP_slowness_on_top_for_left_team = 1.0;
  SP_slowness_on_top_for_right_team = 1.0;
  sprintf(SP_landmark_file,"~/.rcssserver-landmark.xml");
  SP_send_comms = FALSE;
  SP_text_logging = TRUE;
  SP_game_logging = TRUE;
  SP_game_log_version = 3;
  sprintf(SP_text_log_dir,"./");
  sprintf(SP_game_log_dir,"./");
  sprintf(SP_text_log_fixed_name,"rcssserver");
  sprintf(SP_game_log_fixed_name,"rcssserver");
  SP_text_log_fixed = FALSE;
  SP_game_log_fixed = FALSE;
  SP_text_log_dated = TRUE;
  SP_game_log_dated = TRUE;
//  sprintf(SP_log_date_format,"%Y%m%d%H%M-");
  SP_log_times = FALSE;
  SP_record_messages = FALSE;
  SP_text_log_compression = 0;
  SP_game_log_compression = 0;
  SP_profile = FALSE;
  SP_point_to_ban = 5; 
  SP_point_to_duration = 20;
  SP_tackle_dist = 2.5;
  SP_tackle_back_dist = 0.5;
  SP_tackle_width = 1.25;
  SP_tackle_exponent = 6.0;
  SP_tackle_cycles = 10;
  SP_tackle_power_rate = .027;

  SP_freeform_wait_period = 600;
  SP_freeform_send_period = 20;
  SP_clang_del_win = 1; 
  SP_clang_rule_win = 1; 

  SP_free_kick_faults = true;
  SP_back_passes  = true;
  SP_proper_goal_kicks = false;
  SP_stopped_ball_vel = .01;
  SP_max_goal_kicks = 3;

  PP_player_types         = 7;
  PP_subs_max             = 3; 
  PP_pt_max               = 3;
  
  PP_player_speed_max_delta_min    = 0.0;
  PP_player_speed_max_delta_max    = 0.2;
  PP_stamina_inc_max_delta_factor  = -100;
    
  PP_player_decay_delta_min        = 0.0;
  PP_player_decay_delta_max        = 0.2;
  PP_inertia_moment_delta_factor   = 25;

  PP_dash_power_rate_delta_min     = 0.0;
  PP_dash_power_rate_delta_max     = .002;
  PP_player_size_delta_factor      = -100;

  PP_kickable_margin_delta_min     = 0.0;
  PP_kickable_margin_delta_max     = 0.2;
  PP_kick_rand_delta_factor        = 0.5;

  PP_extra_stamina_delta_min       = 0.0;
  PP_extra_stamina_delta_max       = 100;
  PP_effort_max_delta_factor       = -.002;
  PP_effort_min_delta_factor       = -.002;

  PP_new_dash_power_rate_delta_min    =  0.0;
  PP_new_dash_power_rate_delta_max    = .002;
  PP_new_stamina_inc_max_delta_factor = -10000.0;

  strcpy(SP_valid_message_chars, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz().+-*/?<>_ ");

//we cut out a couple of chars to try and catch messages from others
//Just make sure that there are at least 64 chars!
strcpy (CP_coach_message_chars, "012456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz(.+-*/?<>_");
strcpy (CP_player_message_chars, "12345678ABDEFGHIJKLNOPQRSTVWXYZabcdefghijklmnopqrstuvwxyz(.+*/?<>_");


/* these are client params which the coach needs to know about */
  CP_max_cycles_since_io = 20;

  CP_pass_target_speed = 1.0;
  CP_cycles_to_kick = 4;

  CP_min_kick_speed = 2.0;
  CP_avg_kick_speed = 2.5;
  CP_max_kick_speed = 3.0;
  CP_min_cycles_to_kick = 1;
  CP_avg_cycles_to_kick = 5;
  CP_max_cycles_to_kick = 10;
  CP_max_consec_turns = 2;
  CP_min_time_turn_when_going = 4; 


  CP_save_action_log_level = 0; /* 0 means save nothing */
  CP_save_action_freq      = 40;

  CP_SPP_max_coach_wait    = 50;  
  CP_SPP_delay             = 5;
  CP_SPP_max_delay         = 100;
  CP_SPP_max_player_setup  = 175;
  CP_SPP_time_limit        = 150;
  CP_SPP_kickoff_fixed_pos_time = 3;
//CP_SPP_init_dash_pow     = 35; //stamina_inc_max
  CP_SPP_at_point_buffer = 2;
  CP_SPP_kick_dir_ang_buffer = 90;
  CP_SPP_kick_complete_dist = 2;
  CP_SPP_min_goto_dist = 3 ;
  CP_SPP_kickable_buffer = 1.5; //same as breakaway
  CP_SPP_use_bpi_cyc_diff = 10;
  CP_SPP_min_dribble_dist = 3;
  CP_SPP_max_opp_control_time = 2;
  CP_SPP_min_pass_conf = .5;
  CP_SPP_min_pass_prob = .4;
  CP_SPP_pass_watch_for_others = TRUE;
  CP_SPP_coach_wait_buffer = 5;
  CP_SPP_player_coach_wait_buffer = 15;
  CP_SPP_coach_default_plan_time = 2;
  CP_SPP_players_replace_nums = true;
  CP_SPP_use_active_cycles = 4;
  CP_SPP_dist_to_end_for_ck = 45;
  CP_SPP_cycles_to_kick_buffer = 3;
  CP_SPP_gk_start_buffer = 5.0;
  CP_SPP_send_mode = 0;
  CP_SPP_off_dash_pow = 35;
  CP_SPP_def_dash_pow = 35;

  CP_SPP_forbid_area_buffer = 1.0;
  CP_SPP_sideline_buffer = 2;

  CP_SPP_min_pass_dist = 7;
  CP_SPP_max_pass_dist = 20; 
  CP_SPP_max_send_dist = 50;

  CP_SPP_frac_to_focus_best = 2.0/3.0;
  CP_SPP_hillclimb_num_paths = 20;
  CP_SPP_hillclimb_min_len = 3;
  CP_SPP_hillclimb_max_len = 6;
  strcpy(CP_SPP_hillclimb_def_file, "../shared/Plans/def_waypoints");

  CP_OM_obs_interval = 5;
  CP_OM_max_ball_steps = 20;
  CP_OM_grid_cell_width = 1.0;
  CP_OM_grid_cell_height = 1.0;
  CP_OM_num_models = 1;
  CP_OM_max_player_dist = 60;
  CP_OM_num_exits = 5;
