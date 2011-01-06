#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Angle.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::Math;
using namespace Castor::Utils;

real Angle :: Pi = real( 3.1415926535897932384626433832795028841968);
real Angle :: PiDiv2 = Angle::Pi / real( 2.0);
real Angle :: PiMult2 = Angle::Pi * real( 2.0);

real Angle :: RadiansToDegrees = real( 180.0) / real( Angle::Pi);
real Angle :: DegreesToRadians = real( Angle::Pi) / real( 180.0);