#include "Castor3DPch.hpp"

#include <Point.hpp>
#include <SquareMatrix.hpp>

#include <Engine.hpp>
#include <RenderSystem.hpp>

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#elif defined( _MSC_VER )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif

namespace Castor
{
	IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::Engine );
	IMPLEMENT_EXPORTED_OWNED_BY( Castor3D::RenderSystem );
}
