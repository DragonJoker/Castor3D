#include "Overlay.hpp"

#include "BorderPanelOverlay.hpp"
#include "Engine.hpp"
#include "OverlayFactory.hpp"
#include "OverlayManager.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Overlay::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Overlay, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Overlay::TextLoader::operator()( Overlay const & p_overlay, TextFile & p_file )
	{
		return OverlayCategory::TextLoader()( *p_overlay.m_pOverlayCategory, p_file );
	}

	//*************************************************************************************************

	Overlay::BinaryParser::BinaryParser( Path const & p_path, Engine * p_engine )
		:	Castor3D::BinaryParser< Overlay >( p_path )
		,	m_engine( p_engine )
	{
	}

	bool Overlay::BinaryParser::Fill( Overlay const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_OVERLAY );

		if ( l_bReturn )
		{
			l_bReturn = OverlayCategory::BinaryParser( m_path ).Fill( *p_obj.m_pOverlayCategory, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		for ( auto && l_it = p_obj.begin(); l_it != p_obj.end() && l_bReturn; ++l_it )
		{
			OverlaySPtr l_overlay = *l_it;
			l_bReturn = Overlay::BinaryParser( m_path, m_engine ).Fill( *l_overlay, l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Overlay::BinaryParser::Parse( Overlay & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		eOVERLAY_TYPE l_type;
		String l_name;
		OverlaySPtr l_overlay;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( p_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_OVERLAY:
					l_overlay.reset();
					break;

				case eCHUNK_TYPE_OVERLAY_TYPE:
					l_bReturn = DoParseChunk( l_type, l_chunk );

					if ( l_bReturn )
					{
						l_overlay = m_engine->CreateOverlay( l_type, cuT( "" ), p_obj.shared_from_this(), p_obj.GetScene() );
						l_bReturn = Overlay::BinaryParser( m_path, m_engine ).Parse( *l_overlay, l_chunk );
					}

					break;

				case eCHUNK_TYPE_NAME:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetName( l_name );
					}

					break;

				default:
					l_bReturn = OverlayCategory::BinaryParser( m_path ).Parse( *p_obj.m_pOverlayCategory, l_chunk );
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Overlay::Overlay( Engine * p_pEngine, eOVERLAY_TYPE p_eType )
		: m_manager( p_pEngine->GetOverlayManager() )
		, m_pParent()
		, m_pScene()
		, m_factory( p_pEngine->GetOverlayFactory() )
		, m_pEngine( p_pEngine )
		, m_pRenderSystem( p_pEngine->GetRenderSystem() )
		, m_pOverlayCategory( p_pEngine->GetOverlayFactory().Create( p_eType ) )
	{
		m_pOverlayCategory->SetOverlay( this );
	}

	Overlay::Overlay( Engine * p_pEngine, eOVERLAY_TYPE p_eType, SceneSPtr p_pScene, OverlaySPtr p_pParent )
		: m_manager( p_pEngine->GetOverlayManager() )
		, m_pParent( p_pParent )
		, m_pScene( p_pScene )
		, m_factory( p_pEngine->GetOverlayFactory() )
		, m_pRenderSystem( p_pEngine->GetRenderSystem() )
		, m_pEngine( p_pEngine )
		, m_pOverlayCategory( p_pEngine->GetOverlayFactory().Create( p_eType ) )
	{
		m_pOverlayCategory->SetOverlay( this );
	}

	Overlay::~Overlay()
	{
	}

	void Overlay::Render( Castor::Size const & p_size )
	{
		if ( IsVisible() )
		{
			m_pOverlayCategory->UpdatePositionAndSize();
			m_pOverlayCategory->Render();

			for ( auto && l_overlay : m_overlays )
			{
				l_overlay->Render( p_size );
			}
		}
	}

	bool Overlay::AddChild( OverlaySPtr p_pOverlay )
	{
		bool l_bReturn = false;
		int l_index = 1;

		if ( !m_overlays.empty() )
		{
			l_index = ( *( m_overlays.end() - 1 ) )->GetIndex() + 1;
		}

		p_pOverlay->SetOrder( l_index, GetLevel() + 1 );
		m_overlays.push_back( p_pOverlay );
		return true;
	}

	void Overlay::Initialise()
	{
	}

	int Overlay::GetChildsCount( int p_level )const
	{
		int l_return = 0;

		if ( p_level == GetLevel() + 1 )
		{
			l_return = int( m_overlays.size() );
		}
		else if ( p_level > GetLevel() )
		{
			for ( auto && l_overlay : m_overlays )
			{
				l_return += l_overlay->GetChildsCount( p_level );
			}
		}

		return l_return;
	}

	PanelOverlaySPtr Overlay::GetPanelOverlay()const
	{
		PanelOverlaySPtr l_return;

		if ( m_pOverlayCategory->GetType() == eOVERLAY_TYPE_PANEL )
		{
			l_return = std::static_pointer_cast< PanelOverlay >( m_pOverlayCategory );
		}

		return l_return;
	}

	BorderPanelOverlaySPtr Overlay::GetBorderPanelOverlay()const
	{
		BorderPanelOverlaySPtr l_return;

		if ( m_pOverlayCategory->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
		{
			l_return = std::static_pointer_cast< BorderPanelOverlay >( m_pOverlayCategory );
		}

		return l_return;
	}

	TextOverlaySPtr Overlay::GetTextOverlay()const
	{
		TextOverlaySPtr l_return;

		if ( m_pOverlayCategory->GetType() == eOVERLAY_TYPE_TEXT )
		{
			l_return = std::static_pointer_cast< TextOverlay >( m_pOverlayCategory );
		}

		return l_return;
	}
}
