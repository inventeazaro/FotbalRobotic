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


#include <fstream>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include "utils.h"
#include "MemOption.h"

void dump_core(char *msg){
  fprintf(stderr,"dumping core");
  msg[1000000]=0;
  my_error("Core didn't dump");
}


/* NOTE: the base my_error needs to be defined before including this file */

void my_error(char *msg, int param)
{
  char outstring[100];
  sprintf(outstring,msg,param);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2, int param3)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2, int param3, int param4)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2, int param3, int param4, int param5)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4,param5);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2, int param3, int param4, int param5, int param6)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4,param5,param6);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2, int param3, int param4, int param5, char c1, int param6)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4,param5,c1,param6);
  my_error(outstring);
}

void my_error(char *msg, float param)
{
  char outstring[100];
  sprintf(outstring,msg,param);
  my_error(outstring);
}

void my_error(char *msg, float param1, float param2)
{  char outstring[100];
  sprintf(outstring,msg,param1,param2);
  my_error(outstring);
}

void my_error(char *msg, float param1, float param2, float param3)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3);
  my_error(outstring);
}

void my_error(char *msg, float param1, float param2, float param3, float param4)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4);
  my_error(outstring);
}

void my_error(char *msg, float param1, float param2, float param3, float param4, float param5)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4,param5);
  my_error(outstring);
}

void my_error(char *msg, float param1, float param2, float param3, float param4, float param5, float param6)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4,param5,param6);
  my_error(outstring);
}

void my_error(char *msg, int param1, float param2)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2);
  my_error(outstring);
}

void my_error(char *msg, float param1, int param2)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2);
  my_error(outstring);
}

void my_error(char *msg, int param1, int param2, int param3, float param4)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4);
  my_error(outstring);
}

void my_error(char *msg, char* param)
{
  char outstring[1000];
  sprintf(outstring,msg,param);
  my_error(outstring);
}

void my_error(char *msg, char param1, int param2, int param3)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3);
  my_error(outstring);
}

void my_error(char *msg, char param1, char param2, char param3)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3);
  my_error(outstring);
}

void my_error(char *msg , char param1, int param2, float param3, float param4)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3,param4);
  my_error(outstring);
}


void my_error(char *msg, char* param1, int param2)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2);
  my_error(outstring);
}

void my_error(char *msg, char* param1, int param2, int param3)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3);
  my_error(outstring);
}

void my_error(char *msg, char* param1, char* param2)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2);
  my_error(outstring);
}

void my_error(char *msg, char* param1, char* param2, char* param3)
{
  char outstring[100];
  sprintf(outstring,msg,param1,param2,param3);
  my_error(outstring);
}





int closest_int( float x ){
  if ( x < 0 ) x -= 0.5;
  else         x += 0.5;
  return (int) x;
}

void NormalizeAngleDeg(int *ang){
  if (fabs(*ang) > 5000)
    my_error("Huge angle passed to NormalizeAngleDeg");
  while (*ang > 180) *ang-=360;
  while (*ang < -180) *ang+=360;
}

void NormalizeAngleDeg(AngleDeg *ang){
  if (fabs(*ang) > 5000)
    my_error("Huge angle passed to NormalizeAngleDeg");
  while (*ang > 180) *ang-=360;
  while (*ang < -180) *ang+=360;
}

void NormalizeAngleRad(AngleRad *ang){
  while (*ang > M_PI) *ang-=2*M_PI;
  while (*ang < -M_PI) *ang+=2*M_PI;
}

AngleDeg GetNormalizeAngleDeg(AngleDeg ang){
  if (fabs(ang) > 5000)
    my_error("Huge angle passed to GetNormalizeAngleDeg");
  while (ang > 180) ang-=360;
  while (ang < -180) ang+=360;
  return ang;
}


float int_pow(float x, int p)
{
  if (p < 0) 
    return (1.0 / int_pow(x,-p));
  else {
    float ans = 1.0;
    for (int i=0; i<p; i++)
      ans *= x;
    return ans;
  }
}



float GetDistance(float *x, float *y, float *a, float *b){
  return sqrt((*x-*a)*(*x-*a) + (*y-*b)*(*y-*b));
}

float weighted_avg(float val1, float val2, float w1, float w2){
  return (val1*w1 + val2*w2)/(w1+w2);
}



float SumGeomSeries(float first_term, float r, int n)
{
  return first_term * (Exp(r, n) - 1) / (r - 1);
}

float LengthGeomSeries(float first_term, float r, float last_term)
{
  return (log(last_term) - log(first_term)) / (log(r)) + 1.0;
}

float SolveForLengthGeomSeries(float first_term, float r, float sum)
{
  if (r < 0)
    my_error("SolveForSumGeomSeries: can't take r < 0");
  float temp = sum * (r-1) / first_term + 1;
  if (temp <= 0)
    return -1.0;
  return log(temp) / log(r);
}

float SolveForFirstTermGeomSeries(float r, int n, float sum)
{
  return sum * (r - 1) / (Exp(r, n) - 1);
}


float Round(float x, int p=0)
{
  float s = signf(x);
  x = fabs(x);
  double m = pow(10.0, p);
  //printf("m: %f\n", m);
  double f = fmod(x,m);
  //printf("f: %f\n", f);
  //printf("pow: %f\n", .5*m);
  if (f >= .5*m) {
    //printf("Going up");
    x += (m-f);
  } else {
    //printf("Going down");
    x -= f;
  }
  return s*x;
} 



/* returns a pointer to a static buffer, so be careful! */
char* repeat_char(char c, int n)
{
  const int MAX_REP = 100;
  static char out[MAX_REP+1];
  if (n > MAX_REP)
    my_error("repeat_char: asking for too many characters");
  for (int i=0; i<n; i++)
    out[i] = c;
  out[n] = 0;
  return out;
}


const char char_for_num_array[16] =
{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

int num_for_char(char c)
{
  for (int i = 0; i<16; i++)
    if (char_for_num_array[i] == c)
      return i;
  return -1;
}



Bool skip_white_space(istream& infile)
{
  char c;

  while(1) {
    infile.get(c);
    if (!infile) return FALSE;
    if (!isspace(c)) {
      infile.putback(c);
      return TRUE; 
    } 
  } 
} 

Bool skip_line(istream& infile)
{
  char ch;  
  while (1) {
    if (!infile.get(ch)) return false;
    if (ch == '\n') return true;    
  }  
}


Bool skip_to_non_comment(istream& infile, char comm_char)
{
  while (1) {
    if (!skip_white_space(infile)) return FALSE;
    if (infile.peek() == comm_char) {
      if (!skip_line(infile)) return FALSE;    
    }
    else 
      break;
  }
  return TRUE;  
}


/* advances to the specifed character, but only on the same line */
Bool advance_to_on_line(istream& infile, char match_c)
{
  char c;

  while(1) {
    infile.get(c);
    if (!infile) return FALSE;
    if (c == match_c) {
      infile.putback(c);
      return TRUE; 
    }
    if (c == '\n') {
      return FALSE; 
    } 
  } 
}



/* returns a pathname with a trailing / */
void get_leading_directory(const char* path, char* dir)
{
  char *pc;
  pc = strrchr(path, '/');
  if (pc == NULL) {
    /* there are no slashes in the pathname */
    strcpy(dir, "./");
  } else {
    strncpy(dir, path, (pc - path) + 1 );
    dir[(pc-path) + 1] = 0; 
  } 
} 

/* puts the number after the colon in the float, returns FALSE on failure */
Bool GetIntOffLine(char** ppc, int* pval)
{
  *ppc = strchr(*ppc, ':');
  if (*ppc == NULL) {
    my_error("Observe (GetNumFromLine): malformed count line");
    return FALSE;
  } else {
    (*ppc)++;
    *pval = get_int(ppc);
  }
  return TRUE;
}

/* puts the number after the colon in the float, returns FALSE on failure */
Bool GetDoubleOffLine(char** ppc, double* pval)
{
  *ppc = strchr(*ppc, ':');
  if (*ppc == NULL) {
    my_error("Observe (GetNumFromLine): malformed count line");
    return FALSE;
  } else {
    (*ppc)++;
    *pval = (float)get_double(ppc);
  }
  return TRUE;
}

/* puts the number after the colon in the int, returns FALSE on failure */
Bool GetIntFromLine(char* line, int* pval)
{
  char** ppc = &line;
  return GetIntOffLine(ppc, pval);
}

/* puts the number after the colon in the float, returns FALSE on failure */
Bool GetFloatFromLine(char* line, float* pval)
{
  char** ppc = &line;
  double d;
  Bool ret = GetDoubleOffLine(ppc, &d);
  *pval = (float)d;
  return ret;
}

Bool GetDoubleFromLine(char* line, double* pval)
{
  char** ppc = &line;
  return GetDoubleOffLine(ppc, pval);

}


/****************************************************************************/
/* These routines are to save time instead of using sscanf or atof, etc.    */
/* When passing **str_ptr, the pointer is advanced past the number          */
/* When passing  *str    , the pointer remains where it was before          */
/****************************************************************************/

double get_double(char **str_ptr){

  double d_frac, result;
  int  m_flag, d_flag;

  d_frac = 1.0;
  result = 0.0;
  m_flag = d_flag = 0;

  /* Advance to the beginning of the number */
  while( !isdigit(**str_ptr) && **str_ptr!='-' && **str_ptr!='.')
    (*str_ptr)++;

  /* Process the number bit by bit */
  while((**str_ptr!=')') &&
	(**str_ptr!='\t') &&
	(**str_ptr!=' ') &&
	(**str_ptr!='\n') &&
	(**str_ptr!= 0 ) &&
	(**str_ptr!=',')){
    if (**str_ptr=='.')
      d_flag=1;
    else if (**str_ptr=='-')
      m_flag=1;
    else if (d_flag){
      d_frac *= 10.0;
      result+=(double)(**str_ptr-'0')/d_frac;
    }
    else
      result=result*10.0+(double)(**str_ptr-'0');
    (*str_ptr)++;
  }
  if (m_flag)
    result=-result;

  //printf("%.1f\n",result);

  return result;
}

/* Get the number, but don't advance pointer */

double get_double(char *str){
  char **str_ptr = &str;
  return get_double(str_ptr);
}

/****************************************************************************/

float get_float(char **str_ptr){

  float d_frac, result;
  int  m_flag, d_flag;

  d_frac = 1.0;
  result = 0.0;
  m_flag = d_flag = 0;

  /* Advance to the beginning of the number */
  while( !isdigit(**str_ptr) && **str_ptr!='-' && **str_ptr!='.')
    (*str_ptr)++;

  /* Process the number bit by bit */
  while((**str_ptr!=')') &&
	(**str_ptr!=' ') &&
	(**str_ptr!='\t') &&
	(**str_ptr!='\n') &&
	(**str_ptr!= 0 ) &&
	(**str_ptr!=',')){
    if (**str_ptr=='.')
      d_flag=1;
    else if (**str_ptr=='-')
      m_flag=1;
    else if (d_flag){
      d_frac *= 10.0;
      result+=(float)(**str_ptr-'0')/d_frac;
    }
    else
      result=result*10.0+(float)(**str_ptr-'0');
    (*str_ptr)++;
  }
  if (m_flag)
    result=-result;

  //printf("%.1f\n",result);

  return result;
}

/* Get the number, but don't advance pointer */

float get_float(char *str){
  char **str_ptr = &str;
  return get_float(str_ptr);
}

/****************************************************************************/

int get_int(char **str_ptr){

  int result;
  int m_flag;

  result = 0;
  m_flag = 0;

  /* Advance to the beginning of the number */
  while( !isdigit(**str_ptr) && **str_ptr!='-')
    (*str_ptr)++;

  /* Process the number bit by bit */
  while((**str_ptr!=')') && (**str_ptr!=' ') && (**str_ptr!='\t') && (**str_ptr!='\n') && (**str_ptr!=',')){
    if (**str_ptr=='-')
      m_flag=1;
    else
      result=result*10+(int)(**str_ptr-'0');
    (*str_ptr)++;
  }
  if (m_flag)
    result=-result;

  return result;
}

int get_int(char *str){
  char **str_ptr = &str;
  return get_int(str_ptr);
}

/****************************************************************************/

void get_word(char **str_ptr){
  while ( !isalpha(**str_ptr) ) (*str_ptr)++;
}

/****************************************************************************/

void get_next_word(char **str_ptr){
  while ( isalpha(**str_ptr) ) (*str_ptr)++;
  get_word(str_ptr);
}

/****************************************************************************/

void advance_to(char c, char **str_ptr){
  while ( **str_ptr != c ) (*str_ptr)++;
}

/****************************************************************************/

void get_token (char **str_ptr)
{
  advance_past_space(str_ptr);
  while ( (*str_ptr) && !isspace(**str_ptr)) (*str_ptr)++;
}

/****************************************************************************/

void   advance_past_space(char **str_ptr)
{
  while ( (*str_ptr) && isspace(**str_ptr)) (*str_ptr)++;
}




/****************************************************************************/
/* These routines are to save time instead of using sprintf or atof, etc.   */
/* *str should point to the END of the string where the number is going     */
/* return the length of the number placed in                                */
/****************************************************************************/

int put_float(char *str, float fnum, int precision){
  int m_flag = 0, length = 0;
  int num, old_num;

  for (int i=0; i<precision; i++)
    fnum *= 10;

  num = closest_int(fnum);  /* round off the rest */

  if ( precision == 0 ) 
    return put_int(str,num);

  if ( num < 0 ){
    m_flag = 1;
    num = -num;
  }

  old_num = num;
  while ( num > 0 || length < precision ){
    num /= 10;
    *str = '0' + old_num - num*10;
    old_num = num;
    str--;
    length++;
    if ( length == precision ){
      *str = '.';
      str--;
      length++;
      if ( num == 0 ){
	*str = '0';
	str--;
	length++;
	break;
      }
    }
  }

  if ( m_flag ){
    *str = '-';
    length++;
  }

  return length;
}

/****************************************************************************/

int put_int(char *str, int num){

  int m_flag = 0, length = 0;
  int old_num;

  if ( num == 0 ){
    *str = '0';
    return 1;
  }

  if ( num < 0 ){
    m_flag = 1;
    num = -num;
  }

  old_num = num;
  while ( num > 0 ){
    num /= 10;
    *str = '0' + old_num - num*10;
    old_num = num;
    str--;
    length++;
  }

  if ( m_flag ){
    *str = '-';
    length++;
  }

  return length;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

void BubbleSort(int length, int *elements, float *keys){
  
/*  for (int a=0; a<length; a++){
    printf("%d--%.1f ",elements[a], keys[a]);
  }
  printf("\n");*/

  /* Sort the elements in increasing order according to the keys */
  float keytemp;
  int eltemp;
  for (int i=0; i<length; i++){
    for (int j=i+1; j<length; j++){
      if ( keys[j] < keys[i] ){
	keytemp = keys[i];
	keys[i] = keys[j];
	keys[j] = keytemp;
	eltemp = elements[i];
	elements[i] = elements[j];
	elements[j] = eltemp;
      }
    }
  }
/*  for (int a=0; a<length; a++){
    printf("%d--%.1f ",elements[a], keys[a]);
  }
  printf("\n");*/
}

/****************************************************************************/

int BinarySearch(int length, float *elements, float key){
  
  /* Assume the list is already sorted in increasing order */
  int lbound = 0, ubound = length;
  
  for ( int index = length/2; ubound-lbound > 0; index = lbound+(ubound-lbound)/2 ){
    /* printf("%d ",index); */
    if ( elements[index] == key ){
      lbound = ubound = index;
    }
    else if ( elements[index] < key ){
      lbound = index+1;
    }
    else {
      ubound = index-1;
    }
  }
 
  int toReturn = Max(ubound,lbound);
  if (elements[toReturn] < key) toReturn++;  /* Guarantees >= key */

  return toReturn;
}

/****************************************************************************/ 

/* replace all occurrences in a string */
void StrReplace(char *str, char oldchar, char newchar){
  int i=0;
#if 0
  int numReplaced;
#endif
  int strLength = strlen(str);
  while ( i++ < strLength ){
    if ( str[i] == oldchar ){
      str[i] = newchar;
#if 0
      numReplaced++;
#endif
    }
    if ( i==1000 ) 
      my_error("String of length >1000?");
  }
#if 0
  printf("***Replaced %d %c's in string of length %d (%d): %s***\n",
	 numReplaced,oldchar,strlen(str),i,str);
#endif
}

/****************************************************************************/
/***************************   random stuff    ******************************/
/****************************************************************************/
/* From Andrew's C package                                                  */

int int_random(int n)
{
  static int FirstTime = TRUE;
  
  if ( FirstTime ){
    /* initialize the random number seed. */
    timeval tp;
    gettimeofday( &tp, NULL );
    srandom( (unsigned int) tp.tv_usec );
    FirstTime = FALSE;
  }

  if ( n > 2 )
    return( random() % n );
  else if ( n == 2 )
    return( ( (random() % 112) >= 56 ) ? 0 : 1 );
  else if ( n == 1 )
    return(0);
  else
  {
    printf("int_random(%d) ?\n",n);
    my_error("You called int_random(<=0)");
    return(0);
  }
}

float range_random(float lo, float hi)
{
  int x1 = int_random(10000);
  int x2 = int_random(10000);
  float r = (((float) x1) + 10000.0 * ((float) x2))/(10000.0 * 10000.0);
  return( lo + (hi - lo) * r );
}

int very_random_int(int n)
{
  int result = (int) range_random(0.0,(float)n);  /* rounds down */
  if ( result == n ) result = n-1;
  return(result);
}

void GetStampedName( char *name, char *outputName ){
  char date[100],weekday[10],month[10],temp[10];
  int  day,hour,min,sec,year;
  FILE *dateFile;
  
  //system("date > ../date.log");        /* Put the date in a file */ /* done by a player */

  dateFile = fopen("../date.log","r");
  fscanf(dateFile,"%[^\n]",date);         /* Scan it in             */
  fclose(dateFile);
  
  sscanf(date,"%s %s %d %d:%d:%d %s %d",
	 weekday,month,&day,&hour,&min,&sec,temp,&year);
  sprintf(name,"%s-%s%d-%d:%d:%d.dat",outputName,month,day,hour,min,sec);
}

/****************************************************************************************/


RecordBestN::RecordBestN(int N)
{
  this->N = N;
  num_stored = 0;
  arrData = new void*[N];
  arrScore = new float[N];
}

RecordBestN::~RecordBestN()
{
  delete [] arrData;
  delete [] arrScore;
}

  
void RecordBestN::Clear()
{
  num_stored = 0;
}

//0th best is the best
void RecordBestN::AddPoint (void* data, float score)
{
  bool found_spot = false;
  void* held_data = data;
  float held_score = score;
  
  for (int i=0; i<num_stored; i++) {
    if (score > arrScore[i])
      found_spot = TRUE;
    
    if (found_spot) {
      void* tmp_data = arrData[i];
      float tmp_score = arrScore[i];
      arrData[i] = held_data;
      arrScore[i] = held_score;
      held_data = tmp_data;
      held_score = tmp_score;
    }
  }

  if (num_stored < N) {
    arrData[num_stored] = held_data;
    arrScore[num_stored] = held_score;
    num_stored++;
  }
}
