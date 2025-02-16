#ifndef ___CCV_NodeState___
#define ___CCV_NodeState___

#include <CCastor3D/Castor3D.h>

struct NodeState_
{
	C3DSceneNode * node;
	C3DVec3 originalPosition;
	C3DQuat originalOrientation;
	float originalXAngle;
	float originalYAngle;
	float originalZAngle;
	float xAngle;
	float yAngle;
	float zAngle;
	float minSpeed;
	float maxSpeed;
	float xAngularVelocity;
	float yAngularVelocity;
	float xScalarVelocity;
	float yScalarVelocity;
	float zScalarVelocity;
};
typedef struct NodeState_ NodeState;

NodeState stateCreate( C3DSceneNode * node );
void stateDestroy( NodeState * state );
void stateReset( NodeState * state, float speed );
int stateUpdate( NodeState * state );
void stateAddAngularVelocity( NodeState * state, float x, float y );
void stateAddScalarVelocity( NodeState * state, float x, float y, float z );

#endif
