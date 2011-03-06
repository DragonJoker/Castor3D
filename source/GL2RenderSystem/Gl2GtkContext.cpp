#include "Gl2RenderSystem/PrecompiledHeader.h"

#include "Gl2RenderSystem/Gl2Context.h"

#ifndef _WIN32
#	ifdef CASTOR_GTK
using namespace Castor3D;

Gl2Context :: Gl2Context( RenderWindow * p_window)
    :	GlContext( p_window)
{
}

Gl2Context :: ~Gl2Context()
{
}
void Gl2Context :: _createGlContext()
{
    GlContext::_createGlContext();
}

void Gl2Context :: _setMainContext()
{
    GlContext::_setMainContext();
}
#	endif
#endif
