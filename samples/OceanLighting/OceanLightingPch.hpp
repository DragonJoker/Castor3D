/* See LICENSE file in root folder */
#ifndef ___C3D_OCEAN_LIGHTING_PCH_H___
#define ___C3D_OCEAN_LIGHTING_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include "OceanLightingPrerequisites.hpp"

#	include <algorithm>
#	include <limits>
#	include <fstream>
#	include <iostream>
#	include <cmath>
#	include <cstdint>
#	include <ctime>

#	include <Castor3DPrerequisites.hpp>
#	include <Engine.hpp>
#	include <Cache/ShaderCache.hpp>
#	include <Cache/SceneCache.hpp>
#	include <FrameBuffer/FrameBuffer.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Mesh/Buffer/Buffer.hpp>
#	include <Miscellaneous/Parameter.hpp>
#	include <Miscellaneous/WindowHandle.hpp>
#	include <State/BlendState.hpp>
#	include <State/DepthStencilState.hpp>
#	include <State/RasteriserState.hpp>
#	include <Render/Context.hpp>
#	include <Render/RenderPipeline.hpp>
#	include <Render/RenderTarget.hpp>
#	include <Render/RenderWindow.hpp>
#	include <Scene/Scene.hpp>
#	include <Shader/ShaderProgram.hpp>
#	include <Technique/RenderTechnique.hpp>
#	include <Texture/Sampler.hpp>
#	include <Texture/TextureLayout.hpp>

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
#endif

#endif
