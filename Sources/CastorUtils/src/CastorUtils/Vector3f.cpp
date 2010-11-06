#include "PrecompiledHeader.h"

#include "Vector3f.h"

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
	using namespace Castor::Utils;
#endif

using namespace Castor::Math;

unsigned long Vertex :: s_vertexNumber = 0;
Vertex Vertex :: ZeroVertex( 0, 0, 0);