// ---------------------------------------------------------------------------
// Project Name		:	Cage Game
// File Name		:	GameState_Menu.cpp
// Author			:	Antoine Abi Chakra
// Creation Date	:	2012/03/16
// Purpose			:	implementation of the 'play' game state
// History			:
// - 
// ---------------------------------------------------------------------------

#include "main.h"

// ---------------------------------------------------------------------------
// Defines

#define SHAPE_NUM_MAX				32					// The total number of different vertex buffer (Shape)
#define GAME_OBJ_INST_NUM_MAX		2048				// The total number of different game object instances

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// object flag definition

#define FLAG_ACTIVE		0x00000001

#define TEST_PART_2				0
#define DRAW_DEBUG				0									// Set this to 1 in order to draw debug data


#define LINE_SEGMENTS_NUM		5									// Don't change
#define BALL_RADIUS				15.0f
#define PILLARS_NUM				6									// Don't change


// ---------------------------------------------------------------------------

enum OBJECT_TYPE
{
	OBJECT_TYPE_BALL,
	OBJECT_TYPE_LINE,
	OBJECT_TYPE_PILLAR,
	OBJECT_TYPE_DEBUG_LINE,
};

// Struct/Class definitions

typedef struct GameObjectInstance GameObjectInstance;			// Forward declaration needed, since components need to point to their owner "GameObjectInstance"

// ---------------------------------------------------------------------------

typedef struct
{
	unsigned long			mType;				// Object type (Ship, bullet, etc..)
	AEGfxVertexList*		mpMesh;				// This will hold the triangles which will form the shape of the object

}Shape;

// ---------------------------------------------------------------------------

typedef struct
{
	Shape *mpShape;

	GameObjectInstance *	mpOwner;			// This component's owner
}Component_Sprite;

// ---------------------------------------------------------------------------

typedef struct
{
	Vector2D					mPosition;		// Current position
	float					mAngle;			// Current angle
	float					mScaleX;		// Current X scaling value
	float					mScaleY;		// Current Y scaling value

	Matrix2D					mTransform;		// Object transformation matrix: Each frame, calculate the object instance's transformation matrix and save it here

	GameObjectInstance *	mpOwner;		// This component's owner
}Component_Transform;

// ---------------------------------------------------------------------------

typedef struct
{
	Vector2D					mVelocity;		// Current velocity

	GameObjectInstance *	mpOwner;		// This component's owner
}Component_Physics;

// ---------------------------------------------------------------------------

//Game object instance structure
struct GameObjectInstance
{
	unsigned long				mFlag;						// Bit mFlag, used to indicate if the object instance is active or not

	Component_Sprite			*mpComponent_Sprite;		// Sprite component
	Component_Transform			*mpComponent_Transform;		// Transform component
	Component_Physics			*mpComponent_Physics;		// Physics component
};

// ---------------------------------------------------------------------------
// Static variables

// List of original vertex buffers
static Shape				sgShapes[SHAPE_NUM_MAX];									// Each element in this array represents a unique shape 
static unsigned long		sgShapeNum;													// The number of defined shapes

// list of object instances
static GameObjectInstance		sgGameObjectInstanceList[GAME_OBJ_INST_NUM_MAX];		// Each element in this array represents a unique game object instance
static unsigned long			sgGameObjectInstanceNum;								// The number of active game object instances

static Vector2D			gRoomPoints[LINE_SEGMENTS_NUM * 2];
static LineSegment2D	gRoomLineSegments[LINE_SEGMENTS_NUM];

#if(TEST_PART_2)

static Vector2D			gPillarsCenters[PILLARS_NUM];
static float			gPillarsRadii[PILLARS_NUM];
static LineSegment2D	gPillarsWalls[PILLARS_NUM / 2];

#endif


// functions to create/destroy a game object instance
static GameObjectInstance*			GameObjectInstanceCreate(unsigned int ObjectType);			// From OBJECT_TYPE enum
static void							GameObjectInstanceDestroy(GameObjectInstance* pInst);

// ---------------------------------------------------------------------------

// Functions to add/remove components
static void AddComponent_Transform(GameObjectInstance *pInst, Vector2D *pPosition, float Angle, float ScaleX, float ScaleY);
static void AddComponent_Sprite(GameObjectInstance *pInst, unsigned int ShapeType);
static void AddComponent_Physics(GameObjectInstance *pInst, Vector2D *pVelocity);

static void RemoveComponent_Transform(GameObjectInstance *pInst);
static void RemoveComponent_Sprite(GameObjectInstance *pInst);
static void RemoveComponent_Physics(GameObjectInstance *pInst);

static GameObjectInstance		*spBall;
static GameObjectInstance		*spBallVelocityDebugLine;

// ---------------------------------------------------------------------------

static int			sgStopped = 0;

// ---------------------------------------------------------------------------

void GameStatePlayLoad(void)
{
	unsigned int i;
	float CircleAngleStep;
	int Parts;
	Shape* pShape;

	// Zero the shapes array
	memset(sgShapes, 0, sizeof(Shape)* SHAPE_NUM_MAX);
	// No shapes at this point
	sgShapeNum = 0;


	// ===============
	// create the ball
	// ===============
	pShape = sgShapes + sgShapeNum++;
	pShape->mType = OBJECT_TYPE_BALL;

	AEGfxMeshStart();
	CircleAngleStep = PI / 12.0f, i;
	Parts = 24;
	for (i = 0; i < Parts; ++i)
	{
		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf(i * 2 * PI / Parts)*0.5f, sinf(i * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf((i + 1) * 2 * PI / Parts)*0.5f, sinf((i + 1) * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	}

	pShape->mpMesh = AEGfxMeshEnd();

	// ===============
	// create the line
	// ===============
	pShape = sgShapes + sgShapeNum++;
	pShape->mType = OBJECT_TYPE_LINE;

	AEGfxMeshStart();

	AEGfxVertexAdd(0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	AEGfxVertexAdd(1.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

	pShape->mpMesh = AEGfxMeshEnd();


	// =================
	// create the pillar
	// =================
	pShape = sgShapes + sgShapeNum++;
	pShape->mType = OBJECT_TYPE_PILLAR;

	AEGfxMeshStart();
	CircleAngleStep = PI / 12.0f, i;
	Parts = 24;
	for (i = 0; i < Parts; ++i)
	{
		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf(i * 2 * PI / Parts)*0.5f, sinf(i * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f,
			cosf((i + 1) * 2 * PI / Parts)*0.5f, sinf((i + 1) * 2 * PI / Parts)*0.5f, 0xFFFFFF00, 0.0f, 0.0f);
	}

	pShape->mpMesh = AEGfxMeshEnd();


	// =====================
	// create the debug line
	// =====================
	pShape = sgShapes + sgShapeNum++;
	pShape->mType = OBJECT_TYPE_DEBUG_LINE;

	AEGfxMeshStart();

	AEGfxVertexAdd(0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	AEGfxVertexAdd(1.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);

	pShape->mpMesh = AEGfxMeshEnd();


	// Building map boundaries
	Vector2DSet(&gRoomPoints[0], -350.0f, 100.0f);		Vector2DSet(&gRoomPoints[1], 0, 250.0f);
	Vector2DSet(&gRoomPoints[2], 0, 250.0f);				Vector2DSet(&gRoomPoints[3], 350.0f, 100.0f);
	Vector2DSet(&gRoomPoints[4], 350.0f, 100.0f);			Vector2DSet(&gRoomPoints[5], 275.0f, -250.0f);
	Vector2DSet(&gRoomPoints[6], 275.0f, -250.0f);		Vector2DSet(&gRoomPoints[7], -275.0f, -250.0);
	Vector2DSet(&gRoomPoints[8], -275.0f, -250.0f);		Vector2DSet(&gRoomPoints[9], -350.0f, 100.0f);

	// Build line segments
	for(i = 0; i < LINE_SEGMENTS_NUM; ++i)
		LineSegment2DBuildLineSegment2D(&gRoomLineSegments[i], &gRoomPoints[i*2], &gRoomPoints[i*2 + 1]);


#if(TEST_PART_2)
	// Pillars
	Vector2DSet(&gPillarsCenters[0], -200.0f, 0.0f);
	gPillarsRadii[0] = 15;
	
	Vector2DSet(&gPillarsCenters[1], -150.0f, 100.0f);
	gPillarsRadii[1] = 20;
	
	Vector2DSet(&gPillarsCenters[2], -100.0f, -150.0f);
	gPillarsRadii[2] = 15;

	Vector2DSet(&gPillarsCenters[3], 100.0f, -175.0f);
	gPillarsRadii[3] = 10;
	
	Vector2DSet(&gPillarsCenters[4], 175.0f, 100.0f);
	gPillarsRadii[4] = 20;
	
	Vector2DSet(&gPillarsCenters[5], 225.0f, -25.0f);
	gPillarsRadii[5] = 10;

	// Pillars walls
	for(i = 0; i < PILLARS_NUM/2; ++i)
		BuildLineSegment2D(&gPillarsWalls[i], &gPillarsCenters[i*2], &gPillarsCenters[i*2 + 1]);

#endif
}

// ---------------------------------------------------------------------------

void GameStatePlayInit(void)
{
	unsigned int i;

	// zero the game object instance array
	memset(sgGameObjectInstanceList, 0, sizeof(GameObjectInstance)* GAME_OBJ_INST_NUM_MAX);
	// No game object instances (sprites) at this point
	sgGameObjectInstanceNum = 0;

	spBall = GameObjectInstanceCreate(OBJECT_TYPE_BALL);
 
	Vector2DSet(&spBall->mpComponent_Transform->mPosition, 0.0f, 0.0f);
	spBall->mpComponent_Transform->mScaleX = BALL_RADIUS * 2;
	spBall->mpComponent_Transform->mScaleY = BALL_RADIUS * 2;
	Vector2DSet(&spBall->mpComponent_Physics->mVelocity, 130.0f, 110.0f);


	// Wall instances
	for(i = 0; i < LINE_SEGMENTS_NUM; ++i)
	{	
		float length, angle;
		Vector2D v;
		GameObjectInstance *pWall;

		length = Vector2DDistance(&gRoomPoints[2*i], &gRoomPoints[2*i + 1]);
		
		Vector2DSub(&v, &gRoomPoints[2*i + 1], &gRoomPoints[2*i]);
		Vector2DNormalize(&v, &v);
		
		angle = atan2(v.y, v.x);
		pWall = GameObjectInstanceCreate(OBJECT_TYPE_LINE);
		pWall->mpComponent_Transform->mPosition = gRoomPoints[2 * i];
		pWall->mpComponent_Transform->mScaleX = length;
		pWall->mpComponent_Transform->mScaleY = 5.0f;
		pWall->mpComponent_Transform->mAngle = angle;
	}

#if(TEST_PART_2)
	// Segments between pillars instances, and pillars instances
	for(i = 0; i < (PILLARS_NUM >> 1); ++i)
	{	
		float length, angle;
		Vector2D v;
		GameObjectInstance *pInst;

		// segments
		length = Vector2DDistance(&gPillarsCenters[2*i], &gPillarsCenters[2*i + 1]);
	
		Vector2DSub(&v, &gPillarsCenters[2*i + 1], &gPillarsCenters[2*i]);
		Vector2DNormalize(&v, &v);
		
		angle = atan2(v.y, v.x);

		pInst = GameObjectInstanceCreate(OBJECT_TYPE_LINE);
		pInst->mpComponent_Transform->mPosition = gPillarsCenters[2 * i];
		pInst->mpComponent_Transform->mScaleX = length;
		pInst->mpComponent_Transform->mScaleY = 5.0f;
		pInst->mpComponent_Transform->mAngle = angle;

		// Pillars
		pInst = GameObjectInstanceCreate(OBJECT_TYPE_PILLAR);
		pInst->mpComponent_Transform->mPosition = gPillarsCenters[2 * i];
		pInst->mpComponent_Transform->mScaleX = gPillarsRadii[2 * i] * 2;
		pInst->mpComponent_Transform->mScaleY = gPillarsRadii[2 * i] * 2;

		pInst = GameObjectInstanceCreate(OBJECT_TYPE_PILLAR);
		pInst->mpComponent_Transform->mPosition = gPillarsCenters[2 * i + 1];
		pInst->mpComponent_Transform->mScaleX = gPillarsRadii[2 * i + 1] * 2;
		pInst->mpComponent_Transform->mScaleY = gPillarsRadii[2 * i + 1] * 2;
	}
#endif

#if(DRAW_DEBUG)

	// Normals of the outer lines
	for(i = 0; i < LINE_SEGMENTS_NUM; ++i)
	{	
		Vector2D pos;
		float angle;
		GameObjectInstance *pInst;

		pos.x = (gRoomPoints[2*i].x + gRoomPoints[2*i + 1].x) / 2.0f;
		pos.y = (gRoomPoints[2*i].y + gRoomPoints[2*i + 1].y) / 2.0f;
		
		angle = Vector2DAngleFromVec2(&gRoomLineSegments[i].mN);

		pInst = GameObjectInstanceCreate(OBJECT_TYPE_DEBUG_LINE);
		pInst->mpComponent_Transform->mPosition = pos;
		pInst->mpComponent_Transform->mScaleX = 25.0f;
		pInst->mpComponent_Transform->mScaleY = 5.0f;
		pInst->mpComponent_Transform->mAngle = angle;
	}

	spBallVelocityDebugLine = GameObjectInstanceCreate(OBJECT_TYPE_DEBUG_LINE);
	spBallVelocityDebugLine->mpComponent_Transform->mScaleX = 50.0f;
	spBallVelocityDebugLine->mpComponent_Transform->mScaleY = 1.0f;

#if(TEST_PART_2)
	// Normals of the inner lines
	for(i = 0; i < (PILLARS_NUM >> 1); ++i)
	{	
		Vector2D pos;
		float angle;
		GameObjectInstance *pInst;

		pos.x = (gPillarsCenters[2*i].x + gPillarsCenters[2*i + 1].x) / 2.0f;
		pos.y = (gPillarsCenters[2*i].y + gPillarsCenters[2*i + 1].y) / 2.0f;

		angle = Vector2DAngleFromVec2(&gPillarsWalls[i].mN);

		pInst = GameObjectInstanceCreate(OBJECT_TYPE_DEBUG_LINE);
		pInst->mpComponent_Transform->mPosition = pos;
		pInst->mpComponent_Transform->mScaleX = 25.0f;
		pInst->mpComponent_Transform->mScaleY = 5.0f;
		pInst->mpComponent_Transform->mAngle = angle;
	}
#endif

#endif

	
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
}

// ---------------------------------------------------------------------------

void GameStatePlayUpdate(void)
{
	float smallestT;
	Vector2D closestIntersectionPoint, r;
	unsigned int i;
	Vector2D newBallPos;
	Vector2D intersectionPoint;


	float frameTime = AEFrameRateControllerGetFrameTime();
	int stopStep = 0;

	if (0 == sgStopped)
	{
		if (AEInputCheckTriggered('S')) // 'S' to stop the simulation
			sgStopped = 1;
	}
	else
	{
		if (AEInputCheckTriggered('R'))	// 'R' to let the simulation run
			sgStopped = 0;
		else
		if (AEInputCheckTriggered('S'))	// 1 step per 'S' trigger
		{
			frameTime = 0.016f;
			stopStep = 1;
		}
		else
		if (AEInputCheckCurr('G'))		// Simulation runs as long as 'G' is pressed
		{
			frameTime = 0.016f;
			stopStep = 1;
		}
	}

	if (0 == sgStopped || 1 == stopStep)
	{

		// =================
		// update the input
		// =================

		// Update the positions of objects

		Vector2DScaleAdd(&newBallPos, &spBall->mpComponent_Physics->mVelocity, &spBall->mpComponent_Transform->mPosition, frameTime);


		smallestT = -1.0f;

		// Collision with line segments
		for(i = 0; i < LINE_SEGMENTS_NUM; ++i)
		{
			float t = ReflectAnimatedCircleOnStaticLineSegment(&spBall->mpComponent_Transform->mPosition, &newBallPos, BALL_RADIUS, &gRoomLineSegments[i], &intersectionPoint, &r);

			if(t > 0.0f && (t < smallestT || smallestT < 0.0f))
			{
				closestIntersectionPoint = intersectionPoint;
				smallestT = t;
			}
		}

#if(TEST_PART_2)

		// Collision with pillars (Static circles)
		for(i = 0; i < PILLARS_NUM; ++i)
		{
			float t = ReflectAnimatedCircleOnStaticCircle(&spBall->mpComponent_Transform->mPosition, &newBallPos, BALL_RADIUS, &gPillarsCenters[i], gPillarsRadii[i], &intersectionPoint, &r);

			if(t > 0.0f && (t < smallestT || smallestT < 0.0f))
			{
				closestIntersectionPoint = intersectionPoint;
				smallestT = t;
			}
		}

		// Collision with pillars' walls (Line segments between the static circles)
		for (i = 0; i < PILLARS_NUM / 2; ++i)
		{
			float t = ReflectAnimatedCircleOnStaticLineSegment(&spBall->mpComponent_Transform->mPosition, &newBallPos, BALL_RADIUS, &gPillarsWalls[i], &intersectionPoint, &r);

			if (t > 0.0f && (t < smallestT || smallestT < 0.0f))
			{
				closestIntersectionPoint = intersectionPoint;
				smallestT = t;
			}
		}

#endif

		if (smallestT > 0.0)
		{
			Vector2DAdd(&spBall->mpComponent_Transform->mPosition, &closestIntersectionPoint, &r);
			Vector2DNormalize(&r, &r);
			Vector2DScale(&spBall->mpComponent_Physics->mVelocity, &r, Vector2DLength(&spBall->mpComponent_Physics->mVelocity));
		}

		Vector2DScaleAdd(&spBall->mpComponent_Transform->mPosition, &spBall->mpComponent_Physics->mVelocity, &spBall->mpComponent_Transform->mPosition, frameTime);


#if(DRAW_DEBUG)
		{
			float cosine, sine, velLength, angle;

			velLength = Vector2DLength(&spBall->mpComponent_Physics->mVelocity);
			cosine = spBall->mpComponent_Physics->mVelocity.x / velLength;
			sine = spBall->mpComponent_Physics->mVelocity.y / velLength;

			angle = acosf(cosine);

			if (sine < 0)
				angle = -angle;

			spBallVelocityDebugLine->mpComponent_Transform->mPosition = spBall->mpComponent_Transform->mPosition;
			spBallVelocityDebugLine->mpComponent_Transform->mAngle = angle;
		}

#endif

	}

	//Computing the transformation matrices of the game object instances
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		Matrix2D scale, rot, trans;
		GameObjectInstance* pInst = sgGameObjectInstanceList + i;

		// skip non-active object
		if (0 == (pInst->mFlag & FLAG_ACTIVE))
			continue;

		Matrix2DScale(&scale, pInst->mpComponent_Transform->mScaleX, pInst->mpComponent_Transform->mScaleY);
		Matrix2DRot(&rot, pInst->mpComponent_Transform->mAngle);
		Matrix2DTrans(&trans, pInst->mpComponent_Transform->mPosition.x, pInst->mpComponent_Transform->mPosition.y);

		Matrix2DConcat(&pInst->mpComponent_Transform->mTransform, &trans, &rot);
		Matrix2DConcat(&pInst->mpComponent_Transform->mTransform, &pInst->mpComponent_Transform->mTransform, &scale);
	}
}

// ---------------------------------------------------------------------------

void GameStatePlayDraw(void)
{
	unsigned int i;

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);

	// draw all object in the list
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjectInstance* pInst = sgGameObjectInstanceList + i;

		// skip non-active object
		if (0 == (pInst->mFlag & FLAG_ACTIVE))
			continue;

		AEGfxSetTransform(pInst->mpComponent_Transform->mTransform.m);

		switch (pInst->mpComponent_Sprite->mpShape->mType)
		{
		case OBJECT_TYPE_BALL:
		case OBJECT_TYPE_PILLAR:
			AEGfxMeshDraw(pInst->mpComponent_Sprite->mpShape->mpMesh, AE_GFX_MDM_TRIANGLES);

			break;

		case OBJECT_TYPE_LINE:
		case OBJECT_TYPE_DEBUG_LINE:
			AEGfxMeshDraw(pInst->mpComponent_Sprite->mpShape->mpMesh, AE_GFX_MDM_LINES);

			break;
		}

	}
}

// ---------------------------------------------------------------------------

void GameStatePlayFree(void)
{
	unsigned int i;
	// kill all object in the list
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		GameObjectInstanceDestroy(sgGameObjectInstanceList + i);

	sgGameObjectInstanceNum = 0;
}

// ---------------------------------------------------------------------------

void GameStatePlayUnload(void)
{
	unsigned int i;

	// free all mesh
	for (i = 0; i < sgShapeNum; i++)
		AEGfxMeshFree(sgShapes[i].mpMesh);
}

// ---------------------------------------------------------------------------

GameObjectInstance* GameObjectInstanceCreate(unsigned int ObjectType)			// From OBJECT_TYPE enum)
{
	unsigned long i;

	// loop through the object instance list to find a non-used object instance
	for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjectInstance* pInst = sgGameObjectInstanceList + i;

		// Check if current instance is not used
		if (pInst->mFlag == 0)
		{
			// It is not used => use it to create the new instance

			// Active the game object instance
			pInst->mFlag = FLAG_ACTIVE;

			pInst->mpComponent_Transform = 0;
			pInst->mpComponent_Sprite = 0;
			pInst->mpComponent_Physics = 0;

			// Add the components, based on the object type
			switch (ObjectType)
			{
			case OBJECT_TYPE_BALL:
				AddComponent_Sprite(pInst, OBJECT_TYPE_BALL);
				AddComponent_Transform(pInst, 0, 0.0f, 1.0f, 1.0f);
				AddComponent_Physics(pInst, 0);
				break;

			case OBJECT_TYPE_LINE:
				AddComponent_Sprite(pInst, OBJECT_TYPE_LINE);
				AddComponent_Transform(pInst, 0, 0.0f, 1.0f, 1.0f);
				break;

			case OBJECT_TYPE_PILLAR:
				AddComponent_Sprite(pInst, OBJECT_TYPE_PILLAR);
				AddComponent_Transform(pInst, 0, 0.0f, 1.0f, 1.0f);
				break;

			case OBJECT_TYPE_DEBUG_LINE:
				AddComponent_Sprite(pInst, OBJECT_TYPE_DEBUG_LINE);
				AddComponent_Transform(pInst, 0, 0.0f, 1.0f, 1.0f);
				break;
			}

			++sgGameObjectInstanceNum;

			// return the newly created instance
			return pInst;
		}
	}

	// Cannot find empty slot => return 0
	return 0;
}

// ---------------------------------------------------------------------------

void GameObjectInstanceDestroy(GameObjectInstance* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->mFlag == 0)
		return;

	// Zero out the mFlag
	pInst->mFlag = 0;

	RemoveComponent_Transform(pInst);
	RemoveComponent_Sprite(pInst);
	RemoveComponent_Physics(pInst);

	--sgGameObjectInstanceNum;
}

// ---------------------------------------------------------------------------

void AddComponent_Transform(GameObjectInstance *pInst, Vector2D *pPosition, float Angle, float ScaleX, float ScaleY)
{
	if (0 != pInst)
	{
		if (0 == pInst->mpComponent_Transform)
		{
			pInst->mpComponent_Transform = (Component_Transform *)calloc(1, sizeof(Component_Transform));
		}

		Vector2D zeroVec2;
		Vector2DZero(&zeroVec2);

		pInst->mpComponent_Transform->mScaleX = ScaleX;
		pInst->mpComponent_Transform->mScaleY = ScaleY;
		pInst->mpComponent_Transform->mPosition = pPosition ? *pPosition : zeroVec2;;
		pInst->mpComponent_Transform->mAngle = Angle;
		pInst->mpComponent_Transform->mpOwner = pInst;
	}
}

// ---------------------------------------------------------------------------

void AddComponent_Sprite(GameObjectInstance *pInst, unsigned int ShapeType)
{
	if (0 != pInst)
	{
		if (0 == pInst->mpComponent_Sprite)
		{
			pInst->mpComponent_Sprite = (Component_Sprite *)calloc(1, sizeof(Component_Sprite));
		}

		pInst->mpComponent_Sprite->mpShape = sgShapes + ShapeType;
		pInst->mpComponent_Sprite->mpOwner = pInst;
	}
}

// ---------------------------------------------------------------------------

void AddComponent_Physics(GameObjectInstance *pInst, Vector2D *pVelocity)
{
	if (0 != pInst)
	{
		if (0 == pInst->mpComponent_Physics)
		{
			pInst->mpComponent_Physics = (Component_Physics *)calloc(1, sizeof(Component_Physics));
		}

		Vector2D zeroVec2;
		Vector2DZero(&zeroVec2);

		pInst->mpComponent_Physics->mVelocity = pVelocity ? *pVelocity : zeroVec2;
		pInst->mpComponent_Physics->mpOwner = pInst;
	}
}

// ---------------------------------------------------------------------------

void RemoveComponent_Transform(GameObjectInstance *pInst)
{
	if (0 != pInst)
	{
		if (0 != pInst->mpComponent_Transform)
		{
			free(pInst->mpComponent_Transform);
			pInst->mpComponent_Transform = 0;
		}
	}
}

// ---------------------------------------------------------------------------

void RemoveComponent_Sprite(GameObjectInstance *pInst)
{
	if (0 != pInst)
	{
		if (0 != pInst->mpComponent_Sprite)
		{
			free(pInst->mpComponent_Sprite);
			pInst->mpComponent_Sprite = 0;
		}
	}
}

// ---------------------------------------------------------------------------

void RemoveComponent_Physics(GameObjectInstance *pInst)
{
	if (0 != pInst)
	{
		if (0 != pInst->mpComponent_Physics)
		{
			free(pInst->mpComponent_Physics);
			pInst->mpComponent_Physics = 0;
		}
	}
}

// ---------------------------------------------------------------------------