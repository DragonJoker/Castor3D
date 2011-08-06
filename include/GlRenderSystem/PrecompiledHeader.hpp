/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Gl_PrecompiledHeader___
#define ___Gl_PrecompiledHeader___

#ifndef CHECK_MEMORYLEAKS
#	ifndef NDEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#endif

//#pragma message( "********************************************************************")
//#pragma message( "	OpenGlCommon")

#include <string>
/*
#if CHECK_MEMORYLEAKS
#	pragma message( "		Checking Memory leaks")
#endif
*/
#include <CastorUtils/Memory.hpp>

#include "GlRenderSystem/glew.h"

#ifdef _WIN32
#	include "GlRenderSystem/wglew.h"
#else
#	ifdef __GNUG__
#		include "GlRenderSystem/glxew.h"
#	endif
#endif

#include <Cg/cgGL.h>

#include <CastorUtils/CastorUtils.hpp>
#include <CastorUtils/Pixel.hpp>
using namespace Castor;
using namespace Castor::Templates;
using namespace Castor::Math;
using namespace Castor::Resources;

#include <Castor3D/Prerequisites.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/Vertex.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Pipeline.hpp>
#include <Castor3D/RenderWindow.hpp>
#include <Castor3D/Root.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/Overlay.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/Context.hpp>
#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/BufferElement.hpp>
#include <Castor3D/Buffer.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/ShaderObject.hpp>
#include <Castor3D/ShaderProgram.hpp>
#include <Castor3D/FrameVariable.hpp>
#include <Castor3D/CgFrameVariable.hpp>
#include <Castor3D/Font.hpp>
#include <Castor3D/Logger.hpp>
#include <Castor3D/Plugin.hpp>

//#pragma message( "********************************************************************")

using namespace Castor::Utils;

#endif
