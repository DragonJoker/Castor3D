#include "DepthPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	DepthPass::DepthPass( String const & name
		, Scene const & scene
		, Camera & camera
		, SsaoConfig const & ssaoConfig
		, TextureLayoutSPtr depthBuffer )
		: RenderTechniquePass{ name
			, name
			, scene
			, &camera
			, false
			, nullptr
			, ssaoConfig }
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ depthBuffer->getDimensions().width, depthBuffer->getDimensions().height };

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].format = depthBuffer->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() + 0u ), depthBuffer->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );
	}

	DepthPass::~DepthPass()
	{
	}

	void DepthPass::update( RenderInfo & info
		, Point2r const & jitter )
	{
		RenderTechniquePass::doUpdate( info
			, jitter );
	}

	void DepthPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void DepthPass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eDepthPass );
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eEnvironmentMapping );
	}

	void DepthPass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void DepthPass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	renderer::DepthStencilState DepthPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return renderer::DepthStencilState{ 0u, true, true };
	}

	renderer::ColourBlendState DepthPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	glsl::Shader DepthPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto texture = writer.declAttribute< Vec3 >( cuT( "texcoord" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declAttribute< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( writer.getInstanceID() ) );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" ), RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" ), RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ), RenderPass::VertexOutputs::MaterialLocation );
		auto out = gl_PerVertex{ writer };

		writer.implementFunction< void >( cuT( "main" )
			, [&]()
		{
				auto curPosition = writer.declLocale( cuT( "curPosition" )
					, vec4( position.xyz(), 1.0 ) );
				auto v3Texture = writer.declLocale( cuT( "v3Texture" )
					, texture );

				if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, SkinningUbo::computeTransform( writer, programFlags ) );
				}
				else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, transform );
				}
				else
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, c3d_mtxModel );
				}

				if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = material;
				}
				else
				{
					vtx_material = c3d_materialIndex;
				}

				auto mtxModel = writer.declBuiltin< Mat4 >( cuT( "mtxModel" ) );

				if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( glsl::mix( curPosition.xyz(), position2.xyz(), c3d_time ), 1.0 );
					v3Texture = glsl::mix( v3Texture, texture2, c3d_time );
				}

				vtx_texture = v3Texture;
				curPosition = mtxModel * curPosition;
				auto prvPosition = writer.declLocale( cuT( "prvPosition" )
					, c3d_prvViewProj * curPosition );
				out.gl_Position() = c3d_curViewProj * curPosition;
				// Convert the jitter from non-homogeneous coordiantes to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				out.gl_Position().xy() -= c3d_curJitter * out.gl_Position().w();
				prvPosition.xy() -= c3d_prvJitter * out.gl_Position().w();
				vtx_curPosition = out.gl_Position().xyw();
				vtx_prvPosition = prvPosition.xyw();
				// Positions in projection space are in [-1, 1] range, while texture
				// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
				// the scale (and flip the y axis):
				vtx_curPosition.xy() *= vec2( 0.5_f, -0.5 );
				vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5 );
			} );

		return writer.finalise();
	}

	glsl::Shader DepthPass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return glsl::Shader{};
	}

	glsl::Shader DepthPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader DepthPass::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Intputs
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" ), RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" ), RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ), RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = materials->getBaseMaterial( vtx_material );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity() );
			auto alphaRef = writer.declLocale( cuT( "alphaRef" )
				, material->m_alphaRef() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto c3d_mapOpacity = writer.getBuiltin< glsl::Sampler2D >( cuT( "c3d_mapOpacity" ) );
				auto vtx_texture = writer.getBuiltin< glsl::Vec3 >( cuT( "vtx_texture" ) );
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, alphaRef );
			}
			else if ( alphaFunc != renderer::CompareOp::eAlways )
			{
				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, alphaRef );
			}
		} );

		return writer.finalise();
	}
}
