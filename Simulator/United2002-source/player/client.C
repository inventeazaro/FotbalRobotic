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


#include "client.h"
#include "types.h"
#include "netif.h"
#include "Memory.h"
#include "parse.h"
#include "utils.h"
#include "behave.h"

void      send_initialize_message();
void      parse_initialize_message(char *);
Bool      wait_for_signals(sigset_t *);
sigset_t  init_handler();
void      sigio_handler(); 
void      sigalrm_handler();
void      send_action(char *msg_buff);
void      resend_last_action(char *msg_buff);
void      sigterm_handler();
void      sigsegv_handler();

/* Global variables -- don't want to reallocate buffers each time */

sigset_t sigiomask, sigalrmask;

//Memory  Global_Mem;
//Memory *const Mem = &Global_Mem;
Memory *Mem;

char     recvbuf[MAXMESG];	
char     sendbuf[MAXMESG];	

char     *GLOBAL_sense_body_message = "(sense_body)";

int      alrsigs_since_iosig=0;

/****************************************************************************************/

int main(int argc, char *argv[])
{
  Mem = new Memory();

  if ( Mem == NULL ){
    my_error("couldn't allocate Mem");
    exit(0);
  }

  Mem->GetOptions(argc,argv);

  Socket sock = init_connection(Mem->SP_host,Mem->SP_port);

  Mem->sock = &sock;

  if(Mem->sock->socketfd == -1) {
    cerr << "Can't open connection for player" << endl;
    exit(-1);
  }

  send_initialize_message();

  if ( wait_message(recvbuf, Mem->sock) == 0 )
    my_error("wait_message failed");

  parse_initialize_message(recvbuf);

  Mem->Initialize();
  score();  /* find out the score */

  if (Mem->IP_only_connect)
    return 0;

  sigset_t sigfullmask = init_handler();	      

  while ( Mem->ServerAlive == TRUE && wait_for_signals(&sigfullmask) );

  if (Mem->sock->socketfd != -1) close_connection(Mem->sock);

  printf("Shutting down player %d\n",Mem->MyNumber);
}




/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/* Send initialize message */
void send_initialize_message()
{
  if (Mem->IP_reconnect)
    sprintf(sendbuf, "(reconnect %s %d)", Mem->MyTeamName, Mem->IP_reconnect);
  else if ( Mem->CP_goalie == TRUE && Mem->SP_version >= 4.00){
    sprintf(sendbuf, "(init %s (version %.2f) (goalie))", Mem->MyTeamName, Mem->SP_version);
  }
  else
    sprintf(sendbuf, "(init %s (version %.2f))", Mem->MyTeamName, Mem->SP_version);

  if(send_message(sendbuf, Mem->sock) == -1) abort(); 
}


/****************************************************************************************/

/* Parse initialize message */
void parse_initialize_message(char *recvbuf){
  char mode[100];
  if ( !(strncmp(recvbuf,"(init",4)) ) {
    /* It's an init msg */
    sscanf(recvbuf,"(init %c %d %[^)]",&Mem->MySide, &Mem->MyNumber, mode);
    Mem->ServerAlive = TRUE;
  }
  else if ( !(strncmp(recvbuf,"(reconnect",4)) ) {
    /* It's a reconnect msg */
    sscanf(recvbuf,"(reconnect %c %[^)]",&Mem->MySide, mode);
    Mem->MyNumber = Mem->IP_reconnect;
    printf("reconnecting to %d on side %c!\n",Mem->MyNumber,Mem->MySide);
    Mem->ServerAlive = TRUE;
  }
  else {
    my_error("Didn't get an init message: '%s'",recvbuf);
    Mem->ServerAlive = FALSE;
  }

  if ( Mem->CP_goalie && Mem->FP_goalie_number != Mem->MyNumber )
    my_error("goalie number inconsistent with me being goalie");

  if ( !Mem->CP_goalie && Mem->FP_goalie_number == Mem->MyNumber )
    my_error("I should be the goalie");
  
  if ( mode[0] == 'b' ){ /* Before_kick_off */ 
    if (Mem->IP_reconnect != 0) {
      Mem->SetPlayMode(PM_Half_Time);
      Mem->SetPlayMode(PM_Before_Kick_Off);
      if ( Mem->MySide == 'l' )
	Mem->KickOffMode = KO_Theirs;
      else 
	Mem->KickOffMode = KO_Mine;
    } else {
      Mem->SetPlayMode(PM_Before_Kick_Off);
      if ( Mem->MySide == 'l' )
	Mem->KickOffMode = KO_Mine;
      else 
	Mem->KickOffMode = KO_Theirs;
    }
  }
  else                  /* Act as if the game's in progress */
    Mem->SetPlayMode(PM_Play_On);

  clang(8,8);  /* support clang messages version 8 */
}



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/* set time interval between the sensor receiving and command sending */ 
inline void set_timer() {
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = Mem->TimerInterval * 1000;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = Mem->TimerInterval * 1000;
  setitimer(ITIMER_REAL, &itv, NULL);
}

inline void set_timer(int usec) {
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = Mem->TimerInterval * 1000;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = usec;
  setitimer(ITIMER_REAL, &itv, NULL);
}


/****************************************************************************************/

sigset_t init_handler() { 
  sigemptyset(&sigalrmask);
  sigaddset(&sigalrmask, SIGALRM);
  sigemptyset(&sigiomask);
  sigaddset(&sigiomask, SIGIO);
  
  struct sigaction sigact;
  sigact.sa_flags = 0;

  sigact.sa_mask = sigiomask;
  sigact.sa_handler = (void (*)(int))sigalrm_handler; 
  sigaction(SIGALRM, &sigact, NULL);

  sigact.sa_mask = sigalrmask;
  sigact.sa_handler = (void (*)(int))sigio_handler; 
  sigaction(SIGIO, &sigact, NULL);

  //the signals to be blocked while this executes
  sigemptyset(&sigact.sa_mask);
  sigaddset(&sigact.sa_mask, SIGALRM);
  sigaddset(&sigact.sa_mask, SIGIO);
  sigact.sa_handler = (void (*)(int))sigterm_handler; 
  sigaction(SIGINT, &sigact, NULL);
  sigaction(SIGQUIT, &sigact, NULL);
  sigaction(SIGTERM, &sigact, NULL);

  sigemptyset(&sigact.sa_mask);
  sigact.sa_handler = (void (*)(int))sigsegv_handler; 
  sigaction(SIGSEGV, &sigact, NULL);

  set_timer();
  sigprocmask(SIG_UNBLOCK, &sigiomask, NULL);
  sigprocmask(SIG_UNBLOCK, &sigalrmask, NULL);

  sigset_t sigsetmask;
  sigprocmask(SIG_BLOCK, NULL, &sigsetmask);   /* Get's the currently unblocked signals */
  return sigsetmask;   
}


/****************************************************************************************/

/* suspend the process until one of the signals comes through */
/* could check for situation to kill client, return FALSE     */
/* i.e. too many actions with no sensory input coming in      */
Bool wait_for_signals(sigset_t *mask){
  sigsuspend(mask);
  return TRUE;
}


/****************************************************************************************/

/* SIGIO handler: receive and parse messages from server */
void sigio_handler() {
  sigprocmask(SIG_BLOCK, &sigalrmask, NULL);  
  int counter = 0;

  Time StartTime = Mem->CurrentTime;

  while (receive_message(recvbuf, Mem->sock) == 1) {
    Parse(recvbuf);
    counter++;
  }

  if ( Mem->CurrentTime - StartTime > 1 && StartTime.s == 0 && Mem->CurrentTime.s == 0 )
    my_error("Received several steps at once -- missing action ops!!! (%d %d)",
	     StartTime.t,StartTime.s);

  sigprocmask(SIG_UNBLOCK, &sigalrmask, NULL);  

  alrsigs_since_iosig=0;

  // if (counter>1) printf("Got %d messages\n",counter);
}


/****************************************************************************************/

/* SIGALRM handler: extract and send first command in commandlist */
void sigalrm_handler() {
  sigprocmask(SIG_BLOCK, &sigiomask, NULL);  

  if ( Mem->LastInterruptTime != Mem->CurrentTime ){ 
    if ( !Mem->ClockStopped && Mem->CurrentTime-1 != Mem->LastInterruptTime ) my_error("Missed a cycle??"); 
    if ( !Mem->ClockStopped && Mem->InterruptsThisCycle < Mem->CP_interrupts_per_cycle-1 )
      my_error("Only %d interrupts last cycle",Mem->InterruptsThisCycle);
    Mem->LastInterruptTime = Mem->CurrentTime; 
    Mem->InterruptsThisCycle = 0; 
  } 
  Mem->InterruptsThisCycle++; 

  /* Don't act until near the end of a cycle */
  /* there's some leeway in case there aren't enough interrupts in the cycle */
  Bool behave_time = TRUE;
  if ( Mem->CP_interrupts_per_cycle - Mem->InterruptsThisCycle >
       Mem->CP_interrupts_left_to_act){
    behave_time = FALSE;
  }

  if (Mem->ClockStopped)
    Mem->StoppedClockMSec += Mem->TimerInterval;

  if (alrsigs_since_iosig++ > Mem->CP_interrupts_per_cycle * Mem->CP_max_cycles_since_io){
    Mem->ServerAlive = FALSE;
    sigprocmask(SIG_UNBLOCK, &sigiomask, NULL);  
    return;
  }

  /* If a sight is definitely coming every cycle, don't act until getting the sight */
  /* Don't wait if we're in transition to a longer sight interval                   */
  if ( Mem->CP_wait_for_sight &&
       Mem->MySightInterval() < Mem->SP_simulator_step && 
       Mem->LastSightTime < Mem->CurrentTime &&
       !((Mem->ChangeView.valid() || Mem->ChangeView.valid(Mem->CurrentTime-1)) &&
	 (Mem->ChangeView.width > Mem->ViewWidth || Mem->ChangeView.qual > Mem->ViewQuality)) )   {
    LogAction4(200,"Waiting for sight... (%d %d)",
		    Mem->ChangeView.valid(),Mem->ChangeView.valid(Mem->CurrentTime-1));
    sigprocmask(SIG_UNBLOCK, &sigiomask, NULL);  
    return;
  }

  if ( Mem->CurrentTime > Mem->LastActionOpTime && behave_time ){

    if ( !Mem->ClockStopped && Mem->CurrentTime-1 != Mem->LastActionOpTime && Mem->LastActionOpTime != 0 )
      my_error("Missed a cycle!!  (%d %d)",Mem->LastActionOpTime.t,Mem->LastActionOpTime.s);

    if ( Mem->NewSight ) Mem->FirstActionOpSinceLastSight = TRUE;

    Mem->update();
    behave();

    //my_error("acting after %d interrupts",Mem->InterruptsThisCycle);

    Mem->LastActionOpTime = Mem->CurrentTime;
    Mem->FirstActionOpSinceLastSight = FALSE;
  }
  else if ( Mem->Tuned ){
    sigprocmask(SIG_UNBLOCK, &sigiomask, NULL);  
    return;
  }

  /* the server now accepts multiple commands together  (after 5.23) 
  if (0 && Mem->TooSoonForAnotherSend()) {
    LogAction2(200, "It's too soon to send another command. Waiting");
  } else {
  */

  /* However, dumping on the network all at once seems to cause some problems, so we'll still
     send one action per interrupt (we should be interrupting quite often now) */
  
  sendbuf[0] = 0;

  /* do this before the action so it's processed first */
  if ( Mem->PointTo.valid() ){
    if ( Mem->PointTo.time < Mem->CurrentTime-1 ) my_error("old point_to");
    //send_message( Mem->PointTo.command, Mem->sock );
    if ( sendbuf[0] == 0 )
      strcpy( sendbuf, Mem->PointTo.command );
    else
      strcat( sendbuf, Mem->PointTo.command );
    Mem->PointTo.type = CMD_none; /* so it's no longer valid */
  }
  if ( Mem->Action->valid() ) {
    send_action(sendbuf);
  }
  //else 
  if ( Mem->ResendNeeded() ){
    resend_last_action(sendbuf);
  }
  //else 
  if ( Mem->TurnNeck.valid() ){
    if ( Mem->TurnNeck.time < Mem->CurrentTime-1 ) my_error("old turn_neck");

    //send_message( Mem->TurnNeck.command, Mem->sock );
    if ( sendbuf[0] == 0 )
      strcpy( sendbuf, Mem->TurnNeck.command );
    else
      strcat( sendbuf, Mem->TurnNeck.command );
    Mem->turn_necks++;
    Mem->TurnNeck.type = CMD_none; /* so it's no longer valid */
  }
  //else
  if ( Mem->ChangeView.valid() ){
    if ( Mem->ChangeView.time < Mem->CurrentTime-1 && Mem->Tuned )
      my_error("old change_view");

    Bool send_change_view = TRUE;
    if ( Mem->LastChangeViewTime == Mem->CurrentTime )
      send_change_view = FALSE;  //already sent one this cycle

    if ( Mem->CP_tune_timing && send_change_view ){
      if ( Mem->ChangeView.qual==VQ_High && Mem->ChangeView.width==VW_Normal ){
	if ( Mem->SP_send_step / Mem->MySightInterval() == 4 ){
	  if ( Mem->SightsPrev2Cycle != 3 || Mem->SightsPrevCycle != 2 ||
	       Mem->SightsThisCycle != 1 ){
	    //    my_error("Tuning Conditions not met 1 (%d %d %d)\n",
	    //     Mem->SightsPrev2Cycle, Mem->SightsPrevCycle, Mem->SightsThisCycle);
	    send_change_view = FALSE; // don't do it until those conditions are met
	  }
	  else{
	    my_error("tuned");
	    Mem->Tuned = TRUE;
	  }
	}
	else if ( Mem->SP_send_step / Mem->MySightInterval() == 2 ){
	  if ( Mem->SightsPrev2Cycle != 1 || Mem->SightsPrevCycle != 1 ||
	       Mem->SightsThisCycle != 1 ){
	    //my_error("TuningConditions not met 2 (%d %d %d)\n",
	    //     Mem->SightsPrev2Cycle, Mem->SightsPrevCycle, Mem->SightsThisCycle);
	    send_change_view = FALSE; // don't do it until those conditions are met
	  }
	  else{
	    Mem->Tuned = TRUE;
	  }
	}
      }
      if ( Mem->ChangeView.qual < VQ_High || Mem->ChangeView.width < VW_Normal )
	Mem->Tuned = FALSE;
    }
    if ( send_change_view ){
      //send_message( Mem->ChangeView.command, Mem->sock );
      if ( sendbuf[0] == 0 )
	strcpy( sendbuf, Mem->ChangeView.command );
      else
	strcat( sendbuf, Mem->ChangeView.command );
      Mem->ChangeView.type = CMD_none; /* so it's no longer valid */
      Mem->LastChangeViewTime = Mem->CurrentTime;
    }
  }
  if ( Mem->AttentionTo.valid() ){
    if ( Mem->AttentionTo.time < Mem->CurrentTime-1 ) my_error("old attention_to");
    //send_message( Mem->AttentionTo.command, Mem->sock );
    if ( sendbuf[0] == 0 )
      strcpy( sendbuf, Mem->AttentionTo.command );
    else
      strcat( sendbuf, Mem->AttentionTo.command );
    Mem->AttentionTo.type = CMD_none; /* so it's no longer valid */
  }
  if ( Mem->Ear.valid() ){
    if ( Mem->Ear.time < Mem->CurrentTime-1 ) my_error("old ear");
    //send_message( Mem->AttentionTo.command, Mem->sock );
    if ( sendbuf[0] == 0 )
      strcpy( sendbuf, Mem->Ear.command );
    else
      strcat( sendbuf, Mem->Ear.command );
    Mem->Ear.type = CMD_none; /* so it's no longer valid */
  }
  //else
  //  if ( Mem->Communicate() ){
  // LogAction2(200,"saying something");
  //send_message( Mem->SayBuffer(), Mem->sock );
  // if ( sendbuf[0] == 0 )
  //   strcpy( sendbuf, Mem->SayBuffer() );
  //  else
  //    strcat( sendbuf, Mem->SayBuffer() );
  //}
  if ( sendbuf[0] != 0 )
    send_message( sendbuf, Mem->sock );

  sigprocmask(SIG_UNBLOCK, &sigiomask, NULL);  
}

/*****************************************************************************/

/* watch for user kills and send bye */
void sigterm_handler() 
{
  my_error("Got some sort of term signal, sending bye and shutting down");
  bye();
  Mem->FlushFiles();
  Mem->ServerAlive = FALSE;
}

/*****************************************************************************/

/* watch for user kills and send bye */
void sigsegv_handler() 
{
  my_error("Seg fault: sending bye");
  bye();
  Mem->FlushFiles();
  //Mem->ServerAlive = FALSE;
  //reset to the default action
  struct sigaction sigact;
  sigact.sa_flags = 0;
  sigact.sa_handler = SIG_DFL;
  sigaction(SIGSEGV, &sigact, NULL);
}



/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

/* insert turn/dash/kick commands in commandlist */

void turn(AngleDeg ang) 
{
  NormalizeAngleDeg(&ang); 

  /* turn so that the actual turn is the desired turn */
  if ( Mem->MyVelConf() ) ang *= (1 + Mem->My_inertia_moment() * Mem->MySpeed());

  if ( ang > Mem->SP_max_moment ) ang = Mem->SP_max_moment;
  if ( ang < Mem->SP_min_moment ) ang = Mem->SP_min_moment;

  if (ang < .1 && ang > -.1) {
    Mem->Action->type = CMD_none;
    return;          /* No turn           */
  }

  Mem->Action->type = CMD_turn;
  Mem->Action->power = 0;
  Mem->Action->angle = ang;
  Mem->Action->time = Mem->CurrentTime;

  if (Mem->TurnNeck.time == Mem->CurrentTime){ /* Already did a turn_neck     */
    /* adjust as much as possible for the turn */
    Mem->TurnNeck.angle -= ang;
    if ( Mem->MyNeckRelAng() + Mem->TurnNeck.angle < Mem->SP_min_neck_angle )
      Mem->TurnNeck.angle = Mem->SP_min_neck_angle - Mem->MyNeckRelAng();
    if ( Mem->MyNeckRelAng() + Mem->TurnNeck.angle > Mem->SP_max_neck_angle )
      Mem->TurnNeck.angle = Mem->SP_max_neck_angle - Mem->MyNeckRelAng();
  }

  /* SMURF we should check that we don't send any turn_neck 0's to the server */

  sprintf(Mem->Action->command,"(turn %.2f)", ang);
  LogAction3(150, "turn %f", ang);
}  

/****************************************************************************************/

void dash(float power) 
{
  if ( Mem->PlayMode == PM_Before_Kick_Off ) {
    LogAction2(150, "Refusing to dash in before kick off mode");
    return;
  }

  if (power > Mem->SP_max_power) my_error("Can't dash that fast: %.1f",power); 
  if (power < Mem->SP_min_power) my_error("Can't dash that 'slow': %.1f",power);

  /* Factor for stamina--don't dash more than stamina or more than necessary to get you to max speed */
  Mem->VerifyDash(&power);

  if (fabs(power) < 1){
    Mem->Action->type = CMD_none;
    LogAction3(150, "Refusing to dash < 1 power (%f)",power);
    return;                         /* No dash           */
  }
  
  Mem->Action->type = CMD_dash;
  Mem->Action->power = power;
  Mem->Action->angle = 0;
  Mem->Action->time = Mem->CurrentTime;

  sprintf(Mem->Action->command, "(dash %.2f)", power);
  LogAction3(150, "dash %f", power);
}

/****************************************************************************************/

void kick(float power, AngleDeg dir) 
{
  if ( !(Mem->BallKickable()) ) my_error("Can't kick a ball that's too far away");

  if ( Mem->PlayMode == PM_Before_Kick_Off ) return;

  if (power > Mem->SP_max_power) my_error("Can't kick that hard");
  if (power < 0 ) my_error("Can't kick < 0");
  NormalizeAngleDeg(&dir);

  Mem->Action->type = CMD_kick;
  Mem->Action->power = power;
  Mem->Action->angle = dir;
  Mem->Action->time = Mem->CurrentTime;

  sprintf(Mem->Action->command, "(kick %.2f %.2f)", power, dir);
  LogAction4(150, "kick %f %f", power, dir);
}

/****************************************************************************************/

void tackle(float power) 
{
  if ( Mem->PlayMode == PM_Before_Kick_Off ) {
    LogAction2(150, "Refusing to tackle in before kick off mode");
    return;
  }

  if (power > Mem->SP_max_power) my_error("Can't tackle that fast: %.1f",power); 
  if (power < Mem->SP_min_power) my_error("Can't tackle that 'slow': %.1f",power);

  Mem->Action->type = CMD_tackle;
  Mem->Action->power = power;
  Mem->Action->angle = 0;
  Mem->Action->time = Mem->CurrentTime;

  sprintf(Mem->Action->command, "(tackle %.2f)", power);
  LogAction3(150, "tackle %f", power);
}

/****************************************************************************************/

void goalie_catch(AngleDeg dir) 
{
  if (!Mem->GetBall()->catchable())
    my_error("!Catch: GetBall");
  if (!Mem->InOwnPenaltyArea())
    my_error("!Catch: PenArea %f %f %f %f %f",
	     Mem->MyX(),
	     Mem->MarkerX(Mem->RM_My_PC_Flag),
	     Mem->MarkerX(Mem->RM_My_Goal),
	     fabs(Mem->MyY()),
	     Mem->SP_penalty_area_width);

  if (Mem->PlayMode != PM_Play_On)
    my_error("!Catch: PM");
  if ( !(Mem->BallCatchable()) ) my_error("Can't catch a ball that's too far away");
  if ( !Mem->CP_goalie ) my_error("Only goalies can catch");

  if ( Mem->PlayMode == PM_Before_Kick_Off ) return;

  NormalizeAngleDeg(&dir);

  Mem->Action->type = CMD_catch;
  Mem->Action->power = 0;
  Mem->Action->angle = dir;
  Mem->Action->time = Mem->CurrentTime;

  sprintf(Mem->Action->command, "(catch %.2f)", dir);
  LogAction3(150, "catch %f",  dir);
}

/****************************************************************************************/

void score()
{
  static Bool done = FALSE;

  if ( done==TRUE ){
    my_error("Why asking score again?");
  }
  send_message("(score)", Mem->sock);  /* assuming it's only done once */
  done = TRUE;
}

/****************************************************************************************/

void move(Vector dest) 
{
  if ( ! (Mem->PlayMode == PM_Before_Kick_Off || 
	  (Mem->CP_goalie && Mem->PlayMode == PM_My_Goalie_Free_Kick)) )
    my_error("Can only move in before kickoff mode (or after goalie catch)");

  if ( Mem->MyConf() && Mem->DistanceTo(dest)<Mem->CP_at_point_buffer ){
    if ( Mem->Action->valid() )
      Mem->Action->type  = CMD_none;
    return;  /* my_error("Nothing to change about view"); */
  }

  float x = dest.x;
  float y = dest.y;  
  /* Perhaps here convert to a position on the field */
  if ( fabs(y) > Mem->SP_pitch_width/2 || fabs(x) > Mem->SP_pitch_length/2 )
    my_error("Must move to a place on the pitch");

  if ( x > 0 && Mem->SP_forbid_kickoff_offside )
    my_error("Must move to a place on my side of the pitch");

  if ( Mem->PlayMode == PM_My_Goalie_Free_Kick && !Mem->OwnPenaltyArea.IsWithin(Vector(x,y)) )
    my_error("Must move to a place within penalty area");

  Mem->Action->type = CMD_move;
  Mem->Action->dest = dest;
  Mem->Action->time =  Mem->CurrentTime;

  sprintf(Mem->Action->command, "(move %.2f %.2f)", x, y);
  LogAction4(150, "move %f %f", x, y);
}

/****************************************************************************************/

void disconnect()
{
  Mem->Action->type = CMD_bye;
  Mem->Action->time =  Mem->CurrentTime;

  sprintf(Mem->Action->command, "(bye)");
}

/****************************************************************************************/

void bye()
{
  send_message("(bye)", Mem->sock);
}

/****************************************************************************************/

void clang(int min, int max)
{
  char msg[100];
  sprintf(msg,"(clang (ver %d %d))",min,max);
  send_message(msg, Mem->sock);
}

/****************************************************************************************/

void turn_neck(AngleDeg ang) 
{
  NormalizeAngleDeg(&ang); 

  if (ang == 0) {
    LogAction2(150, "Ignoring turn_neck 0");
    return;
  }  

  if ( ang > Mem->SP_max_neck_moment ) ang = Mem->SP_max_neck_moment;
  if ( ang < Mem->SP_min_neck_moment ) ang = Mem->SP_min_neck_moment;

  if ( Mem->MyNeckRelAng() + ang > Mem->SP_max_neck_angle ) {
    ang = Mem->SP_max_neck_angle - Mem->MyNeckRelAng();
    my_error("Can't turn neck that much");
  }
  
  if ( Mem->MyNeckRelAng() + ang < Mem->SP_min_neck_angle ) {
    ang = Mem->SP_min_neck_angle - Mem->MyNeckRelAng();
    my_error("Can't turn neck that little");
  }
  
  Mem->TurnNeck.type = CMD_turn_neck;
  Mem->TurnNeck.power = 0;
  Mem->TurnNeck.angle = ang;
  Mem->TurnNeck.time = Mem->CurrentTime;  

  sprintf(Mem->TurnNeck.command,"(turn_neck %.2f)", ang);
  LogAction3(150, "turn_neck %f", ang);
}

/****************************************************************************************/

void change_view(Vqual qual, Vwidth width) 
{
  if ( qual==Mem->ViewQuality  && width==Mem->ViewWidth ){
    if ( Mem->ChangeView.valid() )
      Mem->ChangeView.type  = CMD_none;
    return;  /* my_error("Nothing to change about view"); */
  }

  Mem->ChangeView.type  = CMD_change_view;
  Mem->ChangeView.qual  = qual;
  Mem->ChangeView.width = width;
  Mem->ChangeView.time  = Mem->CurrentTime;

  char qual_string[10], width_string[10];
  switch (qual){
  case VQ_High: sprintf(qual_string,"high"); break;
  case VQ_Low:  sprintf(qual_string,"low");  break;
  }
  switch (width){
  case VW_Narrow: sprintf(width_string,"narrow"); break;
  case VW_Normal: sprintf(width_string,"normal"); break;
  case VW_Wide:   sprintf(width_string,"wide");   break;
  }

  sprintf(Mem->ChangeView.command, "(change_view %s %s)", width_string, qual_string);
  LogAction4(150, "change_view %s %s",  width_string, qual_string);
}

/****************************************************************************************/

void attention_to(char side, Unum num)
{
  if ( side==Mem->AttentionSide  && num==Mem->AttentionNum ){
    if ( Mem->AttentionTo.valid() )
      Mem->AttentionTo.type  = CMD_none;
    return;  /* my_error("Nothing to change about view"); */
  }

  Mem->AttentionTo.type  = CMD_attention_to;
  Mem->AttentionTo.side  = side;
  Mem->AttentionTo.num   = num;
  Mem->AttentionTo.time  = Mem->CurrentTime;

  if ( num == 0 )
    sprintf(Mem->AttentionTo.command, "(attentionto off)");
  else
    sprintf(Mem->AttentionTo.command, "(attentionto %c %d)", side, num);
  LogAction3(150, "%s", Mem->AttentionTo.command);
}

/****************************************************************************************/

void point_to(Vector dest, Bool off)
{
  if ( !Mem->MyConf() ){
    my_error("Need to know my location to point");
    return;
  }
  if ( Mem->arm_busy ){
    my_error("arm not movable for %d cycles",Mem->arm_busy);
    return;
  }

  if ( Mem->arm_expires && dest.dist(Mem->PointDest)<Mem->CP_at_point_buffer ){
    if ( Mem->PointTo.valid() )
      Mem->PointTo.type  = CMD_none;
    return;  /* my_error("Nothing to change about view"); */
  }

  Mem->PointTo.type  = CMD_point_to;
  Mem->PointTo.dest  = dest;
  Mem->PointTo.time  = Mem->CurrentTime;

  if ( off == TRUE )
    sprintf(Mem->PointTo.command, "(pointto off)");
  else{
    float dist = Mem->DistanceTo(dest);
    AngleDeg ang = Mem->AngleToFromNeck(dest);
    //my_error("changing from (%.2f %.2f) to (%.2f %.2f) - dist/ang = %.2f/%.2f",
    //Mem->PointDest.x,Mem->PointDest.y,dest.x,dest.y,dist,ang);
    sprintf(Mem->PointTo.command, "(pointto %f %f)", dist, ang);
  }
  LogAction3(150, "%s", Mem->PointTo.command);
}




/****************************************************************************************/

void send_action(char *msg_buff)
{	
  if ( !(Mem->Action->valid(Mem->CurrentTime)) )
    my_error("Old action %d %d",Mem->Action->time.t,Mem->Action->time.s);

  if ( Mem->Tackling() ){
    my_error("Still tackling for %d cycles",Mem->tackle_expires);
    if ( Mem->Action->valid() )
      Mem->Action->type = CMD_none;
    return;
  }

  //send_message ( Mem->Action->command, Mem->sock );
  if ( msg_buff[0] == 0 )
    strcpy (msg_buff, Mem->Action->command );
  else
    strcat( msg_buff, Mem->Action->command );

  switch (Mem->Action->type){
  case CMD_kick:
    Mem->kicks++;
    Mem->GetBall()->set_past_kick(Mem->Action->power, Mem->Action->angle,
				  Mem->CurrentTime);
    break;
  case CMD_dash: Mem->dashes++; break;
  case CMD_turn: Mem->turns++;  break;
  default: ;
  }

  Command *tmp = Mem->LastAction;
  Mem->LastAction = Mem->Action;
  Mem->Action     = tmp;

  Mem->Action->type = CMD_none; /* So it's invalid */
  Mem->NewAction = TRUE;
}

/****************************************************************************************/

void resend_last_action(char *msg_buff)
{
  if ( Mem->LastActionType() == Mem->ResendType ){
    my_stamp; printf("resending\n");
    //send_message(Mem->LastAction->command, Mem->sock);
    if ( msg_buff[0] == 0 )
      strcpy (msg_buff, Mem->LastAction->command );
    else
      strcat( msg_buff, Mem->LastAction->command );

    switch (Mem->LastActionType()){
    case CMD_kick: Mem->kicks++;  break;
    case CMD_dash: Mem->dashes++; break;
    case CMD_turn: Mem->turns++;  break;
    default: ;
    }
  }
  else 
    my_error("last action isn't a %d",Mem->ResendType);

  Mem->RequestResend = FALSE;
}
