#include "OverlayManager.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"
#include "Engine.hpp"
#include "SceneFileParser.hpp"
#include "RenderSystem.hpp"

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
		:	m_iCurrentZIndex( 0 )
		,	m_pEngine( p_pEngine )
	{
	}

	OverlayManager::~OverlayManager()
	{
	}

	void OverlayManager::ClearOverlays()
	{
		OverlayCollection::lock();
		OverlayCollection::clear();
		clear_pair_container( m_mapOverlaysByZIndex );
		m_iCurrentZIndex = 1;
		OverlayCollection::unlock();
	}

	void OverlayManager::AddOverlay( Castor::String const & p_strName, OverlaySPtr p_pOverlay, OverlaySPtr p_pParent )
	{
		OverlayCollection::insert( p_strName, p_pOverlay );

		if ( !p_pParent )
		{
			AddOverlay( m_iCurrentZIndex, p_pOverlay );
		}
		else
		{
			p_pParent->AddChild( p_pOverlay, m_iCurrentZIndex );
		}

		m_iCurrentZIndex++;
	}

	void OverlayManager::AddOverlay( int p_iZIndex, OverlaySPtr p_pOverlay )
	{
		p_pOverlay->SetZIndex( p_iZIndex );
		m_mapOverlaysByZIndex.insert( std::make_pair( p_iZIndex, p_pOverlay ) );
	}

	bool OverlayManager::HasOverlay( int p_iZIndex )
	{
		return m_mapOverlaysByZIndex.find( p_iZIndex ) != m_mapOverlaysByZIndex.end();
	}

	bool OverlayManager::HasOverlay( Castor::String const & p_strName )
	{
		return OverlayCollection::has( p_strName );
	}

	OverlaySPtr OverlayManager::GetOverlay( int p_iZIndex )
	{
		OverlaySPtr l_pReturn;
		OverlayPtrIntMapIt l_it = m_mapOverlaysByZIndex.find( p_iZIndex );

		if ( l_it != m_mapOverlaysByZIndex.end() )
		{
			l_pReturn = l_it->second;
		}

		return l_pReturn;
	}

	OverlaySPtr OverlayManager::GetOverlay( Castor::String const & p_strName )
	{
		return OverlayCollection::find( p_strName );
	}

	bool OverlayManager::WriteOverlays( Castor::TextFile & p_file )const
	{
		OverlayCollection::lock();
		bool l_bReturn = true;
		OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();
		bool l_bFirst = true;

		while ( l_bReturn && l_it != m_mapOverlaysByZIndex.end() )
		{
			if ( l_bFirst )
			{
				l_bFirst = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			switch ( l_it->second->GetType() )
			{
			case eOVERLAY_TYPE_PANEL:
				l_bReturn = PanelOverlay::TextLoader()(	* std::static_pointer_cast< PanelOverlay >(	l_it->second->GetOverlayCategory() ), p_file );
				break;

			case eOVERLAY_TYPE_BORDER_PANEL:
				l_bReturn = BorderPanelOverlay::TextLoader()(	* std::static_pointer_cast< BorderPanelOverlay >(	l_it->second->GetOverlayCategory() ), p_file );
				break;

			case eOVERLAY_TYPE_TEXT:
				l_bReturn = TextOverlay::TextLoader()(	* std::static_pointer_cast< TextOverlay >(	l_it->second->GetOverlayCategory() ), p_file );
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
		bool l_bReturn = p_file.Write( uint32_t( m_mapOverlaysByZIndex.size() ) ) == sizeof( uint32_t );
		OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

		while ( l_bReturn && l_it != m_mapOverlaysByZIndex.end() )
		{
			l_bReturn = BinaryLoader< Overlay >()( *l_it->second, p_file );
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

				if ( ! l_pOverlay )
				{
					l_pOverlay = std::make_shared< Overlay >( m_pEngine, l_eType );
					l_pOverlay->SetName( l_strName );
					OverlayCollection::insert( l_strName, l_pOverlay );
				}

				l_bReturn = l_pOverlay.use_count() > 0;
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
				lock();
				std::for_each( begin(), end(), [&]( std::pair< String, OverlaySPtr > pair )
				{
					pair.second->GetOverlayCategory()->SetRenderer( m_pRenderer );
				} );
				unlock();
				m_pRenderer->Initialise();
			}
		}
	}

	void OverlayManager::BeginRendering( Size const & p_size )
	{
		if ( m_pRenderer )
		{
			m_pRenderer->BeginRender( p_size );
		}
	}

	void OverlayManager::EndRendering()
	{
		if ( m_pRenderer )
		{
			m_pRenderer->EndRender();
		}
	}
}
