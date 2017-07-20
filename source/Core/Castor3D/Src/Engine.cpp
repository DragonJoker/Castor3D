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

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call Castor3D::Engine::LoadRenderer before Castor3D::Engine::Initialise";
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
			this->PostEvent( MakeInitialiseEvent( *p_element ) );
		};
		auto eventClean = [this]( auto p_element )
		{
			this->PostEvent( MakeCleanupEvent( *p_element ) );
		};
		auto instantInit = [this]( auto p_element )
		{
			p_element->Initialise();
		};
		auto instantClean = [this]( auto p_element )
		{
			p_element->Cleanup();
		};
		auto listenerClean = [this]( auto p_element )
		{
			p_element->Flush();
		};
		auto mergeResource = []( auto const & p_source
		   , auto & p_destination
		   , auto p_element )
		{
		};
		std::locale::global( std::locale() );
		Image::InitialiseImageLib();

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = MakeCache< FrameListener, String >(	*this
			, []( String const & p_name )
			{
				return std::make_shared< FrameListener >( p_name );
			}
			, dummy
			, listenerClean
			, mergeResource );
		m_defaultListener = m_listenerCache->Add( cuT( "Default" ) );

		m_shaderCache = MakeCache( *this );
		m_samplerCache = MakeCache< Sampler, String >(	*this
			, [this]( String const & p_name )
			{
				return GetRenderSystem()->CreateSampler( p_name );
			}
			, eventInit
			, eventClean
			, mergeResource );
		m_materialCache = MakeCache< Material, String >( *this
			, [this]( String const & p_name, MaterialType p_type )
			{
				return std::make_shared< Material >( p_name, *this, p_type );
			}
			, eventInit
			, eventClean
			, mergeResource );
		m_pluginCache = MakeCache< Plugin, String >( *this
			, []( String const & p_name, PluginType p_type, Castor::DynamicLibrarySPtr p_library )
			{
				return nullptr;
			} );
		m_overlayCache = MakeCache< Overlay, String >(	*this
			, [this]( String const & p_name, OverlayType p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
			{
				auto result = std::make_shared< Overlay >( *this, p_type, p_scene, p_parent );
				result->SetName( p_name );
				return result;
			}
			, dummy
			, dummy
			, mergeResource );
		m_sceneCache = MakeCache< Scene, String >(	*this
			, [this]( Castor::String const & p_name )
			{
				return std::make_shared< Scene >( p_name, *this );
			}
			, instantInit
			, instantClean
			, mergeResource );
		m_targetCache = std::make_unique< RenderTargetCache >( *this );
		m_techniqueCache = MakeCache< RenderTechnique, String >( *this
			, [this]( String const & p_name
				, String const & p_type
				, RenderTarget & p_renderTarget
				, Parameters const & p_parameters
				, SsaoConfig const & p_config )
			{
				return std::make_shared< RenderTechnique >( p_name, p_renderTarget, *GetRenderSystem(), p_parameters, p_config );
			}
			, dummy
			, dummy
			, mergeResource );
		m_windowCache = MakeCache < RenderWindow, String >(	*this
			, [this]( Castor::String const & p_name )
			{
				return std::make_shared< RenderWindow >( p_name
					, *this );
			}
			, dummy
			, eventClean
			, mergeResource );

		if ( !File::DirectoryExists( GetEngineDirectory() ) )
		{
			File::DirectoryCreate( GetEngineDirectory() );
		}

		Logger::LogInfo( StringStream() << cuT( "Castor3D - Core engine version : " ) << Version{} );
		Logger::LogInfo( StringStream() << m_cpuInformations );
	}

	Engine::~Engine()
	{
		m_lightsSampler.reset();
		m_defaultSampler.reset();

		// To destroy before RenderSystem, since it contain elements instantiated in Renderer plug-in
		m_samplerCache->Clear();
		m_shaderCache->Clear();
		m_overlayCache->Clear();
		m_fontCache.Clear();
		m_imageCache.clear();
		m_windowCache->Clear();
		m_sceneCache->Clear();
		m_materialCache->Clear();
		m_listenerCache->Clear();
		m_techniqueCache->Clear();

		// Destroy the RenderSystem.
		if ( m_renderSystem )
		{
			m_renderSystem.reset();
		}

		// and eventually the  plug-ins.
		m_pluginCache->Clear();
		Image::CleanupImageLib();
	}

	void Engine::Initialise( uint32_t p_wanted, bool p_threaded )
	{
		m_threaded = p_threaded;

		if ( m_renderSystem )
		{
			m_defaultSampler = m_samplerCache->Add( cuT( "Default" ) );
			m_defaultSampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_defaultSampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_defaultSampler->SetInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
			m_defaultSampler->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
			m_defaultSampler->SetWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
			m_defaultSampler->SetWrappingMode( TextureUVW::eW, WrapMode::eRepeat );

			m_lightsSampler = m_samplerCache->Add( cuT( "LightsSampler" ) );
			m_lightsSampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_lightsSampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_lightsSampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_lightsSampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_lightsSampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );

			DoLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( C3D_NO_RENDERSYSTEM );
		}

		if ( m_lightsSampler )
		{
			PostEvent( MakeInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			PostEvent( MakeInitialiseEvent( *m_defaultSampler ) );
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

	void Engine::Cleanup()
	{
		if ( !IsCleaned() )
		{
			SetCleaned();

			if ( m_threaded )
			{
				m_renderLoop->Pause();
			}

			m_listenerCache->Cleanup();
			m_windowCache->Cleanup();
			m_sceneCache->Cleanup();
			m_samplerCache->Cleanup();
			m_overlayCache->Cleanup();
			m_materialCache->Cleanup();
			m_shaderCache->Cleanup();

			if ( m_lightsSampler )
			{
				PostEvent( MakeCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				PostEvent( MakeCleanupEvent( *m_defaultSampler ) );
			}

			m_techniqueCache->Cleanup();
			m_renderLoop.reset();

			m_targetCache->Clear();
			m_samplerCache->Clear();
			m_shaderCache->Clear();
			m_overlayCache->Clear();
			m_materialCache->Clear();
			m_sceneCache->Clear();
			m_fontCache.Clear();
			m_imageCache.clear();
			m_shaderCache->Clear();
			m_techniqueCache->Clear();
		}
	}

	bool Engine::LoadRenderer( String const & p_type )
	{
		m_renderSystem = m_renderSystemFactory.Create( p_type, *this );
		return m_renderSystem != nullptr;
	}

	void Engine::PostEvent( FrameEventUPtr && p_event )
	{
		auto lock = make_unique_lock( *m_listenerCache );
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			listener->PostEvent( std::move( p_event ) );
		}
	}

	Path Engine::GetPluginsDirectory()
	{
		Path pathReturn;
		Path pathBin = File::GetExecutableDirectory();
		Path pathUsr = pathBin.GetPath();
		pathReturn = pathUsr / cuT( "lib" ) / cuT( "Castor3D" );
		return pathReturn;
	}

	Castor::Path Engine::GetEngineDirectory()
	{
		return File::GetUserDirectory() / cuT( ".Castor3D" );
	}

	Path Engine::GetDataDirectory()
	{
		Path pathReturn;
		Path pathBin = File::GetExecutableDirectory();
		Path pathUsr = pathBin.GetPath();
		pathReturn = pathUsr / cuT( "share" );
		return pathReturn;
	}

	bool Engine::IsCleaned()
	{
		return m_cleaned;
	}

	void Engine::SetCleaned()
	{
		m_cleaned = true;
	}

	bool Engine::SupportsShaderModel( ShaderModel p_eShaderModel )
	{
		bool result = false;

		if ( m_renderSystem )
		{
			result = m_renderSystem->GetGpuInformations().CheckSupport( p_eShaderModel );
		}

		return result;
	}

	void Engine::RegisterParsers( Castor::String const & p_name, Castor::FileParser::AttributeParsersBySection && p_parsers )
	{
		auto && it = m_additionalParsers.find( p_name );

		if ( it != m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "RegisterParsers - Duplicate entry for " + p_name );
		}

		m_additionalParsers.insert( std::make_pair( p_name, p_parsers ) );
	}

	void Engine::RegisterSections( Castor::String const & p_name, Castor::StrUIntMap const & p_sections )
	{
		auto && it = m_additionalSections.find( p_name );

		if ( it != m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "RegisterSections - Duplicate entry for " + p_name );
		}

		m_additionalSections.insert( std::make_pair( p_name, p_sections ) );
	}

	void Engine::UnregisterParsers( Castor::String const & p_name )
	{
		auto && it = m_additionalParsers.find( p_name );

		if ( it == m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "UnregisterParsers - Unregistered entry " + p_name );
		}

		m_additionalParsers.erase( it );
	}

	void Engine::UnregisterSections( Castor::String const & p_name )
	{
		auto && it = m_additionalSections.find( p_name );

		if ( it == m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "UnregisterSections - Unregistered entry " + p_name );
		}

		m_additionalSections.erase( it );
	}

	void Engine::DoLoadCoreData()
	{
		Path path = Engine::GetDataDirectory() / cuT( "Castor3D" );

		if ( File::FileExists( path / cuT( "Core.zip" ) ) )
		{
			SceneFileParser parser( *this );

			if ( !parser.ParseFile( path / cuT( "Core.zip" ) ) )
			{
				Logger::LogError( cuT( "Can't read Core.zip data file" ) );
			}
		}
	}
}
