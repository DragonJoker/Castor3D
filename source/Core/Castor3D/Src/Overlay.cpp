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

	Overlay::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< Overlay, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Overlay::TextLoader::operator()( Overlay const & p_overlay, TextFile & p_file )
	{
		return OverlayCategory::TextLoader()( *p_overlay.m_category, p_file );
	}

	//*************************************************************************************************

	Overlay::BinaryParser::BinaryParser( Path const & p_path, Engine * p_engine )
		:	Castor3D::BinaryParser< Overlay >( p_path )
		,	m_engine( p_engine )
	{
	}

	bool Overlay::BinaryParser::Fill( Overlay const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_OVERLAY );

		if ( l_return )
		{
			l_return = OverlayCategory::BinaryParser( m_path ).Fill( *p_obj.m_category, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		for ( auto && l_it = p_obj.begin(); l_it != p_obj.end() && l_return; ++l_it )
		{
			OverlaySPtr l_overlay = *l_it;
			l_return = Overlay::BinaryParser( m_path, m_engine ).Fill( *l_overlay, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Overlay::BinaryParser::Parse( Overlay & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		eOVERLAY_TYPE l_type;
		String l_name;
		OverlaySPtr l_overlay;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( p_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_OVERLAY:
					l_overlay.reset();
					break;

				case eCHUNK_TYPE_OVERLAY_TYPE:
					l_return = DoParseChunk( l_type, l_chunk );

					if ( l_return )
					{
						l_overlay = m_engine->GetOverlayManager().CreateOverlay( l_type, cuT( "" ), p_obj.shared_from_this(), p_obj.GetScene() );
						l_return = Overlay::BinaryParser( m_path, m_engine ).Parse( *l_overlay, l_chunk );
					}

					break;

				case eCHUNK_TYPE_NAME:
					l_return = DoParseChunk( l_name, l_chunk );

					if ( l_return )
					{
						p_obj.SetName( l_name );
					}

					break;

				default:
					l_return = OverlayCategory::BinaryParser( m_path ).Parse( *p_obj.m_category, l_chunk );
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Overlay::Overlay( Engine & p_engine, eOVERLAY_TYPE p_type )
		: OwnedBy< Engine >( p_engine )
		, m_manager( p_engine.GetOverlayManager() )
		, m_pParent()
		, m_pScene()
		, m_factory( p_engine.GetOverlayFactory() )
		, m_renderSystem( p_engine.GetRenderSystem() )
		, m_category( p_engine.GetOverlayFactory().Create( p_type ) )
	{
		m_category->SetOverlay( this );
	}

	Overlay::Overlay( Engine & p_engine, eOVERLAY_TYPE p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
		: OwnedBy< Engine >( p_engine )
		, m_manager( p_engine.GetOverlayManager() )
		, m_pParent( p_parent )
		, m_pScene( p_scene )
		, m_factory( p_engine.GetOverlayFactory() )
		, m_renderSystem( p_engine.GetRenderSystem() )
		, m_category( p_engine.GetOverlayFactory().Create( p_type ) )
	{
		m_category->SetOverlay( this );
	}

	Overlay::~Overlay()
	{
	}

	void Overlay::Render( Castor::Size const & p_size )
	{
		if ( IsVisible() )
		{
			m_category->Render();

			for ( auto && l_overlay : m_overlays )
			{
				l_overlay->Render( p_size );
			}
		}
	}

	bool Overlay::AddChild( OverlaySPtr p_overlay )
	{
		bool l_return = false;
		int l_index = 1;

		if ( !m_overlays.empty() )
		{
			l_index = ( *( m_overlays.end() - 1 ) )->GetIndex() + 1;
		}

		p_overlay->SetOrder( l_index, GetLevel() + 1 );
		m_overlays.push_back( p_overlay );
		return true;
	}

	void Overlay::Initialise()
	{
		m_category->Initialise();
	}

	void Overlay::Cleanup()
	{
		m_category->Cleanup();
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
		if ( m_category->GetType() != eOVERLAY_TYPE_PANEL )
		{
			CASTOR_EXCEPTION( "This overlay is not a panel." );
		}

		return std::static_pointer_cast< PanelOverlay >( m_category );
	}

	BorderPanelOverlaySPtr Overlay::GetBorderPanelOverlay()const
	{
		if ( m_category->GetType() != eOVERLAY_TYPE_BORDER_PANEL )
		{
			CASTOR_EXCEPTION( "This overlay is not a border panel." );
		}

		return std::static_pointer_cast< BorderPanelOverlay >( m_category );
	}

	TextOverlaySPtr Overlay::GetTextOverlay()const
	{
		if ( m_category->GetType() != eOVERLAY_TYPE_TEXT )
		{
			CASTOR_EXCEPTION( "This overlay is not a text." );
		}

		return std::static_pointer_cast< TextOverlay >( m_category );
	}

	bool Overlay::IsVisible()const
	{
		bool l_return = m_category->IsVisible();

		if ( l_return && GetParent() )
		{
			l_return = GetParent()->IsVisible();
		}

		return l_return;
	}
}
