/* See LICENSE file in root folder */
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
#	include <Shader/Uniform/Uniform.hpp>
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
