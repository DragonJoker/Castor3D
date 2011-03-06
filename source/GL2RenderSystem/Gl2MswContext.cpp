#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2Context.h"

#ifdef _WIN32
using namespace Castor3D;

Gl2Context :: Gl2Context( RenderWindow * p_window)
    :	GlContext( p_window)
{
}

Gl2Context :: ~Gl2Context()
{
}
#endif
