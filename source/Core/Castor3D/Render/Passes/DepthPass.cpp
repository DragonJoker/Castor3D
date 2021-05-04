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
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, DepthPass )

using namespace castor;

namespace castor3d
{
	DepthPass::DepthPass( String const & prefix
		, RenderDevice const & device
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, SsaoConfig const & ssaoConfig
		, TextureLayoutSPtr depthBuffer )
		: RenderTechniquePass{ device
			, prefix
			, "DepthPass"
			, matrixUbo
			, culler
			, false
			, nullptr
			, ssaoConfig }
	{
		VkExtent2D size{ depthBuffer->getDimensions().width, depthBuffer->getDimensions().height };

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches{ { 0u
			, depthBuffer->getPixelFormat()
			, VK_SAMPLE_COUNT_1_BIT
			, VK_ATTACHMENT_LOAD_OP_CLEAR
			, VK_ATTACHMENT_STORE_OP_STORE
			, VK_ATTACHMENT_LOAD_OP_CLEAR
			, VK_ATTACHMENT_STORE_OP_STORE
			, VK_IMAGE_LAYOUT_UNDEFINED
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL } };
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription{ 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, {}
			, {}
			, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, {} } );
		ashes::VkSubpassDependencyArray dependencies
		{
			{ VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo createInfo{ 0u
			, std::move( attaches )
			, std::move( subpasses )
			, std::move( dependencies ) };
		m_renderPass = device->createRenderPass( "DepthPass"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray fbattaches;
		fbattaches.emplace_back( depthBuffer->getDefaultView().getTargetView() );
		m_frameBuffer = m_renderPass->createFrameBuffer( "DepthPass"
			, size
			, std::move( fbattaches ) );

		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer( "DepthPass" );
		initialise( { size.width, size.height }, nullptr );
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

		m_nodesCommands->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_nodesCommands->beginDebugBlock( { "Depth Pass"
			, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
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
		m_device.graphicsQueue->submit( { *m_nodesCommands }
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

	TextureFlags DepthPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eOpacity | TextureFlag::eHeight };
	}

	void DepthPass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		remFlag( flags.sceneFlags, SceneFlag::eLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing );
		addFlag( flags.programFlags, ProgramFlag::eDepthPass );
	}

	void DepthPass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, pipeline.getFlags().textures );
	}

	void DepthPass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		node.passNode.fillDescriptor( layout
			, index
			, writes
			, pipeline.getFlags().textures );
		node.texDescriptorSet->setBindings( writes );
	}

	ashes::VkDescriptorSetLayoutBindingArray DepthPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( !flags.textures.empty() )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
		}

		return textureBindings;
	}

	void DepthPass::doUpdatePipeline( RenderPipeline & pipeline )
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
		return SceneRenderPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, 1u );
	}

	ShaderPtr DepthPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		UBO_MATRIX( writer, uint32_t( NodeUboIdx::eMatrix ), 0 );
		UBO_SCENE( writer, uint32_t( NodeUboIdx::eScene ), 0 );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0 );
		auto skinningData = SkinningUbo::declare( writer, uint32_t( NodeUboIdx::eSkinning ), 0, flags.programFlags );
		UBO_MORPHING( writer, uint32_t( NodeUboIdx::eMorphing ), 0, flags.programFlags );

		// Outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, inSurface.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( inSurface.normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( inSurface.tangent, 0.0_f ) );
				outSurface.texture = inSurface.texture;
				inSurface.morph( c3d_morphingData
					, curPosition
					, v4Normal
					, v4Tangent
					, outSurface.texture );
				outSurface.material = c3d_modelData.getMaterialIndex( flags.programFlags
					, inSurface.material );

				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
				auto prvMtxModel = writer.declLocale< Mat4 >( "prvMtxModel"
					, c3d_modelData.getPrvModelMtx( flags.programFlags, curMtxModel ) );
				auto prvPosition = writer.declLocale( "prvPosition"
					, prvMtxModel * curPosition );
				curPosition = curMtxModel * curPosition;
				outSurface.worldPosition = curPosition.xyz();
				outSurface.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
					, c3d_modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
				outSurface.computeTangentSpace( flags.programFlags
					, c3d_sceneData.getCameraPosition()
					, mtxNormal
					, v4Normal
					, v4Tangent );
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
		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		bool hasTextures = !flags.textures.empty();

		// Intputs
		shader::InFragmentSurface inSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		auto out = writer.getOut();

		auto materials = shader::createMaterials( writer, flags.passFlags );
		materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		shader::TextureConfigurations textureConfigs{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );

		shader::Utils utils{ writer };
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = materials->getBaseMaterial( inSurface.material );
				auto opacity = writer.declLocale( "opacity"
					, material->m_opacity );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material->m_alphaRef );

				if ( hasTextures )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, inSurface.texture );
					utils.computeGeometryMapsContributions( textures
						, flags.passFlags
						, textureConfigs
						, c3d_textureData.config
						, c3d_maps
						, texCoord
						, opacity
						, inSurface.tangentSpaceViewPosition
						, inSurface.tangentSpaceFragPosition );
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, opacity
					, alphaRef );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
