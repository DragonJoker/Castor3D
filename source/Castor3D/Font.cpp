#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Font.hpp"
#include "Castor3D/BufferElement.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Overlay.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Sampler.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/BlendState.hpp"

using namespace Castor3D;
using namespace Castor;


//*************************************************************************************************

TextFont :: TextFont( String const & p_strFontName, Engine * p_pEngine )
	:	m_pEngine		( p_pEngine	)
{
	// Récupération / Création de la police
	FontCollection & l_fontCollection = p_pEngine->GetFontManager();
	FontSPtr l_pFont = l_fontCollection.find( p_strFontName );

	if( l_pFont )
	{
		m_wpFont = l_pFont;
		uint32_t l_uiMaxWidth = l_pFont->GetMaxWidth();
		uint64_t l_ui64Count = uint64_t( std::ceil( sqrt( l_pFont->End() - l_pFont->Begin() ) ) );

		SamplerSPtr l_pSampler = m_pEngine->CreateSampler( p_strFontName );
		l_pSampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
		l_pSampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
		m_wpSampler = l_pSampler;

		m_pTexture = m_pEngine->GetRenderSystem()->CreateDynamicTexture();
		m_pTexture->SetImage( Size( l_uiMaxWidth * uint32_t( l_ui64Count ), l_pFont->GetMaxHeight() * uint32_t( l_ui64Count ) ), ePIXEL_FORMAT_L8 );
		m_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		m_pTexture->SetSampler( l_pSampler );

		Castor::Font::GlyphArray::const_iterator l_it = l_pFont->Begin();
		uint32_t l_uiCount = uint32_t( std::ceil( sqrt( l_pFont->End() - l_pFont->Begin() ) ) );
		Size l_sizeImg = m_pTexture->GetDimensions();
		uint32_t l_uiMaxHeight = l_pFont->GetMaxHeight();
		uint32_t l_uiTotalWidth = l_sizeImg.width();
		uint32_t l_uiTotalHeight = l_sizeImg.height();
		uint32_t l_uiOffY = 0;
		uint8_t * l_pBuffer = m_pTexture->GetBuffer()->ptr();

		for( uint32_t y = 0; y < l_uiCount && l_it != l_pFont->End(); ++y )
		{
			uint32_t l_uiOffX = 0;

			for( uint32_t x = 0; x < l_uiCount && l_it != l_pFont->End(); ++x )
			{
				ByteArray l_buffer = l_it->GetBitmap();

				for( uint32_t i = 0; i < l_it->GetSize().height(); ++i )
				{
					std::memcpy( &l_pBuffer[(l_uiTotalWidth * (l_uiOffY + i)) + l_uiOffX], &l_buffer[i * l_it->GetSize().width()], l_it->GetSize().width() );
					m_mapGlyphOffsets[l_it->GetCharacter()] = std::make_pair( x, y );
				}

				l_uiOffX += l_uiMaxWidth;
				++l_it;
			}

			l_uiOffY += l_uiMaxHeight;
		}

		// Paramètrage du rendu
		m_pBlendState = m_pEngine->CreateBlendState( p_strFontName );
		m_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
		m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );

		BufferElementDeclaration	l_vertexDeclarationElements[] =
		{	BufferElementDeclaration( 0,	eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_2FLOATS	)
		,	BufferElementDeclaration( 0,	eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS	)
		,	BufferElementDeclaration( 0,	eELEMENT_USAGE_DIFFUSE,		eELEMENT_TYPE_COLOUR	)
		};

		TextVertex l_pTexBuffer[] =
		{	{ { 0, 0 }, { 0, 0 }, 0 }
		,	{ { 0, 1 }, { 0, 1 }, 0 }
		,	{ { 1, 1 }, { 1, 1 }, 0 }
		,	{ { 1, 0 }, { 1, 0 }, 0 }
		};

		std::memcpy( m_pBuffer, l_pTexBuffer, sizeof( l_pTexBuffer ) );

		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
		uint32_t i = 0;

		std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
		} );
	}
	else
	{
		CASTOR_EXCEPTION( "Font " + str_utils::to_str( p_strFontName ) + "not found" );
	}
}

TextFont :: ~TextFont()
{
	m_pDeclaration.reset();
	m_pTexture.reset();
	m_wpFont.reset();
	m_wpSampler.reset();
	m_pBlendState.reset();
}

void TextFont :: Initialise()
{
	if( !m_pGeometryBuffers )
	{
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();
		VertexBufferSPtr l_pVtxBuffer;

		l_pVtxBuffer = std::make_shared< VertexBuffer >( l_pRenderSystem, &(*m_pDeclaration)[0], m_pDeclaration->Size() );
		l_pVtxBuffer->SetShared( l_pVtxBuffer );

		uint32_t l_uiStride = m_pDeclaration->GetStride();
		l_pVtxBuffer->Resize( 4 * l_uiStride );
		m_arrayVertex[0]->LinkCoords( &l_pVtxBuffer->data()[0 * l_uiStride], l_uiStride );
		m_arrayVertex[1]->LinkCoords( &l_pVtxBuffer->data()[1 * l_uiStride], l_uiStride );
		m_arrayVertex[2]->LinkCoords( &l_pVtxBuffer->data()[2 * l_uiStride], l_uiStride );
		m_arrayVertex[3]->LinkCoords( &l_pVtxBuffer->data()[3 * l_uiStride], l_uiStride );

		m_pGeometryBuffers = l_pRenderSystem->CreateGeometryBuffers( l_pVtxBuffer, nullptr, nullptr );
		m_pGeometryBuffers->Initialise();

		m_pBlendState->Initialise();
		m_pTexture->Create();
		m_pTexture->Initialise( 0 );
	}
}

void TextFont :: Cleanup()
{
	if( m_pTexture )
	{
		m_pTexture->Cleanup();
		m_pTexture->Destroy();
	}

	if( m_pBlendState )
	{
		m_pBlendState->Cleanup();
	}

	if( m_pGeometryBuffers )
	{
		m_pGeometryBuffers->GetVertexBuffer()->Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_pGeometryBuffers.reset();
	}
}

void TextFont :: DisplayText( String const & p_strText, Colour const & p_clrColour, ShaderProgramBaseSPtr p_pProgram )
{
	if( !p_strText.empty() && m_pTexture && m_pBlendState && m_pGeometryBuffers && DoGetFont() )
	{
		float x = 0.0;
		float y = 0.0;

		FontSPtr l_pFont = DoGetFont();
		VertexBufferSPtr l_pVtxBuffer = m_pGeometryBuffers->GetVertexBuffer();
		uint32_t l_uiColour = p_clrColour.to_argb();
		p_pProgram->FindFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL )->SetValue( m_pTexture.get() );

		std::vector< TextVertex > l_arrayVtx( p_strText.size() * 6 );

		if( l_pVtxBuffer )
		{
			uint32_t n = 0;
			Size const & l_texDim = m_pTexture->GetDimensions();

			for( uint32_t i = 0; i < p_strText.size(); ++i )
			{
				xchar c = p_strText[i];
				OffsetCharMapIt l_it = m_mapGlyphOffsets.find( c );
				Glyph & l_glyph = l_pFont->GetGlyphAt( c );
				float x2 = x + l_glyph.GetPosition().x();
				float y2 = -y - l_glyph.GetPosition().y();
				float w = float( l_glyph.GetSize().width() );
				float h = float( l_glyph.GetSize().height() );

				x += l_glyph.GetAdvance().width();
				y += l_glyph.GetAdvance().height();

				if( w && h )
				{
					float tt, tl, tb, tr;
					tl = real( l_it->second.first );
					tt = real( l_it->second.second );
					tr = tl + w;
					tb = tt + h;
					l_arrayVtx[n++] = { { x2 + 0, -y2 - 0 }, { tl / l_texDim.width(), tt / l_texDim.height() }, l_uiColour };
					l_arrayVtx[n++] = { { x2 + w, -y2 - 0 }, { tr / l_texDim.width(), tt / l_texDim.height() }, l_uiColour };
					l_arrayVtx[n++] = { { x2 + 0, -y2 - h }, { tl / l_texDim.width(), tb / l_texDim.height() }, l_uiColour };
					l_arrayVtx[n++] = { { x2 + w, -y2 - 0 }, { tr / l_texDim.width(), tt / l_texDim.height() }, l_uiColour };
					l_arrayVtx[n++] = { { x2 + 0, -y2 - h }, { tl / l_texDim.width(), tb / l_texDim.height() }, l_uiColour };
					l_arrayVtx[n++] = { { x2 + w, -y2 - h }, { tr / l_texDim.width(), tb / l_texDim.height() }, l_uiColour };
				}
    		}

			l_pVtxBuffer->Bind();
			l_pVtxBuffer->Fill( (uint8_t const *)l_arrayVtx.data(), l_arrayVtx.size() * sizeof( TextVertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			l_pVtxBuffer->Unbind();

			uint8_t l_byIndex = 0;
			uint8_t l_byCount = 1;

			m_pTexture->Bind();
			m_pBlendState->Apply();
			p_pProgram->Begin( 0, 1 );
			m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLE_STRIPS, p_pProgram, n, 0 );
			p_pProgram->End();
			m_pTexture->Unbind();
		}
	}
}

String const & TextFont :: GetFontName()const
{
	return DoGetFont()->GetName();
}

//*************************************************************************************************
