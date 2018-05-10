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
#include "Technique/Opaque//GeometryPassResult.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

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

#define C3D_DISABLE_SSSSS_TRANSMITTANCE 1

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

	renderer::Format getTextureFormat( DsTexture texture )
	{
		static std::array< renderer::Format, size_t( DsTexture::eCount ) > Values
		{
			{
				renderer::Format::eD24_UNORM_S8_UINT,
				renderer::Format::eR32G32B32A32_SFLOAT,
				renderer::Format::eR16G16B16A16_SFLOAT,
				renderer::Format::eR16G16B16A16_SFLOAT,
				renderer::Format::eR16G16B16A16_SFLOAT,
				renderer::Format::eR16G16B16A16_SFLOAT,
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
					REQUIRE( result >= constant );
					result = sqrtf( ( result - constant ) / quadratic );
				}
				else
				{
					auto delta = linear * linear - 4 * quadratic * ( constant - result );
					REQUIRE( delta >= 0 );
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

	uint32_t constexpr ReceiverMask		= 0x00000080u;
	uint32_t constexpr RefractionMask	= 0x00000040u;
	uint32_t constexpr ReflectionMask	= 0x00000020u;
	uint32_t constexpr EnvMapIndexMask	= 0x0000001Fu;

	uint32_t constexpr ReceiverOffset	= 7u;
	uint32_t constexpr RefractionOffset	= 6u;
	uint32_t constexpr ReflectionOffset = 5u;
	uint32_t constexpr EnvMapIndexOffset= 0u;

	void declareEncodeMaterial( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		using glsl::operator<<;
		writer.implementFunction< Void >( cuT( "encodeMaterial" )
			, [&]( Int const & receiver
				, Int const & reflection
				, Int const & refraction
				, Int const & envMapIndex
				, Float encoded )
			{
				auto flags = writer.declLocale( cuT( "flags" )
					, 0_ui
					+ writer.paren( writer.paren( writer.cast< UInt >( receiver )		<< UInt( ReceiverOffset ) )		& UInt( ReceiverMask ) )
					+ writer.paren( writer.paren( writer.cast< UInt >( refraction )		<< UInt( RefractionOffset ) )	& UInt( RefractionMask ) )
					+ writer.paren( writer.paren( writer.cast< UInt >( reflection )		<< UInt( ReflectionOffset ) )	& UInt( ReflectionMask ) )
					+ writer.paren( writer.paren( writer.cast< UInt >( envMapIndex )	<< UInt( EnvMapIndexOffset ) )	& UInt( EnvMapIndexMask ) ) );
				encoded = writer.cast< Float >( flags );
			}, InInt{ &writer, cuT( "receiver" ) }
			, InInt{ &writer, cuT( "reflection" ) }
			, InInt{ &writer, cuT( "refraction" ) }
			, InInt{ &writer, cuT( "envMapIndex" ) }
			, OutFloat{ &writer, cuT( "encoded" ) } );
	}
	
	void declareDecodeMaterial( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		writer.implementFunction< Void >( cuT( "decodeMaterial" )
			, [&]( Float const & encoded
				, Int receiver
				, Int reflection
				, Int refraction
				, Int envMapIndex )
			{
				auto flags = writer.declLocale( cuT( "flags" )
					, writer.cast< UInt >( encoded ) );
				receiver	= writer.cast< Int >( writer.paren( flags & UInt( ReceiverMask ) )		>> UInt( ReceiverOffset ) );
				refraction	= writer.cast< Int >( writer.paren( flags & UInt( RefractionMask ) )	>> UInt( RefractionOffset ) );
				reflection	= writer.cast< Int >( writer.paren( flags & UInt( ReflectionMask ) )	>> UInt( ReflectionOffset ) );
				envMapIndex	= writer.cast< Int >( writer.paren( flags & UInt( EnvMapIndexMask ) )	>> UInt( EnvMapIndexOffset ) );
			}, InFloat{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) }
			, OutInt{ &writer, cuT( "reflection" ) }
			, OutInt{ &writer, cuT( "refraction" ) }
			, OutInt{ &writer, cuT( "envMapIndex" ) } );
	}

	void declareDecodeReceiver( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		writer.implementFunction< Void >( cuT( "decodeReceiver" )
			, [&]( Int const & encoded
				, Int receiver )
			{
				receiver = writer.paren( encoded & ReceiverMask ) >> ReceiverOffset;
			}, InInt{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) } );
	}

	void encodeMaterial( glsl::GlslWriter & writer
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex
		, glsl::Float const & encoded )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "encodeMaterial" )
			, InInt{ receiver }
			, InInt{ reflection }
			, InInt{ refraction }
			, InInt{ envMapIndex }
			, OutFloat{ encoded } );
		writer << endi;
	}

	void decodeMaterial( glsl::GlslWriter & writer
		, glsl::Float const & encoded
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeMaterial" )
			, InFloat{ encoded }
			, OutInt{ receiver }
			, OutInt{ reflection }
			, OutInt{ refraction }
			, OutInt{ envMapIndex } );
		writer << endi;
	}

	void decodeReceiver( glsl::GlslWriter & writer
		, glsl::Int & encoded
		, glsl::Int const & receiver )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeReceiver" )
			, InInt{ encoded }
			, OutInt{ receiver } );
		writer << endi;
	}

	namespace
	{
		renderer::ShaderStageStateArray doCreateProgram( Engine & engine
			, glsl::Shader const & vtx
			, glsl::Shader const & pxl )
		{
			auto & device = *engine.getRenderSystem()->getCurrentDevice();
			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vtx.getSource() );
			program[1].module->loadShader( pxl.getSource() );
			return program;
		}
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl
		, bool hasShadows )
		: m_engine{ engine }
		, m_program{ ::doCreateProgram( engine, vtx, pxl ) }
		, m_shadows{ hasShadows }
	{
	}

	void LightPass::Program::initialise( renderer::VertexBufferBase & vbo
		, renderer::VertexLayout const & vertexLayout
		, renderer::RenderPass const & firstRenderPass
		, renderer::RenderPass const & blendRenderPass
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		renderer::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( m_engine.getMaterialCache().getPassBuffer().createLayoutBinding() );
		setLayoutBindings.emplace_back( MatrixUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( SceneUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( GpInfoUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );

		if ( modelMatrixUbo )
		{
			setLayoutBindings.emplace_back( ModelMatrixUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		}

		setLayoutBindings.emplace_back( shader::LightingModel::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
		m_uboDescriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_uboDescriptorPool = m_uboDescriptorLayout->createPool( 2u );

		setLayoutBindings = renderer::DescriptorSetLayoutBindingArray
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 3u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 4u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 5u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};

		if ( m_shadows )
		{
			setLayoutBindings.emplace_back( 6u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
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

	void LightPass::Program::render( renderer::CommandBuffer & commandBuffer
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

	LightPass::RenderPass::RenderPass( renderer::RenderPassPtr && renderPass
		, renderer::TextureView const & depthView
		, renderer::TextureView const & diffuseView
		, renderer::TextureView const & specularView )
		: renderPass{ std::move( renderPass ) }
	{
		renderer::FrameBufferAttachmentArray attaches
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
		, renderer::RenderPassPtr && firstRenderPass
		, renderer::RenderPassPtr && blendRenderPass
		, renderer::TextureView const & depthView
		, renderer::TextureView const & diffuseView
		, renderer::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: m_engine{ engine }
		, m_firstRenderPass{std::move( firstRenderPass ), depthView, diffuseView, specularView }
		, m_blendRenderPass{ std::move( blendRenderPass ), depthView, diffuseView, specularView }
		, m_shadows{ hasShadows }
		, m_matrixUbo{ engine }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_sampler{ engine.getDefaultSampler() }
		, m_signalReady{ engine.getRenderSystem()->getCurrentDevice()->createSemaphore() }
		, m_fence{ engine.getRenderSystem()->getCurrentDevice()->createFence( renderer::FenceCreateFlag::eSignaled ) }
	{
	}

	void LightPass::render( bool first
		, renderer::Semaphore const & toWait
		, TextureUnit * shadowMapOpt )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_fence->reset();

		if ( first )
		{
			device.getGraphicsQueue().submit( *m_firstRenderPass.commandBuffer
				, toWait
				, renderer::PipelineStageFlag::eVertexInput
				, *m_signalReady
				, m_fence.get() );
		}
		else
		{
			device.getGraphicsQueue().submit( *m_blendRenderPass.commandBuffer
				, toWait
				, renderer::PipelineStageFlag::eVertexInput
				, *m_signalReady
				, m_fence.get() );
		}

		m_fence->wait( renderer::FenceTimeout );
	}

	void LightPass::doInitialise( Scene const & scene
		, GeometryPassResult const & gp
		, LightType type
		, renderer::VertexBufferBase & vbo
		, renderer::VertexLayout const & vertexLayout
		, SceneUbo & sceneUbo
		, ModelMatrixUbo * modelMatrixUbo
		, RenderPassTimer & timer )
	{
		m_timer = &timer;
		m_geometryPassResult = &gp;
		SceneFlags sceneFlags{ scene.getFlags() };

		if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness )
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetPbrMRPixelShaderSource( sceneFlags, type ) );
		}
		else if ( scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetPbrSGPixelShaderSource( sceneFlags, type ) );
		}
		else
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetLegacyPixelShaderSource( sceneFlags, type ) );
		}

		m_program->initialise( vbo
			, vertexLayout
			, *m_firstRenderPass.renderPass
			, *m_blendRenderPass.renderPass
			, m_matrixUbo
			, sceneUbo
			, m_gpInfoUbo
			, modelMatrixUbo );

		m_uboDescriptorSet = m_program->getUboDescriptorPool().createDescriptorSet( 0u );
		auto & uboLayout = m_program->getUboDescriptorLayout();
		m_engine.getMaterialCache().getPassBuffer().createBinding( *m_uboDescriptorSet, uboLayout.getBinding( 0u ) );
		m_uboDescriptorSet->createBinding( uboLayout.getBinding( MatrixUbo::BindingPoint )
			, m_matrixUbo.getUbo() );
		m_uboDescriptorSet->createBinding( uboLayout.getBinding( SceneUbo::BindingPoint )
			, sceneUbo.getUbo() );
		m_uboDescriptorSet->createBinding( uboLayout.getBinding( GpInfoUbo::BindingPoint )
			, m_gpInfoUbo.getUbo() );
		if ( modelMatrixUbo )
		{
			m_uboDescriptorSet->createBinding( uboLayout.getBinding( ModelMatrixUbo::BindingPoint )
				, modelMatrixUbo->getUbo() );
		}
		m_uboDescriptorSet->createBinding( uboLayout.getBinding( shader::LightingModel::UboBindingPoint )
			, *m_baseUbo
			, 0u
			, m_baseUbo->getElementSize() );
		m_uboDescriptorSet->update();

		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_firstRenderPass.commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_blendRenderPass.commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );

		m_textureDescriptorSet = m_program->getTextureDescriptorPool().createDescriptorSet( 1u );
		auto & texLayout = m_program->getTextureDescriptorLayout();
		auto writeBinding = [&gp, this]( uint32_t index, renderer::ImageLayout layout )
		{
			renderer::SamplerCRef sampler = std::ref( gp.getSampler() );
			renderer::TextureViewCRef view = std::ref( *gp.getViews()[index] );
			return renderer::WriteDescriptorSet
			{
				index,
				0u,
				1u,
				renderer::DescriptorType::eCombinedImageSampler,
				{ { sampler, view, layout } }
			};
		};
		m_textureWrites.push_back( writeBinding( 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal ) );
		m_textureWrites.push_back( writeBinding( 1u, renderer::ImageLayout::eShaderReadOnlyOptimal ) );
		m_textureWrites.push_back( writeBinding( 2u, renderer::ImageLayout::eShaderReadOnlyOptimal ) );
		m_textureWrites.push_back( writeBinding( 3u, renderer::ImageLayout::eShaderReadOnlyOptimal ) );
		m_textureWrites.push_back( writeBinding( 4u, renderer::ImageLayout::eShaderReadOnlyOptimal ) );
		m_textureWrites.push_back( writeBinding( 5u, renderer::ImageLayout::eShaderReadOnlyOptimal ) );

		m_textureDescriptorSet->setBindings( m_textureWrites );
		m_textureDescriptorSet->update();

		if ( m_shadows )
		{
			// Empty descriptor for shadow texture, that will be filled at runtime.
			m_textureWrites.push_back(
				{
					6u,
					0u,
					1u,
					renderer::DescriptorType::eCombinedImageSampler,
					{ { std::nullopt, std::nullopt, renderer::ImageLayout::eShaderReadOnlyOptimal } }
				} );
			m_textureDescriptorSet->setBindings( m_textureWrites );
		}
		else
		{
			doPrepareCommandBuffer( nullptr, true );
			doPrepareCommandBuffer( nullptr, false );
		}
	}

	void LightPass::doCleanup()
	{
		m_matrixUbo.cleanup();
		m_textureDescriptorSet.reset();
		m_uboDescriptorSet.reset();
		m_blendRenderPass.commandBuffer.reset();
		m_firstRenderPass.commandBuffer.reset();
		m_program->cleanup();
		m_program.reset();
	}

	void LightPass::doPrepareCommandBuffer( TextureUnit const * shadowMap
		, bool first )
	{
		static renderer::DepthStencilClearValue const clearDepthStencil{ 1.0, 1 };
		static renderer::ClearColorValue const clearColour{ 0.0, 0.0, 0.0, 1.0 };
		auto & renderPass = first
			? m_firstRenderPass
			: m_blendRenderPass;
		uint32_t const width = renderPass.frameBuffer->getDimensions().width;
		uint32_t const height = renderPass.frameBuffer->getDimensions().height;
		renderer::CommandBufferUsageFlag usage{};

		if ( shadowMap )
		{
			renderer::WriteDescriptorSet & write = m_textureDescriptorSet->getBinding( 6u );
			write.imageInfo[0].imageView = std::ref( shadowMap->getTexture()->getDefaultView() );
			write.imageInfo[0].sampler = std::ref( shadowMap->getSampler()->getSampler() );
			m_textureDescriptorSet->update();
			usage = renderer::CommandBufferUsageFlag::eOneTimeSubmit;
		}

		auto & commandBuffer = *renderPass.commandBuffer;

		if ( commandBuffer.begin( usage ) )
		{
			commandBuffer.resetQueryPool( m_timer->getQuery()
				, 0u
				, 2u );
			commandBuffer.writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_timer->getQuery()
				, 0u );
			commandBuffer.beginRenderPass( *renderPass.renderPass
				, *renderPass.frameBuffer
				, { clearDepthStencil, clearColour, clearColour }
				, renderer::SubpassContents::eInline );
			commandBuffer.setViewport( { width, height, 0, 0 } );
			commandBuffer.setScissor( { 0, 0, width, height } );
			commandBuffer.bindDescriptorSets( { *m_uboDescriptorSet, *m_textureDescriptorSet }, m_program->getPipelineLayout() );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_program->render( commandBuffer, getCount(), first, m_offset );
			commandBuffer.endRenderPass();
			commandBuffer.writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_timer->getQuery()
				, 1u );
			commandBuffer.end();
		}
	}
	
	glsl::Shader LightPass::doGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto index = 0u;
		auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
		auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
		auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
		auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
		auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
		auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto shadowType = getShadowType( sceneFlags );

		// Shader outputs
		auto pxl_diffuse = writer.declFragData< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declFragData< Vec3 >( cuT( "pxl_specular" ), 1 );

		// Utility functions
		auto lighting = shader::legacy::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone
			, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		utils.declareCalcVSPosition();
		declareDecodeReceiver( writer );
		shader::LegacyMaterials materials{ writer };
		materials.declare();
		shader::SssTransmittance sss{ writer
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( type );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord( c3d_renderSize ) );
			auto data1 = writer.declLocale( cuT( "data1" )
				, texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, texture( c3d_mapData4, texCoord ) );
			auto data5 = writer.declLocale( cuT( "data5" )
				, texture( c3d_mapData5, texCoord ) );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			decodeReceiver( writer, flags, shadowReceiver );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, writer.cast< Int >( data5.z() ) );
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
				, texture( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto c3d_light = writer.getBuiltin< shader::DirectionalLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
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
					auto c3d_light = writer.getBuiltin< shader::PointLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
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
					auto c3d_light = writer.getBuiltin< shader::SpotLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
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
		} );

		return writer.finalise();
	}
	
	glsl::Shader LightPass::doGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto index = 0u;
		auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
		auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
		auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
		auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
		auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
		auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto shadowType = getShadowType( sceneFlags );

		// Shader outputs
		auto pxl_diffuse = writer.declFragData< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declFragData< Vec3 >( cuT( "pxl_specular" ), 1 );

		// Utility functions
		auto lighting = shader::pbr::mr::createLightingModel( writer
			, type
			, m_shadows ? shadowType : ShadowType::eNone
			, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		declareDecodeReceiver( writer );
		shader::PbrMRMaterials materials{ writer };
		materials.declare();
		shader::SssTransmittance sss{ writer
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( type );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord( c3d_renderSize ) );
			auto data1 = writer.declLocale( cuT( "data1" )
				, texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, texture( c3d_mapData4, texCoord ) );
			auto data5 = writer.declLocale( cuT( "data5" )
				, texture( c3d_mapData5, texCoord ) );
			auto metallic = writer.declLocale( cuT( "metallic" )
				, data3.r() );
			auto roughness = writer.declLocale( cuT( "roughness" )
				, data3.g() );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			decodeReceiver( writer, flags, shadowReceiver );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, writer.cast< Int >( data5.z() ) );
			auto albedo = writer.declLocale( cuT( "albedo" )
				, data2.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition.xyz() );
			auto depth = writer.declLocale( cuT( "depth" )
				, texture( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto c3d_light = writer.getBuiltin< shader::DirectionalLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
#	if !C3D_DEBUG_SSS_TRANSMITTANCE
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
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#endif
				}
				break;

			case LightType::ePoint:
				{
					auto c3d_light = writer.getBuiltin< shader::PointLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
#	if !C3D_DEBUG_SSS_TRANSMITTANCE
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
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#endif
				}
				break;

			case LightType::eSpot:
				{
					auto c3d_light = writer.getBuiltin< shader::SpotLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
#	if !C3D_DEBUG_SSS_TRANSMITTANCE
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
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#endif
				}
				break;
			}

			pxl_diffuse = lightDiffuse;
			pxl_specular = lightSpecular;
		} );

		return writer.finalise();
	}
	
	glsl::Shader LightPass::doGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto index = 0u;
		auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
		auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
		auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
		auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
		auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
		auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto shadowType = getShadowType( sceneFlags );

		// Utility functions
		auto lighting = shader::pbr::sg::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone
			, index );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		shader::PbrSGMaterials materials{ writer };
		materials.declare();
		declareDecodeReceiver( writer );
		shader::SssTransmittance sss{ writer
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( type );

		// Shader outputs
		auto pxl_diffuse = writer.declFragData< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declFragData< Vec3 >( cuT( "pxl_specular" ), 1 );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord( c3d_renderSize ) );
			auto data1 = writer.declLocale( cuT( "data1" )
				, texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, texture( c3d_mapData4, texCoord ) );
			auto data5 = writer.declLocale( cuT( "data5" )
				, texture( c3d_mapData5, texCoord ) );
			auto specular = writer.declLocale( cuT( "specular" )
				, data3.rgb() );
			auto glossiness = writer.declLocale( cuT( "glossiness" )
				, data2.a() );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			decodeReceiver( writer
				, flags
				, shadowReceiver );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, writer.cast< Int >( data5.z() ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, data2.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition.xyz() );
			auto depth = writer.declLocale( cuT( "depth" )
				, texture( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto c3d_light = writer.getBuiltin< shader::DirectionalLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
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
					auto c3d_light = writer.getBuiltin< shader::PointLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
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
					auto c3d_light = writer.getBuiltin< shader::SpotLight >( cuT( "c3d_light" ) );
					auto light = writer.declLocale( cuT( "light" ), c3d_light );
					lighting->compute( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
#if !C3D_DISABLE_SSSSS_TRANSMITTANCE
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
		} );

		return writer.finalise();
	}

	//************************************************************************************************
}
