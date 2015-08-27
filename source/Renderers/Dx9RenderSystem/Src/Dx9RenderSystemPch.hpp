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
#ifndef ___DX9_RENDER_SYSTEM_PCH_H___
#define ___DX9_RENDER_SYSTEM_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#	pragma warning( disable:4996 )

#	include <string>

#	include <PreciseTimer.hpp>

#	include <Templates.hpp>
#	include <StringUtils.hpp>
#	include <Logger.hpp>
#	include <Point.hpp>
#	include <Quaternion.hpp>
#	include <Colour.hpp>
#	include <Resource.hpp>
#	include <Loader.hpp>
#	include <NonCopyable.hpp>

#	include <Castor3DPrerequisites.hpp>
#	include <BlendState.hpp>
#	include <Buffer.hpp>
#	include <CameraRenderer.hpp>
#	include <ColourRenderBuffer.hpp>
#	include <Context.hpp>
#	include <DepthStencilRenderBuffer.hpp>
#	include <DirectionalLight.hpp>
#	include <Engine.hpp>
#	include <Face.hpp>
#	include <Font.hpp>
#	include <FrameBuffer.hpp>
#	include <FrameVariable.hpp>
#	include <Geometry.hpp>
#	include <Light.hpp>
#	include <Material.hpp>
#	include <Mesh.hpp>
#	include <Overlay.hpp>
#	include <Pass.hpp>
#	include <Pipeline.hpp>
#	include <PlatformWindowHandle.hpp>
#	include <Plugin.hpp>
#	include <PointLight.hpp>
#	include <RenderBufferAttachment.hpp>
#	include <RenderSystem.hpp>
#	include <RenderTarget.hpp>
#	include <RenderTechnique.hpp>
#	include <RenderWindow.hpp>
#	include <Sampler.hpp>
#	include <SceneNode.hpp>
#	include <Scene.hpp>
#	include <ShaderManager.hpp>
#	include <ShaderObject.hpp>
#	include <ShaderProgram.hpp>
#	include <SpotLight.hpp>
#	include <Submesh.hpp>
#	include <TextureAttachment.hpp>
#	include <Texture.hpp>
#	include <TextureUnit.hpp>
#	include <Version.hpp>
#	include <Vertex.hpp>
#	include <Viewport.hpp>
#	include <WindowHandle.hpp>

#	include <fstream>
#	include <set>

using Castor3D::real;

#	include "Dx9RenderSystemPrerequisites.hpp"

#	pragma warning( pop )
#endif

#endif
