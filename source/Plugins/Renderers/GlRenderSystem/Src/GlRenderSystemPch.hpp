/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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

#include <config.hpp>

#if CASTOR_USE_PCH
#	include <Logger.hpp>
#	include <Pixel.hpp>
#	include <Font.hpp>

#	include <Castor3DPrerequisites.hpp>
#	include <Engine.hpp>
#	include <ShaderProgramCache.hpp>
#	include <FrameBuffer/ColourRenderBuffer.hpp>
#	include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#	include <FrameBuffer/FrameBuffer.hpp>
#	include <FrameBuffer/RenderBufferAttachment.hpp>
#	include <FrameBuffer/TextureAttachment.hpp>
#	include <Material/Material.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/Face.hpp>
#	include <Mesh/Mesh.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Mesh/Buffer/Buffer.hpp>
#	include <Miscellaneous/Version.hpp>
#	include <Miscellaneous/WindowHandle.hpp>
#	include <Overlay/Overlay.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Render/Context.hpp>
#	include <Render/Pipeline.hpp>
#	include <Render/RenderSystem.hpp>
#	include <Render/RenderTarget.hpp>
#	include <Render/RenderWindow.hpp>
#	include <Render/Viewport.hpp>
#	include <Scene/BillboardList.hpp>
#	include <Scene/Geometry.hpp>
#	include <Scene/SceneNode.hpp>
#	include <Scene/Scene.hpp>
#	include <Scene/Light/DirectionalLight.hpp>
#	include <Scene/Light/Light.hpp>
#	include <Scene/Light/PointLight.hpp>
#	include <Scene/Light/SpotLight.hpp>
#	include <Shader/FrameVariable.hpp>
#	include <Shader/ShaderObject.hpp>
#	include <Shader/ShaderProgram.hpp>
#	include <State/BlendState.hpp>
#	include <Technique/RenderTechnique.hpp>
#	include <Texture/Sampler.hpp>
#	include <Texture/TextureLayout.hpp>
#	include <Texture/TextureUnit.hpp>

#	include "GlRenderSystemPrerequisites.hpp"

#endif

#include <StringUtils.hpp>

#ifdef None
#	undef None
#endif

#endif
