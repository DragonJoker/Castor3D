#include "Overlay.hpp"

#include "BorderPanelOverlay.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "OverlayFactory.hpp"
#include "OverlayManager.hpp"
#include "OverlayRenderer.hpp"
#include "PanelOverlay.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "SceneFileParser.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "TextOverlay.hpp"
#include "Texture.hpp"
#include "TextureUnit.hpp"
#include "Vertex.hpp"
#include "Viewport.hpp"

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
		return OverlayCategory::TextLoader()( *p_overlay.GetOverlayCategory(), p_file );
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
			l_bReturn = OverlayCategory::BinaryParser( m_path ).Fill( *p_obj.GetOverlayCategory(), l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		for ( Overlay::const_iterator l_it = p_obj.Begin(); l_it != p_obj.End() && l_bReturn; ++l_it )
		{
			l_bReturn = Overlay::BinaryParser( m_path, m_engine ).Fill( *l_it->second, l_chunk );
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
					l_bReturn = OverlayCategory::BinaryParser( m_path ).Parse( *p_obj.GetOverlayCategory(), l_chunk );
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
		:	m_pParent()
		,	m_pScene()
		,	m_factory( p_pEngine->GetOverlayFactory() )
		,	m_pEngine( p_pEngine )
		,	m_pRenderSystem( p_pEngine->GetRenderSystem() )
	{
		m_pOverlayCategory = m_factory.Create( p_eType );
		m_pOverlayCategory->SetRenderer( p_pEngine->GetOverlayManager().GetRenderer() );
		m_pOverlayCategory->SetOverlay( this );
	}

	Overlay::Overlay( Engine * p_pEngine, eOVERLAY_TYPE p_eType, SceneSPtr p_pScene, OverlaySPtr p_pParent )
		:	m_pParent( p_pParent )
		,	m_pScene( p_pScene )
		,	m_factory( p_pEngine->GetOverlayFactory() )
		,	m_pRenderSystem( p_pEngine->GetRenderSystem() )
		,	m_pEngine( p_pEngine )
	{
		m_pOverlayCategory = m_factory.Create( p_eType );
		m_pOverlayCategory->SetRenderer( p_pEngine->GetOverlayManager().GetRenderer() );
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
			Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
			Matrix4x4r l_mtxTransform;
			MtxUtils::set_transform(
				l_mtxTransform,
				Point3r( 0, 0, 0 ),
				Point3r( 1, 1, 0 ),
				Quaternion::Identity()
			);
			l_pPipeline->PushMatrix();
			l_pPipeline->MultMatrix( l_mtxTransform );
			m_pOverlayCategory->Render();

			for ( OverlayPtrIntMapIt l_it = m_mapOverlays.begin(); l_it != m_mapOverlays.end(); ++l_it )
			{
				l_it->second->Render( p_size );
			}

			l_pPipeline->PopMatrix();
		}
	}

	bool Overlay::AddChild( OverlaySPtr p_pOverlay, int p_iZIndex )
	{
		bool l_bReturn = false;

		if ( p_iZIndex == 0 )
		{
			m_mapOverlays.insert( std::make_pair( GetZIndex(), p_pOverlay ) );
			SetZIndex( GetZIndex() + 100 );
			l_bReturn = true;
		}
		else if ( m_mapOverlays.find( p_iZIndex ) == m_mapOverlays.end() )
		{
			m_mapOverlays.insert( std::make_pair( p_iZIndex, p_pOverlay ) );
			l_bReturn = true;
		}

		p_pOverlay->SetZIndex( p_iZIndex );

		while ( m_mapOverlays.find( GetZIndex() ) != m_mapOverlays.end() )
		{
			SetZIndex( GetZIndex() + 100 );
			l_bReturn = true;
		}

		return l_bReturn;
	}

	void Overlay::Initialise()
	{
	}
}
