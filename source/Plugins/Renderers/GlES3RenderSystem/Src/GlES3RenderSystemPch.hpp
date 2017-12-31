/* See LICENSE file in root folder */
#ifndef ___GlES3_PrecompiledHeader___
#define ___GlES3_PrecompiledHeader___

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

#include "GlES3RenderSystemPrerequisites.hpp"

#endif

#include <Miscellaneous/StringUtils.hpp>

#ifdef None
#	undef None
#endif

#endif
