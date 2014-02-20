#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Overlay.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Font.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/Viewport.hpp"

using namespace Castor3D;
using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace
{
	struct stOVERLAY_VERTEX
	{
		real m_position[3];
		real m_texture[2];
	};
}

//*************************************************************************************************

OverlayCategory::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< OverlayCategory, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool OverlayCategory::TextLoader :: operator ()( OverlayCategory const & p_overlay, TextFile & p_file)
{
	String l_strTabs;
	OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.Print( 1024, cuT( "%S\tposition "), l_strTabs.c_str()) > 0;

	if (l_bReturn)
	{
		l_bReturn = Point2r::TextLoader()( p_overlay.GetPosition(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "\n%S\tsize "), l_strTabs.c_str()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Point2r::TextLoader()( p_overlay.GetSize(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tvisible ") + (p_overlay.IsVisible() ? String( cuT( "true" ) ) : String( cuT( "false" ) )) ) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "%S\tzindex %i"), l_strTabs.c_str(), p_overlay.GetZIndex()) > 0;
	}

	if (l_bReturn && p_overlay.GetMaterial())
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tmaterial ") + p_overlay.GetMaterial()->GetName()) > 0;
	}

	for( OverlayPtrIntMapConstIt l_it = p_overlay.GetOverlay().Begin(); l_it != p_overlay.GetOverlay().End(); ++l_it )
	{
		switch( l_it->second->GetType() )
		{
		case eOVERLAY_TYPE_PANEL:
			l_bReturn = PanelOverlay::TextLoader()( *std::static_pointer_cast<PanelOverlay>( l_it->second->GetOverlayCategory() ), p_file );
			break;

		case eOVERLAY_TYPE_BORDER_PANEL:
			l_bReturn = BorderPanelOverlay::TextLoader()( *std::static_pointer_cast<BorderPanelOverlay>( l_it->second->GetOverlayCategory() ), p_file );
			break;

		case eOVERLAY_TYPE_TEXT:
			l_bReturn = TextOverlay::TextLoader()( *std::static_pointer_cast<TextOverlay>( l_it->second->GetOverlayCategory() ), p_file );
			break;

		default:
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

OverlayCategory :: OverlayCategory( eOVERLAY_TYPE p_eType )
	:	m_eType				( p_eType		)
	,	m_bVisible			( true			)
	,	m_iCurrentZIndex	( 1				)
{
}

OverlayCategory :: ~OverlayCategory()
{
}

void OverlayCategory :: SetPosition( OverlaySPtr p_pParent, Point2r const & p_ptPosition )
{
	m_ptPosition = p_ptPosition;
	//if( p_pParent )
	//{
	//	m_ptPosition = p_pParent->GetPosition() + p_ptPosition * p_pParent->GetSize();
	//}
	//else
	//{
	//	m_ptPosition = p_ptPosition;
	//}
}

void OverlayCategory :: SetSize( OverlaySPtr p_pParent, Point2r const & p_ptSize )
{
	m_ptSize = p_ptSize;
	//if( p_pParent )
	//{
	//	m_ptSize = p_ptSize * p_pParent->GetSize();
	//}
	//else
	//{
	//	m_ptSize = p_ptSize;
	//}
}

void OverlayCategory :: SetMaterial( MaterialSPtr p_pMaterial )
{
	m_pMaterial = p_pMaterial;

	if( p_pMaterial )
	{
		m_strMatName = p_pMaterial->GetName();
	}
	else
	{
		m_strMatName = cuEmptyString;
	}
}

String const & OverlayCategory :: GetOverlayName()const
{
	return m_pOverlay->GetName();
}

//*************************************************************************************************

Overlay::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Overlay, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Overlay::TextLoader :: operator ()( Overlay const & p_overlay, TextFile & p_file)
{
	return OverlayCategory::TextLoader()( *p_overlay.GetOverlayCategory(), p_file );
}

Overlay :: Overlay( Engine * p_pEngine, OverlayFactory & p_factory, eOVERLAY_TYPE p_eType )
	:	m_pParent		(								)
	,	m_pScene		(								)
	,	m_factory		( p_factory						)
	,	m_pEngine		( p_pEngine						)
	,	m_pRenderSystem	( p_pEngine->GetRenderSystem()	)
{
	m_pOverlayCategory = p_factory.Create( p_eType );
	m_pOverlayCategory->SetRenderer( p_pEngine->GetOverlayManager().GetRenderer() );
	m_pOverlayCategory->SetOverlay( this );
}

Overlay :: Overlay( Engine * p_pEngine, OverlayFactory & p_factory, eOVERLAY_TYPE p_eType, SceneSPtr p_pScene, OverlaySPtr p_pParent )
	:	m_pParent		( p_pParent						)
	,	m_pScene		( p_pScene						)
	,	m_factory		( p_factory						)
	,	m_pRenderSystem	( p_pEngine->GetRenderSystem()	)
	,	m_pEngine		( p_pEngine						)
{
	m_pOverlayCategory = p_factory.Create( p_eType );
	m_pOverlayCategory->SetRenderer( p_pEngine->GetOverlayManager().GetRenderer() );
	m_pOverlayCategory->SetOverlay( this );
}

Overlay :: ~Overlay()
{
}

void Overlay :: Register( OverlayFactory & p_factory )
{
	p_factory.Register(	eOVERLAY_TYPE_PANEL,		PanelOverlay::Create		);
	p_factory.Register(	eOVERLAY_TYPE_BORDER_PANEL,	BorderPanelOverlay::Create	);
	p_factory.Register(	eOVERLAY_TYPE_TEXT,			TextOverlay::Create			);
}

void Overlay :: Render( )
{
	if( IsVisible() )
	{
		Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
		
		Matrix4x4r l_mtxTransform;
		MtxUtils::set_transform( l_mtxTransform, Point3r( m_pOverlayCategory->GetPosition()[0], m_pOverlayCategory->GetPosition()[1] ), Point3r( m_pOverlayCategory->GetSize()[0], m_pOverlayCategory->GetSize()[1] ), Quaternion::Identity() );

		l_pPipeline->PushMatrix();
		l_pPipeline->MultMatrix( l_mtxTransform );

		m_pOverlayCategory->Render();

		for( OverlayPtrIntMapIt l_it = m_mapOverlays.begin(); l_it != m_mapOverlays.end(); ++l_it )
		{
			l_it->second->Render();
		}

		l_pPipeline->PopMatrix();
	}
}

bool Overlay :: AddChild( OverlaySPtr p_pOverlay, int p_iZIndex )
{
	bool l_bReturn = false;

	if( p_iZIndex == 0 )
	{
		m_mapOverlays.insert( std::make_pair( GetZIndex(), p_pOverlay ) );
		SetZIndex( GetZIndex() + 100 );
		l_bReturn = true;
	}
	else if( m_mapOverlays.find( p_iZIndex) == m_mapOverlays.end() )
	{
		m_mapOverlays.insert( std::make_pair( p_iZIndex, p_pOverlay ) );
		l_bReturn = true;
	}

	p_pOverlay->SetZIndex( p_iZIndex );

	while( m_mapOverlays.find( GetZIndex() ) != m_mapOverlays.end() )
	{
		SetZIndex( GetZIndex() + 100 );
		l_bReturn = true;
	}

	return l_bReturn;
}

void Overlay :: Initialise()
{
}

//*************************************************************************************************

bool PanelOverlay::TextLoader :: operator ()( PanelOverlay const & p_overlay, TextFile & p_file)
{
	String l_strTabs;
	OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteText( l_strTabs + cuT( "panel_overlay ") + p_overlay.GetOverlay().GetName() + cuT( "\n") + l_strTabs + cuT( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = Overlay::TextLoader()( p_overlay.GetOverlay(), p_file );
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

PanelOverlay :: PanelOverlay()
	:	OverlayCategory	( eOVERLAY_TYPE_PANEL	)
{
}

PanelOverlay :: ~PanelOverlay()
{
}

OverlayCategorySPtr PanelOverlay :: Create()
{
	return std::make_shared< PanelOverlay >();
}

void PanelOverlay :: Render()
{
	OverlayRendererSPtr l_pRenderer = m_pRenderer.lock();

	if( IsVisible() && l_pRenderer )
	{
		l_pRenderer->DrawPanel( *this );
	}
}

//*************************************************************************************************

bool BorderPanelOverlay::TextLoader :: operator ()( BorderPanelOverlay const & p_overlay, TextFile & p_file)
{
	String l_strTabs;
	OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteText( l_strTabs + cuT( "border_panel_overlay ") + p_overlay.GetOverlay().GetName() + cuT( "\n") + l_strTabs + cuT( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 1024, cuT( "%S\tborder_size "), l_strTabs.c_str()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Point4r::TextLoader()( p_overlay.GetBorderSize(), p_file);
	}

	if (l_bReturn && p_overlay.GetBorderMaterial())
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tborder_material ") + p_overlay.GetBorderMaterial()->GetName()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::TextLoader()( p_overlay.GetOverlay(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

BorderPanelOverlay :: BorderPanelOverlay()
	:	OverlayCategory	( eOVERLAY_TYPE_BORDER_PANEL	)
{
}

BorderPanelOverlay :: ~BorderPanelOverlay()
{
}

OverlayCategorySPtr BorderPanelOverlay :: Create()
{
	return std::make_shared< BorderPanelOverlay >();
}

void BorderPanelOverlay :: Render()
{
	OverlayRendererSPtr l_pRenderer = m_pRenderer.lock();

	if( IsVisible() && l_pRenderer )
	{
		l_pRenderer->DrawBorderPanel( *this );
	}
}

void BorderPanelOverlay :: SetBorderMaterial( MaterialSPtr p_pMaterial )
{
	m_pBorderMaterial = p_pMaterial;

	if( p_pMaterial )
	{
		m_strBorderMatName = p_pMaterial->GetName();
	}
	else
	{
		m_strBorderMatName = cuEmptyString;
	}
}

//*************************************************************************************************

bool TextOverlay::TextLoader :: operator ()( TextOverlay const & p_overlay, TextFile & p_file)
{
	String l_strTabs;
	OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

	while (l_pParent)
	{
		l_strTabs += cuT( '\t');
		l_pParent = l_pParent->GetParent();
	}

	bool l_bReturn = p_file.WriteText( l_strTabs + cuT( "text_overlay ") + p_overlay.GetOverlayName() + cuT( "\n") + l_strTabs + cuT( "{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tfont ") + p_overlay.GetFontName()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tcaption ") + p_overlay.GetCaption()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Overlay::TextLoader()( p_overlay.GetOverlay(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

TextOverlay :: TextOverlay()
	:	OverlayCategory	( eOVERLAY_TYPE_TEXT	)
{
}

TextOverlay :: ~TextOverlay()
{
}

OverlayCategorySPtr TextOverlay :: Create()
{
	return std::make_shared< TextOverlay >();
}

void TextOverlay :: Render()
{
	OverlayRendererSPtr l_pRenderer = m_pRenderer.lock();

	if( IsVisible() && l_pRenderer )
	{
		l_pRenderer->DrawText( *this );
	}
}

void TextOverlay :: SetFont( String const & p_strFont )
{
	m_pFont = std::make_shared< TextFont >( p_strFont, m_pOverlay->GetEngine() );
	m_pOverlay->GetEngine()->PostEvent( std::make_shared< InitialiseEvent< TextFont > >( *m_pFont ) );
	m_strFontName = p_strFont;
	_updateMaterial();
}

void TextOverlay :: SetMaterial( MaterialSPtr p_pMaterial )
{
	OverlayCategory::SetMaterial( p_pMaterial );
	_updateMaterial();
}

String const & TextOverlay :: GetFontName()const
{
	return m_pFont->GetFontName();
}

void TextOverlay :: Draw( Castor::Colour const & p_clrColour, ShaderProgramBaseSPtr p_pProgram )
{
	m_pFont->DisplayText( m_strCaption, p_clrColour, p_pProgram );
}

void TextOverlay :: _updateMaterial()
{
//	if( m_pFont && GetMaterial() )
//	{
//		StaticTextureSPtr l_pTexture = m_pOverlay->GetEngine()->GetRenderSystem()->CreateStaticTexture();
//		l_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
////		l_pTexture->SetImage( m_pFont->GetImagePixels() );
//		GetMaterial()->GetPass( 0)->AddTextureUnit()->SetTexture( l_pTexture );
//	}
}

//*************************************************************************************************

OverlayRenderer :: OverlayRenderer( RenderSystem * p_pRenderSystem )
	:	m_pRenderSystem	( p_pRenderSystem )
{
	BufferElementDeclaration	l_vertexDeclarationElements[] =
	{	BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS )
	,	BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS )
	};

	stVertex l_pBuffer[] =
	{	{ { 0, 0, 0 }, { 0, 1 } }
	,	{ { 0, 1, 0 }, { 0, 0 } }
	,	{ { 1, 1, 0 }, { 1, 0 } }
	,	{ { 0, 0, 0 }, { 0, 1 } }
	,	{ { 1, 1, 0 }, { 1, 0 } }
	,	{ { 1, 0, 0 }, { 1, 1 } }
	};

	std::memcpy( m_pBuffer, l_pBuffer, sizeof( l_pBuffer ) );

	m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
	uint32_t i = 0;

	std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
	{
		p_vertex = std::make_shared< BufferElementGroup >( reinterpret_cast< uint8_t * >( &m_pBuffer[i++] ) );
	} );
}

OverlayRenderer :: ~OverlayRenderer()
{
}

void OverlayRenderer :: Initialise()
{
	if( !m_pGeometryBuffer )
	{
		VertexBufferSPtr l_pVtxBuffer = std::make_shared< VertexBuffer >( m_pRenderSystem, &(*m_pDeclaration)[0], m_pDeclaration->Size() );
		l_pVtxBuffer->SetShared( l_pVtxBuffer );

		uint32_t l_uiStride = m_pDeclaration->GetStride();
		l_pVtxBuffer->Resize( uint32_t( m_arrayVertex.size() * l_uiStride ) );

		for( uint32_t i = 0; i < m_arrayVertex.size(); ++i )
		{
			m_arrayVertex[i]->LinkCoords( &l_pVtxBuffer->data()[i * l_uiStride], l_uiStride );
		}

		m_pGeometryBuffer = m_pRenderSystem->CreateGeometryBuffers( l_pVtxBuffer, nullptr, nullptr );
		ShaderProgramBaseSPtr l_pProgram = GetPanelProgram( 0 );
		
		l_pVtxBuffer->Create();
//		l_pVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
		l_pVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
		m_pGeometryBuffer->Initialise();

		DoInitialise();
	}
}

void OverlayRenderer :: Cleanup()
{
	DoCleanup();

	std::for_each( m_mapPanelPrograms.begin(), m_mapPanelPrograms.end(), [&]( std::pair< uint32_t, ShaderProgramBaseSPtr > p_pair )
	{
		if( p_pair.second )
		{
			p_pair.second->Cleanup();
		}
	} );

	std::for_each( m_mapTextPrograms.begin(), m_mapTextPrograms.end(), [&]( std::pair< uint32_t, ShaderProgramBaseSPtr > p_pair )
	{
		if( p_pair.second )
		{
			p_pair.second->Cleanup();
		}
	} );

	m_mapPanelPrograms.clear();
	m_mapTextPrograms.clear();

	if( m_pGeometryBuffer )
	{
		m_pGeometryBuffer->GetVertexBuffer()->Cleanup();
		m_pGeometryBuffer->Cleanup();
		m_pGeometryBuffer.reset();
	}
}

void OverlayRenderer :: DrawPanel( PanelOverlay & p_panelOverlay )
{
	MaterialSPtr l_pMaterial = p_panelOverlay.GetMaterial();
	int iZIndex = 1000 - p_panelOverlay.GetOverlay().GetZIndex();

	stVertex l_pBuffer[] =
	{	{ { 0, 0, real( iZIndex ) }, { 0, 1 } }
	,	{ { 0, 1, real( iZIndex ) }, { 0, 0 } }
	,	{ { 1, 1, real( iZIndex ) }, { 1, 0 } }
	,	{ { 0, 0, real( iZIndex ) }, { 0, 1 } }
	,	{ { 1, 1, real( iZIndex ) }, { 1, 0 } }
	,	{ { 1, 0, real( iZIndex ) }, { 1, 1 } }
	};
	
	VertexBufferSPtr l_pVtxBuffer = m_pGeometryBuffer->GetVertexBuffer();
	l_pVtxBuffer->Bind();
	l_pVtxBuffer->Fill( reinterpret_cast< uint8_t* >( l_pBuffer ), sizeof( l_pBuffer ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
	l_pVtxBuffer->Unbind();

	if( l_pMaterial )
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );
		
		std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
		{
			ShaderProgramBaseSPtr l_pProgram = GetPanelProgram( p_pPass->GetTextureFlags() );

			if( l_pProgram )
			{
				p_pPass->BindToProgram( l_pProgram );
				m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_pProgram );
				p_pPass->Render2D( l_byIndex++, l_byCount );
				m_pGeometryBuffer->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, uint32_t( m_arrayVertex.size() ), 0 );
				p_pPass->EndRender();
			}
		} );
	}
}

void OverlayRenderer :: DrawBorderPanel( BorderPanelOverlay & p_borderPanelOverlay )
{
	MaterialSPtr l_pMaterial = p_borderPanelOverlay.GetMaterial();

	if( l_pMaterial )
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );

		std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
		{
			ShaderProgramBaseSPtr l_pProgram = GetPanelProgram( p_pPass->GetTextureFlags() );

			if( l_pProgram )
			{
				p_pPass->BindToProgram( l_pProgram );
				m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_pProgram );
				p_pPass->Render2D( l_byIndex++, l_byCount );
				m_pGeometryBuffer->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, uint32_t( m_arrayVertex.size() ), 0 );
				p_pPass->EndRender();
			}
		} );
	}
}

void OverlayRenderer :: DrawText( TextOverlay & p_textOverlay )
{
	MaterialSPtr l_pMaterial = p_textOverlay.GetMaterial();

	if( l_pMaterial )
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );

		std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
		{
			ShaderProgramBaseSPtr l_pProgram = GetTextProgram( p_pPass->GetTextureFlags() | eTEXTURE_CHANNEL_COLOUR );

			if( l_pProgram )
			{
				p_textOverlay.Draw( Colour::from_predef( Colour::ePREDEFINED_FULLALPHA_WHITE ), l_pProgram );
			}
		} );
	}
}

void OverlayRenderer :: BeginRender( Size const & p_size )
{
}

void OverlayRenderer :: EndRender()
{
}

ShaderProgramBaseSPtr OverlayRenderer :: GetPanelProgram( uint32_t p_uiFlags )
{
	p_uiFlags &= ~eTEXTURE_CHANNEL_AMBIENT;
	p_uiFlags &= ~eTEXTURE_CHANNEL_DIFFUSE;
	p_uiFlags &= ~eTEXTURE_CHANNEL_NORMAL;
	p_uiFlags &= ~eTEXTURE_CHANNEL_SPECULAR;
	p_uiFlags &= ~eTEXTURE_CHANNEL_GLOSS;
	p_uiFlags &= ~eTEXTURE_CHANNEL_HEIGHT;
	p_uiFlags &= ~eTEXTURE_CHANNEL_LGHTPASS;
	std::map< uint32_t, ShaderProgramBaseSPtr >::const_iterator l_it = m_mapPanelPrograms.find( p_uiFlags );

	ShaderProgramBaseSPtr l_pReturn;

	if( l_it != m_mapPanelPrograms.end() )
	{
		l_pReturn = l_it->second;
	}
	else
	{
		l_pReturn = DoGetProgram( p_uiFlags );
		m_mapPanelPrograms.insert( std::make_pair( p_uiFlags, l_pReturn ) );
	}

	return l_pReturn;
}

ShaderProgramBaseSPtr OverlayRenderer :: GetTextProgram( uint32_t p_uiFlags )
{
	p_uiFlags &= ~eTEXTURE_CHANNEL_AMBIENT;
	p_uiFlags &= ~eTEXTURE_CHANNEL_DIFFUSE;
	p_uiFlags &= ~eTEXTURE_CHANNEL_NORMAL;
	p_uiFlags &= ~eTEXTURE_CHANNEL_SPECULAR;
	p_uiFlags &= ~eTEXTURE_CHANNEL_GLOSS;
	p_uiFlags &= ~eTEXTURE_CHANNEL_HEIGHT;
	p_uiFlags &= ~eTEXTURE_CHANNEL_LGHTPASS;
	std::map< uint32_t, ShaderProgramBaseSPtr >::const_iterator l_it = m_mapTextPrograms.find( p_uiFlags );

	ShaderProgramBaseSPtr l_pReturn;

	if( l_it != m_mapTextPrograms.end() )
	{
		l_pReturn = l_it->second;
	}
	else
	{
		l_pReturn = DoGetProgram( p_uiFlags );
		l_pReturn->CreateFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
		m_mapTextPrograms.insert( std::make_pair( p_uiFlags, l_pReturn ) );
	}

	return l_pReturn;
}

//*************************************************************************************************

struct OverlayInitialiser
{
	void operator()( OverlaySPtr p_pOverlay )
	{
		p_pOverlay->Initialise();
	}
};

//*************************************************************************************************

OverlayManager :: OverlayManager( Engine * p_pEngine )
	:	m_iCurrentZIndex	( 0			)
	,	m_pEngine			( p_pEngine	)
{
}

OverlayManager :: ~OverlayManager()
{
}

void OverlayManager :: ClearOverlays()
{
	OverlayCollection::lock();
	OverlayCollection::clear();
	clear_pair_container( m_mapOverlaysByZIndex );
	m_iCurrentZIndex = 1;
	OverlayCollection::unlock();
}

void OverlayManager :: AddOverlay( Castor::String const & p_strName, OverlaySPtr p_pOverlay, OverlaySPtr p_pParent )
{
	OverlayCollection::insert( p_strName, p_pOverlay );

	if( !p_pParent )
	{
		AddOverlay( m_iCurrentZIndex, p_pOverlay );
	}
	else
	{
		p_pParent->AddChild( p_pOverlay, m_iCurrentZIndex );
	}

	m_iCurrentZIndex++;
}

void OverlayManager :: AddOverlay( int p_iZIndex, OverlaySPtr p_pOverlay )
{
	p_pOverlay->SetZIndex( p_iZIndex );
	m_mapOverlaysByZIndex.insert( std::make_pair( p_iZIndex, p_pOverlay ) );
}

bool OverlayManager :: HasOverlay( int p_iZIndex )
{
	return m_mapOverlaysByZIndex.find( p_iZIndex) != m_mapOverlaysByZIndex.end();
}

bool OverlayManager :: HasOverlay( Castor::String const & p_strName )
{
	return OverlayCollection::has( p_strName );
}

OverlaySPtr OverlayManager :: GetOverlay( int p_iZIndex )
{
	OverlaySPtr l_pReturn;
	OverlayPtrIntMapIt l_it = m_mapOverlaysByZIndex.find( p_iZIndex );

	if( l_it != m_mapOverlaysByZIndex.end() )
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

OverlaySPtr OverlayManager :: GetOverlay( Castor::String const & p_strName )
{
	return OverlayCollection::find( p_strName );
}

bool OverlayManager :: WriteOverlays( Castor::TextFile & p_file )const
{
	OverlayCollection::lock();
	bool l_bReturn = true;
	OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

	bool l_bFirst = true;

	while( l_bReturn && l_it != m_mapOverlaysByZIndex.end() )
	{
		if( l_bFirst )
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteText( cuT( "\n" ) );
		}

		switch( l_it->second->GetType() )
		{
		case eOVERLAY_TYPE_PANEL:
			l_bReturn = PanelOverlay::TextLoader()(			* std::static_pointer_cast< PanelOverlay >(			l_it->second->GetOverlayCategory() ), p_file );
			break;

		case eOVERLAY_TYPE_BORDER_PANEL:
			l_bReturn = BorderPanelOverlay::TextLoader()(	* std::static_pointer_cast< BorderPanelOverlay >(	l_it->second->GetOverlayCategory() ), p_file );
			break;

		case eOVERLAY_TYPE_TEXT:
			l_bReturn = TextOverlay::TextLoader()(			* std::static_pointer_cast< TextOverlay >(			l_it->second->GetOverlayCategory() ), p_file );
			break;

		default:
			l_bReturn = false;
		}

		++l_it;
	}

	OverlayCollection::unlock();
	return l_bReturn;
}

bool OverlayManager :: ReadOverlays( Castor::TextFile & p_file )
{
	SceneFileParser l_parser( m_pEngine );
	return l_parser.ParseFile( p_file );
}

bool OverlayManager :: SaveOverlays( Castor::BinaryFile & p_file )const
{
	OverlayCollection::lock();
	bool l_bReturn = p_file.Write( uint32_t( m_mapOverlaysByZIndex.size() ) ) == sizeof( uint32_t );
	OverlayPtrIntMap::const_iterator l_it = m_mapOverlaysByZIndex.begin();

	while( l_bReturn && l_it != m_mapOverlaysByZIndex.end() )
	{
		l_bReturn = BinaryLoader< Overlay >()( *l_it->second, p_file );
		++l_it;
	}

	OverlayCollection::unlock();
	return l_bReturn;
}

bool OverlayManager :: LoadOverlays( Castor::BinaryFile & p_file )
{
	OverlayCollection::lock();
	uint32_t l_uiSize;
	bool l_bReturn = p_file.Write( l_uiSize ) == sizeof( uint32_t );
	String l_strName;
	eOVERLAY_TYPE l_eType;
	OverlaySPtr l_pOverlay;

	for( uint32_t i = 0; i < l_uiSize && l_bReturn; i++ )
	{
		l_bReturn = p_file.Read( l_strName );

		if( l_bReturn )
		{
			l_bReturn = p_file.Read( l_eType ) == sizeof( eOVERLAY_TYPE);
		}

		if( l_bReturn )
		{
			l_pOverlay = OverlayCollection::find( l_strName) ;

			if ( ! l_pOverlay )
			{
				l_pOverlay = std::make_shared< Overlay >( m_pEngine, m_pEngine->GetOverlayFactory(), l_eType );
				l_pOverlay->SetName( l_strName );
				OverlayCollection::insert( l_strName, l_pOverlay);
			}

			l_bReturn = l_pOverlay.use_count() > 0;
		}

		if( l_bReturn )
		{
			l_bReturn = BinaryLoader< Overlay >()( *l_pOverlay, p_file );
		}
	}

	OverlayCollection::unlock();
	return l_bReturn;
}

void OverlayManager :: Update()
{
	if( m_pEngine->IsCleaned() )
	{
		if( m_pRenderer )
		{
			m_pRenderer->Cleanup();
			m_pRenderer.reset();
		}
	}
	else
	{
		if( !m_pRenderer )
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

void OverlayManager :: BeginRendering( Size const & p_size )
{
	if( m_pRenderer )
	{
		m_pRenderer->BeginRender( p_size );
	}
}

void OverlayManager :: EndRendering()
{
	if( m_pRenderer )
	{
		m_pRenderer->EndRender();
	}
}

//*************************************************************************************************
