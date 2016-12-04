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
#ifndef ___TRS_PrecompiledHeader___
#define ___TRS_PrecompiledHeader___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include <Log/Logger.hpp>
#	include <Graphics/Pixel.hpp>
#	include <Graphics/Font.hpp>

#	include <Castor3DPrerequisites.hpp>
#	include <Engine.hpp>
#	include <Cache/ShaderCache.hpp>
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
#	include <Render/RenderPipeline.hpp>
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

#	include "TestRenderSystemPrerequisites.hpp"

#endif

#include <Miscellaneous/StringUtils.hpp>

#endif
