#include "TransparentPass.hpp"

#include "Engine.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Pipeline/ColourBlendState.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslPhongReflection.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		void doBindTexture( renderer::TextureView const & view
			, renderer::Sampler const & sampler
			, renderer::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			writes.push_back( renderer::WriteDescriptorSet
				{
					index++,
					0u,
					1u,
					renderer::DescriptorType::eCombinedImageSampler,
				{
					{
						sampler,
						view,
						renderer::ImageLayout::eShaderReadOnlyOptimal
					},
				}
				} );
		}

		void doBindShadowMaps( ShadowMapRefArray const & shadowMaps
			, renderer::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			std::vector< renderer::DescriptorImageInfo > shadowMapWrites;

			for ( auto & shadowMap : shadowMaps )
			{
				shadowMapWrites.push_back( {
					shadowMap.get().getTexture().getSampler()->getSampler(),
					shadowMap.get().getTexture().getTexture()->getDefaultView(),
					renderer::ImageLayout::eShaderReadOnlyOptimal
					} );
			}

			writes.push_back( renderer::WriteDescriptorSet
				{
					index,
					0u,
					uint32_t( shadowMapWrites.size() ),
					renderer::DescriptorType::eCombinedImageSampler,
					shadowMapWrites
				} );
			index += uint32_t( shadowMapWrites.size() );
		}
	}

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

	renderer::Format getTextureFormat( WbTexture texture )
	{
		static std::array< renderer::Format, size_t( WbTexture::eCount ) > Values
		{
			{
				renderer::Format::eD24_UNORM_S8_UINT,
				renderer::Format::eR16G16B16A16_SFLOAT,
				renderer::Format::eR16_SFLOAT,
				renderer::Format::eR16G16B16A16_SFLOAT,
			}
		};

		return Values[size_t( texture )];
	}

	//************************************************************************************************

	TransparentPass::TransparentPass( MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, SsaoConfig const & config )
		: castor3d::RenderTechniquePass{ cuT( "Transparent" )
			, cuT( "Accumulation" )
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
		auto & device = getCurrentDevice( *this );

		renderer::RenderPassCreateInfo renderPass{};
		renderPass.flags = 0u;

		renderPass.attachments.resize( uint32_t( WbTexture::eCount ) );
		renderPass.attachments[0].format = wbpResult[0].get().getFormat();
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = wbpResult[1].get().getFormat();
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.attachments[2].format = wbpResult[2].get().getFormat();
		renderPass.attachments[2].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[2].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[2].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[2].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[2].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[2].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[2].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.attachments[3].format = wbpResult[3].get().getFormat();
		renderPass.attachments[3].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[3].loadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[3].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[3].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[3].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[3].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[3].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderer::AttachmentReference colourReference;
		colourReference.attachment = 0u;
		colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments =
		{
			{ 1u, renderer::ImageLayout::eColourAttachmentOptimal },
			{ 2u, renderer::ImageLayout::eColourAttachmentOptimal },
			{ 3u, renderer::ImageLayout::eColourAttachmentOptimal },
		};

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass =  device.createRenderPass( renderPass );
		renderer::FrameBufferAttachmentArray attaches;

		for ( size_t i = 0u; i < wbpResult.size(); ++i )
		{
			attaches.emplace_back( *( m_renderPass->getAttachments().begin() + i ), wbpResult[i].get() );
		}

		m_frameBuffer = m_renderPass->createFrameBuffer( { wbpResult[0].get().getTexture().getDimensions().width, wbpResult[0].get().getTexture().getDimensions().height }
			, std::move( attaches ) );

		m_nodesCommands = getCurrentDevice( *this ).getGraphicsCommandPool().createCommandBuffer();
	}

	void TransparentPass::update( RenderInfo & info
		, Point2r const & jitter )
	{
		auto & nodes = m_renderQueue.getCulledRenderNodes();

		if ( nodes.hasNodes() )
		{
			auto & camera = *m_camera;

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

	renderer::Semaphore const & TransparentPass::render( renderer::Semaphore const & toWait )
	{
		static renderer::ClearValueArray const clearValues
		{
			renderer::DepthStencilClearValue{ 1.0, 0 },
			renderer::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 },
			renderer::ClearColorValue{ 1.0, 1.0, 1.0, 1.0 },
			renderer::ClearColorValue{},
		};

		auto * result = &toWait;
		auto & timer = getTimer();
		auto & device = getCurrentDevice( *this );
		auto timerBlock = timer.start();

		m_nodesCommands->begin();
		timer.beginPass( *m_nodesCommands );
		timer.notifyPassRender();
		m_nodesCommands->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, clearValues
			, renderer::SubpassContents::eSecondaryCommandBuffers );
		m_nodesCommands->executeCommands( { getCommandBuffer() } );
		m_nodesCommands->endRenderPass();
		timer.endPass( *m_nodesCommands );
		m_nodesCommands->end();

		device.getGraphicsQueue().submit( *m_nodesCommands
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, getSemaphore()
			, nullptr );
		result = &getSemaphore();

		return *result;
	}

	void TransparentPass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getPassFlags()
			, visitor.getTextureFlags()
			, ProgramFlags{}
			, visitor.getSceneFlags()
			, visitor.getAlphaFunc()
			, false );
		visitor.visit( cuT( "Object" )
			, renderer::ShaderStageFlag::eVertex
			, shaderProgram->getSource( renderer::ShaderStageFlag::eVertex ) );
		visitor.visit( cuT( "Object" )
			, renderer::ShaderStageFlag::eFragment
			, shaderProgram->getSource( renderer::ShaderStageFlag::eFragment ) );
	}

	bool TransparentPass::doInitialise( Size const & size )
	{
		m_finished = getCurrentDevice( *this ).createSemaphore();
		return true;
	}

	renderer::DepthStencilState TransparentPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return renderer::DepthStencilState{ 0u, true, false };
	}

	renderer::ColourBlendState TransparentPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		renderer::ColourBlendState bdState;
		bdState.attachs.push_back( renderer::ColourBlendStateAttachment
			{
				true,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eOne,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eOne,
				renderer::BlendOp::eAdd,
			} );
		bdState.attachs.push_back( renderer::ColourBlendStateAttachment
			{
				true,
				renderer::BlendFactor::eZero,
				renderer::BlendFactor::eInvSrcColour,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eZero,
				renderer::BlendFactor::eInvSrcColour,
				renderer::BlendOp::eAdd,
			} );
		bdState.attachs.push_back( renderer::ColourBlendStateAttachment
			{
				false,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eZero,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eZero,
				renderer::BlendOp::eAdd,
			} );
		return bdState;
	}

	void TransparentPass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		renderer::WriteDescriptorSetArray writes;
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
			auto & background = node.sceneNode.getScene()->getBackground();

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

	void TransparentPass::doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		renderer::WriteDescriptorSetArray writes;
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
			auto & background = node.sceneNode.getScene()->getBackground();

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

	renderer::DescriptorSetLayoutBindingArray TransparentPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		renderer::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.textureFlags, TextureChannel::eDiffuse ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eSpecular ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eGloss ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eNormal ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eOpacity ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eHeight ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eAmbientOcclusion ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eEmissive ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eTransmittance ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eReflection )
			|| checkFlag( flags.textureFlags, TextureChannel::eRefraction ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.passFlags, PassFlag::ePbrMetallicRoughness )
			|| checkFlag( flags.passFlags, PassFlag::ePbrSpecularGlossiness ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapIrradiance
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapPrefiltered
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );	// c3d_mapBrdf
		}

		textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment, 1u );
		textureBindings.emplace_back( index, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment, shader::SpotShadowMapCount );
		index += shader::SpotShadowMapCount;
		textureBindings.emplace_back( index, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment, shader::PointShadowMapCount );
		index += shader::PointShadowMapCount;

		return textureBindings;
	}

	glsl::Shader TransparentPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		REQUIRE( ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declAttribute< Vec3 >( cuT( "normal" )
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declAttribute< Vec3 >( cuT( "tangent" )
			, RenderPass::VertexInputs::TangentLocation );
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
		auto normal2 = writer.declAttribute< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( writer.getInstanceID() ) );
		REQUIRE( ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
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
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = gl_PerVertex{ writer };

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" )
				, vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" )
				, vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, texture );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, SkinningUbo::computeTransform( writer, programFlags ) );
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

			auto curMtxModel = writer.declBuiltin< Mat4 >( cuT( "curMtxModel" ) );
			auto prvMtxModel = writer.declBuiltin< Mat4 >( cuT( "prvMtxModel" ) );
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
				curPosition = vec4( glsl::mix( curPosition.xyz(), position2.xyz(), c3d_time ), 1.0 );
				v4Normal = vec4( glsl::mix( v4Normal.xyz(), normal2.xyz(), c3d_time ), 1.0 );
				v4Tangent = vec4( glsl::mix( v4Tangent.xyz(), tangent2.xyz(), c3d_time ), 1.0 );
				v3Texture = glsl::mix( v3Texture, texture2, c3d_time );
			}

			vtx_texture = v3Texture;
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			vtx_worldPosition = curPosition.xyz();
			prvPosition = c3d_prvViewProj * prvPosition;
			curPosition = c3d_curViewProj * curPosition;
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
			vtx_tangent = normalize( glsl::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;

			auto tbn = writer.declLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

			// Convert the jitter from non-homogeneous coordiantes to homogeneous
			// coordinates and add it:
			// (note that for providing the jitter in non-homogeneous projection space,
			//  pixel coordinates (screen space) need to multiplied by two in the C++
			//  code)
			curPosition.xy() -= c3d_jitter * curPosition.w();
			prvPosition.xy() -= c3d_jitter * prvPosition.w();
			out.gl_Position() = curPosition;

			vtx_curPosition = curPosition.xyw();
			vtx_prvPosition = prvPosition.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			vtx_curPosition.xy() *= vec2( 0.5_f, -0.5 );
			vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5 );
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader TransparentPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );

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
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::LegacyMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( cuT( "c3d_sLights" ), 1u, 0u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( cuT( "c3d_sLights" ), 1u, 0u );
		}

		auto index = MinBufferIndex;
		auto c3d_mapDiffuse( writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" )
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampler< Sampler2D >( cuT( "c3d_mapSpecular" )
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGloss( writer.declSampler< Sampler2D >( cuT( "c3d_mapGloss" )
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampler< SamplerCube >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = shader::legacy::createLightingModel( writer
			, index
			, getCuller().getScene().getDirectionalShadowCascades() );
		shader::PhongReflectionModel reflections{ writer };
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 2 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matShininess = writer.declLocale( cuT( "matShininess" )
				, material.m_shininess() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, matDiffuse * material.m_emissive() );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
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

			shader::legacy::computePreLightingMapContributions( writer
				, normal
				, matShininess
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			shader::OutputComponents output{ lightDiffuse, lightSpecular };
			lighting->computeCombined( worldEye
				, matShininess
				, c3d_shadowReceiver
				, shader::FragmentInput( vtx_viewPosition, vtx_worldPosition, normal )
				, output );

			shader::legacy::computePostLightingMapContributions( writer
				, matDiffuse
				, matSpecular
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );

			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto colour = writer.declLocale< Vec3 >( cuT( "colour" ) );

			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				auto incident = writer.declLocale( cuT( "incident" )
					, reflections.computeIncident( vtx_worldPosition, c3d_cameraPosition.xyz() ) );

				if ( checkFlag( textureFlags, TextureChannel::eReflection )
					&& checkFlag( textureFlags, TextureChannel::eRefraction ) )
				{
					colour = reflections.computeReflRefr( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, material.m_refractionRatio()
						, matDiffuse );
				}
				else if ( checkFlag( textureFlags, TextureChannel::eReflection ) )
				{
					colour = reflections.computeRefl( incident
						, normal
						, occlusion
						, c3d_mapEnvironment );
				}
				else
				{
					colour = reflections.computeRefr( incident
						, normal
						, occlusion
						, c3d_mapEnvironment
						, material.m_refractionRatio()
						, matDiffuse );
				}
			}
			else
			{
				colour = glsl::fma( ambient + lightDiffuse
					, matDiffuse
					, glsl::fma( lightSpecular
						, matSpecular
						, matEmissive ) );
			}

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_invProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z()
				, c3d_cameraNearPlane
				, c3d_cameraFarPlane );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.declLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.declLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_accumulation = vec4( colour * alpha, alpha ) * weight;
			pxl_revealage = alpha;
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return writer.finalise();
	}

	glsl::Shader TransparentPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );

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
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrMRMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( cuT( "c3d_sLights" ), 1u, 0u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( cuT( "c3d_sLights" ), 1u, 0u );
		}

		auto index = MinBufferIndex;
		auto c3d_mapAlbedo( writer.declSampler< Sampler2D >( cuT( "c3d_mapAlbedo" )
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.declSampler< Sampler2D >( cuT( "c3d_mapRoughness" )
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapMetallic( writer.declSampler< Sampler2D >( cuT( "c3d_mapMetallic" )
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampler< SamplerCube >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( cuT( "c3d_mapIrradiance" )
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( cuT( "c3d_mapPrefiltered" )
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampler< Sampler2D >( cuT( "c3d_mapBrdf" )
			, index++
			, 1u );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = shader::pbr::mr::createLightingModel( writer
			, index
			, getCuller().getScene().getDirectionalShadowCascades() );
		glsl::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 2 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, material.m_metallic() );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, material.m_roughness() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, utils.removeGamma( matGamma, material.m_albedo() ) );
			auto matEmissive = writer.declLocale( cuT( "emissive" )
				, matAlbedo * material.m_emissive() );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			shader::pbr::mr::computePreLightingMapContributions( writer
				, normal
				, matMetallic
				, matRoughness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
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
				, shader::FragmentInput( vtx_viewPosition, vtx_worldPosition, normal )
				, output );

			shader::pbr::mr::computePostLightingMapContributions( writer
				, matAlbedo
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );

			ambient *= occlusion * utils.computeMetallicIBL( normal
				, vtx_worldPosition
				, matAlbedo
				, matMetallic
				, matRoughness
				, c3d_cameraPosition.xyz()
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf );
			auto colour = writer.declLocale( cuT( "colour" )
				, glsl::fma( lightDiffuse
					, matAlbedo
					, lightSpecular + matEmissive + ambient ) );

			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_invProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z()
				, c3d_cameraNearPlane
				, c3d_cameraFarPlane );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.declLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.declLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_accumulation = vec4( colour * alpha, alpha ) * weight;
			pxl_revealage = alpha;
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return writer.finalise();
	}

	glsl::Shader TransparentPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = m_renderSystem.createGlslWriter();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );

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
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrSGMaterials materials{ writer };
		materials.declare();

		if ( writer.hasTextureBuffers() )
		{
			auto c3d_sLights = writer.declSampler< SamplerBuffer >( cuT( "c3d_sLights" ), 1u, 0u );
		}
		else
		{
			auto c3d_sLights = writer.declSampler< Sampler1D >( cuT( "c3d_sLights" ), 1u, 0u );
		}

		auto index = MinBufferIndex;
		auto c3d_mapDiffuse( writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" )
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampler< Sampler2D >( cuT( "c3d_mapSpecular" )
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.declSampler< Sampler2D >( cuT( "c3d_mapGloss" )
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, ( checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) && !m_opaque ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_mapEnvironment( writer.declSampler< SamplerCube >( cuT( "c3d_mapEnvironment" )
			, ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) ) );
		auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( cuT( "c3d_mapIrradiance" )
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( cuT( "c3d_mapPrefiltered" )
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampler< Sampler2D >( cuT( "c3d_mapBrdf" )
			, index++
			, 1u );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto lighting = shader::pbr::sg::createLightingModel( writer
			, index
			, getCuller().getScene().getDirectionalShadowCascades() );
		glsl::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareLineariseDepth();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );

		// Fragment Outputs
		auto pxl_accumulation( writer.declFragData< Vec4 >( getTextureName( WbTexture::eAccumulation ), 0 ) );
		auto pxl_revealage( writer.declFragData< Float >( getTextureName( WbTexture::eRevealage ), 1 ) );
		auto pxl_velocity( writer.declFragData< Vec4 >( cuT( "pxl_velocity" ), 2 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto ambient = writer.declLocale( cuT( "ambient" )
				, c3d_ambientLight.xyz() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" )
				, material.m_glossiness() );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, utils.removeGamma( matGamma, material.m_diffuse() ) );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, matDiffuse * material.m_emissive() );
			auto worldEye = writer.declLocale( cuT( "worldEye" )
				, c3d_cameraPosition.xyz() );
			auto envAmbient = writer.declLocale( cuT( "envAmbient" )
				, vec3( 1.0_f ) );
			auto envDiffuse = writer.declLocale( cuT( "envDiffuse" )
				, vec3( 1.0_f ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				occlusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = -writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			shader::pbr::sg::computePreLightingMapContributions( writer
				, normal
				, matSpecular
				, matGlossiness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::sg::computePostLightingMapContributions( writer
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
				, shader::FragmentInput( vtx_viewPosition, vtx_worldPosition, normal )
				, output );

			ambient *= occlusion * utils.computeSpecularIBL( normal
				, vtx_worldPosition
				, matDiffuse
				, matSpecular
				, matGlossiness
				, c3d_cameraPosition.xyz()
				, c3d_mapIrradiance
				, c3d_mapPrefiltered
				, c3d_mapBrdf );
			auto colour = writer.declLocale( cuT( "colour" )
				, glsl::fma( lightDiffuse
					, matDiffuse
					, lightSpecular + matEmissive + ambient ) );

			auto alpha = writer.declLocale( cuT( "alpha" ), material.m_opacity() );

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			}

			//// Naive
			//auto depth = utils.lineariseDepth( gl_FragCoord.z(), c3d_invProjection );
			//auto weight = writer.declLocale( cuT( "weight" ), 1.0_f - depth );

			// (10)
			auto depth = utils.lineariseDepth( gl_FragCoord.z()
				, c3d_cameraNearPlane
				, c3d_cameraFarPlane );
			auto weight = writer.declLocale( cuT( "weight" )
				, max( pow( 1.0_f - depth, 3.0_f ) * 3e3, 1e-2 ) );

			//// (9)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (8)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (7)
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( gl_FragCoord.z() ) / 200.0_f, 6.0_f ) + pow( glsl::abs( gl_FragCoord.z() ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

			//// (other)
			//auto a = writer.declLocale( cuT( "a" )
			//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
			//auto b = writer.declLocale( cuT( "b" )
			//	, -gl_FragCoord.z() * 0.95 + 1.0 );
			///* If your scene has a lot of content very close to the far plane,
			//then include this line (one rsqrt instruction):
			//b /= sqrt(1e4 * abs(csZ)); */
			//auto weight = writer.declLocale( cuT( "weight" )
			//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

			pxl_accumulation = vec4( colour * alpha, alpha ) * weight;
			pxl_revealage = alpha;
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			pxl_velocity.xy() = curPosition - prvPosition;
		} );

		return writer.finalise();
	}

	void TransparentPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
		m_sceneUbo.update( *m_camera->getScene(), *m_camera, true );
	}
}
