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


#ifndef _OPTION_H_
#define _OPTION_H_

#include "types.h"

struct option_t {
   char	optname[50] ;
   void	*vptr ;
   int	vsize ;
};

enum InputType{
 V_INT,
 V_FLOAT,
 V_BOOL,
 V_STRING,	
 V_ONOFF,		
 V_NONE		
};

#define MAX_TEAMNAME_LEN  20
#define MAX_HOST_LEN      50
#define MAX_FILE_LEN      256
#define MAX_FP_LEN        20
#define MAX_TREE_STEM_LEN 50

/* Things to be read at startup that never change */
class OptionInfo {
public:
  OptionInfo();
  void InputOptions(int, char**);
  void GetOptions(int, char**);
  void GetOptions(char*);

  /* Client version params */
  Bool  VP_test_l;
  Bool  VP_test_r;
  Bool  VP_test;
  Bool  VP_train_DT;
  Bool  VP_use_DT;
  Bool  VP_keepaway;

  /* Client test params */
  int   CP_keepaway_mode; //used to test different breakaway styles
  float TP_keepaway_rect_length;
  float TP_keepaway_rect_width;

  /* Initialize params */
  int   IP_my_score;
  int   IP_their_score;
  int   IP_reconnect;    /* If non-zero, reconnect to that unum */  
  Bool  IP_only_connect; /* connect and then exit */
   
  /* Client params */
  char  MyTeamName[MAX_TEAMNAME_LEN];
  Bool  CP_goalie;
  Bool  CP_save_log;
  int   CP_save_freq;
  Bool  CP_save_sound_log;
  int   CP_save_sound_freq;
  /* 
  int   CP_save_action_log_level;
  int   CP_save_action_freq;
  * moved to shared */
  float CP_send_ban_recv_step_factor;
  int   CP_interrupts_per_cycle;
  int   CP_interrupts_left_to_act;
  Bool  CP_wait_for_sight;
  float CP_max_conf;
  float CP_min_valid_conf;
  float CP_conf_decay;
  float CP_player_conf_decay;
  float CP_ball_conf_decay;
  float CP_listen_conf_buffer;
  float CP_max_player_move_factor;  /* multiply by SP_player_speed_max to see how far a player can be 
				       from its last position and still be considered the same player */
  int   CP_max_say_interval;
  float CP_ball_moving_threshold;
  float CP_dodge_angle_buffer;
  float CP_dodge_distance_buffer;
  float CP_dodge_power;
  float CP_dodge_angle;
  char  CP_tree_stem[MAX_TREE_STEM_LEN];
  int   CP_DT_evaluate_interval;
  int   CP_say_tired_interval;
  float CP_tired_buffer;
  float CP_say_tired_buffer;

  Bool  CP_use_new_set_plays;  

  Bool  CP_set_plays;
  int   CP_Setplay_Delay;
  int   CP_Setplay_Say_Delay;
  int   CP_Setplay_Max_Delay;
  int   CP_Setplay_Time_Limit;

  float CP_kickable_buffer;
  int   CP_mark_persist_time;
  float CP_mark_max_dist_from_offside_line;
  int   CP_max_midfield_markers;
  int   CP_max_midfield_markers_when_tied;
  int   CP_max_midfield_markers_when_winning;
  int   CP_max_midfield_markers_when_losing;
  int   CP_max_midfield_markers_when_losing_by_lots;
  float CP_midfielder_leave_mark_min_x;

  float CP_track_max_distance;
  float CP_track_min_distance;
  Bool  CP_pull_offsides;
  Bool  CP_pull_offsides_when_winning;
  Bool  CP_spar;
  Bool  CP_mark;
  Bool  CP_communicate;
  int   CP_change_view_for_ball_cycles;
  float CP_defer_kick_to_teammate_buffer;
  float CP_scan_overlap_angle;

  float CP_pull_offside_threshold;
  float CP_pull_offside_buffer;
  
  float CP_ball_forget_angle_buf;
  float CP_player_forget_angle_buf;
  float CP_ball_forget_dist_buf;
  float CP_player_forget_dist_buf;
  
  /* pat added these */
  Bool CP_use_new_position_based_vel;
  Bool CP_stop_on_error;

  /* these parameters affect turnball */
  float CP_max_turn_kick_pow;
  float CP_opt_ctrl_dist;
  float CP_closest_margin;
  float CP_dokick_factor;

  /* these basically affect the way turnball stops */
  float CP_KickTo_err;
  float CP_max_ignore_vel;

  int   CP_kick_time_space;
  float CP_max_est_err;
  float CP_holdball_kickable_buffer;
  int   CP_stop_ball_power;
  int   CP_possessor_intercept_space;
  int   CP_can_keep_ball_cycle_buffer;
  
  /* no longer used
  float CP_hard_kick_margin;
  float CP_hard_kick_factor; 
  float CP_hard_kick_end_turn_dist; */
  float CP_hard_kick_dist_buffer;
  int   CP_max_hard_kick_angle_err;
  /* angle off perpendicualr to start ball for hardest kick */
  int   CP_hardest_kick_ball_ang; 
  float CP_hardest_kick_ball_dist; 
  int   CP_hardest_kick_player_ang;
  float CP_max_dash_help_kick_angle;
  
  int   CP_max_go_to_point_angle_err;
  int   CP_max_int_lookahead;
  float CP_intercept_close_dist;
  int   CP_intercept_step;
  int   CP_my_intercept_step;
  int   CP_intercept_aim_ahead;
  int   CP_no_turn_max_cyc_diff; /* used for normal interception */
  float CP_no_turn_max_dist_diff; /* used for ball_path intercept */
  float CP_turnball_opp_worry_dist; 
  float CP_collision_buffer;
  float CP_behind_angle;
  int   CP_time_for_full_rotation;
  float CP_ball_vel_invalidation_factor;

  /* dribble params */
  int   CP_dribble_dash_pow;
  float CP_dribble_ball_dist;
  float CP_dribble_behind_angle;
  /* dist where opponent starts to affect where we dribble ball */
  float CP_dribble_ignore_opp_dist ;
  /* dist of opponent that makes us go to DM_Strict mode */
  float CP_dribble_worry_opp_dist;
  /* angle we normally like to dribble at */
  float CP_dribble_angle_norm ;
  /* max and min distnaces to worry about dodging a player */
  float CP_dribble_dodge_max_dist;
  /* angle diff to make us turn if dodging */
  float CP_dribble_dodge_angle_err ;
  /* how far off in expected angle we let a ball before we kick it to correct */
  float CP_dribble_exp_angle_buffer ;
  /* if drib_ang > 180 - X, we will just dribble on the side where the ball is */ 
  float CP_dribble_angle_ignore_buffer;
  float CP_dribble_dodge_close_dist;
  float CP_can_dribble_cone_ratio;
  float CP_dribble_towards_length;
  float CP_dribble_sideline_buffer;
  float CP_dribble_circle_inner_rad;
  float CP_dribble_circle_outer_rad;
  float CP_dribble_circle_ang; //angle realtive to dribble angle to look for players
  Bool  CP_dribble_scan_field; 
  float CP_dribble_fast_max_dist;

  float CP_move_imp_1v1_initial;
  float CP_move_imp_1v1_inc;
  float CP_move_imp_1v1_threshold;
  float CP_at_point_buffer;
  float CP_overrun_dist;
  float CP_def_block_dist;
  float CP_def_block_dist_ratio;
  float CP_overrun_buffer;
  float CP_breakaway_buffer;
  float CP_our_breakaway_kickable_buffer;  
  float CP_their_breakaway_front_kickable_buffer;  
  float CP_their_breakaway_back_kickable_buffer;  
  float CP_goalie_breakaway_kickable_buffer;  
  
  float CP_breakaway_approach_x;
  float CP_breakaway_approach_y;
  int   CP_breakaway_targ_valid_time;
  int   CP_breakaway_min_goalie_steal_time;
  int   CP_breakaway_kick_run_min_cycles;
  int   CP_breakaway_kick_run_max_cycles;
  float CP_their_breakaway_min_cone_dist_wid;
  float CP_our_breakaway_min_cone_dist_wid;
  float CP_breakaway_middle_buffer;
  float CP_breakaway_kick_run_worry_dist;
  int   CP_breakaway_mode; // used to test diff breakaway styles
  
  float CP_beat_offsides_buffer;
  float CP_beat_offsides_threshold;
  float CP_beat_offsides_max_x;
  float CP_congestion_epsilon;
  float CP_back_pass_opponent_buffer;
  float CP_back_pass_offside_buffer;
  float CP_min_less_congested_pass_dist;

  float CP_no_dangerous_pass_or_dribble_x_thresh;
  float CP_min_pass_dist;
  float CP_min_dangerous_pass_dist;
  
  /* parameters for moving to a standing ball */
  float CP_static_kick_dist_err;
  float CP_static_kick_ang_err;
  /* no longer used 
  float CP_static_kick_dist;
  float CP_static_kick_ang;
  float CP_static_kick_overrun_dist;
  */
  
  float CP_goalie_baseline_buffer;
  float CP_goalie_scan_angle_err;
  float CP_goalie_at_point_buffer;
  float CP_goalie_vis_angle_err;
  float CP_goalie_max_shot_distance;
  float CP_goalie_min_pos_dist;
  float CP_goalie_max_pos_dist;
  float CP_goalie_max_forward_percent;
  float CP_goalie_ball_ang_for_corner;  
  float CP_goalie_max_come_out_dist;
  float CP_goalie_ball_dist_for_corner;
  float CP_goalie_ball_dist_for_center;
  float CP_goalie_free_kick_dist;
  float CP_goalie_go_to_ball_cone_ratio;
  int   CP_goalie_warn_space;
  Bool  CP_goalie_comes_out;
  int   CP_goalie_catch_wait_time;
  float CP_goalie_opponent_dist_to_block;
  float CP_goalie_always_block_dist;
  float CP_goalie_position_weight_dist;
  int   CP_goalie_narrow_sideline_cyc;
  float CP_goalie_no_buffer_dist;
  int   CP_goalie_forward_stamina_buffer;
  
  float CP_clear_ball_ang_step;
  float CP_clear_ball_cone_ratio;
  float CP_clear_ball_max_dist;
  float CP_clear_offensive_min_horiz_dist;
  float CP_clear_offensive_min_angle;

  float CP_should_cross_corner_dist;
  float CP_should_cross_baseline_buffer;
  float CP_should_consider_cross_baseline_buffer;
  float CP_should_move_to_cross_corner_dist;
  float CP_cross_pt_x;
  float CP_cross_pt_y;
  float CP_cross_target_vel;

  float CP_dont_dribble_to_middle_min_x;

  float CP_kickop_backang_step;
  float CP_kickop_frontang_step;
  float CP_min_kickop_vel;
  float CP_max_kickop_vel;
  float CP_kickop_vel_step;
  
  /* not used anymore
    float CP_hardest_kick_shot_distance;
    float CP_moderate_kick_shot_distance;
  */
  float CP_good_shot_distance;
  float CP_shot_distance;
  int   CP_cycles_to_kick_buffer;
  float CP_shot_speed;
  int CP_shot_goalie_react_buffer;
  int CP_good_shot_goalie_react_buffer;
  int CP_better_shot_cyc_diff;
  //float CP_breakaway_shot_distance; no longer used

  /* Formation params */
  char  FP_initial_formation[MAX_FP_LEN];
  char  FP_formation_when_tied[MAX_FP_LEN];
  char  FP_formation_when_losing[MAX_FP_LEN];
  char  FP_formation_when_losing_lots[MAX_FP_LEN];
  char  FP_formation_when_winning[MAX_FP_LEN];
  char  FP_initial_hc_method[MAX_FP_LEN];
  char  FP_initial_mc_method[MAX_FP_LEN];
  int   FP_initial_player_1_pos;
  int   FP_initial_player_2_pos;
  int   FP_initial_player_3_pos;
  int   FP_initial_player_4_pos;
  int   FP_initial_player_5_pos;
  int   FP_initial_player_6_pos;
  int   FP_initial_player_7_pos;
  int   FP_initial_player_8_pos;
  int   FP_initial_player_9_pos;
  int   FP_initial_player_10_pos;
  int   FP_initial_player_11_pos;
  int   FP_goalie_number;

  //0 is normal, 1 is always turn, 2 is go left, 3 is go right
  int CP_SPP_execution_type;

  Bool  CP_tune_timing;

#include <opt-incl.h>
};

#endif

