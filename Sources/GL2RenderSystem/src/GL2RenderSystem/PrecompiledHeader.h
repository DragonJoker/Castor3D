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
#ifndef ___GL2_PrecompiledHeader___
#define ___GL2_PrecompiledHeader___

#	ifndef CHECK_MEMORYLEAKS
#		ifdef _DEBUG
#			define CHECK_MEMORYLEAKS 1
#		else
#			define CHECK_MEMORYLEAKS 0
#		endif
#	endif

#pragma message( "********************************************************************")
#pragma message( "	GL2RenderSystem")

#if CHECK_MEMORYLEAKS
#	pragma message( "		Checking Memory leaks")
#endif
#include <CastorUtils/Memory.h>

#	include <CastorUtils/PreciseTimer.h>

#	include <GL/glew.h>
#	ifdef WIN32
#		include <GL/wglew.h>
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

using namespace Castor::Math;
using namespace Castor::Utils;

#	include <Castor3D/Module_General.h>
#	include <Castor3D/camera/Module_Camera.h>
#	include <Castor3D/camera/Camera.h>
#	include <Castor3D/camera/Viewport.h>
#	include <Castor3D/light/Module_Light.h>
#	include <Castor3D/light/DirectionalLight.h>
#	include <Castor3D/light/Light.h>
#	include <Castor3D/light/PointLight.h>
#	include <Castor3D/light/SpotLight.h>
#	include <Castor3D/main/Module_Main.h>
#	include <Castor3D/main/RenderWindow.h>
#	include <Castor3D/main/Root.h>
#	include <Castor3D/main/RendererServer.h>
#	include <Castor3D/main/Pipeline.h>
#	include <Castor3D/main/Plugin.h>
#	include <Castor3D/material/Module_Material.h>
#	include <Castor3D/material/MaterialManager.h>
#	include <Castor3D/material/Material.h>
#	include <Castor3D/material/Pass.h>
#	include <Castor3D/material/TextureEnvironment.h>
#	include <Castor3D/material/TextureUnit.h>
#	include <Castor3D/geometry/Module_Geometry.h>
#	include <Castor3D/geometry/basic/Vertex.h>
#	include <Castor3D/geometry/basic/Face.h>
#	include <Castor3D/geometry/mesh/Submesh.h>
#	include <Castor3D/geometry/mesh/Mesh.h>
#	include <Castor3D/geometry/primitives/Geometry.h>
#	include <Castor3D/scene/Module_Scene.h>
#	include <Castor3D/scene/SceneManager.h>
#	include <Castor3D/scene/Node.h>
#	include <Castor3D/scene/Node.h>
#	include <Castor3D/scene/Scene.h>
#	include <Castor3D/shader/Module_Shader.h>
#	include <Castor3D/shader/ShaderProgram.h>
#	include <Castor3D/shader/ShaderManager.h>
#	include <Castor3D/shader/ShaderObject.h>
#	include <Castor3D/shader/FrameVariable.h>
#	include <Castor3D/render_system/RenderSystem.h>
#	include <Castor3D/render_system/Buffer.h>
#	include <Castor3D/overlay/OverlayManager.h>
#	include <Castor3D/overlay/Overlay.h>
#	include <Castor3D/Log.h>

#	include <OpenGLCommon/GLBuffer.h>
#	include <OpenGLCommon/GLFrameVariable.h>
#	include <OpenGLCommon/GLShaderObject.h>
#	include <OpenGLCommon/GLShaderProgram.h>
#	include <OpenGLCommon/GLContext.h>
#	include <OpenGLCommon/GLPipeline.h>
#	include <OpenGLCommon/GLRenderSystem.h>
#	include <OpenGLCommon/GLCameraRenderer.h>
#	include <OpenGLCommon/GLTexEnvironmentRenderer.h>
#	include <OpenGLCommon/GLOverlayRenderer.h>
#	include <OpenGLCommon/GLLightRenderer.h>
#	include <OpenGLCommon/GLMaterialRenderer.h>
#	include <OpenGLCommon/GLTextureRenderer.h>

#	include <fstream>
#	include <set>

#pragma message( "********************************************************************")

using namespace Castor::Utils;

#endif