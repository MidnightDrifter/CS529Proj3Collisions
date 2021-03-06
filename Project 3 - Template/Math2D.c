#include "Math2D.h"
#include "stdio.h"

int StaticPointToStaticCircle(Vector2D *pP, Vector2D *pCenter, float Radius)
{
	if (Vector2DSquareDistance(pP, pCenter) > Radius*Radius)
	{
		return 0;
	}
	return 1;
}


/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
int StaticPointToStaticRect(Vector2D *pPos, Vector2D *pRect, float Width, float Height)
{
	if (pPos->x < pRect->x - Width / 2 || pPos->x > pRect->x + Width / 2 || pPos->y < pRect->y - Height / 2 || pPos->y > pRect->y + Height / 2)
	{
		return 0;
	}

	return 1;
}

/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
int StaticCircleToStaticCircle(Vector2D *pCenter0, float Radius0, Vector2D *pCenter1, float Radius1)
{
	if (Vector2DSquareDistance(pCenter0, pCenter1) > powf(Radius0 + Radius1, 2))
	{
		return 0;
	}

	return 1;
}

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
int StaticRectToStaticRect(Vector2D *pRect0, float Width0, float Height0, Vector2D *pRect1, float Width1, float Height1)
{
	if (pRect0->x - Width0 / 2 > pRect1->x + Width1 / 2 || pRect1->x - Width1 / 2 > pRect0->x + Width0 / 2 || pRect0->y - Height0 / 2 > pRect1->y + Height1 / 2 || pRect1->y - Height1 / 2 > pRect0->y + Height0 / 2)
	{
		return 0;
	}

	return 1;
}

//////////////////////
// New to Project 2 //
//////////////////////

/*
This function checks if a static circle is intersecting with a static rectangle

Circle:		Center is "Center", radius is "Radius"
Rectangle:	Center is "Rect", width is "Width" and height is "Height"
Function returns true is the circle and rectangle are intersecting, otherwise it returns false
*/

int StaticCircleToStaticRectangle(Vector2D *pCenter, float Radius, Vector2D *pRect, float Width, float Height)
{

	Vector2D closestPoint;
	Vector2DSet(&closestPoint, 0, 0);

	//	SetVector2D(closestPoint, pRect->x, pRect->y);



	if (pCenter->x > pRect->x + 0.5*Width)
	{
		closestPoint.x = pRect->x + 0.5*Width;
	}

	else if (pCenter->x < pRect->x - 0.5*Width)
	{
		closestPoint.x = pRect->x - 0.5*Width;
	}

	else
	{
		closestPoint.x = pCenter->x;
	}


	if (pCenter->y > pRect->y + 0.5*Height)
	{
		closestPoint.y = pRect->y + 0.5*Height;
	}

	else if (pCenter->y < pRect->y - 0.5*Height)
	{
		closestPoint.y = pRect->y - 0.5*Height;
	}

	else
	{
		closestPoint.y = pCenter->y;
	}


	int output = StaticPointToStaticCircle(&closestPoint, pCenter, Radius);

	return output;
	//return 0;
}
//////////////////////
// New to project 3 //
//////////////////////


/*
This function determines the distance separating a point from a line

 - Parameters
	- P:		The point whose location should be determined
	- LS:		The line segment

 - Returned value: The distance. Note that the returned value should be:
	- Negative if the point is in the line's inside half plane
	- Positive if the point is in the line's outside half plane
	- Zero if the point is on the line
*/
float StaticPointToStaticLineSegment(Vector2D *P, LineSegment2D *LS)
{
	//return 0.0f;

	return (Vector2DDotProduct(&LS->mN, P) - LS->mNdotP0);

}


/*
This function checks whether an animated point is colliding with a line segment

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- LS:		The line segment
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedPointToStaticLineSegment(Vector2D *Ps, Vector2D *Pe, LineSegment2D *LS, Vector2D *Pi)
{
	if( (Pe->x == Ps->x && Pe->y == Ps->y) ||(Vector2DDotProduct(&LS->mN, Ps) > LS->mNdotP0 && Vector2DDotProduct(&LS->mN, Pe) > LS->mNdotP0) || (Vector2DDotProduct(&LS->mN, Ps) < LS->mNdotP0 && Vector2DDotProduct(&LS->mN, Pe) < LS->mNdotP0))
	{
		return -1.f;
	}
	//return -1.0f;
	Vector2D v;
	v.x = Pe->x - Ps->x;
	v.y = Pe->y - Ps->y;
	float t = Vector2DDotProduct(&LS->mN, &v);

	if (  t == 0.f)
	{
		return -1;
	}

	 t = ((LS->mNdotP0 - Vector2DDotProduct(&LS->mN, Ps))/ Vector2DDotProduct(&LS->mN, &v));

	 if (t > 1 || t < 0)
	 {
		 return -1.f;
	 }

	 Vector2D tempI, line, nLine, ItoP1, ItoP0;
	 
	 Vector2DScaleAdd(&tempI, &v, Ps, t);
	 Vector2DSub(&line, &LS->mP1, &LS->mP0);
	 Vector2DSub(&ItoP1, &tempI, &LS->mP1);
	 Vector2DSub(&ItoP0, &tempI, &LS->mP0);

	 Vector2DSub(&nLine, &LS->mP0, &LS->mP1);
	 if (Vector2DDotProduct(&line, &ItoP0) < 0 || Vector2DDotProduct(&nLine, &ItoP1) < 0)
	 {
		 return -1.f;
	 }


	 Vector2DSet(Pi, tempI.x, tempI.y);
	 return t;
	



}


/*
This function checks whether an animated circle is colliding with a line segment

 - Parameters
	- Ps:		The center's starting location
	- Pe:		The center's ending location
	- Radius:	The circle's radius
	- LS:		The line segment
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedCircleToStaticLineSegment(Vector2D *Ps, Vector2D *Pe, float Radius, LineSegment2D *LS, Vector2D *Pi)
{
	//return -1.0f;
	float nR = -1 * Radius;
	if ((Pe->x == Ps->x && Pe->y == Ps->y) || (Vector2DDotProduct(&LS->mN, Ps) - LS->mNdotP0) < nR  &&  (Vector2DDotProduct(&LS->mN, Pe) - LS->mNdotP0  < nR) || (Vector2DDotProduct(&LS->mN, Ps)- LS->mNdotP0  > Radius &&   Vector2DDotProduct(&LS->mN, Pe) - LS->mNdotP0 > Radius))
	{
		return -1.f;
	}
	//return -1.0f;

	float d=Radius;

	if (StaticPointToStaticLineSegment(Ps, LS) < 0)
	{
		d *= -1;
	}

	Vector2D v;
	v.x = Pe->x - Ps->x;
	v.y = Pe->y - Ps->y;
	float t = Vector2DDotProduct(&LS->mN, &v);

	if (t == 0.f)
	{
		return -1;
	}

	t = (((LS->mNdotP0 - Vector2DDotProduct(&LS->mN, Ps) + d)) / Vector2DDotProduct(&LS->mN, &v));

	if (t > 1 || t < 0)
	{
		return -1.f;
	}

	Vector2D tempI, line,nLine, ItoP1, ItoP0;
	Vector2DScaleAdd(&tempI, &v, Ps, t);
	Vector2DSub(&line, &LS->mP1, &LS->mP0);
	Vector2DSub(&ItoP1, &tempI, &LS->mP1);
	Vector2DSub(&ItoP0, &tempI, &LS->mP0);
	Vector2DSub(&nLine, &LS->mP0, &LS->mP1);
	if (Vector2DDotProduct(&line, &ItoP0) < 0 || Vector2DDotProduct(&nLine, &ItoP1) < 0)
	{
		return -1.f;
	}


	Vector2DSet(Pi, tempI.x, tempI.y);
	return t;
}


/*
This function reflects an animated point on a line segment.
It should first make sure that the animated point is intersecting with the line 

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- LS:		The line segment
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedPointOnStaticLineSegment(Vector2D *Ps, Vector2D *Pe, LineSegment2D *LS, Vector2D *Pi, Vector2D *R)
{
	float f = AnimatedPointToStaticLineSegment(Ps, Pe, LS, Pi);
	if (f < 0)
	{
		return -1.0f;
	}
	float speed = sqrtf(((Ps->x - Pe->x)*(Ps->x - Pe->x)) + ((Ps->y - Pe->y)*(Ps->y - Pe->y)));  //Scale down by a factor of 1-t  ?
	Vector2D i, r;
	Vector2DSet(&i, Pe->x - Pi->x, Pe->y - Pi->y);
	Vector2DScale(&r, &LS->mN, 2 * Vector2DDotProduct(&i, &LS->mN));
	//Vector2DAdd(&i, Pi, &i);
	Vector2DSub(&r, &i, &r);
	Vector2DSet(R, r.x, r.y);
	Vector2DNormalize(R, R);
	//Vector2DScale(R, R, speed);
	//Vector2DAdd(Pe, &r, Pi);
	return f;




}


/*
This function reflects an animated circle on a line segment.
It should first make sure that the animated point is intersecting with the line 

 - Parameters
	- Ps:		The center's starting location
	- Pe:		The center's ending location
	- Radius:	The circle's radius
	- LS:		The line segment
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedCircleOnStaticLineSegment(Vector2D *Ps, Vector2D *Pe, float Radius, LineSegment2D *LS, Vector2D *Pi, Vector2D *R)
{
	//return -1.0f;

	float f = AnimatedCircleToStaticLineSegment(Ps, Pe,Radius, LS, Pi);
	if (f < 0)
	{
		return -1.0f;
	}
	float speed = sqrtf(((Ps->x - Pe->x)*(Ps->x - Pe->x)) + ((Ps->y - Pe->y)*(Ps->y - Pe->y)));  //Scale down by a factor of 1-t  ?
	Vector2D i, r;
	Vector2DSet(&i, Pe->x - Pi->x, Pe->y - Pi->y);
	Vector2DScale(&r, &LS->mN, 2 * Vector2DDotProduct(&i, &LS->mN));
//	Vector2DAdd(&i, Pi, &i);
	Vector2DSub(&r, &i, &r);
	Vector2DNormalize(&r, &r);
	Vector2DSet(R, r.x, r.y);
	//Vector2DNormalize(R, R);
	//Vector2DScale(&r, &r, speed);
	//Vector2DAdd(Pe, &r, Pi); 
	return f;

}


/*
This function checks whether an animated point is colliding with a static circle

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- Center:	The circle's center
	- Radius:	The circle's radius
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedPointToStaticCircle(Vector2D *Ps, Vector2D *Pe, Vector2D *Center, float Radius, Vector2D *Pi)
{
	//return -1.0f;
	Vector2D v, bc, vUnit;
	float f, disc, a, b, c,m,n;

	Vector2DSub(&v, Pe, Ps);
	Vector2DSub(&bc, Center, Ps);
	Vector2DNormalize(&vUnit, &v);
	m = Vector2DDotProduct(&bc, &vUnit);
	n = ((v.x * v.x) + (v.y * v.y)) - (m*m);
	if ((Pe->x == Ps->x && Pe->y == Ps->y) || (n > Radius*Radius) || (m < 0 && Vector2DSquareDistance(Ps,Center) > Radius*Radius))
	{
		return -1.f;
	}
	//Vector2D bc2;
	//Vector2DScale(&bc2, &bc, -2.f);
	a = Vector2DDotProduct(&v, &v);
	b = -2*  Vector2DDotProduct(&bc, &v);
	c = (Vector2DDotProduct(&bc, &bc)) - (Radius * Radius);
	disc = (b*b) - (4.f * a*c);

	if(disc<0)
	{
		return -1.f;
	}

	else if (disc ==0)
	{
		f = b / (-2 * a);
	}

	else
	{
		float f1, f2;
		f1 = ((-1 * b) + sqrtf(disc)) / (2 * a);
		f2 = ((-1 * b) - sqrtf(disc)) / (2 * a);
		f = fminf(f1,f2);
	}



	if (f > 1.f || f < 0.f)
	{
		return -1;
	}
	Vector2DScaleAdd(Pi, &v, Ps, f);
	return f;

}



/*
This function reflects an animated point on a static circle.
It should first make sure that the animated point is intersecting with the circle 

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- Center:	The circle's center
	- Radius:	The circle's radius
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedPointOnStaticCircle(Vector2D *Ps, Vector2D *Pe, Vector2D *Center, float Radius, Vector2D *Pi, Vector2D *R)
{
	//return -1.0f;

	float f = AnimatedPointToStaticCircle(Ps, Pe,Center, Radius, Pi);
	if(f<0)
	{
		return -1.f;
	}
	//float speed = sqrtf(((Ps->x - Pe->x)*(Ps->x - Pe->x)) + ((Ps->y - Pe->y)*(Ps->y - Pe->y)));
	//speed *= (1.f - f);
	Vector2D m, n,nS;
	Vector2DSub(&m, Ps, Pi);
	Vector2DSub(&n, Pi, Center);
	Vector2DNormalize(&n, &n);
	Vector2DScale(&nS, &n, 2 * Vector2DDotProduct(&m, &n));
	Vector2DSub(R, &nS, &m);
	Vector2DNormalize(R, R);
	//Vector2DSet(R, nS.x, n.y);
	//Vector2DScale(&n, &n, speed);
	//Vector2DAdd(R, Pi, &n);
	return f;
}


/*
This function checks whether an animated circle is colliding with a static circle

 - Parameters
	- Center0s:		The starting position of the animated circle's center 
	- Center0e:		The ending position of the animated circle's center 
	- Radius0:		The animated circle's radius
	- Center1:		The static circle's center
	- Radius1:		The static circle's radius
	- Pi:			This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedCircleToStaticCircle(Vector2D *Center0s, Vector2D *Center0e, float Radius0, Vector2D *Center1, float Radius1, Vector2D *Pi)
{
	//return -1.0f;
	return AnimatedPointToStaticCircle(Center0s, Center0e, Center1, (Radius0 + Radius1), Pi);
}


/*
This function reflects an animated circle on a static circle.
It should first make sure that the animated circle is intersecting with the static one 

 - Parameters
	- Center0s:		The starting position of the animated circle's center 
	- Center0e:		The ending position of the animated circle's center 
	- Radius0:		The animated circle's radius
	- Center1:		The static circle's center
	- Radius1:		The static circle's radius
	- Pi:			This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:			Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedCircleOnStaticCircle(Vector2D *Center0s, Vector2D *Center0e, float Radius0, Vector2D *Center1, float Radius1, Vector2D *Pi, Vector2D *R)
{
//	return -1.0f;

	return ReflectAnimatedPointOnStaticCircle(Center0s, Center0e, Center1, (Radius0 + Radius1), Pi, R);

}
