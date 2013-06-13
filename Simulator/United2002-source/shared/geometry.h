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


#ifndef _GEOMETRY_
#define _GEOMETRY_

#include <stdlib.h>
#include <fstream>
#include "utils.h"

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        Vector Class                                          */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/


class Vector {
public:
  float x;
  float y;
  
  Vector(float vx = 0, float vy = 0) { x = vx; y = vy; }
  Vector operator -() const { return Vector(-x, -y); }
  Vector operator +(const Vector &a) const { return Vector(x + a.x, y + a.y); } 
  Vector operator -(const Vector &a) const { return Vector(x - a.x, y - a.y); } 
  Vector operator *(const float &a) const { return Vector(x * a, y * a); }
  Vector operator *(const Vector &a) const { return Vector(x * a.x, y * a.y); }
  Vector operator /(const float &a) const { return Vector(x / a, y / a); }
  Vector operator /(const Vector &a) const { return Vector(x / a.x, y / a.y); }
  Vector& operator =(const float &a) { x = a; y = a; return *this; }
  Vector& operator +=(const Vector &a) { x += a.x;  y += a.y; return *this; }
  Vector& operator +=(const float &a) { x += a;  y += a; return *this; }
  Vector& operator -=(const Vector &a) { x -= a.x;  y -= a.y; return *this; }
  Vector& operator -=(const float &a) { x -= a;  y -= a; return *this; }
  Vector& operator *=(const float &a) { x *= a;  y *= a; return *this; }
  Vector& operator /=(const float &a) { x /= a;  y /= a; return *this; }
  bool operator !=(const Vector &a) const { return (x != a.x) || (y != a.y); }
  bool operator !=(const float &a) const { return (x != a) || (y != a); } 
  bool operator ==(const Vector &a) const { return (x == a.x) && (y == a.y); } 

  friend ostream& operator <<(ostream & os, Vector v) { return os << "("<<v.x<<", "<<v.y<<")"; }
  friend istream& operator >>(istream & is, Vector &v);  
  void Print() const { cout << *this << endl; } 
  void PrintToStr(char* str) const;  

  Vector FlipCoords(bool flipX=TRUE, bool flipY=TRUE)
  { return Vector(flipX ? -x : x, flipY ? -y : y); }
  Vector FlipX() { return FlipCoords(TRUE, FALSE); }
  Vector FlipY() { return FlipCoords(FALSE, TRUE); }

  void ins(float vx = 0, float vy = 0) { x = vx; y = vy; }
  float mod() const { return sqrt(x*x + y*y); } 
  float mod2() const { return x*x + y*y; } 
  float dist(const Vector &a) const { return (*this-a).mod(); } 
  float dist2(const Vector &a) const { return (*this-a).mod2(); } 
  float dotprod(const Vector &a) const { return (this->x * a.x) + (this->y * a.y); }

  Vector Normalize() { return SetLength(1.0); }
  Vector SetLength(float len) { return (*this) * (len / mod()); }

  AngleDeg dir() const { return ATan2(y,x); } 
  Vector rotate(AngleDeg angle) const
  { return Vector(this->mod()*Cos(this->dir()+angle), this->mod()*Sin(this->dir()+angle)); }
  Vector rotateNeg90() const { return Vector(-y, x); }
  Vector rotate90() const    { return Vector(y, -x); }
    
  Vector Global2Relative(Vector orig, float ang) const;
  Vector Relative2Global(Vector orig, float ang) const;
  Bool InFrontOf(const Vector &a) const { return x > a.x ? TRUE : FALSE; }
  Bool InFrontOf(const float &a) const { return x > a ? TRUE : FALSE; } 
  Bool Behind(const Vector &a) const { return x < a.x ? TRUE : FALSE; }
  Bool Behind(const float &a) const { return x < a ? TRUE : FALSE; } 

  Bool ApproxEqual(const Vector &a) const
    { return (fabs(x-a.x) < FLOAT_EPS && fabs(y-a.y) < FLOAT_EPS) ? TRUE : FALSE; }

  Vector WeightedAverage(const Vector &a, float w) const
    { return Vector((1.0-w)*x + w * a.x, (1.0-w)*y + w * a.y); }
};

inline Vector Polar2Vector(float mod, AngleDeg ang) {
  return Vector(mod * Cos(ang), mod * Sin(ang));
}
  
inline Vector Vector::Global2Relative(Vector orig, float ang) const
{
  return (*this - orig).rotate(-ang);
}

inline Vector Vector::Relative2Global(Vector orig, float ang) const
{
  return (rotate(ang) + orig);
}

inline Vector operator *(const float &a, const Vector &v) 
{ return Vector(v.x * a, v.y * a); }

inline float dot_product(Vector v1, Vector v2){
  return v1.x * v2.x + v1.y *v2.y;
}

inline float cross_product(Vector pt, Vector dir){
  return pt.x * dir.y - pt.y * dir.x;
}


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                       Ray Class                                              */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/* stores origin and direction (which is normalized) */
class Line;
class Rectangle;
class Ray 
{
public:
  Ray() 
    { origin = Vector(0,0); direction = Vector(1,0); }
  Ray(Vector orig, Vector dir);
  Ray(Vector orig, float ang)
    { origin = orig; direction = Polar2Vector(1.0, ang); }

  void SetFrom2Pts(Vector orig, Vector dest) { *this = Ray(orig, dest-orig); }

  Bool OnRay(Vector pt);
  Bool InRightDir(Vector pt, float angle_buffer = 10); // more lenient than above about distance off ray 
  
  Bool intersection(Ray r, Vector *pPt);
  Bool intersection(Line l, Vector *pPt);

  int CircleIntersect(float rad, Vector center,
		      Vector* psol1, Vector* psol2);

  Vector GetClosestPoint(Vector pt);

  Vector RectangleIntersection(Rectangle R);

  Vector GetOrigin() const { return origin; }
  Vector GetDirection() const { return direction; }
  AngleDeg GetAngle() const { return direction.dir(); }
  
protected:
  friend Line;
  friend Rectangle;
  Vector origin;
  Vector direction;
};

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                      Line Class                                              */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

class Line {
 public:
  Line(float x_coef=0.0, float y_coef=0.0, float constant=0.0);
  Line(Ray r) 
    { LineFromRay(r); }

  void LineFromTwoPoints(Vector pt1, Vector pt2);
  void LineFromRay(Ray r);
  void LineFromRay(Vector orig, Vector dir)
    { LineFromRay(Ray(orig, dir)); }
  void LineFromRay(Vector orig, AngleDeg dir)
    { LineFromRay(orig, Polar2Vector(1.0, dir)); }

  void MakeVert(float x) { A = 1.0; B = 0.0; C = -x; }
  void MakeHoriz(float y) { A = 0.0; B = 1.0; C = -y; }
  
  Bool PointOnLine(float x, float y);
  inline Bool PointOnLine(Vector pt) { return PointOnLine(pt.x, pt.y); }

  float dist(Vector pt);
  float dist2(Vector pt);
  float angle(); /* returns the angle of the line in [-90, 90] */
  
  Bool InBetween(Vector pt, Vector end1, Vector end2);
  Vector GetClosestPtInBetween(Vector pt, Vector end1, Vector end2);
  float  GetDistToClosestPtInBetween(Vector pt, Vector end1, Vector end2);

  /* the buffer should really be linked to an option or something,
     but 1.0 is okay */
  inline Bool OnLine(Vector pt, float buffer = 1.0)
    { return (dist2(pt) < Sqr(buffer)) ? TRUE : FALSE; }
  
  Vector ProjectPointUsingCircle(Vector pt);
  inline Vector ProjectPoint(Vector pt) { return intersection(perpendicular(pt)); }

  float get_y(float x);
  float get_x(float y);
  Vector intersection(Line l);
  inline Line perpendicular(Vector pt) { return Line(B,-A,A*pt.y - B*pt.x); }

  Bool RayIntersection(Ray r, Vector *ppt);

  Line shift_y(float val)
    { return Line(A, B, C - val * B); }
  Line shift_x(float val)
    { return Line(A, B, C - val * A); }  

  /* returns whether the projection of pt1 is closer to targ_pt than the
     projection of pt2 */
  Bool IsPtCloserToPtOnLine(Vector pt1, Vector pt2, Vector targ_pt);
  
  Bool HalfPlaneTest(Vector pt); //return TRUE on top/left part of plane
  Bool SameSlope(Line l);
  
  friend ostream& operator <<(ostream & os, Line l)
    { return os << "#L("<<l.A<<", "<<l.B<<", "<<l.C<<")"; } 
  void Print(void) { cout << *this << endl; }

  // private:
  float A,B,C; /* the three coeffs in the line equation */
  /* Ax + By + C = 0 */
} ;

inline Line LineFromTwoPoints(Vector pt1, Vector pt2) { Line l; l.LineFromTwoPoints(pt1,pt2); return l; }
inline Line LineFromRay      (Ray r)
{ Line l; l.LineFromRay     (r); return l; }
inline Line LineFromRay      (Vector orig, Vector dir)
{ Line l; l.LineFromRay     (orig,dir); return l; }
inline Line LineFromRay      (Vector orig, AngleDeg dir)
{ Line l; l.LineFromRay     (orig,dir); return l; }

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        Rectangle Class                                       */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

class Rectangle {
public:

  Rectangle() ;
  Rectangle(const float left, const float right, 
	    const float top, const float bottom) ;
  Rectangle(const Vector center, const Vector size) ;

  bool operator ==(const Rectangle &a) 
  { return (left_x == a.left_x) && (right_x == a.right_x) && 
      (top_y == a.top_y) && (bottom_y == a.bottom_y); } 

  inline float TopY()     { return top_y; }
  inline float BottomY()  { return bottom_y; }
  inline float RightX()   { return right_x; }
  inline float LeftX()    { return left_x; }

  inline Vector TopLeftCorner() { return Vector(left_x, top_y); }
  inline Vector TopRightCorner() { return Vector(right_x, top_y); }
  inline Vector BottomLeftCorner() { return Vector(left_x, bottom_y); }
  inline Vector BottomRightCorner() { return Vector(right_x, bottom_y); }        

  inline Line LeftEdge() {return LineFromTwoPoints(TopLeftCorner(), BottomLeftCorner());}
  inline Line RightEdge() {return LineFromTwoPoints(TopRightCorner(), BottomRightCorner());}
  inline Line TopEdge() {return LineFromTwoPoints(TopLeftCorner(), TopRightCorner());}
  inline Line BottomEdge() {return LineFromTwoPoints(BottomLeftCorner(), BottomRightCorner());}

  Line GetEdge(int n);  //order: top, right, bot, left; starting at 0
  Vector GetPoint(int n); //order: TL, TR, BR, BL; starting at 0
  
  inline float Width()    { return right_x - left_x; }   
  inline float Height()   { return bottom_y - top_y; }  

  inline Vector Center()  { return Vector( LeftX() + Width()/2, TopY() + Height()/2 ); }

  Bool   IsWithin(const Vector& p, const float& buffer = 0) ;
  Vector nearestHEdge(const Vector& p) ;
  Vector nearestVEdge(const Vector& p) ;
  Vector nearestEdge(const Vector& p) ;

  float  DistanceToEdge(const Vector& p);
  Vector AdjustToWithin(const Vector& p, const float& buffer = 0);
  Vector AdjustToOutside(const Vector& p, const float& buffer = 0);

  Line nearestHEdgeLine(const Vector& p) ;
  Line nearestVEdgeLine(const Vector& p) ;
  Line nearestEdgeLine(const Vector& p) ;
  Vector nearestCorner(Vector p, float interior_dist = 0) ;
  
  Vector random() ;

  void FlipCoords(bool flipX, bool flipY);
  void FlipX() { FlipCoords(TRUE, FALSE); }
  void FlipY() { FlipCoords(FALSE, TRUE); }

  Rectangle shrink(float val)
    { return expand(-val); }
  Rectangle expand(float val);

  Rectangle expandLeft(float val)
    {return Rectangle(left_x - val, right_x, top_y, bottom_y);}
  Rectangle expandRight(float val)
    {return Rectangle(left_x, right_x + val, top_y, bottom_y);}
  Rectangle expandTop(float val)
    {return Rectangle(left_x, right_x, top_y - val, bottom_y);}
  Rectangle expandBottom(float val)
    {return Rectangle(left_x, right_x, top_y, bottom_y + val);}
  Rectangle shrinkLeft(float val)
    { return expandLeft(-val); }
  Rectangle shrinkRight(float val)
    { return expandRight(-val); }
  Rectangle shrinkTop(float val)
    { return expandTop(-val); }
  Rectangle shrinkBottom(float val)
    { return expandBottom(-val); }
  
  friend ostream& operator <<(ostream & os, Rectangle r)
    { return os << "RECTANGLE:  x = " << r.LeftX() << " to " << r.RightX()
		<< "   y = " << r.TopY() << " to " << r.BottomY(); }
  void Print();

  Vector RayIntersection(Ray r);

private:
  float left_x ;
  float right_x ;
  float top_y ;
  float bottom_y ;

} ;


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                  Miscellaneous Geometry Functions                            */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int QuadraticFormula(float a, float b, float c, float* psol1, float* psol2);

inline int RayCircleIntersect(Ray r, float rad, Vector center,
			      Vector* psol1, Vector* psol2)
{ return r.CircleIntersect(rad, center, psol1, psol2); }

int LineCircleIntersect(Line l, float rad, Vector center,
			Vector* psol1, Vector* psol2);

Vector AdjustPtToRectOnLine(Vector pt, Rectangle r, Line l);

Bool InBetween(Vector pt, Vector end1, Vector end2);

Vector PointInBetween(Vector pt1, Vector pt2, float pt1dist=0);

Vector PointInFracBetween(Vector pt1, Vector pt2, float pt1frac=.5);

AngleDeg GetAngle(Vector pt1, Vector vertex, Vector pt2);

AngleDeg AngleBisect(AngleDeg a1, AngleDeg a2);

Vector GetClosestPtInBetween(Vector pt, Vector end1, Vector end2);
float  GetDistToClosestPtInBetween(Vector pt, Vector end1, Vector end2);

Bool IsPointInCone(Vector pt, float wid_dist_ratio, Vector end, Vector vert);

float GetDistToVerticalSegment(float x_val, float top_y, float bot_y, Vector pt);

inline float GetDistToLineSegment(Vector pt, Vector end1, Vector end2) 
{ Line l = LineFromTwoPoints(end1, end2); return l.GetDistToClosestPtInBetween(pt, end1, end2); }

#endif

