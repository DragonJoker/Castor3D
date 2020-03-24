#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	//************************************************************************************************

	String getTextureName( WbTexture texture )
	{
		static std::array< String, size_t( WbTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapAccumulation" ),
				cuT( "c3d_mapRevealage" ),
				cuT( "c3d_mapVelocity" ),
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getTextureFormat( WbTexture texture )
	{
		static std::array< VkFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};

		CU_Require( texture != WbTexture::eDepth
			&& "You can't use this function for depth texture format" );
		return Values[size_t( texture )];
	}

	//************************************************************************************************

	TransparentPass::TransparentPass( MatrixUbo & matrixUbo
		, SceneCuller & culler
		, SsaoConfig const & config )
		: castor3d::RenderTechniquePass{ "Transparent"
			, "Accumulation"
			, matrixUbo
			, culler
			, true
			, false
			, nullptr
			, config }
	{
	}

	TransparentPass::~TransparentPass()
	{
	}

	void TransparentPass::initialiseRenderPass( WeightedBlendTextures const & wbpResult )
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				wbpResult[0]->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				wbpResult[1]->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				wbpResult[2]->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				wbpResult[3]->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{
					{ 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					{ 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					{ 3u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
				},
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
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
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
		auto & device = getCurrentRenderDevice( *this );
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		ashes::ImageViewCRefArray fbAttaches;

		for ( size_t i = 0u; i < wbpResult.size(); ++i )
		{
			fbAttaches.emplace_back( wbpResult[i] );
		}

		m_frameBuffer = m_renderPass->createFrameBuffer( { wbpResult[0].image->getDimensions().width, wbpResult[0].image->getDimensions().height }
			, std::move( fbAttaches ) );

		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer();
	}

	void TransparentPass::update( RenderInfo & info
		, castor::Point2f const & jitter )
	{
		auto & nodes = m_renderQueue.getCulledRenderNodes();

		if ( nodes.hasNodes() )
		{
			RenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			RenderPass::doUpdate( nodes.staticNodes.frontCulled );
			RenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			RenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.staticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled, info );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled, info );
		}
	}

	ashes::Semaphore const & TransparentPass::render( ashes::Semaphore const & toWait )
	{
		static ashes::VkClearValueArray const clearValues
		{
			defaultClearDepthStencil,
			transparentBlackClearColor,
			opaqueWhiteClearColor,
			transparentBlackClearColor,
		};

		auto * result = &toWait;
		auto & timer = getTimer();
		auto timerBlock = timer.start();

		m_nodesCommands->begin();
		m_nodesCommands->beginDebugBlock(
			{
				"Weighted Blended - Transparent accumulation",
				{
					1.0f,
					1.0f,
					0.2f,
					1.0f,
				},
			} );
		timer.beginPass( *m_nodesCommands );
		timer.notifyPassRender();
		auto & view = m_frameBuffer->begin()->get();
		auto subresource = view->subresourceRange;
		subresource.aspectMask = ashes::getAspectMask( view->format );
		m_nodesCommands->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, makeVkType< VkImageMemoryBarrier >( VkAccessFlags( 0u )
				, VkAccessFlags( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT )
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, view->image
				, subresource ) );
		m_nodesCommands->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, clearValues
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		m_nodesCommands->executeCommands( { getCommandBuffer() } );
		m_nodesCommands->endRenderPass();
		timer.endPass( *m_nodesCommands );
		m_nodesCommands->endDebugBlock();
		m_nodesCommands->end();

		auto & device = getCurrentRenderDevice( *this );
		device.graphicsQueue->submit( *m_nodesCommands
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, getSemaphore()
			, nullptr );
		result = &getSemaphore();

		return *result;
	}

	void TransparentPass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getFlags() );
		visitor.visit( cuT( "Object" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ).shader );
		visitor.visit( cuT( "Object" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ).shader );
	}

	bool TransparentPass::doInitialise( Size const & size )
	{
		m_finished = getCurrentRenderDevice( *this )->createSemaphore();
		return true;
	}

	ashes::PipelineDepthStencilStateCreateInfo TransparentPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, false };
	}

	ashes::PipelineColorBlendStateCreateInfo TransparentPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments
		{
			VkPipelineColorBlendAttachmentState
			{
				true,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
			VkPipelineColorBlendAttachmentState
			{
				true,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
			VkPipelineColorBlendAttachmentState
			{
				false,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
		};
		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			std::move( attachments ),
		};
	}

	namespace
	{
		template< typename DataTypeT, typename InstanceTypeT >
		void fillTexDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ObjectRenderNode< DataTypeT, InstanceTypeT > & node
			, ShadowMapLightTypeArray const & shadowMaps
			, Scene const & scene )
		{
			ashes::WriteDescriptorSetArray writes;
			node.passNode.fillDescriptor( layout
				, index
				, writes );

			if ( node.passNode.pass.hasEnvironmentMapping() )
			{
				auto & envMap = scene.getEnvironmentMap( node.sceneNode );
				bindTexture( envMap.getTexture().getTexture()->getDefaultView()
					, envMap.getTexture().getSampler()->getSampler()
					, writes
					, index );
			}

			if ( node.passNode.pass.getType() != MaterialType::ePhong )
			{
				auto & background = *node.sceneNode.getScene()->getBackground();

				if ( background.hasIbl() )
				{
					auto & ibl = background.getIbl();
					bindTexture( ibl.getIrradianceTexture()
						, ibl.getIrradianceSampler()
						, writes
						, index );
					bindTexture( ibl.getPrefilteredEnvironmentTexture()
						, ibl.getPrefilteredEnvironmentSampler()
						, writes
						, index );
					bindTexture( ibl.getPrefilteredBrdfTexture()
						, ibl.getPrefilteredBrdfSampler()
						, writes
						, index );
				}
			}

			bindShadowMaps( shadowMaps[size_t( LightType::eDirectional )]
				, writes
				, index );
			bindShadowMaps( shadowMaps[size_t( LightType::eSpot )]
				, writes
				, index );
			bindShadowMaps( shadowMaps[size_t( LightType::ePoint )]
				, writes
				, index );
			node.texDescriptorSet->setBindings( writes );
		}
	}

	void TransparentPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( layout
			, index
			, node
			, shadowMaps
			, m_scene );
	}

	void TransparentPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( layout
			, index
			, node
			, shadowMaps
			, m_scene );
	}

	ashes::VkDescriptorSetLayoutBindingArray TransparentPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = getMinTextureIndex();
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( flags.texturesCount )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, flags.texturesCount ) );
			index += flags.texturesCount;
		}

		if ( checkFlag( flags.textures, TextureFlag::eReflection )
			|| checkFlag( flags.textures, TextureFlag::eRefraction ) )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness )
			|| checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapIrradiance
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefiltered
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
		}

		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// Directional linear shadow map.
		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// Directional VSM shadow map.
		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// Spot linear shadow map.
		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// Spot VSM shadow map.
		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// Point linear shadow map.
		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// Point VSM shadow map.

		return textureBindings;
	}

	ShaderPtr TransparentPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( "position"
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( "normal"
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( "tangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( "uv"
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( "bone_ids0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( "bone_ids1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( "weights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( "weights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( "transform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( "material"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( "position2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( "normal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( "tangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( "texture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declOutput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( "curPosition"
				, vec4( position.xyz(), 1.0_f ) );
			auto v4Normal = writer.declLocale( "v4Normal"
				, vec4( normal, 0.0_f ) );
			auto v4Tangent = writer.declLocale( "v4Tangent"
				, vec4( tangent, 0.0_f ) );
			auto v3Texture = writer.declLocale( "v3Texture"
				, uv );

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, curMtxModel );
			}
			else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, transform );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, curMtxModel );
			}
			else
			{
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, c3d_curMtxModel );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, c3d_prvMtxModel );
			}

			auto curMtxModel = writer.getVariable< Mat4 >( "curMtxModel" );
			auto prvMtxModel = writer.getVariable< Mat4 >( "prvMtxModel" );
			auto mtxNormal = writer.declLocale( "mtxNormal"
				, transpose( inverse( mat3( curMtxModel ) ) ) );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = writer.cast< UInt >( material );
			}
			else
			{
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				curPosition = vec4( sdw::mix( curPosition.xyz(), position2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), normal2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), tangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v3Texture = sdw::mix( v3Texture, texture2, vec3( c3d_time ) );
			}

			vtx_texture = v3Texture;
			auto prvPosition = writer.declLocale( "prvPosition"
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			vtx_worldPosition = curPosition.xyz();
			prvPosition = c3d_prvViewProj * prvPosition;
			curPosition = c3d_curViewProj * curPosition;
			vtx_viewPosition = curPosition.xyz();

			if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
			{
				vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = writer.cast< UInt >( in.gl_InstanceID );

			auto tbn = writer.declLocale( "tbn", transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

			// Convert the jitter from non-homogeneous coordiantes to homogeneous
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
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr TransparentPass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) ) ) );

		auto in = writer.getIn();

		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };
		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareComputeAccumulation();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, index
			, getCuller().getScene().getDirectionalShadowCascades()
			, m_opaque );
		shader::PhongReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_accumulation( writer.declOutput< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declOutput< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 2 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( vtx_material ) );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto shininess = writer.declLocale( "shininess"
					, material.m_shininess );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto diffuse = writer.declLocale( "diffuse"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto emissive = writer.declLocale( "emissive"
					, diffuse * material.m_emissive );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 1.0_f );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto environment = writer.declLocale( "environment"
					, 0_u );

				utils.computeHeightMapContribution( flags
					, textureConfigs
					, c3d_textureConfig
					, vtx_tangentSpaceViewPosition
					, vtx_tangentSpaceFragPosition
					, c3d_maps
					, texCoord );
				shader::PhongLightingModel::computeMapContributions( writer
					, utils
					, flags
					, gamma
					, textureConfigs
					, c3d_textureConfig
					, vtx_tangent
					, vtx_bitangent
					, c3d_maps
					, texCoord
					, normal
					, diffuse
					, specular
					, emissive
					, shininess
					, alpha
					, occlusion
					, transmittance );
				emissive *= diffuse;
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->computeCombined( worldEye
					, shininess
					, c3d_shadowReceiver
					, shader::FragmentInput( in.gl_FragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );

				auto ambient = writer.declLocale( cuT( "ambient" )
					, clamp( c3d_ambientLight.xyz() + material.m_ambient * material.m_diffuse()
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );

				if ( checkFlag( flags.textures, TextureFlag::eReflection )
					|| checkFlag( flags.textures, TextureFlag::eRefraction ) )
				{
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( vtx_worldPosition, c3d_cameraPosition.xyz() ) );

					if ( checkFlag( flags.textures, TextureFlag::eReflection )
						&& checkFlag( flags.textures, TextureFlag::eRefraction ) )
					{
						ambient = reflections.computeReflRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, material.m_refractionRatio
							, diffuse
							, shininess );
						diffuse = vec3( 0.0_f );
					}
					else if ( checkFlag( flags.textures, TextureFlag::eReflection ) )
					{
						diffuse *= reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment );
						ambient = vec3( 0.0_f );
					}
					else
					{
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, material.m_refractionRatio
							, diffuse
							, shininess );
						diffuse = vec3( 0.0_f );
					}
				}
				else
				{
					ambient *= occlusion * diffuse;
					diffuse *= lightDiffuse;
				}

				auto colour = writer.declLocale< Vec3 >( "colour"
					, sdw::fma( ambient + lightDiffuse
						, diffuse
						, sdw::fma( lightSpecular
							, specular
							, emissive ) ) );

				pxl_accumulation = utils.computeAccumulation( in.gl_FragCoord.z()
					, colour
					, alpha
					, c3d_clipInfo.z()
					, c3d_clipInfo.w()
					, material.m_bwAccumulationOperator );
				pxl_revealage = alpha;
				auto curPosition = writer.declLocale( "curPosition"
					, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
				auto prvPosition = writer.declLocale( "prvPosition"
					, vtx_prvPosition.xy() / vtx_prvPosition.z() );
				pxl_velocity.xy() = curPosition - prvPosition;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr TransparentPass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, 1u );

		auto in = writer.getIn();

		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareComputeAccumulation();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, index
			, getCuller().getScene().getDirectionalShadowCascades()
			, m_opaque );
		shader::MetallicPbrReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_accumulation( writer.declOutput< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declOutput< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 2 ) );

		writer.implementFunction< sdw::Void >( "main", [&]()
		{
			auto normal = writer.declLocale( "normal"
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( "ambient"
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( "material"
				, materials.getMaterial( vtx_material ) );
			auto metallic = writer.declLocale( "metallic"
				, material.m_metallic );
			auto roughness = writer.declLocale( "roughness"
				, material.m_roughness );
			auto gamma = writer.declLocale( "gamma"
				, material.m_gamma );
			auto albedo = writer.declLocale( "albedo"
				, utils.removeGamma( gamma, material.m_albedo ) );
			auto emissive = writer.declLocale( "emissive"
				, albedo * material.m_emissive );
			auto worldEye = writer.declLocale( "worldEye"
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( "envAmbient"
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( "envDiffuse"
				, vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( "texCoord"
				, vtx_texture );
			auto occlusion = writer.declLocale( "occlusion"
				, 1.0_f );
			auto transmittance = writer.declLocale( "transmittance"
				, 1.0_f );
			auto alpha = writer.declLocale( "alpha"
				, material.m_opacity );

			utils.computeHeightMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangentSpaceViewPosition
				, vtx_tangentSpaceFragPosition
				, c3d_maps
				, texCoord );
			shader::MetallicBrdfLightingModel::computeMapContributions( writer
				, utils
				, flags
				, gamma
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangent
				, vtx_bitangent
				, c3d_maps
				, texCoord
				, normal
				, albedo
				, metallic
				, emissive
				, roughness
				, alpha
				, occlusion
				, transmittance );
			emissive *= albedo;
			auto lightDiffuse = writer.declLocale( "lightDiffuse"
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( "lightSpecular"
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, albedo
				, metallic
				, roughness
				, c3d_shadowReceiver
				, shader::FragmentInput( in.gl_FragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );

			if ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "incident" )
					, reflections.computeIncident( vtx_worldPosition, worldEye ) );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio );

				if ( checkFlag( flags.textures, TextureFlag::eReflection ) )
				{
					// Reflection from environment map.
					ambient = reflections.computeRefl( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, c3d_ambientLight.xyz()
						, albedo );
				}
				else
				{
					// Reflection from background skybox.
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, vtx_worldPosition
							, albedo
							, metallic
							, roughness
							, c3d_cameraPosition.xyz()
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
				}

				if ( checkFlag( flags.textures, TextureFlag::eRefraction ) )
				{
					// Refraction from environment map.
					ambient = reflections.computeRefrEnvMap( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, ratio
						, ambient
						, albedo
						, roughness );
				}
				else
				{
					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefrSkybox( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
							, ambient
							, albedo
							, roughness );
					}
					FI;
				}
			}
			else
			{
				// Reflection from background skybox.
				ambient *= occlusion * utils.computeMetallicIBL( normal
					, vtx_worldPosition
					, albedo
					, metallic
					, roughness
					, c3d_cameraPosition.xyz()
					, c3d_mapIrradiance
					, c3d_mapPrefiltered
					, c3d_mapBrdf );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio );

				IF( writer, ratio != 0.0_f )
				{
					// Refraction from background skybox.
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( vtx_worldPosition, worldEye ) );
					ambient = reflections.computeRefrSkybox( incident
						, normal
						, occlusion
						, c3d_mapPrefiltered
						, material.m_refractionRatio
						, ambient
						, albedo
						, roughness );
				}
				FI;
			}

			auto colour = writer.declLocale( "colour"
				, sdw::fma( lightDiffuse
					, albedo
					, lightSpecular + emissive + ambient ) );

			pxl_accumulation = utils.computeAccumulation( in.gl_FragCoord.z()
				, colour
				, alpha
				, c3d_clipInfo.z()
				, c3d_clipInfo.w()
				, material.m_bwAccumulationOperator );
			pxl_revealage = alpha;
			auto curPosition = writer.declLocale( "curPosition"
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( "prvPosition"
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr TransparentPass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );
		index += flags.texturesCount;
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, 1u );

		auto in = writer.getIn();

		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareComputeAccumulation();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, index
			, getCuller().getScene().getDirectionalShadowCascades()
			, m_opaque );
		shader::SpecularPbrReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_accumulation( writer.declOutput< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declOutput< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declOutput< Vec4 >( "pxl_velocity", 2 ) );

		writer.implementFunction< sdw::Void >( "main", [&]()
		{
			auto normal = writer.declLocale( "normal"
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( "ambient"
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( "material"
				, materials.getMaterial( vtx_material ) );
			auto specular = writer.declLocale( "specular"
				, material.m_specular );
			auto glossiness = writer.declLocale( "glossiness"
				, material.m_glossiness );
			auto gamma = writer.declLocale( "gamma"
				, material.m_gamma );
			auto albedo = writer.declLocale( "albedo"
				, utils.removeGamma( gamma, material.m_diffuse() ) );
			auto emissive = writer.declLocale( "emissive"
				, albedo * material.m_emissive );
			auto worldEye = writer.declLocale( "worldEye"
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( "envAmbient"
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( "envDiffuse"
				, vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( "texCoord"
				, vtx_texture );
			auto occlusion = writer.declLocale( "occlusion"
				, 1.0_f );
			auto alpha = writer.declLocale( "alpha"
				, material.m_opacity );
			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			utils.computeHeightMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangentSpaceViewPosition
				, vtx_tangentSpaceFragPosition
				, c3d_maps
				, texCoord );
			shader::SpecularBrdfLightingModel::computeMapContributions( writer
				, utils
				, flags
				, gamma
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangent
				, vtx_bitangent
				, c3d_maps
				, texCoord
				, normal
				, albedo
				, specular
				, emissive
				, glossiness
				, alpha
				, occlusion
				, transmittance );
			emissive *= albedo;
			auto lightDiffuse = writer.declLocale( "lightDiffuse"
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( "lightSpecular"
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, albedo
				, specular
				, glossiness
				, c3d_shadowReceiver
				, shader::FragmentInput( in.gl_FragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );

			if ( checkFlag( flags.textures, TextureFlag::eReflection )
				|| checkFlag( flags.textures, TextureFlag::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "incident" )
					, reflections.computeIncident( vtx_worldPosition, worldEye ) );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio );

				if ( checkFlag( flags.textures, TextureFlag::eReflection ) )
				{
					// Reflection from environment map.
					ambient = reflections.computeRefl( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, c3d_ambientLight.xyz()
						, albedo );
				}
				else
				{
					// Reflection from background skybox.
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, vtx_worldPosition
							, albedo
							, specular
							, glossiness
							, c3d_cameraPosition.xyz()
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
				}

				if ( checkFlag( flags.textures, TextureFlag::eRefraction ) )
				{
					// Refraction from environment map.
					ambient = reflections.computeRefrEnvMap( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, ratio
						, ambient
						, albedo
						, glossiness );
				}
				else
				{
					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefrSkybox( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
							, ambient
							, albedo
							, glossiness );
					}
					FI;
				}
			}
			else
			{
				// Reflection from background skybox.
				ambient *= occlusion * utils.computeSpecularIBL( normal
					, vtx_worldPosition
					, albedo
					, specular
					, glossiness
					, c3d_cameraPosition.xyz()
					, c3d_mapIrradiance
					, c3d_mapPrefiltered
					, c3d_mapBrdf );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio );

				IF( writer, ratio != 0.0_f )
				{
					// Refraction from background skybox.
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( vtx_worldPosition, worldEye ) );
					ambient = reflections.computeRefrSkybox( incident
						, normal
						, occlusion
						, c3d_mapPrefiltered
						, material.m_refractionRatio
						, ambient
						, albedo
						, glossiness );
				}
				FI;
			}
			auto colour = writer.declLocale( "colour"
				, sdw::fma( lightDiffuse
					, albedo
					, lightSpecular + emissive + ambient ) );

			pxl_accumulation = utils.computeAccumulation( in.gl_FragCoord.z()
				, colour
				, alpha
				, c3d_clipInfo.z()
				, c3d_clipInfo.w()
				, material.m_bwAccumulationOperator );
			pxl_revealage = alpha;
			auto curPosition = writer.declLocale( "curPosition"
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( "prvPosition"
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	void TransparentPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
		m_sceneUbo.update( *m_camera->getScene(), m_camera, true );
	}
}
