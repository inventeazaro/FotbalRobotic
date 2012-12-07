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


#ifndef _UTILS_
#define _UTILS_

#include <istream>
#include <math.h>
#include <fstream>
#include "types_all.h"

using namespace std;

void dump_core(char*);

void my_error(char*);
void my_error(char*,int);
void my_error(char*,int,int);
void my_error(char*,int,int,int);
void my_error(char*,int,int,int,int);
void my_error(char*,int,int,int,int,int);
void my_error(char*,int,int,int,int,int,int);
void my_error(char*,int,int,int,int,int,char,int);
void my_error(char*,float);
void my_error(char*,float,float);
void my_error(char*,float,float,float);
void my_error(char*,float,float,float,float);
void my_error(char*,float,float,float,float,float);
void my_error(char*,float,float,float,float,float,float);
void my_error(char*,int,float);
void my_error(char*,float,int);
void my_error(char*,int,int,int,float);
void my_error(char*,char*);
void my_error(char*,char,int,int);
void my_error(char*,char,char,char);
void my_error(char*,char,int,float,float);
void my_error(char*, char*, int);
void my_error(char*, char*, int, int);
void my_error(char*, char*, char*);
void my_error(char*, char*, char*, char*);


int closest_int( float x );

typedef float Value;
typedef float AngleRad;
typedef float AngleDeg;

inline AngleDeg Rad2Deg(AngleRad x) { return x * 180 / M_PI; }
inline AngleRad Deg2Rad(AngleDeg x) { return x * M_PI / 180; }

/* needed? */
/* inline float cos(AngleRad x) { return cos((float) x); } */
/* inline float sin(AngleRad x) { return sin((float) x); } */
/* inline float tan(AngleRad x) { return tan((float) x); } */

inline float Cos(AngleDeg x) { return cos(Deg2Rad(x)); }
inline float Sin(AngleDeg x) { return sin(Deg2Rad(x)); }
inline float Tan(AngleDeg x) { return tan(Deg2Rad(x)); }
inline AngleDeg ACos(float x)           { return ((x) >= 1 ? 0 : ((x) <= -1 ? 180 : (Rad2Deg(acos(x))))); }
inline AngleDeg ASin(float x)           { return ((x) >= 1 ? 90 : ((x) <= -1 ? -90 : (Rad2Deg(asin(x))))); }
inline AngleDeg ATan(float x)           { return (Rad2Deg(atan(x))); }
inline AngleDeg ATan2(float x, float y) { return ((x == 0 && y == 0) ? 0 : (Rad2Deg(atan2(x,y)))); } 

void NormalizeAngleDeg(int*);
void NormalizeAngleDeg(AngleDeg*);
void NormalizeAngleRad(AngleRad*);
AngleDeg GetNormalizeAngleDeg(AngleDeg);
float GetDistance(float *x, float *y, float *a, float *b);

#define FLOAT_EPS .001

#define Mod(a,b) (a - (b)*(int)((a)/(b)))
#define Sign(x) ((x) >= 0 ? 1 : -1)
#define Min(x,y) ((x) < (y) ? (x) : (y))
#define Max(x,y) ((x) > (y) ? (x) : (y))
#define MinMax(min, x, max) Min(Max((min),(x)), (max))

//From Dave M.
#define sigmoid(x) (1/(1+exp(-x)))
#define sigmoid_deriv(x) (1/((1+exp(-x))*(1+exp(x))))
#define soft_not(x) (1-(x))
#define soft_and(x, y) ((x)*(y))
#define soft_or(x,y) (1-(1-(x))*(1-(y)))
#define soft_greater(x,y,delta) (sigmoid(((x)-(y))/(delta)))
#define soft_less(x,y,delta) (sigmoid(((y)-(x))/(delta)))
inline float soft_equal(float x, float y, float delta){
  float z = (x-y)/delta;
  return exp(-z*z);
}

inline float soft_if(float p, float x, float y){
  return p*x + (1-p)*y;
}


float int_pow(float x, int p);
inline int Sqr(int x){ return x*x; }
inline float Sqr(float x) { return x*x; }
inline double Sqr(double x) { return x*x; }
inline  float Exp(float x, int y) { float a = 1; for (int i=0; i<y; i++) a*=x; return a; }

inline float SumInfGeomSeries(float first_term, float r)
{ return first_term / (1 - r); }
float SumGeomSeries(float first_term, float r, int n);
/* returns -1 on error */
float LengthGeomSeries(float first_term, float r, float last_term);
inline float SumGeomSeriesGivenLast(float first_term, float r, float last_term)
{ return SumGeomSeries(first_term, r, 
		       closest_int(LengthGeomSeries(first_term, r, last_term))); 
}
float SolveForLengthGeomSeries(float first_term, float r, float sum);
float SolveForFirstTermGeomSeries(float r, int n, float sum);
inline float SolveForFirstTermInfGeomSeries(float r, float sum)
{ return sum * (1 - r); }

inline double gaussian_func(double x, double mean = 0.0, double stdev = 1.0)
{
  static const double piterm = 1.0 / sqrt(2 * M_PI);

  return (1.0 / stdev) * piterm * pow(M_E, - Sqr(x-mean) / (2*Sqr(stdev)));
}



#define signf(x) ( ((x) >= 0.0) ? 1.0 : -1.0) 
float Round(float x, int p=0);
inline int RoundToInt(float x) 
{
  return (int)Round(x,0); 
} 

inline float WeightedSum (float v1, float w1, float v2)
{
  return (w1*v1 + (1.0-w1)*v2);
}
inline float WeightedSum (float v1, float w1, float v2, float w2)
{
  return (w1*v1 + w2*v2) / (w1+w2);
}


extern const char char_for_num_array[16];
inline char char_for_num(int num)
{ return char_for_num_array[num]; }
int num_for_char(char c);


/* returns a pointer to a static buffer, so be careful! */
char* repeat_char(char c, int n);


Bool skip_line(istream& infile);
Bool skip_white_space(istream& infile) ;
Bool skip_to_non_comment(istream& infile, char comm_char = '#');
/* advances to the specifed character, but only on the same line */
Bool advance_to_on_line(istream& infile, char c);
/* path goes in, dir goes out */
/* returns a pathname with a trailing / */
void get_leading_directory(const char* path, char* dir); 

/* puts the number after the colon in the int, returns FALSE on failure */
Bool GetIntFromLine(char* line, int* pval); 
/* puts the number after the colon in the float, returns FALSE on failure */
Bool GetFloatFromLine(char* line, float* pval); 
Bool GetDoubleFromLine(char* line, double* pval); 
/* puts the number after the colon in the int, returns FALSE on failure */
Bool GetIntOffLine(char** pline, int* pval); 
/* puts the number after the colon in the float, returns FALSE on failure */
Bool GetDoubleOffLine(char** pline, double* pval); 


extern double get_double(char **str_ptr);
extern double get_double(char *str);
extern float  get_float (char **str_ptr);
extern float  get_float (char *str);
extern int    get_int   (char **str_ptr);
extern int    get_int   (char *str);
extern void   get_word  (char **str_ptr);
extern void   get_next_word (char **str_ptr);
extern void   get_token  (char **str_ptr);
extern void   advance_to(char c, char **str_ptr);
extern void   advance_past_space(char **str_ptr);

extern int put_int(char *str, int num);
extern int put_float(char *str, float fnum, int precision);

extern void BubbleSort  (int length, int *elements, float *keys);
extern int  BinarySearch(int length, float *elements, float key);
extern void StrReplace  (char *str, char oldchar, char newchar);

extern int   int_random(int n);
extern float range_random(float lo, float hi);
extern int   very_random_int(int n);

extern float weighted_avg(float val1, float val2, float w1, float w2);

extern void GetStampedName( char *name, char *outputName );

//you give it a series of void* pointers and score and then you can get out the best N
class RecordBestN 
{
public:
  RecordBestN(int N);
  ~RecordBestN();
  
  void Clear();
  
  void AddPoint (void* data, float score);
  //0th best is the best
  void* GetBest(int idx = 0) { return (idx < num_stored) ? arrData[idx] : NULL; }
  float GetBestScore(int idx = 0) { return (idx < num_stored) ? arrScore[idx] : 0.0; }
private:
  int N;
  int num_stored;
  void** arrData;
  float* arrScore;
  
} ;



#endif
