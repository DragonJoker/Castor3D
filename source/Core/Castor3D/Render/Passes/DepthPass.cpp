#include "Castor3D/Render/Passes/DepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	DepthPass::DepthPass( String const & prefix
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, SsaoConfig const & ssaoConfig
		, TextureLayoutSPtr depthBuffer )
		: RenderTechniquePass{ prefix
			, "DepthPass"
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
				{},
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
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
		m_renderPass = device->createRenderPass( "DepthPass"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray fbattaches;
		fbattaches.emplace_back( depthBuffer->getDefaultView().getTargetView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( "DepthPass"
			, size
			, std::move( fbattaches ) );

		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer( "DepthPass" );
	}

	DepthPass::~DepthPass()
	{
	}

	ashes::Semaphore const & DepthPass::render( ashes::SemaphoreCRefArray const & semaphores )
	{
		static ashes::VkClearValueArray const clearValues
		{
			defaultClearDepthStencil,
		};

		RenderPassTimerBlock timerBlock{ getTimer().start() };
		auto & device = getCurrentRenderDevice( *this );

		m_nodesCommands->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_nodesCommands->beginDebugBlock(
			{
				"Depth Pass",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		timerBlock->beginPass( *m_nodesCommands );
		timerBlock->notifyPassRender();
		m_nodesCommands->beginRenderPass( getRenderPass()
			, *m_frameBuffer
			, clearValues
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		if ( hasNodes() )
		{
			m_nodesCommands->executeCommands( { getCommandBuffer() } );
		}
		m_nodesCommands->endRenderPass();
		timerBlock->endPass( *m_nodesCommands );
		m_nodesCommands->endDebugBlock();
		m_nodesCommands->end();

		ashes::VkPipelineStageFlagsArray const stages( semaphores.size(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
		device.graphicsQueue->submit( { *m_nodesCommands }
			, semaphores
			, stages
			, { getSemaphore() }
			, nullptr );

		return getSemaphore();
	}

	void DepthPass::doCleanup()
	{
		m_nodesCommands.reset();
		m_frameBuffer.reset();
		m_renderPass.reset();
		RenderTechniquePass::doCleanup();
	}

	void DepthPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	TextureFlags DepthPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eOpacity | TextureFlag::eHeight };
	}

	void DepthPass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		addFlag( flags.programFlags, ProgramFlag::eDepthPass );
	}

	void DepthPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, node.pipeline.getFlags().textures );
	}

	void DepthPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, node.pipeline.getFlags().textures );
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo DepthPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE };
	}

	ashes::PipelineColorBlendStateCreateInfo DepthPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, 1u );
	}

	ShaderPtr DepthPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition"
			, RenderPass::VertexInputs::PositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, RenderPass::VertexInputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexInputs::TextureLocation
			, hasTextures );
		auto inBoneIds0 = writer.declInput< IVec4 >( "inBoneIds0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inBoneIds1 = writer.declInput< IVec4 >( "inBoneIds1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights0 = writer.declInput< Vec4 >( "inWeights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights1 = writer.declInput< Vec4 >( "inWeights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inTransform = writer.declInput< Mat4 >( "inTransform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inMaterial = writer.declInput< Int >( "inMaterial"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inNormal2 = writer.declInput< Vec3 >( "inNormal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTangent2 = writer.declInput< Vec3 >( "inTangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTexture2 = writer.declInput< Vec3 >( "inTexture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) && hasTextures );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto outCurPosition = writer.declOutput< Vec3 >( "outCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto outPrvPosition = writer.declOutput< Vec3 >( "outPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture"
			, RenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto outTangentSpaceFragPosition = writer.declOutput< Vec3 >( "outTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto outTangentSpaceViewPosition = writer.declOutput< Vec3 >( "outTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
		{
				auto curPosition = writer.declLocale( "curPosition"
					, vec4( inPosition.xyz(), 1.0_f ) );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( inNormal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( inTangent, 0.0_f ) );
				outTexture = inTexture;

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					auto mtxModel = writer.declLocale( "mtxModel"
						, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					auto mtxModel = writer.declLocale( "mtxModel"
						, inTransform );
				}
				else
				{
					auto mtxModel = writer.declLocale( "mtxModel"
						, c3d_curMtxModel );
				}

				if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					outMaterial = writer.cast< UInt >( inMaterial );
				}
				else
				{
					outMaterial = writer.cast< UInt >( c3d_materialIndex );
				}

				auto mtxModel = writer.getVariable< Mat4 >( "mtxModel" );

				if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( sdw::mix( curPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v4Normal = vec4( sdw::mix( v4Normal.xyz(), inNormal2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), inTangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
					outTexture = sdw::mix( outTexture, inTexture2, vec3( c3d_time ) );
				}

				curPosition = mtxModel * curPosition;
				auto worldPosition = writer.declLocale( "worldPosition"
					, curPosition.xyz() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_prvViewProj * curPosition );
				curPosition = c3d_curViewProj * curPosition;
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, transpose( inverse( mat3( mtxModel ) ) ) );

				auto normal = writer.declLocale( "normal"
					, normalize( mtxNormal * v4Normal.xyz() ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( mtxNormal * v4Tangent.xyz() ) );
				tangent = normalize( sdw::fma( -normal, vec3( dot( tangent, normal ) ), tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, cross( normal, tangent ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
				{
					normal = -normal;
					tangent = -tangent;
					bitangent = -bitangent;
				}

				auto tbn = writer.declLocale( "tbn"
					, transpose( mat3( tangent, bitangent, normal ) ) );
				outTangentSpaceFragPosition = tbn * worldPosition;
				outTangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

				// Convert the jitter from non-homogeneous coordinates to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				curPosition.xy() -= c3d_jitter * curPosition.w();
				prvPosition.xy() -= c3d_jitter * prvPosition.w();
				out.vtx.position = curPosition;

				outCurPosition = curPosition.xyw();
				outPrvPosition = prvPosition.xyw();
				// Positions in projection space are in [-1, 1] range, while texture
				// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
				// the scale (and flip the y axis):
				outCurPosition.xy() *= vec2( 0.5_f, -0.5_f );
				outPrvPosition.xy() *= vec2( 0.5_f, -0.5_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
		bool hasTextures = !flags.textures.empty();

		// Intputs
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, getMinTextureIndex()
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		auto out = writer.getOut();

		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		shader::Utils utils{ writer };
		utils.declareParallaxMappingFunc( flags );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = materials->getBaseMaterial( inMaterial );
				auto opacity = writer.declLocale( "opacity"
					, material->m_opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material->m_alphaRef );

				if ( hasTextures )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, inTexture );
					utils.computeGeometryMapsContributions( flags.textures
						, flags.passFlags
						, textureConfigs
						, c3d_textureConfig
						, c3d_maps
						, texCoord
						, opacity
						, inTangentSpaceViewPosition
						, inTangentSpaceFragPosition );
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, alphaRef );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
