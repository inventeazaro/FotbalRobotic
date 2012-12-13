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


#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Memory.h"

extern Memory *Mem;

void turn(AngleDeg ang);
void dash(float power);
void kick(float power, AngleDeg dir);
void tackle(float power);
void goalie_catch(AngleDeg dir);
void score();
void move(Vector dest);
inline void move(float x, float y) { move(Vector(x,y)); };
void disconnect();
void bye(); //bye is like disconnect, except that it send it immediately
void clang(int min, int max);

void turn_neck(AngleDeg ang);
void change_view(Vqual qual, Vwidth width);
inline void change_view(Vqual qual) { change_view(qual,Mem->ViewWidth); }
inline void change_view(Vwidth width) { change_view(Mem->ViewQuality,width); }
void attention_to(char side, Unum num);
inline void attention_to_off() { attention_to(' ',0); }
void point_to(Vector dest, Bool off=FALSE);
inline void point_to_off() { point_to(Vector(0,0),TRUE); }

#endif
