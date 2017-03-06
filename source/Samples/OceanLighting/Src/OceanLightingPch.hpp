/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
