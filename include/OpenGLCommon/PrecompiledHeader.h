/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#	ifdef _DEBUG
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
#include <CastorUtils/Memory.h>

#include "OpenGlCommon/glew.h"

#ifdef _WIN32
#	include "OpenGlCommon/wglew.h"
#else
#	ifdef __GNUG__
#		include "OpenGlCommon/glxew.h"
#	endif
#endif

#include <Cg/cgGL.h>

#include <CastorUtils/Module_Utils.h>
using namespace Castor;
using namespace Castor::Templates;
using namespace Castor::Math;

#include <Castor3D/Prerequisites.h>
#include <Castor3D/camera/Camera.h>
#include <Castor3D/geometry/basic/Vertex.h>
#include <Castor3D/main/Pipeline.h>
#include <Castor3D/main/RenderWindow.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureEnvironment.h>
#include <Castor3D/overlay/Overlay.h>
#include <Castor3D/render_system/Buffer.h>
#include <Castor3D/render_system/Context.h>
#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/render_system/BufferElement.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/shader/ShaderObject.h>
#include <Castor3D/shader/ShaderProgram.h>
#include <Castor3D/shader/FrameVariable.h>
#include <Castor3D/shader/Cg/CgFrameVariable.h>
#include <Castor3D/text/Font.h>
#include <Castor3D/Log.h>

//#pragma message( "********************************************************************")

using namespace Castor::Utils;

#endif
