#include "LightPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Technique/Opaque/GeometryPassResult.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Ashes/Command/CommandBufferInheritanceInfo.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/Pipeline/ColourBlendState.hpp>
#include <Ashes/Pipeline/DepthStencilState.hpp>
#include <Ashes/Pipeline/MultisampleState.hpp>
#include <Ashes/Pipeline/RasterisationState.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"

using namespace castor;
using namespace castor3d;

#define C3D_UseLightPassFence 1
#define C3D_DisableSSSTransmittance 1
#define C3D_DebugEye 0
#define C3D_DebugVSPosition 0
#define C3D_DebugWSPosition 0
#define C3D_DebugWSNormal 0
#define C3D_DebugTexCoord 0

namespace castor3d
{
	//************************************************************************************************

	String getTextureName( DsTexture texture )
	{
		static std::array< String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapData1" ),
				cuT( "c3d_mapData2" ),
				cuT( "c3d_mapData3" ),
				cuT( "c3d_mapData4" ),
				cuT( "c3d_mapData5" ),
			}
		};

		return Values[size_t( texture )];
	}

	ashes::Format getTextureFormat( DsTexture texture )
	{
		static std::array< ashes::Format, size_t( DsTexture::eCount ) > Values
		{
			{
				ashes::Format::eD24_UNORM_S8_UINT,
				ashes::Format::eR32G32B32A32_SFLOAT,
				ashes::Format::eR16G16B16A16_SFLOAT,
				ashes::Format::eR16G16B16A16_SFLOAT,
				ashes::Format::eR16G16B16A16_SFLOAT,
				ashes::Format::eR16G16B16A16_SFLOAT,
			}
		};

		return Values[size_t( texture )];
	}

	uint32_t getTextureAttachmentIndex( DsTexture texture )
	{
		static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
		{
			{
				0,
				0,
				1,
				2,
				3,
				4,
			}
		};

		return Values[size_t( texture )];
	}

	float getMaxDistance( LightCategory const & light
		, Point3f const & attenuation )
	{
		constexpr float threshold = 0.000001f;
		auto constant = std::abs( attenuation[0] );
		auto linear = std::abs( attenuation[1] );
		auto quadratic = std::abs( attenuation[2] );
		float result = std::numeric_limits< float >::max();

		if ( constant >= threshold
			|| linear >= threshold
			|| quadratic >= threshold )
		{
			float maxChannel = std::max( std::max( light.getColour()[0]
				, light.getColour()[1] )
				, light.getColour()[2] );
			result = 256.0f * maxChannel * light.getDiffuseIntensity();

			if ( quadratic >= threshold )
			{
				if ( linear < threshold )
				{
					CU_Require( result >= constant );
					result = sqrtf( ( result - constant ) / quadratic );
				}
				else
				{
					auto delta = linear * linear - 4 * quadratic * ( constant - result );
					CU_Require( delta >= 0 );
					result = ( -linear + sqrtf( delta ) ) / ( 2 * quadratic );
				}
			}
			else if ( linear >= threshold )
			{
				result = ( result - constant ) / linear;
			}
			else
			{
				result = 4000.0f;
			}
		}
		else
		{
			Logger::logError( cuT( "Light's attenuation is set to (0.0, 0.0, 0.0), which results in infinite litten distance, not representable." ) );
		}

		return result;
	}

	float getMaxDistance( LightCategory const & light
		, Point3f const & attenuation
		, float max )
	{
		return std::min( max, getMaxDistance( light, attenuation ) );
	}

	namespace
	{
		ashes::ShaderStageStateArray doCreateProgram( Engine & engine
			, ShaderModule const & vtx
			, ShaderModule const & pxl )
		{
			auto & device = getCurrentDevice( engine );
			ashes::ShaderStageStateArray program
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( engine.getRenderSystem()->compileShader( vtx ) );
			program[1].module->loadShader( engine.getRenderSystem()->compileShader( pxl ) );
			return program;
		}
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & engine
		, ShaderModule const & vtx
		, ShaderModule const & pxl
		, bool hasShadows )
		: m_engine{ engine }
		, m_program{ ::doCreateProgram( engine, vtx, pxl ) }
		, m_shadows{ hasShadows }
	{
	}

	void LightPass::Program::initialise( ashes::VertexBufferBase & vbo
		, ashes::VertexLayout const & vertexLayout
		, ashes::RenderPass const & firstRenderPass
		, ashes::RenderPass const & blendRenderPass
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( m_engine.getMaterialCache().getPassBuffer().createLayoutBinding() );
		setLayoutBindings.emplace_back( MatrixUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( SceneUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( GpInfoUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );

		if ( modelMatrixUbo )
		{
			setLayoutBindings.emplace_back( ModelMatrixUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex );
		}

		setLayoutBindings.emplace_back( shader::LightingModel::UboBindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment );
		m_uboDescriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_uboDescriptorPool = m_uboDescriptorLayout->createPool( 2u );
		uint32_t index = MinBufferIndex;

		setLayoutBindings = ashes::DescriptorSetLayoutBindingArray
		{
			{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
		};

		if ( m_shadows )
		{
			setLayoutBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		m_textureDescriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_textureDescriptorPool = m_textureDescriptorLayout->createPool( 2u );

		m_pipelineLayout = device.createPipelineLayout( { *m_uboDescriptorLayout, *m_textureDescriptorLayout } );
		m_firstPipeline = doCreatePipeline( vertexLayout, firstRenderPass, false );
		m_blendPipeline = doCreatePipeline( vertexLayout, blendRenderPass, true );
	}

	void LightPass::Program::cleanup()
	{
		m_blendPipeline.reset();
		m_firstPipeline.reset();
		m_pipelineLayout.reset();
		m_textureDescriptorPool.reset();
		m_textureDescriptorLayout.reset();
		m_uboDescriptorPool.reset();
		m_uboDescriptorLayout.reset();
	}

	void LightPass::Program::bind( Light const & light )
	{
		doBind( light );
	}

	void LightPass::Program::render( ashes::CommandBuffer & commandBuffer
		, uint32_t count
		, bool first
		, uint32_t offset )const
	{
		if ( first )
		{
			commandBuffer.bindPipeline( *m_firstPipeline );
		}
		else
		{
			commandBuffer.bindPipeline( *m_blendPipeline );
		}

		commandBuffer.draw( count, 1u, offset );
	}

	//************************************************************************************************

	LightPass::RenderPass::RenderPass( ashes::RenderPassPtr && renderPass
		, ashes::TextureView const & depthView
		, ashes::TextureView const & diffuseView
		, ashes::TextureView const & specularView )
		: renderPass{ std::move( renderPass ) }
	{
		ashes::FrameBufferAttachmentArray attaches
		{
			{ *( this->renderPass->getAttachments().begin() + 0u ), depthView },
			{ *( this->renderPass->getAttachments().begin() + 1u ), diffuseView },
			{ *( this->renderPass->getAttachments().begin() + 2u ), specularView },
		};
		frameBuffer = this->renderPass->createFrameBuffer( { depthView.getTexture().getDimensions().width, depthView.getTexture().getDimensions().height }
			, std::move( attaches ) );
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & engine
		, ashes::RenderPassPtr && firstRenderPass
		, ashes::RenderPassPtr && blendRenderPass
		, ashes::TextureView const & depthView
		, ashes::TextureView const & diffuseView
		, ashes::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: m_engine{ engine }
		, m_firstRenderPass{std::move( firstRenderPass ), depthView, diffuseView, specularView }
		, m_blendRenderPass{ std::move( blendRenderPass ), depthView, diffuseView, specularView }
		, m_shadows{ hasShadows }
		, m_matrixUbo{ engine }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sampler{ engine.getDefaultSampler() }
		, m_signalReady{ getCurrentDevice( engine ).createSemaphore() }
		, m_fence{ getCurrentDevice( engine ).createFence( ashes::FenceCreateFlag::eSignaled ) }
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "LightPass" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "LightPass" }
	{
	}

	void LightPass::update( bool first
		, castor::Size const & size
		, Light const & light
		, Camera const & camera
		, ShadowMap const * shadowMap
		, uint32_t shadowMapIndex )
	{
		auto index = size_t( light.getShadowType() ) + size_t( ( light.getVolumetricSteps() > 0 ? 1u : 0u ) * size_t( ShadowType::eCount ) );
		m_pipeline = &m_pipelines[index];
		doUpdate( first
			, size
			, light
			, camera
			, shadowMap
			, shadowMapIndex );
	}

	ashes::Semaphore const & LightPass::render( uint32_t index
		, ashes::Semaphore const & toWait )
	{
		static ashes::DepthStencilClearValue const clearDepthStencil{ 1.0, 1 };
		static ashes::ClearColorValue const clearColour{ 0.0, 0.0, 0.0, 1.0 };
		CU_Require( m_pipeline );
		auto result = &toWait;
		auto & device = getCurrentDevice( m_engine );

		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		m_timer->beginPass( *m_commandBuffer, index );
		m_timer->notifyPassRender( index );

		if ( !index )
		{
			m_commandBuffer->beginRenderPass( *m_firstRenderPass.renderPass
				, *m_firstRenderPass.frameBuffer
				, { clearDepthStencil, clearColour, clearColour }
				, ashes::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { *m_pipeline->firstCommandBuffer } );
		}
		else
		{
			m_commandBuffer->beginRenderPass( *m_blendRenderPass.renderPass
				, *m_blendRenderPass.frameBuffer
				, { clearDepthStencil, clearColour, clearColour }
				, ashes::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { *m_pipeline->blendCommandBuffer } );
		}

		m_commandBuffer->endRenderPass();
		m_timer->endPass( *m_commandBuffer, index );
		m_commandBuffer->end();

#if C3D_UseLightPassFence
		m_fence->reset();
		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_signalReady
			, m_fence.get() );
		m_fence->wait( ashes::FenceTimeout );
#else
		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_signalReady
			, nullptr );
#endif
		result = m_signalReady.get();

		return *result;
	}

	void LightPass::doInitialise( Scene const & scene
		, GeometryPassResult const & gp
		, LightType lightType
		, ashes::VertexBufferBase & vbo
		, ashes::VertexLayout const & vertexLayout
		, SceneUbo & sceneUbo
		, ModelMatrixUbo * modelMatrixUbo
		, RenderPassTimer & timer )
	{
		m_scene = &scene;
		m_timer = &timer;
		m_geometryPassResult = &gp;
		SceneFlags sceneFlags{ scene.getFlags() };
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		auto pipelineIndex = 0u;

		for ( auto & pipeline : m_pipelines )
		{
			auto shadowType = ShadowType( pipelineIndex % uint32_t( ShadowType::eCount ) );
			bool volumetric = pipelineIndex >= uint32_t( ShadowType::eCount );
			m_vertexShader.shader = doGetVertexShaderSource( sceneFlags );

			if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness )
			{
				m_pixelShader.shader = doGetPbrMRPixelShaderSource( sceneFlags
					, lightType
					, shadowType
					, volumetric );
			}
			else if ( scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_pixelShader.shader = doGetPbrSGPixelShaderSource( sceneFlags
					, lightType
					, shadowType
					, volumetric );
			}
			else
			{
				m_pixelShader.shader = doGetLegacyPixelShaderSource( sceneFlags
					, lightType
					, shadowType
					, volumetric );
			}

			pipeline.program = doCreateProgram();
			pipeline.program->initialise( vbo
				, vertexLayout
				, *m_firstRenderPass.renderPass
				, *m_blendRenderPass.renderPass
				, m_matrixUbo
				, sceneUbo
				, m_gpInfoUbo
				, modelMatrixUbo );
			pipeline.uboDescriptorSet = pipeline.program->getUboDescriptorPool().createDescriptorSet( 0u );
			auto & uboLayout = pipeline.program->getUboDescriptorLayout();
			m_engine.getMaterialCache().getPassBuffer().createBinding( *pipeline.uboDescriptorSet, uboLayout.getBinding( 0u ) );
			pipeline.uboDescriptorSet->createBinding( uboLayout.getBinding( MatrixUbo::BindingPoint )
				, m_matrixUbo.getUbo() );
			pipeline.uboDescriptorSet->createBinding( uboLayout.getBinding( SceneUbo::BindingPoint )
				, sceneUbo.getUbo() );
			pipeline.uboDescriptorSet->createBinding( uboLayout.getBinding( GpInfoUbo::BindingPoint )
				, m_gpInfoUbo.getUbo() );
			if ( modelMatrixUbo )
			{
				pipeline.uboDescriptorSet->createBinding( uboLayout.getBinding( ModelMatrixUbo::BindingPoint )
					, modelMatrixUbo->getUbo() );
			}
			pipeline.uboDescriptorSet->createBinding( uboLayout.getBinding( shader::LightingModel::UboBindingPoint )
				, *m_baseUbo
				, 0u
				, m_baseUbo->getElementSize() );
			pipeline.uboDescriptorSet->update();

			pipeline.firstCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
			pipeline.blendCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
			m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );

			pipeline.textureDescriptorSet = pipeline.program->getTextureDescriptorPool().createDescriptorSet( 1u );
			auto & texLayout = pipeline.program->getTextureDescriptorLayout();
			auto writeBinding = [&gp, this]( uint32_t index, ashes::ImageLayout layout )
			{
				ashes::SamplerCRef sampler = std::ref( gp.getSampler() );
				ashes::TextureViewCRef view = std::cref( *gp.getViews()[index - MinBufferIndex] );
				return ashes::WriteDescriptorSet
				{
					index,
					0u,
					1u,
					ashes::DescriptorType::eCombinedImageSampler,
					{ { sampler, view, layout } }
				};
			};
			uint32_t index = MinBufferIndex;
			pipeline.textureWrites.push_back( writeBinding( index++, ashes::ImageLayout::eDepthStencilAttachmentOptimal ) );
			pipeline.textureWrites.push_back( writeBinding( index++, ashes::ImageLayout::eShaderReadOnlyOptimal ) );
			pipeline.textureWrites.push_back( writeBinding( index++, ashes::ImageLayout::eShaderReadOnlyOptimal ) );
			pipeline.textureWrites.push_back( writeBinding( index++, ashes::ImageLayout::eShaderReadOnlyOptimal ) );
			pipeline.textureWrites.push_back( writeBinding( index++, ashes::ImageLayout::eShaderReadOnlyOptimal ) );
			pipeline.textureWrites.push_back( writeBinding( index++, ashes::ImageLayout::eShaderReadOnlyOptimal ) );

			pipeline.textureDescriptorSet->setBindings( pipeline.textureWrites );
			pipeline.textureDescriptorSet->update();

			if ( m_shadows )
			{
				// Empty descriptor for shadow texture, that will be filled at runtime.
				pipeline.textureWrites.push_back(
					{
						index++,
						0u,
						1u,
						ashes::DescriptorType::eCombinedImageSampler,
						{ { ashes::nullopt, ashes::nullopt, ashes::ImageLayout::eShaderReadOnlyOptimal } }
					} );
				pipeline.textureDescriptorSet->setBindings( pipeline.textureWrites );
			}
			else
			{
				doPrepareCommandBuffer( pipeline, nullptr, 0u, true );
				doPrepareCommandBuffer( pipeline, nullptr, 0u, false );
			}

			++pipelineIndex;
		}
	}

	void LightPass::doCleanup()
	{
		m_matrixUbo.cleanup();
		m_commandBuffer.reset();

		for ( auto & pipeline : m_pipelines )
		{
			pipeline.textureDescriptorSet.reset();
			pipeline.uboDescriptorSet.reset();
			pipeline.blendCommandBuffer.reset();
			pipeline.firstCommandBuffer.reset();
			pipeline.program->cleanup();
			pipeline.program.reset();
		}
	}

	void LightPass::doPrepareCommandBuffer( Pipeline & pipeline
		, ShadowMap const * shadowMap
		, uint32_t shadowMapIndex
		, bool first )
	{
		auto & renderPass = first
			? m_firstRenderPass
			: m_blendRenderPass;
		auto & commandBuffer = first
			? *pipeline.firstCommandBuffer
			: *pipeline.blendCommandBuffer;
		auto & dimensions = renderPass.frameBuffer->getDimensions();

		if ( shadowMap )
		{
			ashes::WriteDescriptorSet & write = pipeline.textureDescriptorSet->getBinding( 6u );
			write.imageInfo[0].imageView = std::ref( shadowMap->getView( shadowMapIndex ) );
			write.imageInfo[0].sampler = std::ref( shadowMap->getSampler() );
			pipeline.textureDescriptorSet->update();
		}

		commandBuffer.begin( ashes::CommandBufferUsageFlag::eRenderPassContinue
			, ashes::CommandBufferInheritanceInfo
			{
				renderPass.renderPass.get(),
				0u,
				renderPass.frameBuffer.get(),
				false,
				0u,
				0u
			} );
		commandBuffer.setViewport( { { 0, 0 }, dimensions } );
		commandBuffer.setScissor( { { 0, 0 }, dimensions } );
		commandBuffer.bindDescriptorSets( { *pipeline.uboDescriptorSet, *pipeline.textureDescriptorSet }, pipeline.program->getPipelineLayout() );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		pipeline.program->render( commandBuffer, getCount(), first, m_offset );
		commandBuffer.end();
	}
	
	ShaderPtr LightPass::doGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool volumetric )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *m_engine.getRenderSystem();

		// Shader inputs
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto index = MinBufferIndex;
		auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
		auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
		auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
		auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
		auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
		auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
		auto in = writer.getIn();

		shadowType = m_shadows
			? shadowType
			: ShadowType::eNone;

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( cuT( "pxl_specular" ), 1 );

		// Utility functions
		auto lighting = lightType == LightType::eDirectional
			? shader::PhongLightingModel::createModel( writer
				, shadowType
				, volumetric
				, index
				, m_scene->getDirectionalShadowCascades() )
			: shader::PhongLightingModel::createModel( writer
				, lightType
				, shadowType
				, volumetric
				, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
		utils.declareCalcTexCoord();
		utils.declareCalcVSPosition();
		utils.declareCalcWSPosition();
		utils.declareDecodeReceiver();
		utils.declareInvertVec2Y();
		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::SssTransmittance sss{ writer
			, lighting->getShadowModel()
			, utils
			, m_shadows };
		sss.declare( lightType );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.bottomUpToTopDown( utils.calcTexCoord( in.gl_FragCoord.xy()
					, c3d_renderSize ) ) );
			auto data1 = writer.declLocale( cuT( "data1" )
				, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, textureLod( c3d_mapData2, texCoord, 0.0_f ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, textureLod( c3d_mapData3, texCoord, 0.0_f ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, textureLod( c3d_mapData4, texCoord, 0.0_f ) );
			auto data5 = writer.declLocale( cuT( "data5" )
				, textureLod( c3d_mapData5, texCoord, 0.0_f ) );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			utils.decodeReceiver( flags, shadowReceiver );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, writer.cast< UInt >( data5.z() ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, data2.xyz() );
			auto shininess = writer.declLocale( cuT( "shininess" )
				, data2.w() );
			auto specular = writer.declLocale( cuT( "specular" )
				, data3.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition.xyz() );
			auto depth = writer.declLocale( cuT( "depth" )
				, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto vsPosition = writer.declLocale( cuT( "vsPosition" )
				, utils.calcVSPosition( utils.bottomUpToTopDown( texCoord ), depth, c3d_mtxInvProj ) );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( utils.bottomUpToTopDown( texCoord ), depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( lightType )
			{
			case LightType::eDirectional:
				{
					auto c3d_light = writer.getVariable< shader::DirectionalLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#if !C3D_DisableSSSTransmittance
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency );
#endif
				}
				break;

			case LightType::ePoint:
				{
					auto c3d_light = writer.getVariable< shader::PointLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#if !C3D_DisableSSSTransmittance
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency );
#endif
				}
				break;

			case LightType::eSpot:
				{
					auto c3d_light = writer.getVariable< shader::SpotLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#if !C3D_DisableSSSTransmittance
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency );
#endif
				}
				break;
			}

			pxl_diffuse = lightDiffuse * diffuse;
			pxl_specular = lightSpecular * specular;

#if C3D_DebugEye
			pxl_diffuse = eye;
#elif C3D_DebugVSPosition
			pxl_diffuse = vsPosition;
#elif C3D_DebugWSPosition
			pxl_diffuse = wsPosition;
#elif C3D_DebugWSNormal
			pxl_diffuse = wsNormal;
#elif C3D_DebugTexCoord
			pxl_diffuse = vec3( texCoord, 1.0_f );
#endif
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}
	
	ShaderPtr LightPass::doGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool volumetric )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *m_engine.getRenderSystem();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto index = MinBufferIndex;
		auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
		auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
		auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
		auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
		auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
		auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
		auto in = writer.getIn();

		shadowType = m_shadows
			? shadowType
			: ShadowType::eNone;

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( cuT( "pxl_specular" ), 1 );

		// Utility functions
		auto lighting = lightType == LightType::eDirectional
			? shader::MetallicBrdfLightingModel::createModel( writer
				, shadowType
				, volumetric
				, index
				, m_scene->getDirectionalShadowCascades() )
			: shader::MetallicBrdfLightingModel::createModel( writer
				, lightType
				, shadowType
				, volumetric
				, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
		utils.declareCalcTexCoord();
		utils.declareCalcVSPosition();
		utils.declareCalcWSPosition();
		utils.declareDecodeReceiver();
		utils.declareInvertVec2Y();
		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::SssTransmittance sss{ writer
			, lighting->getShadowModel()
			, utils
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( lightType );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord( in.gl_FragCoord.xy()
					, c3d_renderSize ) );
			auto data1 = writer.declLocale( cuT( "data1" )
				, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, textureLod( c3d_mapData2, texCoord, 0.0_f ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, textureLod( c3d_mapData3, texCoord, 0.0_f ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, textureLod( c3d_mapData4, texCoord, 0.0_f ) );
			auto data5 = writer.declLocale( cuT( "data5" )
				, textureLod( c3d_mapData5, texCoord, 0.0_f ) );
			auto metallic = writer.declLocale( cuT( "metallic" )
				, data3.r() );
			auto roughness = writer.declLocale( cuT( "roughness" )
				, data3.g() );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			utils.decodeReceiver( flags, shadowReceiver );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, writer.cast< UInt >( data5.z() ) );
			auto albedo = writer.declLocale( cuT( "albedo" )
				, data2.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition.xyz() );
			auto depth = writer.declLocale( cuT( "depth" )
				, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto vsPosition = writer.declLocale( cuT( "vsPosition" )
				, utils.calcVSPosition( utils.bottomUpToTopDown( texCoord ), depth, c3d_mtxInvProj ) );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( utils.bottomUpToTopDown( texCoord ), depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( lightType )
			{
			case LightType::eDirectional:
				{
					auto c3d_light = writer.getVariable< shader::DirectionalLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
#if !C3D_DisableSSSTransmittance
#	if !C3D_DebugSSSTransmittance
					lighting->compute( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, transmittance );
#	else
					lightDiffuse = sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, transmittance );
#	endif
#else
					lighting->compute( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#endif
				}
				break;

			case LightType::ePoint:
				{
					auto c3d_light = writer.getVariable< shader::PointLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
#if !C3D_DisableSSSTransmittance
#	if !C3D_DebugSSSTransmittance
					lighting->compute( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, transmittance );
#	else
					lightDiffuse = sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, transmittance );
#	endif
#else
					lighting->compute( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#endif
				}
				break;

			case LightType::eSpot:
				{
					auto c3d_light = writer.getVariable< shader::SpotLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
#if !C3D_DisableSSSTransmittance
#	if !C3D_DebugSSSTransmittance
					lighting->compute( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, transmittance );
#	else
					lightDiffuse = sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, transmittance );
#	endif
#else
					lighting->compute( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#endif
				}
				break;
			}

			pxl_diffuse = lightDiffuse;
			pxl_specular = lightSpecular;

#if C3D_DebugEye
			pxl_diffuse = eye;
#elif C3D_DebugVSPosition
			pxl_diffuse = vsPosition;
#elif C3D_DebugWSPosition
			pxl_diffuse = wsPosition;
#elif C3D_DebugWSNormal
			pxl_diffuse = wsNormal;
#elif C3D_DebugTexCoord
			pxl_diffuse = vec3( texCoord, 1.0_f );
#endif
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}
	
	ShaderPtr LightPass::doGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
		, LightType lightType
		, ShadowType shadowType
		, bool volumetric )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *m_engine.getRenderSystem();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto index = MinBufferIndex;
		auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
		auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
		auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
		auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
		auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
		auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
		auto in = writer.getIn();

		shadowType = m_shadows
			? shadowType
			: ShadowType::eNone;

		// Utility functions
		auto lighting = lightType == LightType::eDirectional
			? shader::SpecularBrdfLightingModel::createModel( writer
				, shadowType
				, volumetric
				, index
				, m_scene->getDirectionalShadowCascades() )
			: shader::SpecularBrdfLightingModel::createModel( writer
				, lightType
				, shadowType
				, volumetric
				, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
		utils.declareCalcTexCoord();
		utils.declareCalcVSPosition();
		utils.declareCalcWSPosition();
		utils.declareDecodeReceiver();
		utils.declareInvertVec2Y();
		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::SssTransmittance sss{ writer
			, lighting->getShadowModel()
			, utils
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( lightType );

		// Shader outputs
		auto pxl_diffuse = writer.declOutput< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declOutput< Vec3 >( cuT( "pxl_specular" ), 1 );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.bottomUpToTopDown( utils.calcTexCoord( in.gl_FragCoord.xy()
					, c3d_renderSize ) ) );
			auto data1 = writer.declLocale( cuT( "data1" )
				, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, textureLod( c3d_mapData2, texCoord, 0.0_f ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, textureLod( c3d_mapData3, texCoord, 0.0_f ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, textureLod( c3d_mapData4, texCoord, 0.0_f ) );
			auto data5 = writer.declLocale( cuT( "data5" )
				, textureLod( c3d_mapData5, texCoord, 0.0_f ) );
			auto specular = writer.declLocale( cuT( "specular" )
				, data3.rgb() );
			auto glossiness = writer.declLocale( cuT( "glossiness" )
				, data2.a() );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			utils.decodeReceiver( flags, shadowReceiver );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, writer.cast< UInt >( data5.z() ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, data2.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition.xyz() );
			auto depth = writer.declLocale( cuT( "depth" )
				, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto vsPosition = writer.declLocale( cuT( "vsPosition" )
				, utils.calcVSPosition( utils.bottomUpToTopDown( texCoord ), depth, c3d_mtxInvProj ) );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( utils.bottomUpToTopDown( texCoord ), depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( lightType )
			{
			case LightType::eDirectional:
				{
					auto c3d_light = writer.getVariable< shader::DirectionalLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#if !C3D_DisableSSSTransmittance
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency );
#endif
				}
				break;

			case LightType::ePoint:
				{
					auto c3d_light = writer.getVariable< shader::PointLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#if !C3D_DisableSSSTransmittance
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency );
#endif
				}
				break;

			case LightType::eSpot:
				{
					auto c3d_light = writer.getVariable< shader::SpotLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( in.gl_FragCoord.xy(), vsPosition, wsPosition, wsNormal )
						, output );
#if !C3D_DisableSSSTransmittance
					lightDiffuse += sss.compute( material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency );
#endif
				}
				break;
			}

			pxl_diffuse = lightDiffuse;
			pxl_specular = lightSpecular;

#if C3D_DebugEye
			pxl_diffuse = eye;
#elif C3D_DebugVSPosition
			pxl_diffuse = vsPosition;
#elif C3D_DebugWSPosition
			pxl_diffuse = wsPosition;
#elif C3D_DebugWSNormal
			pxl_diffuse = wsNormal;
#elif C3D_DebugTexCoord
			pxl_diffuse = vec3( texCoord, 1.0_f );
#endif
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	//************************************************************************************************
}
