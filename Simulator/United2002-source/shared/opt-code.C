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


/* this file is meant to be included in a MemOption.C
   this is all the reading in code */
/* skip command name */
  argv++ ; argc-- ;

  /* first, search option '-file' */
  int i ;
  FILE *fp ;
  for(i = 0 ; i < argc ; i++) {
    if (!strcmp(*(argv + i),"-file") || !strcmp(*(argv + i),"-pfile") || 
	!strcmp(*(argv + i),"-sfile")) {
      if ((fp = fopen(*(argv+i+1),"r")) == NULL) {
	cerr << "cat't open config file " << *(argv+i+1) << endl ;
	break ;
      }
			
      char buf[100] ;
      while(fgets(buf,100,fp) != NULL) {
				/* ignore remark line */
	char *t = buf ;
	advance_past_space(&t);
	if (t[0] == '#' || t[0] == '\n' || t[0] == 0)
	  continue ;

	//printf("Processing line '%s'\n", buf);

				/* replace from ':' to ' ' */
	t = buf;
	while(*t != NULLCHAR) {
	  if (*t == ':') *t = ' ' ;
	  t++ ;
	}

	int n, p ;
	char com[256] ;
	char onoff[16] ;
	n = sscanf(buf,"%s", com) ;
	if (n < 1) {
	  cerr << "Illegal line : " << buf ;
	  continue ;
	}

	for (p = 0 ; opt[p].vptr != NULL ; p++) {
	  if (strcmp(com, opt[p].optname))
	    continue ;

	  /* match */
	  switch(opt[p].vsize) {
	  case V_INT:
	    n = sscanf(buf, "%s %d", com, (int *)opt[p].vptr) ;
	    break ;

	  case V_STRING:
	    n = sscanf(buf, "%s %s", com, (char *)opt[p].vptr) ;
	    break ;

	  case V_FLOAT:
	    n = sscanf(buf, "%s %f", com, (float *)opt[p].vptr) ;
	    break ;

	  case V_BOOL:
	    n = 2 ;
	    *((Bool *)opt[p].vptr) = TRUE ;
	    break ;

	  case V_ONOFF:
	    n = sscanf(buf, "%s %s", com, onoff) ;
	    if (n == 1) {
	      n = 2;
	      *((Bool *)opt[p].vptr) = TRUE;
	    } else
	      *((Bool *)opt[p].vptr) = (!strcmp(onoff, "on")) ? TRUE :FALSE;
	    break ;
	    /* old code: the better stuff above was taken from the player
	    n = sscanf(buf, "%s %s", com, onoff) ;
	    *((Bool *)opt[p].vptr) = (!strcmp(onoff, "on")) ? TRUE :FALSE;
	    break ;
	    */
	  }

	  if (n < 2)
	    cerr << "Illegal line (" << com << ") " << endl ;

	  break ;
	}
				
	if (opt[p].vptr == NULL)
	  cerr << "Illegal line (" << com << ") " << endl ;
      }

      fclose(fp) ;
      /* break ; */   // Without this, more than one file can be read
    }
  }

  /* next, analyze command line option */
  int p ;

  while (argc) {
    if (!strcmp(*argv, "-file") || !strcmp(*argv, "-sfile") || !strcmp(*argv, "-pfile")){
      argv += 2 ;
      argc -= 2 ;
      continue ;
    }
			
    for (p = 0 ; opt[p].vptr != NULL ; p++) {
      if (strcmp(*argv + 1, opt[p].optname))
	continue ;

      //      printf("Processing arg '%s'\n", opt[p].optname);

      /* match */
      argv++ ;
      argc-- ;

      switch(opt[p].vsize) {
      case V_INT:
	*((int *)opt[p].vptr) = atoi(*argv) ;
	break ;

      case V_STRING:
	strcpy((char *)opt[p].vptr, *argv) ;
	break ;

      case V_FLOAT:
	*((float *)opt[p].vptr) = atof(*argv) ;
	break ;

      case V_BOOL:
	*((Bool *)opt[p].vptr) = TRUE ;
	argv-- ;
	argc++ ;
	break ;

      case V_ONOFF:
	if (argc > 0 && (*argv)[0] != '-') {	  
	  *((Bool *)opt[p].vptr) = (!strcmp(*argv, "on")) ? TRUE : FALSE ;
	} else {
	  /* if there's nothing specified, then we set it to true */
	  *((Bool *)opt[p].vptr) = TRUE;
	  argv-- ;
	  argc++ ;
	}
	break ;
      }

      break ;
    }

    if (opt[p].vptr == NULL){
      cerr << "Unrecognized Option : " << *argv ;
      argv--;
      cerr << "    previous was : " << *argv << endl ;
      argv++;
    }

    argv++ ;
    argc-- ;
  }

  SP_half_time = SP_half_time * 1000 / SP_simulator_step ;
  SP_kickable_area = SP_kickable_margin + SP_ball_size + SP_player_size ;
  SP_simulator_step  *= SP_slow_down_factor;
  SP_sense_body_step *= SP_slow_down_factor;
  SP_send_vi_step    *= SP_slow_down_factor;
  SP_send_step       *= SP_slow_down_factor;
  SP_player_global_speed_max = SP_player_speed_max + PP_player_speed_max_delta_max;
  SP_player_min_size = SP_player_size + 
                        PP_player_size_delta_factor * PP_dash_power_rate_delta_max;
  SP_player_max_size = SP_player_size + 
                        PP_player_size_delta_factor * PP_dash_power_rate_delta_min;
  if ( SP_player_max_size > SP_player_min_size ){
    float tmp = SP_player_min_size;
    SP_player_min_size = SP_player_max_size;
    SP_player_max_size = tmp;
  }
  SP_max_kickable_area = SP_kickable_margin + PP_kickable_margin_delta_min + 
                          SP_ball_size + SP_player_max_size;
