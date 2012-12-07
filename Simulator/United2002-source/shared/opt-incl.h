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
  float SP_pitch_length;
  float SP_pitch_width;
  float SP_pitch_margin;
  float SP_penalty_area_length;
  float SP_penalty_area_width;
  float SP_goal_area_length;
  float SP_goal_area_width;
  float SP_penalty_spot_dist;
  float SP_corner_arc_r;
  float SP_free_kick_buffer;
  int   SP_after_goal_wait;
  int   SP_after_offside_wait;
  int   SP_drop_ball_time;
  float SP_feel_distance;
  int   SP_num_lines;
  int   SP_num_markers;
  float SP_unum_far_length;
  float SP_unum_too_far_length;
  float SP_team_far_length;
  float SP_team_too_far_length;

  float SP_version;
  int   SP_team_size;
  int   SP_half;
  char  SP_host[MAX_HOST_LEN];
  float SP_goal_width;       
  float SP_player_size;	
  float SP_player_decay;    	
  float SP_player_rand;	
  float SP_player_weight;	
  float SP_player_speed_max;	
  float SP_player_accel_max;	
  float SP_stamina_max;	
  float SP_stamina_inc;	
  float SP_recover_init;	
  float SP_recover_dec_thr;	
  float SP_recover_min; 	
  float SP_recover_dec; 	
  float SP_effort_init;	
  float SP_effort_dec_thr;	
  float SP_effort_min;	
  float SP_effort_dec;	
  float SP_effort_inc_thr;	
  float SP_effort_inc;	
  float SP_kick_rand;
  Bool SP_team_actuator_noise;
  float SP_prand_factor_l;
  float SP_prand_factor_r;
  float SP_kick_rand_factor_l;
  float SP_kick_rand_factor_r;
  float SP_ball_size;		
  float SP_ball_decay;	
  float SP_ball_rand;		
  float SP_ball_weight;	
  float SP_ball_speed_max;	
  float SP_ball_accel_max;	
  float SP_dash_power_rate;	
  float SP_kick_power_rate;	
  float SP_kickable_margin;	
  float SP_control_radius;	
  float SP_kickable_area;	
  float SP_catch_prob;	
  float SP_catch_area_l;	
  float SP_catch_area_w;	
  float SP_max_power;		
  float SP_min_power;		
  float SP_max_moment;		
  float SP_min_moment;		
  float SP_max_neck_angle;
  float SP_min_neck_angle;
  float SP_max_neck_moment;
  float SP_min_neck_moment;
  float SP_visible_angle;	
  float SP_audio_cut_dist;	
  float SP_dist_qstep;	
  float SP_land_qstep;	
  float SP_dir_qstep;	
  float SP_dist_qstep_l;	
  float SP_dist_qstep_r;	
  float SP_land_qstep_l;	
  float SP_land_qstep_r;	
  float SP_dir_qstep_l;	
  float SP_dir_qstep_r;	
  float SP_ckmargin;	
  float SP_wind_dir;	
  float SP_wind_force;	
  float SP_wind_rand;	
  Bool SP_wind_none;
  Bool SP_wind_random;
  int SP_half_time;
  int SP_port;
  int SP_coach_port;
  int SP_olcoach_port;
  int SP_simulator_step;
  int SP_send_step;
  int SP_recv_step;
  int SP_say_msg_size;
  int SP_clang_win_size;
  int SP_clang_define_win;
  int SP_clang_meta_win;
  int SP_clang_advice_win;
  int SP_clang_info_win;
  int SP_clang_mess_delay;
  int SP_clang_mess_per_cycle;
  int SP_hear_max;
  int SP_hear_inc;
  int SP_hear_decay;
  int SP_catch_ban_cycle;
  Bool SP_coach_mode;
  Bool SP_coach_w_referee_mode;
  Bool SP_old_coach_hear;
  int SP_say_coach_cnt_max;	
  int SP_say_coach_msg_size;
  int SP_send_vi_step;
  int SP_look_step;
  float SP_offside_kick_margin;
  int SP_slow_down_factor;

  Bool SP_use_offside;
  Bool SP_forbid_kickoff_offside;
  char SP_logfile[MAX_FILE_LEN];
  char SP_recfile[MAX_FILE_LEN];
  Bool SP_rec_log;
  int  SP_rec_ver;
  char SP_replay[MAX_FILE_LEN];
  Bool SP_verbose;
  Bool SP_send_log;
  Bool SP_log_times;
  float SP_offside_area;
  float SP_inertia_moment;
  int   SP_sense_body_step;
  Bool  SP_record_messages;
  int   SP_goalie_max_moves;

  char  SP_valid_message_chars[MAX_MESSAGE_CHARS];

  char  CP_coach_message_chars[MAX_MESSAGE_CHARS];
  char  CP_player_message_chars[MAX_MESSAGE_CHARS];
  int SP_start_goal_l;
  int SP_start_goal_r;
  Bool SP_fullstate_l;
  Bool SP_fullstate_r;

  Bool SP_synch_mode;

  float SP_slowness_on_top_for_left_team; /* Kinda self explanatory */
  float SP_slowness_on_top_for_right_team; /* ditto */

  char SP_landmark_file[MAX_FILE_LEN];

  Bool SP_send_comms;
  Bool SP_text_logging;
  Bool SP_game_logging;
  int SP_game_log_version;
  char SP_text_log_dir[MAX_FILE_LEN];
  char SP_game_log_dir[MAX_FILE_LEN];
  char SP_text_log_fixed_name[MAX_FILE_LEN];
  char SP_game_log_fixed_name[MAX_FILE_LEN];
  Bool SP_text_log_fixed;
  Bool SP_game_log_fixed;
  Bool SP_text_log_dated;
  Bool SP_game_log_dated;
  char SP_log_date_format[MAX_FILE_LEN];
  int SP_text_log_compression;
  int SP_game_log_compression;
  Bool SP_profile;
  
  int SP_point_to_ban;
  int SP_point_to_duration;
  
  float SP_tackle_dist;
  float SP_tackle_back_dist;
  float SP_tackle_width;
  float SP_tackle_exponent;
  int SP_tackle_cycles;

  int SP_freeform_wait_period;
  int SP_freeform_send_period;
  int SP_clang_del_win; 
  int SP_clang_rule_win; 

  float SP_tackle_power_rate;
  bool SP_free_kick_faults;
  bool SP_back_passes;
  bool SP_proper_goal_kicks;
  float SP_stopped_ball_vel;
  int SP_max_goal_kicks;


/*Player parameters*/

  int PP_player_types;
  int PP_subs_max;
  int PP_pt_max;
  
  float PP_player_speed_max_delta_min;
  float PP_player_speed_max_delta_max;
  float PP_stamina_inc_max_delta_factor;
    
  float PP_player_decay_delta_min;
  float PP_player_decay_delta_max;
  float PP_inertia_moment_delta_factor;

  float PP_dash_power_rate_delta_min;
  float PP_dash_power_rate_delta_max;
  float PP_player_size_delta_factor;

  float PP_kickable_margin_delta_min;
  float PP_kickable_margin_delta_max;
  float PP_kick_rand_delta_factor;

  float PP_extra_stamina_delta_min;
  float PP_extra_stamina_delta_max;
  float PP_effort_max_delta_factor;
  float PP_effort_min_delta_factor;

  float PP_new_dash_power_rate_delta_min;
  float PP_new_dash_power_rate_delta_max;
  float PP_new_stamina_inc_max_delta_factor;

  float SP_player_global_speed_max;
  float SP_player_min_size;
  float SP_player_max_size;
  float SP_max_kickable_area;

  int CP_max_cycles_since_io;
float CP_min_kick_speed;
float CP_avg_kick_speed;
float CP_max_kick_speed;
int   CP_min_cycles_to_kick;
int   CP_avg_cycles_to_kick;
int   CP_max_cycles_to_kick;
//this is used to get upper bounds on how long a player should take to go from A to B
int   CP_max_consec_turns; //max number of turns needed to turn 180
int   CP_min_time_turn_when_going; 
 

/* these are client params which the coach needs to know about */
float CP_cycles_to_kick;
float CP_pass_target_speed;

  int   CP_save_action_log_level;
  int   CP_save_action_freq;

  //SPP is for SetplayPlan
  int   CP_SPP_max_coach_wait;
  int   CP_SPP_delay;
  int   CP_SPP_max_delay;
  int   CP_SPP_max_player_setup;
  int   CP_SPP_time_limit;
  int   CP_SPP_kickoff_fixed_pos_time;
//int   CP_SPP_init_dash_pow;
  int   CP_SPP_at_point_buffer;
  float CP_SPP_kick_dir_ang_buffer;
  float CP_SPP_kick_complete_dist;
  float CP_SPP_min_goto_dist;
  float CP_SPP_kickable_buffer;
  int   CP_SPP_use_bpi_cyc_diff; //if less cycle diff than this, use bpi
  float CP_SPP_min_dribble_dist;
  int   CP_SPP_max_opp_control_time;
  float CP_SPP_min_pass_conf;
  float CP_SPP_min_pass_prob;
  float CP_SPP_pass_watch_for_others;
  int   CP_SPP_coach_wait_buffer;
  int   CP_SPP_player_coach_wait_buffer;
  int   CP_SPP_coach_default_plan_time; //time before limit before switching to default
  bool  CP_SPP_players_replace_nums;
  int   CP_SPP_use_active_cycles;
  float CP_SPP_dist_to_end_for_ck;
  int   CP_SPP_cycles_to_kick_buffer;
  float CP_SPP_gk_start_buffer;
  int   CP_SPP_send_mode;
  float CP_SPP_off_dash_pow;
  float CP_SPP_def_dash_pow;

  float CP_SPP_forbid_area_buffer;
  float CP_SPP_sideline_buffer;

  float CP_SPP_min_pass_dist;
  float CP_SPP_max_pass_dist; //min send distance is the same as max_pass_dist
  float CP_SPP_max_send_dist;

  float CP_SPP_frac_to_focus_best;
  int CP_SPP_hillclimb_num_paths;
  int CP_SPP_hillclimb_min_len;
  int CP_SPP_hillclimb_max_len;
  char CP_SPP_hillclimb_def_file[MAX_FILE_LEN];

  /* DEB: add you parameters here */

  int CP_OM_obs_interval;
  int CP_OM_max_ball_steps;
  float CP_OM_grid_cell_width;
  float CP_OM_grid_cell_height;
  int CP_OM_num_models;
  float CP_OM_max_player_dist;

  int CP_OM_num_exits;
