#ifndef ___C3D_OCEAN_LIGHTING_PCH_H___
#define ___C3D_OCEAN_LIGHTING_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include "OceanLightingPrerequisites.hpp"

#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif

#	include <algorithm>
#	include <limits>
#	include <fstream>
#	include <iostream>
#	include <cmath>
#	include <cstdint>
#	include <ctime>

#	include <Castor3DPrerequisites.hpp>
#	include <BlendState.hpp>
#	include <Buffer.hpp>
#	include <Context.hpp>
#	include <DepthStencilState.hpp>
#	include <Engine.hpp>
#	include <FrameBuffer.hpp>
#	include <Parameter.hpp>
#	include <Pipeline.hpp>
#	include <RasteriserState.hpp>
#	include <RenderTarget.hpp>
#	include <RenderTechnique.hpp>
#	include <RenderWindow.hpp>
#	include <Sampler.hpp>
#	include <Scene.hpp>
#	include <ShaderManager.hpp>
#	include <ShaderProgram.hpp>
#	include <Texture.hpp>
#	include <Vertex.hpp>
#	include <WindowHandle.hpp>

#	if defined( __linux )
#		include <GL/glx.h>
#		include <gtk/gtk.h>
#		include <gdk/gdkx.h>
#	elif defined( _WIN32 )
#		include <Windows.h>
#	endif

#	if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#		include <vld.h>
#	endif

#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#endif

#endif
