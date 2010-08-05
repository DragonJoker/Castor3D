#include "PrecompiledHeader.h"

#include "render_system/Module_Render.h"

#include "render_system/RenderSystem.h"
#include "material/MaterialManager.h"

using namespace Castor3D;

RenderSystem * RenderSystem :: sm_singleton = NULL;
bool RenderSystem :: sm_useMultiTexturing = false;
bool RenderSystem :: sm_initialised = false;


RenderSystem :: RenderSystem()
{
}


