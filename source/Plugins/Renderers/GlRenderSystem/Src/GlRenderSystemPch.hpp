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
#ifndef ___Gl_PrecompiledHeader___
#define ___Gl_PrecompiledHeader___

#include <config.hpp>

#if CASTOR_USE_PCH
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include <Log/Logger.hpp>
#include <Graphics/PixelBufferBase.hpp>
#include <Graphics/PixelFormat.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Rectangle.hpp>
#include <Graphics/Size.hpp>
#include <Miscellaneous/Utils.hpp>

#include <GlslSource.hpp>

#include <Engine.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/ShaderCache.hpp>
#include <FrameBuffer/BackBuffers.hpp>
#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/ComputePipeline.hpp>
#include <Miscellaneous/GpuInformations.hpp>
#include <Miscellaneous/GpuQuery.hpp>
#include <Miscellaneous/TransformFeedback.hpp>
#include <Render/Context.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderSystemFactory.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ProgramInputLayout.hpp>
#include <Shader/ShaderObject.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/ShaderStorageBuffer.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureStorage.hpp>
#include <Texture/TextureUnit.hpp>

#include "GlRenderSystemPrerequisites.hpp"

#endif

#include <Miscellaneous/StringUtils.hpp>

#ifdef None
#	undef None
#endif

#endif
