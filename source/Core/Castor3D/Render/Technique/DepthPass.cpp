#include "Castor3D/Technique/DepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureView.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	DepthPass::DepthPass( String const & name
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, SsaoConfig const & ssaoConfig
		, TextureLayoutSPtr depthBuffer )
		: RenderTechniquePass{ name
			, name
			, matrixUbo
			, culler
			, false
			, nullptr
			, ssaoConfig }
	{
		auto & device = getCurrentRenderDevice( *this );
		VkExtent2D size{ depthBuffer->getDimensions().width, depthBuffer->getDimensions().height };

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				depthBuffer->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( std::move( createInfo ) );

		ashes::ImageViewCRefArray fbattaches;
		fbattaches.emplace_back( depthBuffer->getDefaultView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( size
			, std::move( fbattaches ) );
	}

	DepthPass::~DepthPass()
	{
	}

	void DepthPass::update( RenderInfo & info
		, castor::Point2f const & jitter )
	{
		RenderTechniquePass::doUpdate( info
			, jitter );
	}

	void DepthPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	ashes::VkDescriptorSetLayoutBindingArray DepthPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		return RenderPass::doCreateUboBindings( flags );
	}

	void DepthPass::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eDepthPass );
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eEnvironmentMapping );
		remFlag( flags.textures, TextureFlag::eAllButOpacity );
		flags.texturesCount = checkFlag( flags.textures, TextureFlag::eOpacity )
			? 1u
			: 0u;
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo DepthPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo DepthPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
	}

	ShaderPtr DepthPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto uv = writer.declInput< Vec3 >( cuT( "uv" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declOutput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( cuT( "main" )
			, [&]()
		{
				auto curPosition = writer.declLocale( cuT( "curPosition" )
					, vec4( position.xyz(), 1.0_f ) );
				auto v3Texture = writer.declLocale( cuT( "v3Texture" )
					, uv );

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, transform );
				}
				else
				{
					auto mtxModel = writer.declLocale( cuT( "mtxModel" )
						, c3d_curMtxModel );
				}

				if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = writer.cast< UInt >( material );
				}
				else
				{
					vtx_material = writer.cast< UInt >( c3d_materialIndex );
				}

				auto mtxModel = writer.getVariable< Mat4 >( cuT( "mtxModel" ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( sdw::mix( curPosition.xyz(), position2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v3Texture = sdw::mix( v3Texture, texture2, vec3( c3d_time ) );
				}

				vtx_texture = v3Texture;
				curPosition = mtxModel * curPosition;
				auto prvPosition = writer.declLocale( cuT( "prvPosition" )
					, c3d_prvViewProj * curPosition );
				curPosition = c3d_curViewProj * curPosition;

				// Convert the jitter from non-homogeneous coordinates to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				curPosition.xy() -= c3d_jitter * curPosition.w();
				prvPosition.xy() -= c3d_jitter * prvPosition.w();
				out.gl_out.gl_Position = curPosition;

				vtx_curPosition = curPosition.xyw();
				vtx_prvPosition = prvPosition.xyw();
				// Positions in projection space are in [-1, 1] range, while texture
				// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
				// the scale (and flip the y axis):
				vtx_curPosition.xy() *= vec2( 0.5_f, -0.5_f );
				vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
			} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr DepthPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr DepthPass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr DepthPass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr DepthPass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr DepthPass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Intputs
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( cuT( "c3d_maps" )
			, getMinTextureIndex()
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		auto out = writer.getOut();

		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		shader::Utils utils{ writer };

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = materials->getBaseMaterial( vtx_material );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material->m_opacity );
			auto alphaRef = writer.declLocale( cuT( "alphaRef" )
				, material->m_alphaRef );
			utils.computeOpacityMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, c3d_maps
				, vtx_texture
				, alpha );
			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, alphaRef );
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}
}
