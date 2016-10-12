#include "Vector2D.h"

#include "Vector2D.h"

#define EPSILON 0.0001

// ---------------------------------------------------------------------------

void Vector2DZero(Vector2D *pResult)
{
	pResult->x = 0;
	pResult->y = 0;
}

// ---------------------------------------------------------------------------

void Vector2DSet(Vector2D *pResult, float x, float y)
{
	pResult->x = x;
	pResult->y = y;
}

// ---------------------------------------------------------------------------

void Vector2DNeg(Vector2D *pResult, Vector2D *pVec0)
{
	pResult->x = -1 * pVec0->x;
	pResult->y = -1 * pVec0->y;
}

// ---------------------------------------------------------------------------

void Vector2DAdd(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1)
{
	pResult->x = pVec0->x + pVec1->x;
	pResult->y = pVec0->y + pVec1->y;
}

// ---------------------------------------------------------------------------

void Vector2DSub(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1)
{
	pResult->x = pVec0->x - pVec1->x;
	pResult->y = pVec0->y - pVec1->y;
}

// ---------------------------------------------------------------------------

void Vector2DNormalize(Vector2D *pResult, Vector2D *pVec0)
{
	float magnitude = sqrtf(powf(pVec0->x, 2) + powf(pVec0->y, 2));

	pResult->x = pVec0->x / magnitude;
	pResult->y = pVec0->y / magnitude;
}

// ---------------------------------------------------------------------------

void Vector2DScale(Vector2D *pResult, Vector2D *pVec0, float c)
{
	pResult->x = pVec0->x * c;
	pResult->y = pVec0->y *c;
}

// ---------------------------------------------------------------------------

//Scale THEN add
void Vector2DScaleAdd(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1, float c)
{
	pResult->x = c * pVec0->x + pVec1->x;
	pResult->y = c*pVec0->y + pVec1->y;
}

// ---------------------------------------------------------------------------

void Vector2DScaleSub(Vector2D *pResult, Vector2D *pVec0, Vector2D *pVec1, float c)
{
	pResult->x = c * pVec0->x - pVec1->x;
	pResult->y = c*pVec0->y - pVec1->y;
}

// ---------------------------------------------------------------------------

float Vector2DLength(Vector2D *pVec0)
{
	return sqrtf(powf(pVec0->x, 2) + powf(pVec0->y, 2));
}

// ---------------------------------------------------------------------------

float Vector2DSquareLength(Vector2D *pVec0)
{
	return powf(pVec0->x, 2) + powf(pVec0->y, 2);
}

// ---------------------------------------------------------------------------

float Vector2DDistance(Vector2D *pVec0, Vector2D *pVec1)
{
	return sqrtf(powf((pVec0->x - pVec1->x), 2) + powf(pVec0->y - pVec1->y, 2));
}

// ---------------------------------------------------------------------------

float Vector2DSquareDistance(Vector2D *pVec0, Vector2D *pVec1)
{
	return	(powf((pVec0->x - pVec1->x), 2) + powf(pVec0->y - pVec1->y, 2));

}

// ---------------------------------------------------------------------------

float Vector2DDotProduct(Vector2D *pVec0, Vector2D *pVec1)
{
	return pVec0->x * pVec1->x + pVec1->y * pVec0->y;
}

// ---------------------------------------------------------------------------

void Vector2DFromAngleDeg(Vector2D *pResult, float angle)
{
	Vector2DFromAngleRad(pResult, angle * 3.14159265358979323846f / 180.f);
}

// ---------------------------------------------------------------------------

void Vector2DFromAngleRad(Vector2D *pResult, float angle)
{
	pResult->x = cosf(angle);
	pResult->y = sinf(angle);
}

// ---------------------------------------------------------------------------
