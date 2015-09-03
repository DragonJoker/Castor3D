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
		void operator()( OverlaySPtr p_pOverlay )
		{
			p_pOverlay->Initialise();
		}
	};

	//*************************************************************************************************

	OverlayManager::OverlayManager( Engine * p_pEngine )
		: m_pEngine( p_pEngine )
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

	void OverlayManager::AddOverlay( Castor::String const & p_strName, OverlaySPtr p_pOverlay, OverlaySPtr p_pParent )
	{
		OverlayCollection::insert( p_strName, p_pOverlay );
		int l_level = 0;

		if ( !p_pParent )
		{
			m_overlays.push_back( p_pOverlay );
		}
		else
		{
			l_level = p_pParent->GetLevel() + 1;
			p_pParent->AddChild( p_pOverlay );
		}

		if ( l_level > int( m_overlayCountPerLevel.size() ) )
		{
			m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
		}

		m_overlayCountPerLevel[l_level]++;
	}

	bool OverlayManager::HasOverlay( Castor::String const & p_strName )
	{
		return OverlayCollection::has( p_strName );
	}

	OverlaySPtr OverlayManager::GetOverlay( Castor::String const & p_strName )
	{
		return OverlayCollection::find( p_strName );
	}

	bool OverlayManager::WriteOverlays( Castor::TextFile & p_file )const
	{
		OverlayCollection::lock();
		bool l_bReturn = true;
		auto && l_it = m_overlays.begin();
		bool l_bFirst = true;

		while ( l_bReturn && l_it != m_overlays.end() )
		{
			OverlaySPtr l_overlay = *l_it;

			if ( l_bFirst )
			{
				l_bFirst = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			switch ( l_overlay->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_bReturn = PanelOverlay::TextLoader()( *l_overlay->GetPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_bReturn = BorderPanelOverlay::TextLoader()( *l_overlay->GetBorderPanelOverlay(), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_bReturn = TextOverlay::TextLoader()( *l_overlay->GetTextOverlay(), p_file );
				break;

			default:
				l_bReturn = false;
			}

			++l_it;
		}

		OverlayCollection::unlock();
		return l_bReturn;
	}

	bool OverlayManager::ReadOverlays( Castor::TextFile & p_file )
	{
		SceneFileParser l_parser( m_pEngine );
		return l_parser.ParseFile( p_file );
	}

	bool OverlayManager::SaveOverlays( Castor::BinaryFile & p_file )const
	{
		OverlayCollection::lock();
		bool l_bReturn = p_file.Write( uint32_t( m_overlays.size() ) ) == sizeof( uint32_t );
		auto && l_it = m_overlays.begin();

		while ( l_bReturn && l_it != m_overlays.end() )
		{
			OverlaySPtr l_overlay = *l_it;
			l_bReturn = BinaryLoader< Overlay >()( *l_overlay, p_file );
			++l_it;
		}

		OverlayCollection::unlock();
		return l_bReturn;
	}

	bool OverlayManager::LoadOverlays( Castor::BinaryFile & p_file )
	{
		OverlayCollection::lock();
		uint32_t l_uiSize;
		bool l_bReturn = p_file.Write( l_uiSize ) == sizeof( uint32_t );
		String l_strName;
		eOVERLAY_TYPE l_eType;
		OverlaySPtr l_pOverlay;

		for ( uint32_t i = 0; i < l_uiSize && l_bReturn; i++ )
		{
			l_bReturn = p_file.Read( l_strName );

			if ( l_bReturn )
			{
				l_bReturn = p_file.Read( l_eType ) == sizeof( eOVERLAY_TYPE );
			}

			if ( l_bReturn )
			{
				l_pOverlay = OverlayCollection::find( l_strName ) ;

				if ( !l_pOverlay )
				{
					l_pOverlay = std::make_shared< Overlay >( m_pEngine, l_eType );
					l_pOverlay->SetName( l_strName );
					AddOverlay( l_strName, l_pOverlay, nullptr );
				}

				l_bReturn = l_pOverlay != nullptr;
			}

			if ( l_bReturn )
			{
				l_bReturn = BinaryLoader< Overlay >()( *l_pOverlay, p_file );
			}
		}

		OverlayCollection::unlock();
		return l_bReturn;
	}

	void OverlayManager::Update()
	{
		if ( m_pEngine->IsCleaned() )
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
				m_pRenderer = m_pEngine->GetRenderSystem()->CreateOverlayRenderer();
				m_pRenderer->Initialise();
			}
		}
	}

	void OverlayManager::RenderOverlays( Scene const & p_scene, Castor::Size const & p_size )
	{
		lock();
		Update();
		RenderSystem * l_prenderSystem = m_pEngine->GetRenderSystem();
		Context * l_context = l_prenderSystem->GetCurrentContext();

		if ( l_context && m_pRenderer )
		{
			Pipeline * l_pipeline = l_prenderSystem->GetPipeline();
			l_context->CullFace( eFACE_BACK );

			eMTXMODE l_oldMode = l_pipeline->MatrixMode( eMTXMODE_PROJECTION );
			l_pipeline->PushMatrix();
			l_pipeline->Ortho( 0.0, real( p_size.width() ), real( p_size.height() ), 0.0, 0.0, 1000.0 );
			Matrix4x4r l_mtxTransform;
			MtxUtils::set_transform_rh(
				l_mtxTransform,
				Point3r( 0, 0, 0 ),
				Point3r( 1, 1, 0 ),
				Quaternion::Identity()
			);
			l_pipeline->MultMatrix( l_mtxTransform );
			m_pRenderer->BeginRender( p_size );

			for ( auto l_overlay : m_overlays )
			{
				SceneSPtr l_scene = l_overlay->GetScene();

				if ( !l_scene || l_scene->GetName() == p_scene.GetName() )
				{
					l_overlay->Render( p_size );
				}
			}

			l_pipeline->PopMatrix();
			l_pipeline->MatrixMode( l_oldMode );
			unlock();
		}
	}
}
