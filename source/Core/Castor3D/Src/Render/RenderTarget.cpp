#include "RenderTarget.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "HDR/ToneMapping.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Image.hpp>

#if !defined( NDEBUG )
#	define DISPLAY_DEBUG 1
#else
#	define DISPLAY_DEBUG 0
#endif

using namespace castor;

namespace castor3d
{
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
			result = file.print( 256, ( m_tabs + cuT( "\tsize %d %d\n" ) ).c_str(), target.getSize().getWidth(), target.getSize().getHeight() ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget size" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tformat " ) + castor::PF::getFormatName( target.getPixelFormat() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget format" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttone_mapping \"" ) + target.m_toneMapping->getName() + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget tone mapping" );
		}

		if ( result
			&& target.m_renderTechnique
			&& target.m_renderTechnique->isMultisampling() )
		{
			result = file.writeText( m_tabs + cuT( "\t" ) )
				&& target.m_renderTechnique->writeInto( file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget technique" );
		}

		if ( result )
		{
			for ( auto const & effect : target.m_postEffects )
			{
				result = file.writeText( m_tabs + cuT( "\tpostfx \"" ) + effect->getName() + cuT( "\"" ) )
						   && effect->writeInto( file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< RenderTarget >::checkError( result, "RenderTarget post effect" );
			}
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
		: m_renderTarget{ renderTarget }
		, m_colourTexture{ *renderTarget.getEngine() }
	{
	}

	bool RenderTarget::TargetFbo::initialise( uint32_t index, Size const & size )
	{
		m_frameBuffer = m_renderTarget.getEngine()->getRenderSystem()->createFrameBuffer();
		m_frameBuffer->initialise();

		SamplerSPtr sampler = m_renderTarget.getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( m_renderTarget.m_index ) );
		auto texture = m_renderTarget.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, m_renderTarget.getPixelFormat()
			, size );
		m_colourAttach = m_frameBuffer->createAttachment( texture );
		m_colourTexture.setTexture( texture );
		m_colourTexture.setSampler( sampler );
		m_colourTexture.setIndex( index );
		m_colourTexture.getTexture()->getImage().initialiseSource();
		m_colourTexture.getTexture()->initialise();

		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture.getTexture()->getType() );
		m_frameBuffer->setDrawBuffers();
		bool result = m_frameBuffer->isComplete();
		REQUIRE( result );
		m_frameBuffer->unbind();

		return result;
	}

	void RenderTarget::TargetFbo::cleanup()
	{
		m_frameBuffer->bind();
		m_frameBuffer->detachAll();
		m_frameBuffer->unbind();
		m_frameBuffer->cleanup();
		m_colourTexture.cleanup();
		m_colourAttach.reset();
		m_frameBuffer.reset();
	}

	//*************************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & engine, TargetType type )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_pixelFormat{ PixelFormat::eA8R8G8B8 }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_index{ ++sm_uiCount }
		, m_frameBuffer{ *this }
		, m_velocityTexture{ engine }
	{
		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( cuT( "linear" ), *getEngine(), Parameters{} );
		SamplerSPtr sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) );
		sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );

		sampler = getEngine()->getSamplerCache().add( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) );
		sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
		sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::initialise( uint32_t index )
	{
		if ( !m_initialised )
		{
			m_frameBuffer.initialise( index, m_size );

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

			m_size = m_frameBuffer.m_colourTexture.getTexture()->getDimensions();
			m_renderTechnique->initialise( index );

			auto velocityTexture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eRead
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eAL16F32F
				, m_size );
			m_velocityTexture.setTexture( velocityTexture );
			m_velocityTexture.setSampler( getEngine()->getSamplerCache().find( RenderTarget::DefaultSamplerName + string::toString( m_index ) + cuT( "_Point" ) ) );
			m_velocityTexture.getTexture()->getImage().initialiseSource();
			m_velocityTexture.getTexture()->initialise();

			m_postPostFxTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "sRGB Post effects" ), cuT( "sRGB Post effects" ) );
			m_overlaysTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Overlays" ), cuT( "Overlays" ) );

			for ( auto effect : m_postEffects )
			{
				effect->initialise();
			}

			m_initialised = m_toneMapping->initialise();

			for ( auto effect : m_postPostEffects )
			{
				effect->initialise();
			}
		}
	}

	void RenderTarget::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;

			for ( auto effect : m_postPostEffects )
			{
				effect->cleanup();
			}

			m_toneMapping->cleanup();
			m_toneMapping.reset();

			for ( auto effect : m_postEffects )
			{
				effect->cleanup();
			}

			m_postEffects.clear();
			m_postPostEffects.clear();

			m_postPostFxTimer.reset();
			m_overlaysTimer.reset();

			m_velocityTexture.cleanup();
			m_velocityTexture.setTexture( nullptr );

			m_renderTechnique->cleanup();
			m_frameBuffer.cleanup();
			m_renderTechnique.reset();
			getEngine()->getRenderTechniqueCache().remove( cuT( "RenderTargetTechnique_" ) + string::toString( m_index ) );
		}
	}

	void RenderTarget::render( RenderInfo & info )
	{
		SceneSPtr scene = getScene();

		if ( scene )
		{
			if ( m_initialised && scene->isInitialised() )
			{
				CameraSPtr pCamera = getCamera();

				if ( pCamera )
				{
					scene->getGeometryCache().fillInfo( info );
					doRender( info, m_frameBuffer, getCamera() );
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
		m_camera = camera;
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

		m_toneMapping = getEngine()->getRenderTargetCache().getToneMappingFactory().create( name, *getEngine(), parameters );
	}

	void RenderTarget::addPostEffect( PostEffectSPtr effect )
	{
		if ( effect->isAfterToneMapping() )
		{
			m_postPostEffects.push_back( effect );
		}
		else
		{
			m_postEffects.push_back( effect );
		}
	}

	void RenderTarget::setSize( Size const & size )
	{
		m_size = size;
	}

	void RenderTarget::addTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.add( parameters );
	}

	void RenderTarget::doRender( RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		SceneSPtr scene = getScene();
		fbo.m_frameBuffer->setClearColour( RgbaColour::fromRGBA( toRGBAFloat( scene->getBackgroundColour() ) ) );

		// Render the scene through the RenderTechnique.
		getEngine()->getMaterialCache().getPassBuffer().bind();
		m_renderTechnique->render( m_jitter
			, m_velocityTexture
			, info );

		// Then draw the render's result to the RenderTarget's frame buffer.
		fbo.m_frameBuffer->bind( FrameBufferTarget::eDraw );
		fbo.m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_toneMapping->update( scene->getHdrConfig() );
		m_toneMapping->apply( getSize()
			, m_renderTechnique->getResult()
			, info );
		fbo.m_frameBuffer->unbind();
		m_postPostFxTimer->start();

		for ( auto & effect : m_postPostEffects )
		{
			effect->apply( *fbo.m_frameBuffer );
		}

		m_postPostFxTimer->stop();

		// We also render overlays.
		fbo.m_frameBuffer->bind();
		getEngine()->getMaterialCache().getPassBuffer().bind();
		m_overlaysTimer->start();
		getEngine()->getOverlayCache().render( *scene, m_size );
		m_overlaysTimer->stop();

#if DISPLAY_DEBUG

		camera->apply();
		m_renderTechnique->debugDisplay( Size{ camera->getWidth(), camera->getHeight() } );

#endif

		fbo.m_frameBuffer->unbind();
	}
}
