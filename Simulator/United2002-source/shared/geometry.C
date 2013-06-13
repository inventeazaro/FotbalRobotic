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


#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "geometry.h"

#ifdef DEBUG_OUTPUT
#define DebugGeom(x) 
#define DebugSim(x) 
#else
#define DebugGeom(x)
#define DebugSim(x) 
#endif

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                           Vector Class                                       */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/* reads in an optional '(', the x val, an optional ',', the y val, 
   and then an optional ')' */
istream& operator >>(istream & is, Vector &v)
{
  char ch;  
  Bool paren = FALSE;
  
  while (isspace(ch = is.get())) ;
  if (ch != '(') is.putback(ch);
  else paren = TRUE;  
  is >> v.x;
  while (isspace(ch = is.get())) ;
  if (ch != ',') is.putback(ch);
  is >> v.y;
  if (paren) {
    while (isspace(ch = is.get())) ;
    if (ch != ')') {
      my_error("Vector read: open paren, but no close");    
      is.putback(ch);
    }
  }  
  return is;  
}

void Vector::PrintToStr(char* str) const
{
  sprintf(str, "(%.2f, %.2f)", x, y);  
}



/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                        Rectangle Class                                       */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

Rectangle::Rectangle()
{
  left_x = right_x = top_y = bottom_y = 0;
}

/********************************************************************************/

Rectangle::Rectangle(const float l, const float r, const float t, const float b) 
{
  if ( l >= r ) my_error("width must be positive %f %f %f %f", l, r, t, b);
  if ( t >= b ) my_error("height must be positive %f %f %f %f", l, r, t, b);

  left_x = l ; right_x = r ;
  top_y = t ; bottom_y = b ;
}

/********************************************************************************/

Rectangle::Rectangle(const Vector center,const Vector size)
{
  left_x = center.x - size.x/2.0 ;
  right_x = center.x + size.x/2.0 ;
  top_y = center.y - size.y/2.0 ;
  bottom_y = center.y + size.y/2.0 ;
}

/********************************************************************************/

Bool Rectangle::IsWithin(const Vector &p, const float& buffer)
{
  return (Bool) ((p.x >= left_x + buffer) && 
		 (p.x <= right_x - buffer) && 
		 (p.y >= top_y + buffer) && 
		 (p.y <= bottom_y - buffer) );
}

/********************************************************************************/

Vector Rectangle::AdjustToWithin(const Vector &p, const float &buffer){
  // Point ends up at least buffer within the rectangle
  // buffer<0 ==> outside the rectangle

  Vector r = p;

  if ( buffer > Width()/2 || buffer > Height()/2 )
    my_error("Rectangle::AdjustToWithin -- buffer too large");
  
  if (r.y - top_y    < buffer) r.y = top_y+buffer;
  if (bottom_y - r.y < buffer) r.y = bottom_y-buffer;
  if (right_x - r.x  < buffer) r.x = right_x-buffer;
  if (r.x - left_x   < buffer) r.x = left_x+buffer;

  return r;
}

/********************************************************************************/

Vector Rectangle::AdjustToOutside(const Vector& p, const float& buffer = 0)
{
  Vector r = p;
  Rectangle rectBuff = expand(buffer);

  if (!rectBuff.IsWithin(r))
    return r;

  Line l = rectBuff.nearestEdgeLine(r);

  return l.ProjectPoint(r);
}

/********************************************************************************/
//order: top, right, bot, left
Line Rectangle::GetEdge(int n)
{
  switch (n % 4) {
  case 0: return TopEdge();
  case 1: return RightEdge();
  case 2: return BottomEdge();
  case 3: return LeftEdge();
  }
  my_error("Rectangle::GetEdge: how did I get here");
  return Line();
}

/********************************************************************************/
//order: TL, TR, BR, BL
Vector Rectangle::GetPoint(int n)
{
  switch (n % 4) {
  case 0: return TopLeftCorner();
  case 1: return TopRightCorner();
  case 2: return BottomRightCorner();
  case 3: return BottomLeftCorner();
  }    
  my_error("Rectangle::GetPoint: how did I get here");
  return Vector(0,0);
}

/********************************************************************************/

Vector Rectangle::nearestHEdge(const Vector &p) 
     /* find nearest horizontal line */
{
  static Vector r ;

  r.x = Min(Max(p.x,left_x),right_x) ;
  r.y = ((p.y - top_y) < (bottom_y - p.y)) ? top_y : bottom_y ;

  return r ;
}

/********************************************************************************/

Vector Rectangle::nearestVEdge(const Vector &p) 
     /* find nearest vertical line */
{
  static Vector r ;

  r.x = ((p.x - left_x) < (right_x - p.x)) ? left_x : right_x ;
  r.y = Min(Max(p.y,top_y),bottom_y) ;

  return r ;
}

/********************************************************************************/

Vector Rectangle::nearestEdge(const Vector &p) 
{
  if(Min((p.x-left_x),(right_x-p.x)) < Min((p.y-top_y),(bottom_y-p.y)))
    return nearestVEdge(p) ;
  else
    return nearestHEdge(p) ;
}

/********************************************************************************/

Line Rectangle::nearestHEdgeLine(const Vector& p) 
{
  return ((p.y - top_y) < (bottom_y - p.y)) ? TopEdge() : BottomEdge() ;
}


/********************************************************************************/

Line Rectangle::nearestVEdgeLine(const Vector& p) 
{
  return ((p.x - left_x) < (right_x - p.x)) ? LeftEdge() : RightEdge() ;
}


/********************************************************************************/

Line Rectangle::nearestEdgeLine(const Vector& p) 
{
  if(Min((p.x-left_x),(right_x-p.x)) < Min((p.y-top_y),(bottom_y-p.y)))
    return nearestVEdgeLine(p) ;
  else
    return nearestHEdgeLine(p) ;  
}


/********************************************************************************/

float Rectangle::DistanceToEdge(const Vector &p) 
{
  if ( !IsWithin(p) ){
    Vector q = AdjustToWithin(p);
    return -(q.dist(p)); /* distance outside is a negative number */
  }
   
  return Min((p.x-left_x),Min((right_x-p.x),Min((p.y-top_y),(bottom_y-p.y))));
}

/********************************************************************************/

Vector Rectangle::nearestCorner(Vector p, float interior_dist) 
{
  Vector corner = TopLeftCorner();
  float dist = corner.dist(p);
  float tmp;

  if ( (tmp=TopRightCorner().dist(p)) < dist ){
    corner = TopRightCorner();
    dist = tmp;
  }

  if ( (tmp=BottomRightCorner().dist(p)) < dist ){
    corner = BottomRightCorner();
    dist = tmp;
  }

  if ( (tmp=BottomLeftCorner().dist(p)) < dist ){
    corner = BottomLeftCorner();
    dist = tmp;
  }

  /* shift the point towards the center */
  if ( interior_dist > 0 )
    return PointInBetween(corner, Center(), interior_dist);
  else
    return corner;
}

/********************************************************************************/

void Rectangle::FlipCoords(bool flipX, bool flipY)
{
  if (flipX) {
    float tmp = left_x;
    left_x = -right_x;
    right_x = -tmp;
  }
  if (flipY) {
    float tmp = top_y;
    top_y = -bottom_y;
    bottom_y = -tmp;
  }
}

/********************************************************************************/

Vector Rectangle::random() 
{
  static Vector r ;

  r.x = range_random(left_x, right_x) ;
  //r.y = range_random(bottom_y, top_y) ;
  r.y = range_random(top_y, bottom_y) ;

  return r ;
}

/********************************************************************************/

Rectangle Rectangle::expand(float val)
{
  return Rectangle(left_x - val, right_x + val, top_y - val, bottom_y + val);
}

/********************************************************************************/

void  Rectangle::Print(){
  printf("RECTANGLE:  x = %.1f to %.1f   y = %.1f to %.1f\n",
	 LeftX(),RightX(),TopY(),BottomY());
}

/********************************************************************************/

Vector Rectangle::RayIntersection(Ray r)
{
  if (!IsWithin(r.origin)) {
    my_error("Rectangle/Ray intersection only handles rays inside the rectangle!");
    return 0;
  }

  Vector int_pt[2];
  int num_int = 0;
  for (int i=0; i < 4; i++) {
    Vector pt;
    if (GetEdge(i).RayIntersection(r, &pt))
      int_pt[num_int++] = pt;
  }

  if (num_int == 0) {
    my_error("Rectangle ray intersection: no edge intersection?");
    return 0;
  } else if (num_int == 1) {
    return int_pt[0]; // same slope as one pair of edges 
  } else if (num_int == 2) {
    Rectangle temp_rect = expand(FLOAT_EPS);
    for (int j = 0; j < 2; j++)
      if (temp_rect.IsWithin(int_pt[j]))
	return int_pt[j];
    my_error("Rectangle ray intersection: how did I get here");
    return 0;
  } else {
    my_error("Rectangle ray intersection: how did num_int get so big: %d", num_int);
    return 0;
  }
}


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                         Line Class                                           */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

Line::Line(float x_coef, float y_coef, float constant) 
{
  A = x_coef;
  B = y_coef;
  C = constant;
}

/********************************************************************************/

void Line::LineFromTwoPoints(Vector pt1, Vector pt2)
{
  float temp = (pt2.x - pt1.x);
  if (fabs(temp) < FLOAT_EPS) {
    if (fabs(pt2.y - pt1.y) < FLOAT_EPS)
      my_error("LineFromTwoPoints: points can not be the same!");
    A = 1;
    B = 0;
  } else {
    float m = (pt2.y - pt1.y) / temp;
    A = -m;
    B = 1;
  }  
  C = -(A * pt2.x + B * pt2.y);
}

/********************************************************************************/

void Line::LineFromRay(Ray r)   
{
  if (fabs(r.direction.y) < FLOAT_EPS && fabs(r.direction.x) < FLOAT_EPS)
    my_error("LineFromRay: dir can not be zero");
  LineFromTwoPoints(r.origin, r.origin + r.direction);
}

/********************************************************************************/

Bool Line::PointOnLine(float x, float y)
{
  return (Bool)(fabs(A * x + B * y + C) < FLOAT_EPS);
}

/********************************************************************************/

float Line::dist(Vector pt)     
{
  return fabs( (A*pt.x + B*pt.y + C) / sqrt(Sqr(A) + Sqr(B)) );
}

/********************************************************************************/

float Line::dist2(Vector pt)     
{
  return fabs( Sqr(A*pt.x + B*pt.y + C) / (Sqr(A) + Sqr(B)) );
}

/********************************************************************************/

float Line::angle()
{
  return ATan(-A / B);
}

/********************************************************************************/

Vector Line::ProjectPointUsingCircle(Vector pt)
{
  /* here's the idea:
     first get the plane equation for the ray
     then use the closest distance formula to get the closest distance
     then using the eq for that line and the eq for a circle of the dist radius
       around our curretn position, find the nearest point.
       Whew! */

  my_error("You shouldn't call ProjectPointUsingCircle");
  
  /* compute the dist */
  Vector retPt;
  float d = dist(pt); /* the min distance */

  /* intersect the circle and the line */
  float a,b,c; /* coefficent in quadratic to solve for x */
  float disc; /* discriminant in quadratic equation */
  a = 1 + Sqr(A);
  b = 2 * (A * (pt.y + C) - pt.x);
  c = Sqr(pt.x) + Sqr(pt.y + C) - Sqr(d);
  disc = Sqr(b) - 4 * a * c;
  /* the discriminant should be zero since this is the radius
     is the closest distance between the center and the line */
  if (fabs(disc) > FLOAT_EPS)
    fprintf(stderr, "GetClosestPointToBallPath: discrimannt is bad! %f\n", disc);
  retPt.x = - b / (2 * a);

  /* we compute two possible solutions for y and then see which one is on the
     line of the ray's path */
  float sol1, sol2;
  sol1 = sqrt(Sqr(d) - Sqr(retPt.x - pt.x));
  sol2 = -sol1;
  sol1 += pt.y;
  sol2 += pt.y;

  if (fabs(A * (retPt.x) + B * sol1 + C) < FLOAT_EPS ){
    /* sol1 is on line */
    retPt.y = sol1;
  } else if (fabs(A * (retPt.x) + B * sol2 + C) < FLOAT_EPS ){
    /* sol2 is on line */
    retPt.y = sol2;
  } else
    fprintf(stderr, "GetClosestPointToBallPath: neither solution works!\n");

  DebugGeom(printf("  dist: %f\t ptMod: %f\n", d,
		  sqrt(Sqr(pt.x - retPt.x) + Sqr(pt.y - retPt.y))));

  return retPt;
}

/********************************************************************************/

float Line::get_y(float x)
{
  if ( B != 0 ) 
    return (-A*x - C)/B;
  
  my_error("can't get y"); 
  return 0;
}

/********************************************************************************/

float Line::get_x(float y)
{
  if ( A != 0 ) 
    return (-B*y - C)/A;
  
  my_error("can't get x"); 
  return 0;
}

/********************************************************************************/

Bool Line::InBetween(Vector pt, Vector end1, Vector end2)
{
  if (!OnLine(end1) || !OnLine(end2))
    my_error("Line::InBetween: passed in points that weren't on line");

  pt = ProjectPoint(pt);
  float dist2 = end1.dist2(end2);
  
  return (pt.dist2(end1) <= dist2 && pt.dist2(end2) <= dist2) ? TRUE : FALSE;
}

/********************************************************************************/

Vector Line::GetClosestPtInBetween(Vector pt, Vector end1, Vector end2)
{
  if (!OnLine(end1) || !OnLine(end2))
    my_error("Line::InBetween: passed in points that weren't on line");

  if (InBetween(pt, end1, end2))
    return ProjectPoint(pt);
  if (end1.dist2(pt) < end2.dist2(pt))
    return end1;
  else
    return end2;
}

/********************************************************************************/

float Line::GetDistToClosestPtInBetween(Vector pt, Vector end1, Vector end2)
{
  if (!OnLine(end1) || !OnLine(end2))
    my_error("Line::InBetween: passed in points that weren't on line");

  if (InBetween(pt, end1, end2))
    return dist(pt);
  float d1 = end1.dist(pt);
  float d2 = end2.dist(pt);
  if (d1 < d2)
    return d1;
  else
    return d2;
}


/********************************************************************************/

Vector Line::intersection(Line l)
{
  Vector result = 0;
  if (SameSlope(l)) {
    //if ( B == 0 && l.B == 0 || A/B == l.A/l.B ){
    my_error("Lines have same slope");
    DebugGeom(cout << "Lines have same slope" << endl);
    return result;
  }

  if ( B == 0 ){
    result.x = -C/A;
    result.y = l.get_y(result.x);
    return result;
  }

  if ( l.B == 0){
    result.x = -l.C/l.A;
    result.y = get_y(result.x);
    return result;
  }

  result.x = (C*l.B - B*l.C)/(l.A*B - A*l.B);
  result.y = get_y(result.x);
  return result;
}  

/********************************************************************************/
Bool Line::RayIntersection(Ray r, Vector *ppt)
{
  Line lRay(r);

  if (SameSlope(lRay))
    return FALSE;
  
  *ppt = intersection(lRay);
  return (r.InRightDir(*ppt))
      //fabs(GetNormalizeAngleDeg((*ppt - r.origin).dir() - r.direction.dir())) < 10)
    ? TRUE : FALSE;
}


/********************************************************************************/
Bool Line::IsPtCloserToPtOnLine(Vector pt1, Vector pt2, Vector targ_pt)
{
  if (!OnLine(targ_pt)) 
    my_error("IsPtCloserToPtOnLine: targ_pt not on line");

  pt1 = ProjectPoint(pt1);
  pt2 = ProjectPoint(pt2);

  return (pt1.dist(targ_pt) < pt2.dist(targ_pt)) ? TRUE : FALSE;  
}


/********************************************************************************/

//return TRUE on top/left part of plane
Bool Line::HalfPlaneTest(Vector pt)
{
  if (B==0)
    return (pt.x < -C/A) ? TRUE : FALSE;
  return (pt.y > get_y(pt.x)) ? TRUE : FALSE;
}

Bool Line::SameSlope(Line l)
{
  return ( B == 0 && l.B == 0 || A/B == l.A/l.B ) ? TRUE : FALSE;    
}


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                       Ray Class                                              */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
Ray::Ray(Vector orig, Vector dir) 
{
  origin = orig;
  if (fabs(dir.y) < FLOAT_EPS && fabs(dir.x) < FLOAT_EPS) {
    my_error("Ray: dir can not be zero");
    direction = Vector (1,0);
  } else {
    direction = dir;
    direction = direction.Normalize();
  }
}

Bool Ray::OnRay(Vector pt)
{
  Vector v = pt - origin;
  AngleDeg angoff = GetNormalizeAngleDeg(v.dir() - direction.dir());  
  return (fabs(angoff) < 90) && (fabs(Sin(angoff) * v.mod()) < FLOAT_EPS)
    ? TRUE : FALSE;
}

Bool Ray::InRightDir(Vector pt, float angle_buffer)
{
  return (fabs(GetNormalizeAngleDeg((pt - origin).dir() - direction.dir())) < 
	  angle_buffer)
    ? TRUE : FALSE;
}

Bool Ray::intersection(Line l, Vector *pPt)
{
  return l.RayIntersection(*this, pPt);
}

Bool Ray::intersection(Ray r, Vector *pPt)
{
  Line thisLine(*this), argLine(r);

  if (thisLine.SameSlope(argLine))
    return FALSE;
  *pPt = thisLine.intersection(argLine);

  /* now make sure that the intersection is the correct direction on both lines */
  return  (InRightDir(*pPt) && r.InRightDir(*pPt))
	   //	   fabs(GetNormalizeAngleDeg((*pPt - origin).dir() - direction.dir())) < 10 &&
	   //fabs(GetNormalizeAngleDeg((*pPt - r.origin).dir() - r.direction.dir())) < 10)
    ? TRUE : FALSE;
}

/* intersects a ray and a cricle */
/* return the number of solutions */
/* psol1 1 is not as afar along the ray as psol2 */
int Ray::CircleIntersect(float rad, Vector center, Vector* psol1, Vector* psol2)
{
  DebugGeom(cout << "RCI: origin: " << origin << "\tdirection: " << direction << endl
	    << "rad: " << rad << "\tcenter: " << center << endl);
  float a,b,c,disc;
  float t1, t2;
  a = Sqr(direction.x) + Sqr(direction.y);
  b = 2.0 * ((origin.x-center.x) * direction.x + (origin.y-center.y) * direction.y);
  c = Sqr(origin.x-center.x) + Sqr(origin.y-center.y) - Sqr(rad);
  DebugGeom(printf(" RCI: a: %f\tb: %f\t c: %f\n", a,b,c));
  
  disc = Sqr(b) - 4 * a * c;
  if (disc < 0) {
    DebugGeom(printf(" RCI disc < 0: %f\n", disc));
    return 0;
  }
  
  disc = sqrt(disc);
  t1 = (-b + disc) / (2.0 * a);
  t2 = (-b - disc) / (2.0 * a);
  DebugGeom(printf(" RCI: t1: %f\tt2: %f\n", t1, t2));
  
  if (t1 > t2) {
    DebugGeom(printf(" RCI: reversing t1, t2\n"));
    float temp = t1;
    t1 = t2;
    t2 = temp;
  }
 
  if (t1 > 0.0) {
    if (t2 > 0.0) {
      *psol1 = origin + direction * t1;
      *psol2 = origin + direction * t2;
      DebugGeom(printf(" RCI:two sols\n"));
      return 2;
    } else {
      my_error("RayCircleIntersect: weird roots");
      return 0;
    }
  } else if (t2 > 0.0) {
    *psol1 = origin + direction * t2;
    DebugGeom(printf(" RCI:t2 only sol\n"));
    return 1;
  } else
    return 0;

  return 0;
}

Vector Ray::RectangleIntersection(Rectangle R)
{
  return R.RayIntersection(*this);
}

Vector Ray::GetClosestPoint(Vector pt)
{
  Line l(*this);
  Vector close_pt = l.ProjectPoint(pt);
  if (OnRay(close_pt))
    return close_pt;
  else
    return origin;
}





/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/*                  Miscellaneous Geometry Functions                            */
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/


/********************************************************************************/

int QuadraticFormula(float a, float b, float c, float* psol1, float* psol2)
{
  float d = Sqr(b) - 4*a*c;
  if (fabs(d) < FLOAT_EPS) {
    *psol1 = -b / (2 * a);
    return 1;
  } else if (d < 0) {
    return 0;
  } else {
    d = sqrt(d);
    *psol1 = (-b + d ) / (2 * a);
    *psol2 = (-b - d ) / (2 * a);
    return 2;
  }
}

/********************************************************************************/

/* some test code
   Vector sol1, sol2;
  int num;
  num = LineCircleIntersect(LineFromTwoPoints(Vector(-10, 2), Vector(10, 2)),
					      1, Vector(1,2), &sol1, &sol2);
  cout << "num: " << num << "\tsol1: " << sol1 << "\tsol2: " << sol2 << endl;
  num = LineCircleIntersect(LineFromTwoPoints(Vector(2, 10), Vector(2, -10)),
					      1, Vector(2,1), &sol1, &sol2);
  cout << "num: " << num << "\tsol1: " << sol1 << "\tsol2: " << sol2 << endl;
  num = LineCircleIntersect(LineFromTwoPoints(Vector(-10, -10), Vector(10, 10)),
					      sqrt(2), Vector(1,1), &sol1, &sol2);
  cout << "num: " << num << "\tsol1: " << sol1 << "\tsol2: " << sol2 << endl;
  exit(1);
  */
int LineCircleIntersect(Line l, float rad, Vector center,
			Vector* psol1, Vector* psol2)
{
  *psol1 = *psol2 = Vector(0,0);
  if (fabs(l.A) > FLOAT_EPS) {
    float a,b,c;
    a = 1 + Sqr(l.B/l.A);
    b = 2*(-center.y + (l.C/l.A + center.x)*(l.B/l.A));
    c = -Sqr(rad) + Sqr(l.C/l.A + center.x) + Sqr(center.y);
    int numSol = QuadraticFormula(a, b, c, &(psol1->y), &(psol2->y));
    psol1->x = - ( (l.B * psol1->y + l.C) / l.A );
    psol2->x = - ( (l.B * psol2->y + l.C) / l.A );
    return numSol;
  } else {
    int numSol = QuadraticFormula(1, -2*center.x,
				  Sqr(center.x) + Sqr(l.C/l.B + center.y) - Sqr(rad),
				  &(psol1->x), &(psol2->x));
    psol1->y = psol2->y = - l.C / l.B;
    return numSol;
  }
}


/********************************************************************************/

/* loop over rectangle edges. See if we're outside of the edge.
   If so, try to intersect with edge to move it inside */
Vector AdjustPtToRectOnLine(Vector pt, Rectangle r, Line l)
{
  DebugGeom(cout << "Adjust: " << pt << "\t" << r << "\t" << l << endl);
  Vector c = r.Center();
  for (int i = 0; i < 4; i++) {
    Line edge = r.GetEdge(i);
    if ( edge.SameSlope(l) )
      continue;
    if (edge.HalfPlaneTest(pt) != edge.HalfPlaneTest(c)) {
      // pt is outside this edge
      DebugGeom(printf("AdjustPtToRectOnLine: HalfPlaneTest failed for %d\n", i));
      Vector newPt = edge.intersection(l);
      if (edge.InBetween(newPt, r.GetPoint(i), r.GetPoint(i+1))) {
	DebugGeom(printf("AdjustPtToRectOnLine: Intersection okay for %d\n", i));	
	return newPt;
      }      
    }    
  }
  return pt;
}

/********************************************************************************/

Bool InBetween(Vector pt, Vector end1, Vector end2)
{
  pt = LineFromTwoPoints(end1, end2).ProjectPoint(pt);
  float dist2 = end1.dist2(end2);
  
  return (pt.dist2(end1) <= dist2 && pt.dist2(end2) <= dist2) ? TRUE : FALSE;
}

/********************************************************************************/

Vector PointInBetween(Vector pt1, Vector pt2, float pt1dist)
{
  if ( pt1dist < 0 ) my_error ("no neg dists");
  Vector targ = pt2 - pt1;
  targ *= pt1dist / targ.mod();
  return(pt1 + targ);
}

/********************************************************************************/

Vector PointInFracBetween(Vector pt1, Vector pt2, float pt1frac=.5)
{
  Vector targ = pt2 - pt1;
  targ *= pt1frac;
  return(pt1 + targ);
}

/********************************************************************************/

AngleDeg GetAngle(Vector pt1, Vector vertex, Vector pt2)
{
  AngleDeg dir1 = (pt1-vertex).dir();
  AngleDeg dir2 = (pt2-vertex).dir();
  
  return fabs(GetNormalizeAngleDeg(dir1 - dir2));
}


/********************************************************************************/

AngleDeg AngleBisect(AngleDeg a1, AngleDeg a2)
{
  if ( fabs(a1-a2) > 180 )
    return GetNormalizeAngleDeg((Min(a1,a2)+360+Max(a1,a2))/2);
  
  return GetNormalizeAngleDeg((a1+a2)/2);
}

/********************************************************************************/

Vector GetClosestPtInBetween(Vector pt, Vector end1, Vector end2)
{
  Line l;
  l.LineFromTwoPoints(end1, end2);
  return l.GetClosestPtInBetween(pt, end1, end2);
}

/********************************************************************************/

float GetDistToClosestPtInBetween(Vector pt, Vector end1, Vector end2)
{
  Line l;
  l.LineFromTwoPoints(end1, end2);
  return l.GetDistToClosestPtInBetween(pt, end1, end2);
}

/********************************************************************************/

Bool IsPointInCone(Vector pt, float wid_dist_ratio, Vector end, Vector vert)
{
  Line l = LineFromTwoPoints(vert, end);
  Vector proj_pt = l.ProjectPoint(pt);
  return  ((proj_pt.dist2(pt) < proj_pt.dist2(vert)*wid_dist_ratio*wid_dist_ratio &&
	   l.InBetween(proj_pt, vert, end)))
    ? TRUE : FALSE;
}

/********************************************************************************/

//top_y is negative, bot_y is positive
float GetDistToVerticalSegment(float x_val, float top_y, float bot_y, Vector pt)
{
  if (top_y > bot_y) {
    float tmp = top_y;
    top_y = bot_y;
    bot_y = tmp;
  }
  if (pt.y < top_y) {
    return pt.dist(Vector(x_val, top_y));
  } else if (pt.y > bot_y) {
    return pt.dist(Vector(x_val, bot_y));
  } else {
    return fabs(pt.x - x_val);
  }
}
