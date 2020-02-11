#include "Castor3D/Render/RenderTarget.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ShaderWriter/Source.hpp>

#if !defined( NDEBUG )
#	define DISPLAY_DEBUG 1
#else
#	define DISPLAY_DEBUG 0
#endif

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr OverlayImgIdx = 0u;
		static uint32_t constexpr ObjectImgIdx = 1u;
	}

	//*************************************************************************************************

	RenderTarget::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< RenderTarget >{ tabs }
	{
	}

	bool RenderTarget::TextWriter::operator()( RenderTarget const & target, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing RenderTarget" ) );
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget name" );

		if ( result && target.getScene() )
		{
			result = file.writeText( m_tabs + cuT( "\tscene \"" ) + target.getScene()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget scene" );
		}

		if ( result && target.getCamera() )
		{
			result = file.writeText( m_tabs + cuT( "\tcamera \"" ) + target.getCamera()->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget camera" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tsize " )
				+ string::toString( target.getSize().getWidth(), std::locale{ "C" } ) + cuT( " " )
				+ string::toString( target.getSize().getHeight(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget size" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tformat " ) + PF::getFormatName( convert( target.getPixelFormat() ) ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget format" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttone_mapping \"" ) + target.m_toneMapping->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget tone mapping" );
		}

		if ( result )
		{
			for ( auto const & effect : target.m_hdrPostEffects )
			{
				result = effect->writeInto( file, m_tabs + cuT( "\t" ) ) && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			for ( auto const & effect : target.m_srgbPostEffects )
			{
				result = effect->writeInto( file, m_tabs + cuT( "\t" ) ) && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			result = HdrConfig::TextWriter( m_tabs + cuT( "\t" ) )( target.getHdrConfig(), file );
		}

		if ( result )
		{
			result = SsaoConfig::TextWriter{ m_tabs + cuT( "\t" ) }( target.m_ssaoConfig, file );
		}

		file.writeText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	RenderTarget::TargetFbo::TargetFbo( RenderTarget & renderTarget )
		: renderTarget{ renderTarget }
		, colourTexture{ *renderTarget.getEngine() }
	{
	}

	bool RenderTarget::TargetFbo::initialise( ashes::RenderPass & renderPass
		, VkFormat format
		, Size const & size )
	{
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();

		SamplerSPtr sampler = renderTarget.getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( renderTarget.m_index ) );
		ashes::ImageCreateInfo createInfo
		{
			0u,
			VK_IMAGE_TYPE_2D,
			format,
			{
				size.getWidth(),
				size.getHeight(),
				1u,
			},
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		};
		auto texture = std::make_shared< TextureLayout >( renderSystem
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "RenderTarget_Colour" ) );
		colourTexture.setTexture( texture );
		colourTexture.setSampler( sampler );
		colourTexture.getTexture()->getDefaultImage().initialiseSource();
		colourTexture.getTexture()->initialise();

		ashes::ImageViewCRefArray attaches;
		attaches.emplace_back( colourTexture.getTexture()->getDefaultView() );
		frameBuffer = renderPass.createFrameBuffer( VkExtent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );

		return true;
	}

	void RenderTarget::TargetFbo::cleanup()
	{
		colourTexture.cleanup();
		frameBuffer.reset();
	}

	//*************************************************************************************************

	RenderTarget::CombineQuad::CombineQuad( RenderSystem & renderSystem
		, ashes::ImageView const & ovView )
		: RenderQuad{ renderSystem, true }
		, m_ovView{ ovView }
	{
	}

	void RenderTarget::CombineQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_ovView
			, m_sampler->getSampler() );
	}

	//*************************************************************************************************
	
	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine, TargetType type )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_pixelFormat{ VK_FORMAT_R8G8B8A8_UNORM }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_objectsFrameBuffer{ *this }
		, m_overlaysFrameBuffer{ *this }
		, m_combinedFrameBuffer{ *this }
		, m_velocityTexture{ engine }
	{
		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( cuT( "linear" )
			, *getEngine()
			, m_hdrConfig
			, Parameters{} );
		SamplerSPtr sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) );
		sampler->setMinFilter( VK_FILTER_LINEAR );
		sampler->setMagFilter( VK_FILTER_LINEAR );

		sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) );
		sampler->setMinFilter( VK_FILTER_NEAREST );
		sampler->setMagFilter( VK_FILTER_NEAREST );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::initialise()
	{
		if ( !m_initialised )
		{
			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
			auto & renderSystem = *getEngine()->getRenderSystem();
			auto & device = getCurrentRenderDevice( renderSystem );
			doInitialiseRenderPass();
			m_initialised = doInitialiseFrameBuffer();

			if ( m_initialised )
			{
				m_initialised = doInitialiseVelocityTexture();
			}

			if ( m_initialised )
			{
				m_initialised = doInitialiseTechnique();
			}

			m_overlaysTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Overlays" ), cuT( "Overlays" ) );
			m_toneMappingTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Tone Mapping" ), cuT( "Tone Mapping" ) );

			if ( !m_hdrPostEffects.empty() )
			{
				auto const * sourceView = &m_renderTechnique->getResult();

				for ( auto effect : m_hdrPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( *sourceView );
						sourceView = &effect->getResult();
					}
				}

				doInitialiseCopyCommands( m_hdrCopyCommands
					, sourceView->getImage().getView()
					, m_renderTechnique->getResult().getDefaultView() );
				m_hdrCopyFinished = device->createSemaphore();
			}

			if ( m_initialised )
			{
				m_initialised = doInitialiseToneMapping();
			}

			if ( !m_srgbPostEffects.empty() )
			{
				auto const * sourceView = m_objectsFrameBuffer.colourTexture.getTexture().get();

				for ( auto effect : m_srgbPostEffects )
				{
					if ( m_initialised )
					{
						m_initialised = effect->initialise( *sourceView );
						sourceView = &effect->getResult();
					}
				}

				doInitialiseCopyCommands( m_srgbCopyCommands
					, sourceView->getDefaultView()
					, m_objectsFrameBuffer.colourTexture.getTexture()->getDefaultView() );
				m_srgbCopyFinished = device->createSemaphore();
			}

			doInitialiseFlip();
			m_overlayRenderer = std::make_shared< OverlayRenderer >( *getEngine()->getRenderSystem()
				, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView() );
			m_overlayRenderer->initialise();

			m_signalReady = device->createSemaphore();
		}
	}

	void RenderTarget::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_culler.reset();

			m_signalReady.reset();

			m_combineQuad.reset();
			m_combineCommands.reset();
			m_combineFinished.reset();

			m_toneMappingCommandBuffer.reset();

			if ( m_overlayRenderer )
			{
				m_overlayRenderer->cleanup();
				m_overlayRenderer.reset();
			}

			for ( auto effect : m_srgbPostEffects )
			{
				effect->cleanup();
			}

			if ( m_toneMapping )
			{
				m_toneMapping->cleanup();
				m_toneMapping.reset();
			}

			for ( auto effect : m_hdrPostEffects )
			{
				effect->cleanup();
			}

			m_hdrCopyCommands.reset();
			m_srgbCopyCommands.reset();
			m_hdrCopyFinished.reset();
			m_srgbCopyFinished.reset();
			m_hdrPostEffects.clear();
			m_srgbPostEffects.clear();

			m_overlaysTimer.reset();
			m_toneMappingTimer.reset();

			m_velocityTexture.cleanup();
			m_velocityTexture.setTexture( nullptr );

			m_renderTechnique->cleanup();
			m_overlaysFrameBuffer.cleanup();
			m_objectsFrameBuffer.cleanup();
			m_combinedFrameBuffer.cleanup();
			m_renderTechnique.reset();
			m_renderPass.reset();
			getEngine()->getRenderTechniqueCache().remove( cuT( "RenderTargetTechnique_" ) + string::toString( m_index ) );
		}
	}

	void RenderTarget::update()
	{
		auto & camera = *getCamera();
		camera.resize( m_size );
		camera.update();

		CU_Require( m_culler );
		m_culler->compute();
	}

	void RenderTarget::render( RenderInfo & info )
	{
		SceneSPtr scene = getScene();

		if ( scene )
		{
			if ( m_initialised && scene->isInitialised() )
			{
				CameraSPtr camera = getCamera();

				if ( camera )
				{
					getEngine()->getRenderSystem()->pushScene( scene.get() );
					scene->getGeometryCache().fillInfo( info );
					doRender( info, m_objectsFrameBuffer, getCamera() );
					getEngine()->getRenderSystem()->popScene();
				}
			}
		}
	}

	ViewportType RenderTarget::getViewportType()const
	{
		return ( getCamera() ? getCamera()->getViewportType() : ViewportType::eCount );
	}

	void RenderTarget::setViewportType( ViewportType value )
	{
		if ( getCamera() )
		{
			getCamera()->setViewportType( value );
		}
	}

	void RenderTarget::setCamera( CameraSPtr camera )
	{
		auto myCamera = getCamera();

		if ( myCamera != camera )
		{
			m_camera = camera;
			camera->resize( m_size );
			m_culler = std::make_unique< FrustumCuller >( *getScene(), *getCamera() );
		}
	}

	void RenderTarget::setScene( SceneSPtr scene )
	{
		auto myScene = getScene();

		if ( myScene != scene )
		{
			m_scene = scene;
			m_culler.reset();
		}
	}

	void RenderTarget::setToneMappingType( String const & name
		, Parameters const & parameters )
	{
		if ( m_toneMapping )
		{
			ToneMappingSPtr toneMapping;
			std::swap( m_toneMapping, toneMapping );
			// Give ownership of the tone mapping to the event (capture by value in the lambda).
			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, [toneMapping]()
			{
				toneMapping->cleanup();
			} ) );
		}

		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( name
			, *getEngine()
			, m_hdrConfig
			, parameters );
	}

	void RenderTarget::addPostEffect( PostEffectSPtr effect )
	{
		if ( effect->isAfterToneMapping() )
		{
			m_srgbPostEffects.push_back( effect );
		}
		else
		{
			m_hdrPostEffects.push_back( effect );
		}
	}

	void RenderTarget::setSize( Size const & size )
	{
		m_size = size;
		auto camera = getCamera();

		if ( camera )
		{
			camera->resize( m_size );
		}
	}

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::doInitialiseRenderPass()
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			}
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
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
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
		setDebugObjectName( device, *m_renderPass, "RenderTargetRenderPass" );
	}

	bool RenderTarget::doInitialiseFrameBuffer()
	{
		auto result = m_objectsFrameBuffer.initialise( *m_renderPass
			, VK_FORMAT_R8G8B8A8_UNORM
			, m_size );
		auto & dimensions = m_objectsFrameBuffer.colourTexture.getTexture()->getDimensions();
		m_size.set( dimensions.width, dimensions.height );

		if ( result )
		{
			result = m_overlaysFrameBuffer.initialise( *m_renderPass
				, VK_FORMAT_R8G8B8A8_UNORM
				, m_size );
		}

		if ( result )
		{
			result = m_combinedFrameBuffer.initialise( *m_renderPass
				, getPixelFormat()
				, m_size );
		}

		return result;
	}

	bool RenderTarget::doInitialiseVelocityTexture()
	{
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_R16G16B16A16_SFLOAT,
			{ m_size[0], m_size[1], 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		};
		auto velocityTexture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "RenderTargetVelocity" ) );
		m_velocityTexture.setTexture( std::move( velocityTexture ) );
		m_velocityTexture.setSampler( getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) ) );
		m_velocityTexture.getTexture()->getDefaultImage().initialiseSource();
		return m_velocityTexture.initialise();
	}

	bool RenderTarget::doInitialiseTechnique()
	{
		if ( !m_renderTechnique )
		{
			try
			{
				auto name = cuT( "RenderTargetTechnique_" ) + string::toString( m_index );
				m_renderTechnique = getEngine()->getRenderTechniqueCache().add( name
					, std::make_shared< RenderTechnique >( name
						, *this
						, *getEngine()->getRenderSystem()
						, m_techniqueParameters
						, m_ssaoConfig ) );
			}
			catch ( Exception & p_exc )
			{
				Logger::logError( cuT( "Couldn't load render technique: " ) + string::stringCast< xchar >( p_exc.getFullDescription() ) );
				throw;
			}
		}

		return m_renderTechnique->initialise();
	}

	bool RenderTarget::doInitialiseToneMapping()
	{
		auto & device = getCurrentRenderDevice( *this );
		auto result = m_toneMapping->initialise( getSize()
			, m_renderTechnique->getResult()
			, *m_renderPass );

		m_toneMappingCommandBuffer = device.graphicsCommandPool->createCommandBuffer();
		auto const clear{ makeClearValue( 0.0f, 0.0f, 1.0f, 1.0f ) };

		if ( result )
		{
			m_toneMappingCommandBuffer->begin();
			m_toneMappingCommandBuffer->beginDebugBlock(
				{
					"Tone Mapping",
					{
						1.0f,
						0.0f,
						1.0f,
						1.0f,
					},
				} );
			m_toneMappingTimer->beginPass( *m_toneMappingCommandBuffer );
			// Put render technique image in shader input layout.
			m_toneMappingCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_renderTechnique->getResult().getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			m_toneMappingCommandBuffer->beginRenderPass( *m_renderPass
				, *m_objectsFrameBuffer.frameBuffer
				, { clear }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_toneMapping->registerFrame( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->endRenderPass();
			// Put render technique image back in colour attachment layout.
			m_toneMappingCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, m_renderTechnique->getResult().getDefaultView().makeColourAttachment( VK_IMAGE_LAYOUT_UNDEFINED ) );
			m_toneMappingTimer->endPass( *m_toneMappingCommandBuffer );
			m_toneMappingCommandBuffer->endDebugBlock();
			m_toneMappingCommandBuffer->end();
		}

		return result;
	}

	void RenderTarget::doInitialiseCopyCommands( ashes::CommandBufferPtr & commandBuffer
		, ashes::ImageView const & source
		, ashes::ImageView const & target )
	{
		auto & device = getCurrentRenderDevice( *this );
		commandBuffer = device.graphicsCommandPool->createCommandBuffer();

		commandBuffer->begin();
		commandBuffer->beginDebugBlock(
			{
				"Image Copy",
				{
					1.0f,
					0.2f,
					1.0f,
					1.0f,
				},
			} );

		if ( &source != &target )
		{
			// Put source image in transfer source layout.
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, source.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			// Put target image in transfer destination layout.
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, target.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			// Copy source to target.
			commandBuffer->copyImage( source, target );
		}

		// Put target image in fragment shader input layout.
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, target.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		commandBuffer->endDebugBlock();
		commandBuffer->end();
	}

	void RenderTarget::doInitialiseFlip()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Combine" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_textureObjects = writer.declOutput< Vec2 >( cuT( "vtx_textureObjects" ), 0u );
			auto vtx_textureOverlays = writer.declOutput< Vec2 >( cuT( "vtx_textureOverlays" ), 1u );
			auto out = writer.getOut();

			shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					vtx_textureObjects = utils.topDownToBottomUp( uv );
					vtx_textureOverlays = uv;

					if ( getTargetType() != TargetType::eWindow )
					{
						vtx_textureObjects.y() = 1.0_f - vtx_textureObjects.y();
						vtx_textureOverlays.y() = 1.0_f - vtx_textureOverlays.y();
					}

					out.gl_out.gl_Position = vec4( position, 0.0_f, 1.0_f );
				} );
			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Combine" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapOverlays = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapOverlays" ), OverlayImgIdx, 0u );
			auto c3d_mapObjects = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapObjects" ), ObjectImgIdx, 0u );
			auto vtx_textureObjects = writer.declInput< Vec2 >( cuT( "vtx_textureObjects" ), 0u );
			auto vtx_textureOverlays = writer.declInput< Vec2 >( cuT( "vtx_textureOverlays" ), 1u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					auto overlayColor = writer.declLocale( cuT( "overlayColor" )
						, texture( c3d_mapOverlays, vtx_textureOverlays ) );
					auto objectsColor = writer.declLocale( cuT( "objectsColor" )
						, texture( c3d_mapObjects, vtx_textureObjects ) );
					objectsColor.rgb() *= 1.0_f - overlayColor.a();
					//overlayColor.rgb() *= overlayColor.a();
					pxl_fragColor = vec4( objectsColor.rgb() + overlayColor.rgb(), 1.0_f );
				} );
			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		auto & device = getCurrentRenderDevice( *this );
		auto program = ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};

		m_combineCommands = device.graphicsCommandPool->createCommandBuffer();
		m_combineFinished = device->createSemaphore();
		m_combineQuad = std::make_unique< CombineQuad >( *getEngine()->getRenderSystem()
			, m_overlaysFrameBuffer.colourTexture.getTexture()->getDefaultView() );
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( OverlayImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT )
		};
		m_combineQuad->createPipeline( { m_size.getWidth(), m_size.getHeight() }
			, Position{}
			, std::move( program )
			, m_objectsFrameBuffer.colourTexture.getTexture()->getDefaultView()
			, *m_renderPass
			, std::move( bindings )
			, {} );

		m_combineCommands->begin();
		m_combineCommands->beginDebugBlock(
			{
				"Combine 3D-2D",
				{
					1.0f,
					0.4f,
					1.0f,
					1.0f,
				},
			} );
		m_combineCommands->beginRenderPass( *m_renderPass
			, *m_combinedFrameBuffer.frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		m_combineQuad->registerFrame( *m_combineCommands );
		m_combineCommands->endRenderPass();
		m_combineCommands->endDebugBlock();
		m_combineCommands->end();
	}

	void RenderTarget::doRender( RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		auto elapsedTime = m_timer.getElapsed();
		SceneSPtr scene = getScene();
		m_toneMapping->update();
		ashes::SemaphoreCRefArray signalsToWait;
		
		if ( m_type == TargetType::eWindow )
		{
			signalsToWait = scene->getRenderTargetsSemaphores();
		}

		// Render the scene through the RenderTechnique.
		m_signalFinished = &m_renderTechnique->render( m_jitter
			, signalsToWait
			, info );

		// Draw HDR post effects.
		m_signalFinished = &doApplyPostEffects( *m_signalFinished
			, m_hdrPostEffects
			, m_hdrCopyCommands
			, m_hdrCopyFinished
			, elapsedTime );

		// Then draw the render's result to the RenderTarget's frame buffer, applying the tone mapping operator.
		m_signalFinished = &doApplyToneMapping( *m_signalFinished );

		// Apply the sRGB post effects.
		m_signalFinished = &doApplyPostEffects( *m_signalFinished
			, m_srgbPostEffects
			, m_srgbCopyCommands
			, m_srgbCopyFinished
			, elapsedTime );

		// And now render overlays.
		m_signalFinished = &doRenderOverlays( *m_signalFinished, *camera );

		// Combine objects and overlays framebuffers, flipping them if necessary.
		m_signalFinished = &doCombine( *m_signalFinished );

#if DISPLAY_DEBUG

		m_renderTechnique->debugDisplay( Size{ camera->getWidth(), camera->getHeight() } );

#endif
	}

	ashes::Semaphore const & RenderTarget::doApplyPostEffects( ashes::Semaphore const & toWait
		, PostEffectPtrArray const & effects
		, ashes::CommandBufferPtr const & copyCommandBuffer
		, ashes::SemaphorePtr const & copyFinished
		, castor::Nanoseconds const & elapsedTime )
	{
		auto * result = &toWait;

		if ( !effects.empty() )
		{
			auto & device = getCurrentRenderDevice( *this );
			auto & queue = *device.graphicsQueue;

			for ( auto effect : effects )
			{
				auto timerBlock = effect->start();
				effect->update( elapsedTime );

				for ( auto & commands : effect->getCommands() )
				{
					effect->notifyPassRender();

					queue.submit( *commands.commandBuffer
						, *result
						, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
						, *commands.semaphore
						, nullptr );
					result = commands.semaphore.get();
				}
			}

			queue.submit( *copyCommandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *copyFinished
				, nullptr );
			result = copyFinished.get();
		}

		return *result;
	}

	ashes::Semaphore const & RenderTarget::doApplyToneMapping( ashes::Semaphore const & toWait )
	{
		auto & device = getCurrentRenderDevice( *this );
		auto & queue = *device.graphicsQueue;
		auto timerBlock = m_toneMappingTimer->start();
		m_toneMappingTimer->notifyPassRender();
		auto * result = &toWait;

		queue.submit( *m_toneMappingCommandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, m_toneMapping->getSemaphore()
			, nullptr );
		result = &m_toneMapping->getSemaphore();

		return *result;
	}

	ashes::Semaphore const & RenderTarget::doRenderOverlays( ashes::Semaphore const & toWait
		, Camera const & camera )
	{
		auto * result = &toWait;
		auto timerBlock = m_overlaysTimer->start();
		{
			using LockType = std::unique_lock< OverlayCache >;
			LockType lock{ makeUniqueLock( getEngine()->getOverlayCache() ) };
			m_overlayRenderer->beginPrepare( camera
				, *m_overlaysTimer
				, *result );
			auto preparer = m_overlayRenderer->getPreparer();

			for ( auto category : getEngine()->getOverlayCache() )
			{
				SceneSPtr scene = category->getOverlay().getScene();

				if ( category->getOverlay().isVisible()
					&& ( ( !scene && m_type == TargetType::eWindow )
						|| scene.get() == getScene().get() ) )
				{
					category->update( *m_overlayRenderer );
					category->accept( preparer );
				}
			}

			m_overlayRenderer->endPrepare( *m_overlaysTimer );
			m_overlayRenderer->render( *m_overlaysTimer );
			result = &m_overlayRenderer->getSemaphore();
		}
		return *result;
	}

	ashes::Semaphore const & RenderTarget::doCombine( ashes::Semaphore const & toWait )
	{
		auto & device = getCurrentRenderDevice( *this );
		auto & queue = *device.graphicsQueue;
		auto * result = &toWait;

		queue.submit( *m_combineCommands
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_combineFinished
			, nullptr );
		result = m_combineFinished.get();

		return *result;
	}
}
