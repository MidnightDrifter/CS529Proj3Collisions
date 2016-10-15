#include "LineSegment2D.h"


int BuildLineSegment2D(LineSegment2D *LS, Vector2D *Point0, Vector2D *Point1)
{
	if (Point0->x == Point1->x && Point0->y == Point1->y)
	{
		return 0;
	}

	else
	{
		LS->mP0.x = Point0->x;
		LS->mP0.y = Point0->y;
		LS->mP1.x = Point1->x;
		LS->mP1.y = Point1->y;

		LS->mN.x = LS->mP1.y - LS->mP0.y;
		LS->mN.y = (LS->mP0.x - LS->mP1.x);

		Vector2DNormalize(&LS->mN, &LS->mN);
		LS->mNdotP0 = Vector2DDotProduct(&LS->mN, &LS->mP0);

		return 1;
	}
	//return 0;
}