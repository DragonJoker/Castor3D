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

	Engine::Engine()
		: Unique< Engine >( this )
		, m_renderSystem( nullptr )
		, m_cleaned( true )
		, m_perObjectLighting( true )
		, m_threaded( false )
	{
		auto dummy = []( auto p_element )
		{
		};
		auto eventInit = [this]( auto p_element )
		{
			this->postEvent( MakeInitialiseEvent( *p_element ) );
		};
		auto eventClean = [this]( auto p_element )
		{
			this->postEvent( MakeCleanupEvent( *p_element ) );
		};
		auto instantInit = [this]( auto p_element )
		{
			p_element->initialise();
		};
		auto instantClean = [this]( auto p_element )
		{
			p_element->cleanup();
		};
		auto listenerClean = [this]( auto p_element )
		{
			p_element->flush();
		};
		auto mergeResource = []( auto const & p_source
		   , auto & p_destination
		   , auto p_element )
		{
		};
		std::locale::global( std::locale() );
		Image::initialiseImageLib();

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = makeCache< FrameListener, String >(	*this
			, []( String const & p_name )
			{
				return std::make_shared< FrameListener >( p_name );
			}
			, dummy
			, listenerClean
			, mergeResource );
		m_defaultListener = m_listenerCache->add( cuT( "Default" ) );

		m_shaderCache = makeCache( *this );
		m_samplerCache = makeCache< Sampler, String >(	*this
			, [this]( String const & p_name )
			{
				return getRenderSystem()->createSampler( p_name );
			}
			, eventInit
			, eventClean
			, mergeResource );
		m_materialCache = makeCache< Material, String >( *this
			, [this]( String const & p_name, MaterialType p_type )
			{
				return std::make_shared< Material >( p_name, *this, p_type );
			}
			, eventInit
			, eventClean
			, mergeResource );
		m_pluginCache = makeCache< Plugin, String >( *this
			, []( String const & p_name, PluginType p_type, castor::DynamicLibrarySPtr p_library )
			{
				return nullptr;
			} );
		m_overlayCache = makeCache< Overlay, String >(	*this
			, [this]( String const & p_name, OverlayType p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
			{
				auto result = std::make_shared< Overlay >( *this, p_type, p_scene, p_parent );
				result->setName( p_name );
				return result;
			}
			, dummy
			, dummy
			, mergeResource );
		m_sceneCache = makeCache< Scene, String >(	*this
			, [this]( castor::String const & p_name )
			{
				return std::make_shared< Scene >( p_name, *this );
			}
			, instantInit
			, instantClean
			, mergeResource );
		m_targetCache = std::make_unique< RenderTargetCache >( *this );
		m_techniqueCache = makeCache< RenderTechnique, String >( *this
			, [this]( String const & p_name
				, String const & p_type
				, RenderTarget & p_renderTarget
				, Parameters const & p_parameters
				, SsaoConfig const & p_config )
			{
				return std::make_shared< RenderTechnique >( p_name, p_renderTarget, *getRenderSystem(), p_parameters, p_config );
			}
			, dummy
			, dummy
			, mergeResource );
		m_windowCache = makeCache < RenderWindow, String >(	*this
			, [this]( castor::String const & p_name )
			{
				return std::make_shared< RenderWindow >( p_name
					, *this );
			}
			, dummy
			, eventClean
			, mergeResource );

		if ( !File::directoryExists( getEngineDirectory() ) )
		{
			File::directoryCreate( getEngineDirectory() );
		}

		Logger::logInfo( StringStream() << cuT( "Castor3D - Core engine version : " ) << Version{} );
		Logger::logInfo( StringStream() << m_cpuInformations );
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
	}

	void Engine::initialise( uint32_t p_wanted, bool p_threaded )
	{
		m_threaded = p_threaded;

		if ( m_renderSystem )
		{
			m_defaultSampler = m_samplerCache->add( cuT( "Default" ) );
			m_defaultSampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_defaultSampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_defaultSampler->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
			m_defaultSampler->setWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
			m_defaultSampler->setWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
			m_defaultSampler->setWrappingMode( TextureUVW::eW, WrapMode::eRepeat );

			m_lightsSampler = m_samplerCache->add( cuT( "LightsSampler" ) );
			m_lightsSampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_lightsSampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_lightsSampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_lightsSampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_lightsSampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );

			doLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( C3D_NO_RENDERSYSTEM );
		}

		if ( m_lightsSampler )
		{
			postEvent( MakeInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			postEvent( MakeInitialiseEvent( *m_defaultSampler ) );
		}

		if ( p_threaded )
		{
			m_renderLoop = std::make_unique< RenderLoopAsync >( *this, p_wanted );
		}
		else
		{
			m_renderLoop = std::make_unique< RenderLoopSync >( *this, p_wanted );
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
				postEvent( MakeCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				postEvent( MakeCleanupEvent( *m_defaultSampler ) );
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

	bool Engine::loadRenderer( String const & p_type )
	{
		m_renderSystem = m_renderSystemFactory.create( p_type, *this );
		return m_renderSystem != nullptr;
	}

	void Engine::postEvent( FrameEventUPtr && p_event )
	{
		auto lock = makeUniqueLock( *m_listenerCache );
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			listener->postEvent( std::move( p_event ) );
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

	bool Engine::isCleaned()
	{
		return m_cleaned;
	}

	void Engine::setCleaned()
	{
		m_cleaned = true;
	}

	bool Engine::supportsShaderModel( ShaderModel p_eShaderModel )
	{
		bool result = false;

		if ( m_renderSystem )
		{
			result = m_renderSystem->getGpuInformations().checkSupport( p_eShaderModel );
		}

		return result;
	}

	void Engine::registerParsers( castor::String const & p_name, castor::FileParser::AttributeParsersBySection && p_parsers )
	{
		auto && it = m_additionalParsers.find( p_name );

		if ( it != m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "registerParsers - Duplicate entry for " + p_name );
		}

		m_additionalParsers.insert( std::make_pair( p_name, p_parsers ) );
	}

	void Engine::registerSections( castor::String const & p_name, castor::StrUIntMap const & p_sections )
	{
		auto && it = m_additionalSections.find( p_name );

		if ( it != m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "registerSections - Duplicate entry for " + p_name );
		}

		m_additionalSections.insert( std::make_pair( p_name, p_sections ) );
	}

	void Engine::unregisterParsers( castor::String const & p_name )
	{
		auto && it = m_additionalParsers.find( p_name );

		if ( it == m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "unregisterParsers - Unregistered entry " + p_name );
		}

		m_additionalParsers.erase( it );
	}

	void Engine::unregisterSections( castor::String const & p_name )
	{
		auto && it = m_additionalSections.find( p_name );

		if ( it == m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "unregisterSections - Unregistered entry " + p_name );
		}

		m_additionalSections.erase( it );
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
