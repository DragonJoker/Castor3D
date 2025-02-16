#include "NodeState.h"

#include <math.h>
#include <string.h>

#define Pi 3.1415926535897932384626433832795
#define RadToDeg( x ) ( float )( 180.0f * ( x ) / Pi )
#define DegToRad( x ) ( float )( Pi * ( x ) / 180.0f )

static const float MaxAngularSpeed = 5.0f;
static const float MaxScalarSpeed = 20.0f;

static float stateUpdateVelocity( float velocity
	, float rangeMin
	, float rangeMax )
{
	float result = velocity / 1.2f;

	if ( fabs( result ) < 0.2f )
	{
		result = 0.0f;
	}

	if ( result > rangeMax )
	{
		result = rangeMax;
	}

	if ( result < rangeMin )
	{
		result = rangeMin;
	}

	return result;
}

NodeState stateCreate( C3DSceneNode * node )
{
	NodeState result;
	result.node = node;
	c3dSceneNode_getPosition( node, &result.originalPosition );
	c3dSceneNode_getOrientation( node, &result.originalOrientation );
	c3dQuat_getPitch( &result.originalOrientation, &result.originalXAngle );
	c3dQuat_getYaw( &result.originalOrientation, &result.originalYAngle );
	c3dQuat_getRoll( &result.originalOrientation, &result.originalZAngle );
	result.originalZAngle = 0.0f;
	result.xAngle = result.originalXAngle;
	result.yAngle = result.originalYAngle;
	result.zAngle = result.originalZAngle;
	return result;
}

void stateDestroy( NodeState * state )
{
	memset( state, 0, sizeof( NodeState ) );
}

void stateReset( NodeState * state, float speed )
{
	state->minSpeed = -speed;
	state->maxSpeed = speed;
	c3dSceneNode_setOrientation( state->node, &state->originalOrientation );
	c3dSceneNode_setPosition( state->node, &state->originalPosition );
}

int stateUpdate( NodeState * state )
{
	bool result = false;
	C3DVec3 translate;
	C3DQuat orientation;
	C3DVec3 right = { 1.0f, 0.0f, 0.0f };
	C3DVec3 up = { 0.0f, 1.0f, 0.0f };
	C3DVec3 front = { 0.0f, 0.0f, 1.0f };
	C3DVec3 resultX;
	C3DVec3 resultY;
	C3DVec3 resultZ;
	C3DVec3 xAxis = { 1.0f, 0.0f, 0.0f };
	C3DVec3 yAxis = { 0.0f, 1.0f, 0.0f };
	C3DVec3 zAxis = { 0.0f, 0.0f, 1.0f };
	C3DQuat pitch;
	C3DQuat yaw;
	C3DQuat roll;

	float xAngle = RadToDeg( state->xAngle );
	float yAngle = RadToDeg( state->yAngle );
	state->xAngle += DegToRad( state->xAngularVelocity );
	state->yAngle += DegToRad( state->yAngularVelocity );
	state->xAngularVelocity = stateUpdateVelocity( state->xAngularVelocity, -MaxAngularSpeed, MaxAngularSpeed );
	state->yAngularVelocity = stateUpdateVelocity( state->yAngularVelocity, -MaxAngularSpeed, MaxAngularSpeed );

	translate.x = state->xScalarVelocity;
	translate.y = state->yScalarVelocity;
	translate.z = state->zScalarVelocity;
	state->xScalarVelocity = stateUpdateVelocity( state->xScalarVelocity, -MaxScalarSpeed, MaxScalarSpeed );
	state->yScalarVelocity = stateUpdateVelocity( state->yScalarVelocity, -MaxScalarSpeed, MaxScalarSpeed );
	state->zScalarVelocity = stateUpdateVelocity( state->zScalarVelocity, -MaxScalarSpeed, MaxScalarSpeed );

	result = xAngle != RadToDeg( state->xAngle )
		|| yAngle != RadToDeg( state->yAngle );

	if ( translate.x != 0.0f
		|| translate.y != 0.0f
		|| translate.z != 0.0f )
	{
		c3dSceneNode_getOrientation( state->node, &orientation );
		c3dQuat_transform( &orientation, &right, &right );
		c3dQuat_transform( &orientation, &up, &up );
		c3dQuat_transform( &orientation, &front, &front );
		c3dVec3_mul( &right, translate.x, &resultX );
		c3dVec3_mul( &up, translate.y, &resultY );
		c3dVec3_mul( &front, translate.z, &resultZ );
		c3dVec3_compAdd( &resultX, &resultY, &resultX );
		c3dVec3_compAdd( &resultX, &resultZ, &translate );
		result = true;
	}

	if ( result )
	{
		c3dSceneNode_translate( state->node, &translate );
		c3dQuat_fromAxisAngle( &xAxis, state->xAngle, &pitch );
		c3dQuat_fromAxisAngle( &yAxis, state->yAngle, &yaw );
		c3dQuat_fromAxisAngle( &zAxis, state->zAngle, &roll );
		c3dQuat_mul( &yaw, &pitch, &pitch );
		c3dQuat_mul( &roll, &pitch, &pitch );
		c3dSceneNode_setOrientation( state->node, &pitch );
	}

	return result;
}

void stateAddAngularVelocity( NodeState * state, float x, float y )
{
	state->xAngularVelocity += x;
	state->yAngularVelocity += y;
}

void stateAddScalarVelocity( NodeState * state, float x, float y, float z )
{
	state->xScalarVelocity += x;
	state->yScalarVelocity += y;
	state->zScalarVelocity += z;
}
