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
#ifndef ___Gl3_PrecompiledHeader___
#define ___Gl3_PrecompiledHeader___

#	ifndef CHECK_MEMORYLEAKS
#		ifdef _DEBUG
#			define CHECK_MEMORYLEAKS 1
#		else
#			define CHECK_MEMORYLEAKS 0
#		endif
#	endif

#include <string>
/*
#pragma message( "********************************************************************")
#pragma message( "	Gl3RenderSystem")

#if CHECK_MEMORYLEAKS
#	pragma message( "		Checking Memory leaks")
#endif
*/
#include <CastorUtils/Memory.h>

//#pragma message( "********************************************************************")

#	include <CastorUtils/PreciseTimer.h>

#	include <OpenGlCommon/glew.h>
#	ifdef _WIN32
#		include <OpenGlCommon/wglew.h>
#	else
#		ifdef __GNUG__
#			include <OpenGlCommon/glxew.h>
#		endif
#	endif

#	include <CastorUtils/Value.h>
using namespace Castor::Templates;

#	include <CastorUtils/CastorString.h>
#	include <CastorUtils/Vector.h>
#	include <CastorUtils/List.h>
#	include <CastorUtils/Map.h>
#	include <CastorUtils/Multimap.h>
#	include <CastorUtils/Set.h>
#	include <CastorUtils/Point.h>
#	include <CastorUtils/Quaternion.h>
#	include <CastorUtils/Colour.h>
#	include <CastorUtils/Module_Resource.h>
#	include <CastorUtils/Resource.h>
#	include <CastorUtils/ResourceLoader.h>
#	include <CastorUtils/PixelFormat.h>

using namespace Castor::Math;
using namespace Castor::Utils;

#	include <Castor3D/Prerequisites.h>
#	include <Castor3D/camera/Camera.h>
#	include <Castor3D/camera/Viewport.h>
#	include <Castor3D/light/DirectionalLight.h>
#	include <Castor3D/light/Light.h>
#	include <Castor3D/light/PointLight.h>
#	include <Castor3D/light/SpotLight.h>
#	include <Castor3D/main/RenderWindow.h>
#	include <Castor3D/main/Root.h>
#	include <Castor3D/main/RendererServer.h>
#	include <Castor3D/main/Pipeline.h>
#	include <Castor3D/main/Plugin.h>
#	include <Castor3D/main/Version.h>
#	include <Castor3D/material/MaterialManager.h>
#	include <Castor3D/material/Material.h>
#	include <Castor3D/material/Pass.h>
#	include <Castor3D/material/TextureEnvironment.h>
#	include <Castor3D/material/TextureUnit.h>
#	include <Castor3D/geometry/basic/Vertex.h>
#	include <Castor3D/geometry/basic/Face.h>
#	include <Castor3D/geometry/mesh/Submesh.h>
#	include <Castor3D/geometry/mesh/Mesh.h>
#	include <Castor3D/geometry/primitives/Geometry.h>
#	include <Castor3D/scene/SceneManager.h>
#	include <Castor3D/scene/SceneNode.h>
#	include <Castor3D/scene/Scene.h>
#	include <Castor3D/shader/ShaderProgram.h>
#	include <Castor3D/shader/ShaderManager.h>
#	include <Castor3D/shader/ShaderObject.h>
#	include <Castor3D/shader/FrameVariable.h>
#	include <Castor3D/shader/Cg/CgFrameVariable.h>
#	include <Castor3D/render_system/RenderSystem.h>
#	include <Castor3D/render_system/Buffer.h>
#	include <Castor3D/overlay/OverlayManager.h>
#	include <Castor3D/overlay/Overlay.h>
#	include <Castor3D/Log.h>

#	include <OpenGlCommon/GlBuffer.h>
#	include <OpenGlCommon/GlFrameVariable.h>
#	include <OpenGlCommon/GlShaderObject.h>
#	include <OpenGlCommon/GlShaderProgram.h>
#	include <OpenGlCommon/GlContext.h>
#	include <OpenGlCommon/GlPipeline.h>
#	include <OpenGlCommon/GlRenderSystem.h>
#	include <OpenGlCommon/GlCameraRenderer.h>
#	include <OpenGlCommon/GlTexEnvironmentRenderer.h>
#	include <OpenGlCommon/GlOverlayRenderer.h>
#	include <OpenGlCommon/GlLightRenderer.h>
#	include <OpenGlCommon/GlTextureRenderer.h>
#	include <OpenGlCommon/GlMaterialRenderer.h>

#	include <fstream>
#	include <set>

using namespace Castor::Utils;

#endif
