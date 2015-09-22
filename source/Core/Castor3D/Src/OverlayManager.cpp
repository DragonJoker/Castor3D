#include "OverlayManager.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"
#include "Engine.hpp"
#include "SceneFileParser.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "Pipeline.hpp"

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	struct OverlayInitialiser
	{
		void operator()( OverlaySPtr p_overlay )
		{
			p_overlay->Initialise();
		}
	};

	//*************************************************************************************************

	OverlayManager::OverlayManager( Engine * p_engine )
		: m_engine( p_engine )
		, m_overlayCountPerLevel( 1000, 0 )
	{
	}

	OverlayManager::~OverlayManager()
	{
	}

	void OverlayManager::ClearOverlays()
	{
		OverlayCollection::lock();
		OverlayCollection::clear();
		m_overlays.clear();
		OverlayCollection::unlock();
	}

	void OverlayManager::AddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent )
	{
		OverlayCollection::insert( p_name, p_overlay );
		int l_level = 0;

		if ( !p_parent )
		{
			m_overlays.push_back( p_overlay );
		}
		else
		{
			l_level = p_parent->GetLevel() + 1;
			p_parent->AddChild( p_overlay );
		}

		if ( l_level > int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		m_overlayCountPerLevel[l_level]++;
	}

	bool OverlayManager::HasOverlay( Castor::String const & p_name )
	{
		return OverlayCollection::has( p_name );
	}

	OverlaySPtr OverlayManager::GetOverlay( Castor::String const & p_name )
	{
		return OverlayCollection::find( p_name );
	}

	bool OverlayManager::WriteOverlays( Castor::TextFile & p_file )const
	{
		OverlayCollection::lock();
		bool l_return = true;
		auto && l_it = m_overlays.begin();
		bool l_first = true;

		while ( l_return && l_it != m_overlays.end() )
		{
			OverlaySPtr l_overlay = *l_it;

			if ( l_first )
			{
				l_first = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_return = PanelOverlay::TextLoader()( *l_overlay->GetPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_return = BorderPanelOverlay::TextLoader()( *l_overlay->GetBorderPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_return = TextOverlay::TextLoader()( *l_overlay->GetTextOverlay(), p_file );
				break;

			default:
				l_return = false;
			}

			++l_it;
		}

		OverlayCollection::unlock();
		return l_return;
	}

	bool OverlayManager::ReadOverlays( Castor::TextFile & p_file )
	{
		SceneFileParser l_parser( m_engine );
		return l_parser.ParseFile( p_file );
	}

	bool OverlayManager::SaveOverlays( Castor::BinaryFile & p_file )const
	{
		OverlayCollection::lock();
		bool l_return = p_file.Write( uint32_t( m_overlays.size() ) ) == sizeof( uint32_t );
		auto && l_it = m_overlays.begin();

		while ( l_return && l_it != m_overlays.end() )
		{
			OverlaySPtr l_overlay = *l_it;
			l_return = BinaryLoader< Overlay >()( *l_overlay, p_file );
			++l_it;
		}

		OverlayCollection::unlock();
		return l_return;
	}

	bool OverlayManager::LoadOverlays( Castor::BinaryFile & p_file )
	{
		OverlayCollection::lock();
		uint32_t l_size;
		bool l_return = p_file.Write( l_size ) == sizeof( uint32_t );
		String l_name;
		eOVERLAY_TYPE l_type;
		OverlaySPtr l_overlay;

		for ( uint32_t i = 0; i < l_size && l_return; i++ )
		{
			l_return = p_file.Read( l_name );

			if ( l_return )
			{
				l_return = p_file.Read( l_type ) == sizeof( eOVERLAY_TYPE );
			}

			if ( l_return )
			{
				l_overlay = OverlayCollection::find( l_name ) ;

				if ( !l_overlay )
				{
					l_overlay = std::make_shared< Overlay >( m_engine, l_type );
					l_overlay->SetName( l_name );
					AddOverlay( l_name, l_overlay, nullptr );
				}

				l_return = l_overlay != nullptr;
			}

			if ( l_return )
			{
				l_return = BinaryLoader< Overlay >()( *l_overlay, p_file );
			}
		}

		OverlayCollection::unlock();
		return l_return;
	}

	void OverlayManager::Update()
	{
		if ( m_engine->IsCleaned() )
		{
			if ( m_pRenderer )
			{
				m_pRenderer->Cleanup();
				m_pRenderer.reset();
			}
		}
		else
		{
			if ( !m_pRenderer )
			{
				m_pRenderer = m_engine->GetRenderSystem()->CreateOverlayRenderer();
				m_pRenderer->Initialise();
			}
		}
	}

	void OverlayManager::RenderOverlays( Scene const & p_scene, Castor::Size const & p_size )
	{
		RenderSystem * l_renderSystem = m_engine->GetRenderSystem();
		Pipeline & l_pipeline = l_renderSystem->GetPipeline();

		if ( m_size != p_size )
		{
			m_size = p_size;
			l_pipeline.Ortho( real( 0.0 ), real( m_size.width() ), real( m_size.height() ), real( 0.0 ), real( 0.0 ), real( 1000.0 ) );
			m_projection = l_pipeline.GetProjectionMatrix();
		}
		else
		{
			l_pipeline.SetProjectionMatrix( m_projection );
		}

		lock();
		Update();
		Context * l_context = l_renderSystem->GetCurrentContext();

		if ( l_context && m_pRenderer )
		{
			l_context->CullFace( eFACE_BACK );
			l_pipeline.ApplyViewport( m_size.width(), m_size.height() );
			m_pRenderer->BeginRender( m_size );

			for ( auto l_overlay : m_overlays )
			{
				SceneSPtr l_scene = l_overlay->GetScene();

				if ( !l_scene || l_scene->GetName() == p_scene.GetName() )
				{
					l_overlay->Render( m_size );
				}
			}
		}

		unlock();
	}
}
