#include "ForwardRenderTechniquePass.hpp"

#include "Cache/MaterialCache.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslPhongReflection.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularPbrReflection.hpp"

#include <Image/TextureView.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		void doBindTexture( ashes::TextureView const & view
			, ashes::Sampler const & sampler
			, ashes::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			writes.push_back( ashes::WriteDescriptorSet
				{
					index++,
					0u,
					1u,
					ashes::DescriptorType::eCombinedImageSampler,
					{
						{
							sampler,
							view,
							ashes::ImageLayout::eShaderReadOnlyOptimal
						},
					}
				} );
		}

		void doBindShadowMaps( ShadowMapRefArray const & shadowMaps
			, ashes::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			std::vector< ashes::DescriptorImageInfo > shadowMapWrites;

			for ( auto & shadowMap : shadowMaps )
			{
				shadowMapWrites.push_back( {
					shadowMap.first.get().getSampler(),
					shadowMap.first.get().getView(),
					ashes::ImageLayout::eShaderReadOnlyOptimal
					} );
			}

			writes.push_back( ashes::WriteDescriptorSet
				{
					index,
					0u,
					uint32_t( shadowMapWrites.size() ),
					ashes::DescriptorType::eCombinedImageSampler,
					shadowMapWrites
				} );
			index += uint32_t( shadowMapWrites.size() );
		}
	}

	//*********************************************************************************************

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & name
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderTechniquePass{ name
			, name
			, matrixUbo
			, culler
			, environment
			, ignored
			, config }
	{
	}

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & name
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderTechniquePass{ name
			, name
			, matrixUbo
			, culler
			, oit
			, environment
			, ignored
			, config }
	{
	}

	ForwardRenderTechniquePass::~ForwardRenderTechniquePass()
	{
	}

	void ForwardRenderTechniquePass::initialiseRenderPass( ashes::TextureView const & colourView
		, ashes::TextureView const & depthView
		, castor::Size const & size
		, bool clear )
	{
		auto & device = getCurrentDevice( *this );

		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = depthView.getFormat();
		renderPass.attachments[0].loadOp = clear
			? ashes::AttachmentLoadOp::eClear
			: ashes::AttachmentLoadOp::eLoad;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ( clear && ashes::isDepthStencilFormat( renderPass.attachments[0].format ) )
			? ashes::AttachmentLoadOp::eClear
			: ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::isDepthStencilFormat( renderPass.attachments[0].format )
			? ashes::AttachmentStoreOp::eStore
			: ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = clear
			? ashes::ImageLayout::eUndefined
			: ashes::ImageLayout::eDepthStencilAttachmentOptimal;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = colourView.getFormat();
		renderPass.attachments[1].loadOp = clear
			? ashes::AttachmentLoadOp::eClear
			: ashes::AttachmentLoadOp::eLoad;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = clear
			? ashes::ImageLayout::eUndefined
			: ashes::ImageLayout::eColourAttachmentOptimal;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments.push_back( { 1u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = clear
			? ashes::PipelineStageFlag::eFragmentShader
			: ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = clear
			? ashes::AccessFlag::eShaderRead
			: ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = clear
			? ashes::PipelineStageFlag::eColourAttachmentOutput
			: ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = clear
			? ashes::AccessFlag::eColourAttachmentWrite
			: ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );
		ashes::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() + 0u ), depthView );
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() + 1u ), colourView );

		m_frameBuffer = m_renderPass->createFrameBuffer( { colourView.getTexture().getDimensions().width, colourView.getTexture().getDimensions().height }
			, std::move( attaches ) );

		m_nodesCommands = device.getGraphicsCommandPool().createCommandBuffer();
		m_fence = device.createFence( ashes::FenceCreateFlag::eSignaled );
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getPassFlags()
			, visitor.getTextureFlags()
			, ProgramFlags{}
			, visitor.getSceneFlags()
			, visitor.getAlphaFunc()
			, false );
		visitor.visit( cuT( "Object" )
			, ashes::ShaderStageFlag::eVertex
			, *shaderProgram->getSource( ashes::ShaderStageFlag::eVertex ).shader );
		visitor.visit( cuT( "Object" )
			, ashes::ShaderStageFlag::eFragment
			, *shaderProgram->getSource( ashes::ShaderStageFlag::eFragment ).shader );
	}

	void ForwardRenderTechniquePass::update( RenderInfo & info
		, Point2r const & jitter )
	{
		getSceneUbo().update( m_scene, m_camera );
		doUpdate( info, jitter );
	}

	ashes::Semaphore const & ForwardRenderTechniquePass::render( ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;

		if ( hasNodes() )
		{
			static ashes::ClearValueArray const clearValues
			{
				ashes::DepthStencilClearValue{ 1.0, 0 },
				ashes::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			};

			getEngine()->setPerObjectLighting( true );
			auto timerBlock = getTimer().start();
			auto & device = getCurrentDevice( *this );

			m_nodesCommands->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
			getTimer().beginPass( *m_nodesCommands );
			getTimer().notifyPassRender();
			m_nodesCommands->beginRenderPass( getRenderPass()
				, *m_frameBuffer
				, clearValues
				, ashes::SubpassContents::eSecondaryCommandBuffers );
			m_nodesCommands->executeCommands( { getCommandBuffer() } );
			m_nodesCommands->endRenderPass();
			getTimer().endPass( *m_nodesCommands );
			m_nodesCommands->end();

			device.getGraphicsQueue().submit( { *m_nodesCommands }
				, { *result }
				, { ashes::PipelineStageFlag::eColourAttachmentOutput }
				, { getSemaphore() }
				, nullptr );
			result = &getSemaphore();
		}

		return *result;
	}

	void ForwardRenderTechniquePass::doCleanup()
	{
		m_fence.reset();
		m_nodesCommands.reset();
		m_frameBuffer.reset();
		RenderTechniquePass::doCleanup();
	}

	ashes::DescriptorSetLayoutBindingArray ForwardRenderTechniquePass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		ashes::DescriptorSetLayoutBindingArray uboBindings;

		if ( !checkFlag( flags.programFlags, ProgramFlag::eDepthPass ) )
		{
			uboBindings.emplace_back( getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eLighting ) )
		{
			uboBindings.emplace_back( LightBufferIndex, ashes::DescriptorType::eUniformTexelBuffer, ashes::ShaderStageFlag::eFragment );
		}

		uboBindings.emplace_back( MatrixUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );
		uboBindings.emplace_back( SceneUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );

		if ( !checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
		{
			uboBindings.emplace_back( ModelMatrixUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex );
		}

		uboBindings.emplace_back( ModelUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			uboBindings.push_back( SkinningUbo::createLayoutBinding( SkinningUbo::BindingPoint, flags.programFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			uboBindings.emplace_back( MorphingUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			uboBindings.emplace_back( BillboardUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex );
		}

		return uboBindings;
	}

	ashes::DescriptorSetLayoutBindingArray ForwardRenderTechniquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		ashes::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.textureFlags, TextureChannel::eDiffuse ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eSpecular ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eGloss ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eNormal ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		bool opacityMap = checkFlag( flags.textureFlags, TextureChannel::eOpacity )
			&& ( !m_opaque || flags.alphaFunc != ashes::CompareOp::eAlways );

		if ( opacityMap )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eHeight ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eAmbientOcclusion ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eEmissive ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eTransmittance ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eReflection )
			|| checkFlag( flags.textureFlags, TextureChannel::eRefraction ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.passFlags, PassFlag::ePbrMetallicRoughness )
			|| checkFlag( flags.passFlags, PassFlag::ePbrSpecularGlossiness ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapIrradiance
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapPrefiltered
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapBrdf
		}

		textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapShadowDirectional
		textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapShadowSpot
		textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapShadowPoint
		return textureBindings;
	}

	void ForwardRenderTechniquePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		node.passNode.fillDescriptor( layout
			, index
			, writes );

		if ( node.passNode.pass.hasEnvironmentMapping() )
		{
			auto & envMap = m_scene.getEnvironmentMap( node.sceneNode );
			doBindTexture( envMap.getTexture().getTexture()->getDefaultView()
				, envMap.getTexture().getSampler()->getSampler()
				, writes
				, index );
		}

		if ( node.passNode.pass.getType() != MaterialType::eLegacy )
		{
			auto & background = *node.sceneNode.getScene()->getBackground();

			if ( background.hasIbl() )
			{
				auto & ibl = background.getIbl();
				doBindTexture( ibl.getIrradianceTexture()
					, ibl.getIrradianceSampler()
					, writes
					, index );
				doBindTexture( ibl.getPrefilteredEnvironmentTexture()
					, ibl.getPrefilteredEnvironmentSampler()
					, writes
					, index );
				doBindTexture( ibl.getPrefilteredBrdfTexture()
					, ibl.getPrefilteredBrdfSampler()
					, writes
					, index );
			}
		}

		doBindShadowMaps( shadowMaps[size_t( LightType::eDirectional )], writes, index );
		doBindShadowMaps( shadowMaps[size_t( LightType::eSpot )], writes, index );
		doBindShadowMaps( shadowMaps[size_t( LightType::ePoint )], writes, index );
		node.texDescriptorSet->setBindings( writes );
	}

	void ForwardRenderTechniquePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		node.passNode.fillDescriptor( layout
			, index
			, writes );

		if ( node.passNode.pass.hasEnvironmentMapping() )
		{
			auto & envMap = m_scene.getEnvironmentMap( node.sceneNode );
			doBindTexture( envMap.getTexture().getTexture()->getDefaultView()
				, envMap.getTexture().getSampler()->getSampler()
				, writes
				, index );
		}

		if ( node.passNode.pass.getType() != MaterialType::eLegacy )
		{
			auto & background = *node.sceneNode.getScene()->getBackground();

			if ( background.hasIbl() )
			{
				auto & ibl = background.getIbl();
				doBindTexture( ibl.getIrradianceTexture()
					, ibl.getIrradianceSampler()
					, writes
					, index );
				doBindTexture( ibl.getPrefilteredEnvironmentTexture()
					, ibl.getPrefilteredEnvironmentSampler()
					, writes
					, index );
				doBindTexture( ibl.getPrefilteredBrdfTexture()
					, ibl.getPrefilteredBrdfSampler()
					, writes
					, index );
			}
		}

		doBindShadowMaps( shadowMaps[size_t( LightType::eDirectional )], writes, index );
		doBindShadowMaps( shadowMaps[size_t( LightType::eSpot )], writes, index );
		doBindShadowMaps( shadowMaps[size_t( LightType::ePoint )], writes, index );
		node.texDescriptorSet->setBindings( writes );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( cuT( "normal" )
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( cuT( "tangent" )
			, RenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( cuT( "uv" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" )
				, vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" )
				, vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, uv );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, SkinningUbo::computeTransform( skinningData, writer, programFlags ) );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, curMtxModel );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, transform );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, curMtxModel );
			}
			else
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, c3d_curMtxModel );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, c3d_prvMtxModel );
			}

			auto curMtxModel = writer.getVariable< Mat4 >( cuT( "curMtxModel" ) );
			auto prvMtxModel = writer.getVariable< Mat4 >( cuT( "prvMtxModel" ) );
			auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( curMtxModel ) ) ) );

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				curPosition = vec4( sdw::mix( curPosition.xyz(), position2.xyz(), vec3( c3d_time ) ), 1.0 );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), normal2.xyz(), vec3( c3d_time ) ), 1.0 );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), tangent2.xyz(), vec3( c3d_time ) ), 1.0 );
				v3Texture = sdw::mix( v3Texture, texture2, vec3( c3d_time ) );
			}

			vtx_texture = v3Texture;
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			vtx_worldPosition = curPosition.xyz();
			prvPosition = c3d_prvView * curPosition;
			curPosition = c3d_curView * curPosition;
			vtx_viewPosition = curPosition.xyz();

			if ( invertNormals )
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
			vtx_instance = in.gl_InstanceID;
			prvPosition = c3d_projection * prvPosition;
			curPosition = c3d_projection * curPosition;

			auto tbn = writer.declLocale( cuT( "tbn" )
				, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
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
			vtx_curPosition.xy() *= vec2( 0.5_f, -0.5 );
			vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5 );
		};

		writer.implementFunction< sdw::Void >( cuT( "main" ), main );
		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		using namespace sdw;
		FragmentWriter writer;

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::LegacyMaterials materials{ writer };
		materials.declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( cuT( "c3d_sLights" ), 1u, 0u );

		auto index = MinBufferIndex;
		auto c3d_mapDiffuse( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" )
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapSpecular" )
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGloss( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapGloss" )
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		bool opacityMap = checkFlag( textureFlags, TextureChannel::eOpacity )
			&& ( !m_opaque || alphaFunc != ashes::CompareOp::eAlways );
		auto c3d_mapOpacity( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOpacity" )
			, opacityMap ? index++ : 0u
			, 1u
			, opacityMap ) );
		auto c3d_mapHeight( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ) );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto in = writer.getIn();

		auto lighting = shader::legacy::createLightingModel( writer
			, index
			, getCuller().getScene().getDirectionalShadowCascades() );
		shader::PhongReflectionModel reflections{ writer };
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth(), renderSystem.isInvertedNormals() };
		utils.declareInvertNormal();
		utils.declareApplyGamma();
		utils.declareRemoveGamma();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity );

			if ( m_opaque && alphaFunc != ashes::CompareOp::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef );
			}
			else if ( !m_opaque && alphaFunc == ashes::CompareOp::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}
			}

			auto gamma = writer.declLocale( cuT( "gamma" )
				, material.m_gamma );
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, utils.removeGamma( gamma, material.m_diffuse() ) );
			auto specular = writer.declLocale( cuT( "specular" )
				, material.m_specular );
			auto matShininess = writer.declLocale( cuT( "matShininess" )
				, material.m_shininess );
			auto emissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive ) );
			shader::legacy::computePreLightingMapContributions( writer
				, utils
				, normal
				, matShininess
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::legacy::computePostLightingMapContributions( writer
				, utils
				, diffuse
				, specular
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matShininess
				, c3d_shadowReceiver
				, shader::FragmentInput( in.gl_FragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );
			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			auto ambient = writer.declLocale( cuT( "ambient" )
				, clamp( c3d_ambientLight.xyz() + material.m_ambient * material.m_diffuse()
					, vec3( 0.0_f )
					, vec3( 1.0_f ) ) );

			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "incident" )
					, reflections.computeIncident( vtx_worldPosition, c3d_cameraPosition.xyz() ) );

				if ( checkFlag( textureFlags, TextureChannel::eReflection )
					&& checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					ambient = reflections.computeReflRefr( incident
						, normal
						, ambientOcclusion
						, c3d_mapEnvironment
						, material.m_refractionRatio
						, diffuse );
					diffuse = vec3( 0.0_f );
				}
				else if ( checkFlag( textureFlags, TextureChannel::eReflection ) )
				{
					diffuse *= reflections.computeRefl( incident
						, normal
						, ambientOcclusion
						, c3d_mapEnvironment );
					ambient = vec3( 0.0_f );
				}
				else
				{
					ambient = reflections.computeRefr( incident
						, normal
						, ambientOcclusion
						, c3d_mapEnvironment
						, material.m_refractionRatio
						, diffuse );
					diffuse = vec3( 0.0_f );
				}
			}
			else
			{
				ambient *= ambientOcclusion * diffuse;
				diffuse *= lightDiffuse;
			}

			pxl_fragColor = vec4( diffuse + lightSpecular + emissive + ambient, alpha );

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
					, pxl_fragColor
					, length( vtx_viewPosition )
					, vtx_viewPosition.y() );
			}
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		using namespace sdw;
		FragmentWriter writer;

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrMRMaterials materials{ writer };
		materials.declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( cuT( "c3d_sLights" ), 1u, 0u );

		auto index = MinBufferIndex;
		auto c3d_mapAlbedo( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapAlbedo" )
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapRoughness" )
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapMetallic( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapMetallic" )
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapNormal( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		bool opacityMap = checkFlag( textureFlags, TextureChannel::eOpacity )
			&& ( !m_opaque || alphaFunc != ashes::CompareOp::eAlways );
		auto c3d_mapOpacity( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOpacity" )
			, opacityMap ? index++ : 0u
			, 1u
			, opacityMap ) );
		auto c3d_mapHeight( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapIrradiance" )
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapPrefiltered" )
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBrdf" )
			, index++
			, 1u );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto in = writer.getIn();

		auto lighting = shader::pbr::mr::createLightingModel( writer
			, index
			, getCuller().getScene().getDirectionalShadowCascades() );
		shader::MetallicPbrReflectionModel reflections{ writer };
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth(), renderSystem.isInvertedNormals() };
		utils.declareInvertNormal();
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity );

			if ( m_opaque && alphaFunc != ashes::CompareOp::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, material.m_metallic );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, material.m_roughness );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma );
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, utils.removeGamma( matGamma, material.m_albedo ) );
			auto matEmissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			shader::pbr::mr::computePreLightingMapContributions( writer
				, utils
				, normal
				, matMetallic
				, matRoughness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::mr::computePostLightingMapContributions( writer
				, utils
				, matAlbedo
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matAlbedo
				, matMetallic
				, matRoughness
				, c3d_shadowReceiver
				, shader::FragmentInput( in.gl_FragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );

			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "incident" )
					, reflections.computeIncident( vtx_worldPosition, worldEye ) );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio );

				if ( checkFlag( textureFlags, TextureChannel::eReflection ) )
				{
					// Reflection from environment map.
					ambient = reflections.computeRefl( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, c3d_ambientLight.xyz()
						, matAlbedo );
				}
				else
				{
					// Reflection from background skybox.
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, vtx_worldPosition
							, matAlbedo
							, matMetallic
							, matRoughness
							, c3d_cameraPosition.xyz()
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
				}

				if ( checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					// Refraction from environment map.
					ambient = reflections.computeRefr( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, ratio
						, ambient
						, matAlbedo
						, matRoughness );
				}
				else
				{
					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
							, ambient
							, matAlbedo
							, matRoughness );
					}
					FI;
				}
			}
			else
			{
				// Reflection from background skybox.
				ambient *= occlusion * utils.computeMetallicIBL( normal
					, vtx_worldPosition
					, matAlbedo
					, matMetallic
					, matRoughness
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
					ambient = reflections.computeRefr( incident
						, normal
						, occlusion
						, c3d_mapPrefiltered
						, material.m_refractionRatio
						, ambient
						, matAlbedo
						, matRoughness );
				}
				FI;
			}

			pxl_fragColor.xyz() = sdw::fma( lightDiffuse
				, matAlbedo
				, lightSpecular + matEmissive + ambient );

			if ( !m_opaque && alphaFunc != ashes::CompareOp::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				pxl_fragColor.a() = alpha;
			}

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				pxl_fragColor = fog.apply( vec4( utils.removeGamma( matGamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
					, pxl_fragColor
					, length( vtx_viewPosition )
					, vtx_viewPosition.y() );
			}
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		using namespace sdw;
		FragmentWriter writer;

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" )
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrSGMaterials materials{ writer };
		materials.declare( getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( cuT( "c3d_sLights" ), 1u, 0u );

		auto index = MinBufferIndex;
		auto c3d_mapDiffuse( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" )
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapSpecular" )
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapGloss" )
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		bool opacityMap = checkFlag( textureFlags, TextureChannel::eOpacity )
			&& ( !m_opaque || alphaFunc != ashes::CompareOp::eAlways );
		auto c3d_mapOpacity( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOpacity" )
			, opacityMap ? index++ : 0u
			, 1u
			, opacityMap ) );
		auto c3d_mapHeight( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapIrradiance" )
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapPrefiltered" )
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBrdf" )
			, index++
			, 1u );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto in = writer.getIn();

		auto lighting = shader::pbr::sg::createLightingModel( writer
			, index
			, getCuller().getScene().getDirectionalShadowCascades() );
		shader::SpecularPbrReflectionModel reflections{ writer };
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth(), renderSystem.isInvertedNormals() };
		utils.declareInvertNormal();
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 ) );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, -normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity );

			if ( m_opaque && alphaFunc != ashes::CompareOp::eAlways )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" )
				, material.m_glossiness );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive ) );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			shader::pbr::sg::computePreLightingMapContributions( writer
				, utils
				, normal
				, matSpecular
				, matGlossiness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::sg::computePostLightingMapContributions( writer
				, utils
				, matDiffuse
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matDiffuse
				, matSpecular
				, matGlossiness
				, c3d_shadowReceiver
				, shader::FragmentInput( in.gl_FragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
				, output );
			
			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "incident" )
					, reflections.computeIncident( vtx_worldPosition, worldEye ) );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio );

				if ( checkFlag( textureFlags, TextureChannel::eReflection ) )
				{
					// Reflection from environment map.
					ambient = reflections.computeRefl( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, c3d_ambientLight.xyz()
						, matDiffuse );
				}
				else
				{
					// Reflection from background skybox.
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, vtx_worldPosition
							, matDiffuse
							, matSpecular
							, matGlossiness
							, c3d_cameraPosition.xyz()
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
				}

				if ( checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					// Refraction from environment map.
					ambient = reflections.computeRefr( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, ratio
						, ambient
						, matDiffuse
						, matGlossiness );
				}
				else
				{
					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
							, ambient
							, matDiffuse
							, matGlossiness );
					}
					FI;
				}
			}
			else
			{
				// Reflection from background skybox.
				ambient *= occlusion * utils.computeSpecularIBL( normal
					, vtx_worldPosition
					, matDiffuse
					, matSpecular
					, matGlossiness
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
					ambient = reflections.computeRefr( incident
						, normal
						, occlusion
						, c3d_mapPrefiltered
						, material.m_refractionRatio
						, ambient
						, matDiffuse
						, matGlossiness );
				}
				FI;
			}

			pxl_fragColor.xyz() = sdw::fma( lightDiffuse
				, matDiffuse
				, lightSpecular + matEmissive + ambient );

			if ( !m_opaque )
			{
				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				pxl_fragColor.a() = alpha;
			}

			if ( getFogType( sceneFlags ) != FogType::eDisabled )
			{
				pxl_fragColor = fog.apply( vec4( utils.removeGamma( matGamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
					, pxl_fragColor
					, length( vtx_viewPosition )
					, vtx_viewPosition.y() );
			}
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	//*********************************************************************************************
}
