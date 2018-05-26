#include "Engine.hpp"

#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"
#include "Overlay/DebugOverlays.hpp"
#include "Plugin/Plugin.hpp"
#include "Render/RenderLoopAsync.hpp"
#include "Render/RenderLoopSync.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderWindow.hpp"
#include "Scene/SceneFileParser.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/Sampler.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>
#include <Graphics/Image.hpp>
#include <Pool/UniqueObjectPool.hpp>

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call castor3d::Engine::loadRenderer before castor3d::Engine::Initialise";
	static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";

	Engine::Engine( castor::String const & appName
		, bool enableValidation )
		: Unique< Engine >( this )
		, m_renderSystem( nullptr )
		, m_cleaned( true )
		, m_perObjectLighting( true )
		, m_threaded( false )
		, m_materialType{ MaterialType::eLegacy }
		, m_appName{ appName }
		, m_enableValidation{ enableValidation }
	{
#if defined( NDEBUG )
		m_enableValidation = false;
#else
		Debug::initialise();
#endif

		auto dummy = []( auto element )
		{
		};
		auto eventInit = [this]( auto element )
		{
			this->postEvent( makeInitialiseEvent( *element ) );
		};
		auto eventClean = [this]( auto element )
		{
			this->postEvent( makeCleanupEvent( *element ) );
		};
		auto instantInit = [this]( auto element )
		{
			element->initialise();
		};
		auto instantClean = [this]( auto element )
		{
			element->cleanup();
		};
		auto listenerClean = [this]( auto element )
		{
			element->flush();
		};
		auto mergeResource = []( auto const & source
		   , auto & destination
		   , auto element )
		{
		};
		Image::initialiseImageLib();
		renderer::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logDebug( msg );
			}
			else
			{
				Logger::logDebugNoNL( msg );
			}
		} );
		renderer::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logInfo( msg );
			}
			else
			{
				Logger::logInfoNoNL( msg );
			}
		} );
		renderer::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logWarning( msg );
			}
			else
			{
				Logger::logWarningNoNL( msg );
			}
		} );
		renderer::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logError( msg );
			}
			else
			{
				Logger::logErrorNoNL( msg );
			}
		} );

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = makeCache< FrameListener, String >(	*this
			, []( String const & name )
			{
				return std::make_shared< FrameListener >( name );
			}
			, dummy
			, listenerClean
			, mergeResource );
		m_defaultListener = m_listenerCache->add( cuT( "Default" ) );

		m_shaderCache = makeCache( *this );
		m_samplerCache = makeCache< Sampler, String >(	*this
			, [this]( String const & name )
			{
				return std::make_shared< Sampler >( *this, name );
			}
			, eventInit
			, eventClean
			, mergeResource );
		m_materialCache = makeCache< Material, String >( *this
			, [this]( String const & name, MaterialType type )
			{
				return std::make_shared< Material >( name, *this, type );
			}
			, eventInit
			, eventClean
			, mergeResource );
		m_pluginCache = makeCache< Plugin, String >( *this
			, []( String const & name, PluginType type, castor::DynamicLibrarySPtr library )
			{
				return nullptr;
			} );
		m_overlayCache = makeCache< Overlay, String >(	*this
			, [this]( String const & name, OverlayType type, SceneSPtr scene, OverlaySPtr parent )
			{
				auto result = std::make_shared< Overlay >( *this, type, scene, parent );
				result->setName( name );
				return result;
			}
			, dummy
			, dummy
			, mergeResource );
		m_sceneCache = makeCache< Scene, String >(	*this
			, [this]( castor::String const & name )
			{
				return std::make_shared< Scene >( name, *this );
			}
			, instantInit
			, instantClean
			, mergeResource );
		m_targetCache = std::make_unique< RenderTargetCache >( *this );
		m_techniqueCache = makeCache< RenderTechnique, String >( *this
			, [this]( String const & name
				, String const & type
				, RenderTarget & renderTarget
				, Parameters const & parameters
				, SsaoConfig const & config )
			{
				return std::make_shared< RenderTechnique >( name, renderTarget, *getRenderSystem(), parameters, config );
			}
			, dummy
			, dummy
			, mergeResource );
		m_windowCache = makeCache < RenderWindow, String >(	*this
			, [this]( castor::String const & name )
			{
				return std::make_shared< RenderWindow >( name
					, *this );
			}
			, dummy
			, eventClean
			, mergeResource );

		if ( !File::directoryExists( getEngineDirectory() ) )
		{
			File::directoryCreate( getEngineDirectory() );
		}

		Logger::logInfo( makeStringStream() << cuT( "Castor3D - Core engine version : " ) << Version{} );
		Logger::logDebug( makeStringStream() << m_cpuInformations );
	}

	Engine::~Engine()
	{
		m_lightsSampler.reset();
		m_defaultSampler.reset();

		// To destroy before RenderSystem, since it contain elements instantiated in Renderer plug-in
		m_samplerCache->clear();
		m_shaderCache->clear();
		m_overlayCache->clear();
		m_fontCache.clear();
		m_imageCache.clear();
		m_windowCache->clear();
		m_sceneCache->clear();
		m_materialCache->clear();
		m_listenerCache->clear();
		m_techniqueCache->clear();

		// Destroy the RenderSystem.
		if ( m_renderSystem )
		{
			m_renderSystem.reset();
		}

		// and eventually the  plug-ins.
		m_pluginCache->clear();
		Image::cleanupImageLib();

#if !defined( NDEBUG )
		Debug::cleanup();
#endif
	}

	void Engine::initialise( uint32_t wanted, bool threaded )
	{
		m_threaded = threaded;

		if ( m_renderSystem )
		{
			m_defaultSampler = m_samplerCache->add( cuT( "Default" ) );
			m_defaultSampler->setMinFilter( renderer::Filter::eLinear );
			m_defaultSampler->setMagFilter( renderer::Filter::eLinear );
			m_defaultSampler->setWrapS( renderer::WrapMode::eRepeat );
			m_defaultSampler->setWrapT( renderer::WrapMode::eRepeat );
			m_defaultSampler->setWrapR( renderer::WrapMode::eRepeat );

			m_lightsSampler = m_samplerCache->add( cuT( "LightsSampler" ) );
			m_lightsSampler->setMinFilter( renderer::Filter::eNearest );
			m_lightsSampler->setMagFilter( renderer::Filter::eNearest );
			m_lightsSampler->setWrapS( renderer::WrapMode::eClampToEdge );
			m_lightsSampler->setWrapT( renderer::WrapMode::eClampToEdge );
			m_lightsSampler->setWrapR( renderer::WrapMode::eClampToEdge );

			doLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( C3D_NO_RENDERSYSTEM );
		}

		if ( m_lightsSampler )
		{
			postEvent( makeInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			postEvent( makeInitialiseEvent( *m_defaultSampler ) );
		}

		if ( threaded )
		{
			m_renderLoop = std::make_unique< RenderLoopAsync >( *this, wanted );
		}
		else
		{
			m_renderLoop = std::make_unique< RenderLoopSync >( *this, wanted );
		}

		m_cleaned = false;
	}

	void Engine::cleanup()
	{
		if ( !isCleaned() )
		{
			setCleaned();

			if ( m_threaded )
			{
				m_renderLoop->pause();
			}

			m_listenerCache->cleanup();
			m_windowCache->cleanup();
			m_sceneCache->cleanup();
			m_samplerCache->cleanup();
			m_overlayCache->cleanup();
			m_materialCache->cleanup();
			m_shaderCache->cleanup();

			if ( m_lightsSampler )
			{
				postEvent( makeCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				postEvent( makeCleanupEvent( *m_defaultSampler ) );
			}

			m_techniqueCache->cleanup();

			m_renderLoop.reset();

			m_targetCache->clear();
			m_samplerCache->clear();
			m_shaderCache->clear();
			m_overlayCache->clear();
			m_materialCache->clear();
			m_sceneCache->clear();
			m_fontCache.clear();
			m_imageCache.clear();
			m_shaderCache->clear();
			m_techniqueCache->clear();
		}
	}

	bool Engine::loadRenderer( String const & type )
	{
		m_renderSystem = m_renderSystemFactory.create( type
			, *this
			, m_appName
			, m_enableValidation );
		return m_renderSystem != nullptr;
	}

	void Engine::sendEvent( FrameEventUPtr && event )
	{
		if ( m_renderSystem && m_renderSystem->hasCurrentDevice() )
		{
			event->apply();
		}
		else
		{
			postEvent( std::move( event ) );
		}
	}

	void Engine::postEvent( FrameEventUPtr && event )
	{
		auto lock = makeUniqueLock( *m_listenerCache );
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			listener->postEvent( std::move( event ) );
		}
	}

	Path Engine::getPluginsDirectory()
	{
		Path pathReturn;
		Path pathBin = File::getExecutableDirectory();
		Path pathUsr = pathBin.getPath();
		pathReturn = pathUsr / cuT( "lib" ) / cuT( "Castor3D" );
		return pathReturn;
	}

	castor::Path Engine::getEngineDirectory()
	{
		return File::getUserDirectory() / cuT( ".Castor3D" );
	}

	Path Engine::getDataDirectory()
	{
		Path pathReturn;
		Path pathBin = File::getExecutableDirectory();
		Path pathUsr = pathBin.getPath();
		pathReturn = pathUsr / cuT( "share" );
		return pathReturn;
	}

	std::locale const & Engine::getLocale()
	{
		static std::locale const loc{ "C" };
		return loc;
	}

	bool Engine::isCleaned()
	{
		return m_cleaned;
	}

	void Engine::setCleaned()
	{
		m_cleaned = true;
	}

	void Engine::registerParsers( castor::String const & name, castor::FileParser::AttributeParsersBySection const & parsers )
	{
		auto && it = m_additionalParsers.find( name );

		if ( it != m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "registerParsers - Duplicate entry for " + name );
		}

		m_additionalParsers.emplace( name, parsers );
	}

	void Engine::registerSections( castor::String const & name, castor::StrUIntMap const & sections )
	{
		auto && it = m_additionalSections.find( name );

		if ( it != m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "registerSections - Duplicate entry for " + name );
		}

		m_additionalSections.emplace( name, sections );
	}

	void Engine::unregisterParsers( castor::String const & name )
	{
		auto && it = m_additionalParsers.find( name );

		if ( it == m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "unregisterParsers - Unregistered entry " + name );
		}

		m_additionalParsers.erase( it );
	}

	void Engine::unregisterSections( castor::String const & name )
	{
		auto && it = m_additionalSections.find( name );

		if ( it == m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "unregisterSections - Unregistered entry " + name );
		}

		m_additionalSections.erase( it );
	}

	bool Engine::isTopDown()const
	{
		return m_renderSystem->isTopDown();
	}

	void Engine::renderDepth( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, castor::Position const & position
		, castor::Size const & size
		, TextureLayout const & texture )
	{
		if ( m_renderDepth )
		{
			m_renderDepth->render( renderPass
				, frameBuffer
				, position
				, size
				, texture );
		}
	}

	void Engine::doLoadCoreData()
	{
		Path path = Engine::getDataDirectory() / cuT( "Castor3D" );

		if ( File::fileExists( path / cuT( "Core.zip" ) ) )
		{
			SceneFileParser parser( *this );

			if ( !parser.parseFile( path / cuT( "Core.zip" ) ) )
			{
				Logger::logError( cuT( "Can't read Core.zip data file" ) );
			}
		}
	}
}
